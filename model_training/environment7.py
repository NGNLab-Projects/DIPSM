# #ENVIRONMENT - DISTORTION optional, mux order selection action possilble
# ## number of UEs is randomized each episode.

import numpy as np
import math
import gymnasium as gym
from gymnasium import spaces
from scipy.special import erfc


# ==========================================================
# Utility functions
# ==========================================================
def db2pow(db):
    return 10 ** (db / 10)


def pow2db(p):
    return 10 * np.log10(p + 1e-12)


CQI_TABLE = np.array(
    [-6.9360, -5.1470, -3.1800, -1.2530, 0.7610, 2.6990, 4.6940,
     6.5250, 8.5730, 10.3660, 12.2890, 14.1730, 15.8880, 17.8140, 19.8290]
)

# ==========================================================
# Environment
# ==========================================================
class Env4(gym.Env):

    def __init__(self, use_distortion, use_sm, U=None, P_pa=None):
        super().__init__()
        # ---------- System Parameters ----------
        self.use_distortion = use_distortion
        self.use_sm = use_sm
        self.eta = 2/3   # in-band distortion fraction

        self.U_fixed = U
        self.P_pa_fixed = P_pa

        self.Kmax = 4
        self.K_active = 4

        self.U_min = 12
        self.U_max = 120
        self.N = 3
        self.M = 8

        self.Kmax = 4
        self.Nt = 64

        self.Bw = 720000
        self.No = db2pow(-174 + 10 * np.log10(self.Bw))

        # self.Pmax = 40
        self.Pmin = 0
        self.P_PA_LIST = [0.025, 0.05, 0.075, 0.1,0.125, 0.15, 0.175, 0.2]

        self.L = 800
        self.B = 800

        self.xb = np.array([200, 600, 400])
        self.yb = np.array([200, 200, 600])

        self.MaxSteps = 200
       
        self.DR = []
        self.EE = []
        self.IBO = []
    
        # extra_dims = self.N + 2 if self.use_distortion else 1
        extra_dims = self.N + 3
        # extra_dims = self.N + 2

        self.observation_space = spaces.Box(
            low=-1.0,
            high=1.0,
            shape=(3 + self.N + self.N * self.M * 3,),
            dtype=np.float32,
        )

        # 24-dimensional vector in {-1,0,1}
        self.action_space = spaces.MultiDiscrete([3] * (self.N * self.M) +[4])

        self.reset()

    # ======================================================
    # RESET
    # ======================================================
    def reset(self, seed=None, options=None):
        super().reset(seed=seed)

        self.StepCount = 0
        self.ep_rate_sum = 0.0
        self.ep_ee_sum = 0.0
        self.ep_steps = 0
        self.ep_ibo_sum = 0.0
    
        self.U = self.U_fixed if self.U_fixed is not None \
         else np.random.randint(self.U_min, self.U_max+1)

        self.P_pa = self.P_pa_fixed if self.P_pa_fixed is not None \
            else np.random.choice(self.P_PA_LIST)

        self.pstep = 0.03 * self.P_pa if self.use_distortion else 0.001

        # UE positions
        self.xu = self.L * np.random.rand(self.U)
        self.yu = self.B * np.random.rand(self.U)

        # --- Compute distances ---
        dx = self.xb[:, None] - self.xu[None, :]
        dy = self.yb[:, None] - self.yu[None, :]
        d2d = np.sqrt(dx**2 + dy**2)

        # --- LOS probability (3GPP UMi) ---
        P_los = np.where(
            d2d <= 18,
            1.0,
            18/d2d + np.exp(-d2d/36)*(1 - 18/d2d)
        )

        # --- Sample LOS once per episode ---
        self.is_los = np.random.rand(*d2d.shape) < P_los

        shadow_sigma = np.where(self.is_los, 4.0, 7.8)
        self.shadow = shadow_sigma * np.random.randn(*d2d.shape)
        # Initial power
        self.pmat = 0.001 * np.ones((self.N, self.M))

        # ---- compute beta for association ----
        dx = self.xb[:, None] - self.xu[None, :]
        dy = self.yb[:, None] - self.yu[None, :]
        d = np.sqrt(dx**2 + dy**2)

        h_bs = 10
        h_ut = 1.5
        fc = 3.5

        d2d = d
        d3d = np.sqrt(d2d**2 + (h_bs - h_ut)**2)

        # LOS/NLOS model (same as step)
        pl_los = 32.4 + 21*np.log10(d3d + 1e-9) + 20*np.log10(fc)
        pl_nlos = 22.4 + 35.3*np.log10(d3d + 1e-9) + 21.3*np.log10(fc)

        pl_db = np.where(self.is_los, pl_los, np.maximum(pl_los, pl_nlos))

        pl_db += self.shadow

        beta = 1 / db2pow(pl_db)
        self.beta = beta        

        # ---- compute and store association ----
        self.prev_K = self.K_active
        self.A = self.compute_association(self.beta)

        h_real = np.random.randn(self.N, self.M, self.U)
        h_imag = np.random.randn(self.N, self.M, self.U)
        self.h = (h_real + 1j*h_imag) / np.sqrt(2)
        
        self.fading = np.abs(self.h)**2
        self.beta_eff = self.beta[:, None, :] * self.fading

        state, F_curr = self._compute_state(return_DRM=True)
        self.F_prev = F_curr
        return state, {}

    # ======================================================
    # STEP
    # ======================================================
    def step(self, action):

        self.StepCount += 1

        # Convert {0,1,2} → {-1,0,1}
        a = np.array(action[:-1]).reshape(self.N, self.M) - 1
        self.K_active = action[-1] + 1
        # --------------------------------------------------
        # Apply V(t) over all RBs
        # --------------------------------------------------
        if self.K_active != self.prev_K:
            self.A = self.compute_association(self.beta)
            self.prev_K = self.K_active
        # Apply action
        for n in range(self.N):
            for m in range(self.M):
                if a[n, m] != 0:
                    proposed = self.pmat[n, m] + a[n, m] * self.pstep
                    self.pmat[n, m] = max(proposed, self.Pmin)

        # Enforce total power constraint per RU
        for n in range(self.N):
            total_power = np.sum(self.pmat[n])
            if not self.use_distortion:
                if total_power > self.Nt * self.P_pa:
                     self.pmat[n] *= self.Nt * self.P_pa / (total_power + 1e-12)
        # --------------------------------------------------
        # Compute new state and througput
        # --------------------------------------------------
        state, F_curr = self._compute_state(return_DRM=True)


        # reward = (F_curr-self.F_prev)/(self.U)
        reward = F_curr/self.U
        self.F_prev = F_curr

        terminated = False
        truncated = self.StepCount >= self.MaxSteps

        done = terminated or truncated

        if done:
            self.DR.append(self.ep_rate)
            self.EE.append(self.ep_ee)
            self.IBO.append(self.ep_ibo)

        return state, reward, terminated, truncated, {}

    # ======================================================
    # Internal Computation
    # ======================================================
    def _compute_state(self, return_DRM=False):
        A = self.A
        beta = self.beta
        
        beta_eff = self.beta_eff

        I = np.zeros((self.N, self.M, self.U))
        for n in range(self.N):
            for k in range(self.N):
                if k != n:
                    I[n] += self.pmat[k][:, None] * beta[k][None, :] * self.fading[k]

        # ===============================================
        # PA distortion computed per RU
        # ===============================================
        P_ru = np.sum(self.pmat, axis=1)
        P_avg_ant = P_ru / self.Nt
        Psi = self.P_pa / (P_avg_ant + 1e-12)
        if self.use_distortion:

            lam = (
                1 - np.exp(-Psi)
                + 0.5 * np.sqrt(np.pi * Psi) * erfc(np.sqrt(Psi))
            )**2

            D = self.eta * np.maximum(0, 1 - np.exp(-Psi) - lam) * P_ru
            self.distortion_power = np.mean(D)
            lam_exp = lam[:, None, None]
            D_exp = D[:, None, None]

        else:
            lam_exp = np.ones((self.N,1,1))
            D_exp = np.zeros((self.N,1,1))

        D_scalar = D_exp[:, 0, 0]

        K_rb = np.sum(A, axis=2)

        if self.use_sm:
            array_gain = np.maximum(self.Nt - K_rb, 1)
            array_gain_exp = array_gain[:, :, None]

            P_user = np.zeros((self.N, self.M, self.U))

            for n in range(self.N):
                for m in range(self.M):
                    users = np.where(A[n, m] == 1)[0]
                    K = users.size
                    if K == 0:
                        continue
                    P_tot = self.pmat[n, m]
                    uu_arr = users
                    P_user[n, m, users] = P_tot / (K + 1e-12)
        else:
            array_gain = self.Nt - 1
            array_gain_exp = array_gain * np.ones((self.N, self.M, 1))
            P_user = self.pmat[:, :, None] * A

        gamm = (array_gain_exp * lam_exp * P_user * beta_eff) / \
           (I + self.No + beta_eff * D_exp + 1e-12)
        F = self.Bw * np.log2(1 + gamm)
 
        # --------------------------------------------------
        # System throughput
        # --------------------------------------------------
        total_rate = np.sum(A * F)
        total_power = np.sum(self.pmat)

        F_curr = total_rate /  1e6
        self.ep_rate = F_curr

        self.ep_ee = F_curr / (total_power + 1e-12)
        
        self.ep_ibo = np.mean(self.P_pa / (P_avg_ant + 1e-12))
        # ======================================================
        # Global features
        # ======================================================
        global_features = np.array([
            self.U / self.U_max,
            self.P_pa / max(self.P_PA_LIST),
            self.K_active / self.Kmax,
        ], dtype=np.float32)
        
        # ======================================================
        # RU features
        # ======================================================
        P_ru = np.sum(self.pmat, axis=1)
        ru_features = P_ru / (self.Nt * self.P_pa + 1e-12)
        
        # ======================================================
        # RB features
        # ======================================================
        sinr_db = pow2db(gamm)
        
        cqi_full = np.searchsorted(CQI_TABLE, sinr_db, side="right") + 1
        cqi_full = np.clip(cqi_full, 1, 15)
        
        avg_cqi = np.zeros((self.N, self.M), dtype=np.float32)
        
        for n in range(self.N):
            for m in range(self.M):
        
                users = np.where(A[n, m] == 1)[0]
        
                if len(users) > 0:
                    avg_cqi[n, m] = np.mean(cqi_full[n, m, users])
        
        avg_cqi /= 15.0
        
        rb_power = self.pmat / (self.Nt * self.P_pa + 1e-12)
        
        rb_loading = K_rb / self.Kmax
        
        rb_features = np.stack(
            [
                rb_power,
                avg_cqi,
                rb_loading,
            ],
            axis=2,
        )
        
        # ======================================================
        # Final state
        # ======================================================
        state = np.concatenate([
            global_features,
            ru_features,
            rb_features.reshape(-1)
        ]).astype(np.float32)
               

        if return_DRM:
            return state, F_curr

        return state

    def compute_association(self, beta):
        A = np.zeros((self.N, self.M, self.U))

        # ---- Step 1: UE → best RU ----
        best_ru = np.argmax(beta, axis=0)  # shape (U,)

        # ---- Step 2: RB assignment within each RU ----
        for n in range(self.N):

            ue_list = np.where(best_ru == n)[0]

            # sort by channel strength
            ue_order = ue_list[np.argsort(-beta[n, ue_list])]

            ptr = 0
            for m in range(self.M):

                max_users = self.K_active if self.use_sm else 1

                for _ in range(max_users):
                    if ptr >= len(ue_order):
                        break

                    u = ue_order[ptr]
                    ptr += 1

                    A[n, m, u] = 1

        return A

