import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import random
from collections import deque


# ==========================================================
# Dueling Network
# ==========================================================
class DuelingNet(nn.Module):
    def __init__(
        self,
        state_dim,
        num_rbs,
        num_rus=3,
        rbs_per_ru=8,
        rb_feat_dim=3,
        embed_dim=64,
    ):

        super().__init__()

        self.num_rbs = num_rbs
        self.num_rus = num_rus
        self.rbs_per_ru = rbs_per_ru

        self.rb_feat_dim = rb_feat_dim

        self.embed_dim = embed_dim

        self.rb_encoder = nn.Sequential(
            nn.Linear(self.rb_feat_dim, self.embed_dim),
            nn.ReLU(),
            nn.Linear(self.embed_dim, self.embed_dim),
            nn.ReLU(),
        )

        self.ru_power_encoder = nn.Sequential(
            nn.Linear(1, self.embed_dim),
            nn.ReLU(),
        )

        self.ru_encoder = nn.Sequential(
            nn.Linear(2*self.embed_dim, self.embed_dim),
            nn.ReLU(),
            nn.Linear(self.embed_dim, self.embed_dim),
            nn.ReLU(),
        )

        self.global_encoder = nn.Sequential(
            nn.Linear(3, self.embed_dim),
            nn.ReLU(),
            nn.Linear(self.embed_dim, self.embed_dim),
            nn.ReLU(),
        )
        joint_dim = (self.num_rus + 2) * self.embed_dim

        self.power_head = nn.Sequential(
            nn.Linear(3 * self.embed_dim, 128),
            nn.ReLU(),
            nn.Linear(128, 64),
            nn.ReLU(),
            nn.Linear(64, 3),
        )

        self.k_head = nn.Sequential(
            nn.Linear(joint_dim, 256),
            nn.ReLU(),
            nn.Linear(256, 128),
            nn.ReLU(),
            nn.Linear(128, 4),
        )

        self.value_power = nn.Sequential(
            nn.Linear(joint_dim, 256),
            nn.ReLU(),
            nn.Linear(256, 128),
            nn.ReLU(),
            nn.Linear(128, 1),
        )
        
        self.value_k = nn.Sequential(
            nn.Linear(joint_dim, 256),
            nn.ReLU(),
            nn.Linear(256, 128),
            nn.ReLU(),
            nn.Linear(128, 1),
        )

    def forward(self, x):
        global_feat, ru_feat, rb_feat = self.parse_state(x)
        global_emb = self.global_encoder(global_feat)
        rb_emb = self.rb_encoder(rb_feat)
    
        rb_summary = rb_emb.mean(dim=2)
        ru_power_emb = self.ru_power_encoder(ru_feat)
        ru_input = torch.cat((rb_summary, ru_power_emb), dim=-1)
        ru_emb = self.ru_encoder(ru_input)
        ru_emb = ru_emb + rb_summary
    
        ru_expand = ru_emb.unsqueeze(2).expand(-1, self.num_rus, self.rbs_per_ru, -1)
        global_expand = global_emb.unsqueeze(1).unsqueeze(2).expand(-1, self.num_rus, self.rbs_per_ru, -1)
        power_input = torch.cat((rb_emb, ru_expand, global_expand), dim=-1)
        q_power = self.power_head(power_input)
    
        rb_global = rb_emb.mean(dim=(1, 2))
        ru_global = ru_emb.reshape(-1, self.num_rus * self.embed_dim)
        joint_emb = torch.cat((ru_global, global_emb, rb_global), dim=-1)
    
        v_power = self.value_power(joint_emb)
        v_k = self.value_k(joint_emb)
        q_k = self.k_head(joint_emb)
    
        q_power = q_power.reshape(-1, self.num_rbs, 3)
        q_power = q_power - q_power.mean(dim=2, keepdim=True)
        q_power = v_power.unsqueeze(1) + q_power
    
        q_k = q_k - q_k.mean(dim=1, keepdim=True)
        q_k = v_k + q_k

        return q_power, q_k
       
    def parse_state(self, x):
        B = x.size(0)  
        idx = 0    
        # --------------------------
        # Global features
        # --------------------------
        global_feat = x[:, idx:idx+3]
        idx += 3    
        # --------------------------
        # RU features
        # --------------------------
        ru_feat = x[:, idx:idx+self.num_rus]
        ru_feat = ru_feat.unsqueeze(-1)
        idx += self.num_rus   
        # --------------------------
        # RB features
        # --------------------------
        rb_feat = x[:, idx:]
    
        rb_feat = rb_feat.reshape(
            B,
            self.num_rus,
            self.rbs_per_ru,
            self.rb_feat_dim
        )
    
        return global_feat, ru_feat, rb_feat


# ==========================================================
# Agent
# ==========================================================
class Agent1:

    def __init__(self, state_dim, num_rbs, device="cpu"):

        self.device = device
        self.num_rbs = num_rbs
        # Learning
        self.gamma = 0.5
        self.lr = 3e-4

        # Epsilon
        self.epsilon = 1.0
        self.epsilon_min = 0.05
        self.epsilon_decay = 0.9995

        # Target network
        self.tau = 0.005
        self.step_count = 0

        # Replay buffer
        self.buffer_size = 200000
        self.batch_size = 64
        self.buffer = deque(maxlen=self.buffer_size)

        # Networks
        self.qnet = DuelingNet(state_dim, num_rbs).to(device)
        self.qtarget = DuelingNet(state_dim, num_rbs).to(device)
        self.qtarget.load_state_dict(self.qnet.state_dict())

        self.optimizer = optim.AdamW(self.qnet.parameters(),lr=self.lr,weight_decay=1e-5)
        self.last_loss = 0.0
        self.last_loss_power = 0.0
        self.last_loss_k = 0.0

    # ======================================================
    # Greedy action (returns 0/1/2)
    # ======================================================
    def get_action(self, state):

        s = torch.tensor(state, dtype=torch.float32, device=self.device).unsqueeze(0)

        with torch.no_grad():
            Qp, Qk = self.qnet(s)
        
        power_action = Qp.argmax(dim=2).squeeze(0).cpu().numpy()
        
        k_action = Qk.argmax(dim=1).item()
        
        action = np.concatenate([
            power_action,
            np.array([k_action])
        ])
        
        return action

    # ======================================================
    # Epsilon-greedy exploration
    # ======================================================
    def get_action_explore(self, state):

        if np.random.rand() < self.epsilon:
            power_action = np.random.randint(0, 3, size=self.num_rbs)
            k_action = np.random.randint(0, 4, size=1)        
            return np.concatenate([power_action, k_action])

        return self.get_action(state)

    # ======================================================
    # Store transition
    # ======================================================
    def store(self, s, a, r, sp, done):

        self.buffer.append((
            np.asarray(s, dtype=np.float32),
            np.asarray(a, dtype=np.int64),
            float(r),
            np.asarray(sp, dtype=np.float32),
            float(done)
        ))

    # ======================================================
    # Training step
    # ======================================================
    def train_step(self):

        if len(self.buffer) < self.batch_size:
            return

        batch = random.sample(self.buffer, self.batch_size)
        S, A, R, Sp, D = zip(*batch)

        S  = torch.tensor(np.array(S), dtype=torch.float32, device=self.device)
        Sp = torch.tensor(np.array(Sp), dtype=torch.float32, device=self.device)
        A  = torch.tensor(np.array(A),  dtype=torch.long,    device=self.device)
        R  = torch.tensor(np.array(R),  dtype=torch.float32, device=self.device)
        D  = torch.tensor(np.array(D),  dtype=torch.float32, device=self.device)

        # ---------------------------
        # Compute target
        # ---------------------------
        with torch.no_grad():
            Qp_online, Qk_online = self.qnet(Sp)

            best_power = Qp_online.argmax(dim=2, keepdim=True)
            best_k = Qk_online.argmax(dim=1, keepdim=True)
            
            Qp_target, Qk_target = self.qtarget(Sp)
            
            maxQp = Qp_target.gather(2, best_power).squeeze(2)
            
            maxQk = Qk_target.gather(1, best_k)
            
            target_power = (R.unsqueeze(1)+ self.gamma * (1 - D.unsqueeze(1)) * maxQp)
            
            target_k = (R.unsqueeze(1) + self.gamma * (1 - D.unsqueeze(1)) * maxQk)

        # ---------------------------
        # Current Q-values
        # ---------------------------
        Qp_pred, Qk_pred = self.qnet(S)

        Ap = A[:, :self.num_rbs]
        Ak = A[:, -1].unsqueeze(1)
        
        Qp_chosen = Qp_pred.gather(2, Ap.unsqueeze(2)).squeeze(2)
        
        Qk_chosen = Qk_pred.gather(1, Ak)

        # ---------------------------
        # Loss
        # ---------------------------
        loss_power = nn.SmoothL1Loss()(Qp_chosen, target_power)
        loss_k = nn.SmoothL1Loss()(Qk_chosen, target_k)
        loss = loss_power + loss_k
        
        self.last_loss = loss.item()
        self.last_loss_power = loss_power.item()
        self.last_loss_k = loss_k.item()
        
        self.optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(self.qnet.parameters(), 10.0)
        self.optimizer.step()

        # ---------------------------
        # Target update
        # ---------------------------
        self.step_count += 1
        for target_param, param in zip(self.qtarget.parameters(), self.qnet.parameters()):
            target_param.data.copy_(
                self.tau * param.data + (1 - self.tau) * target_param.data
            )

    # ======================================================
    # Episode-level epsilon decay
    # ======================================================
    def end_episode(self):
        self.epsilon = max(self.epsilon_min,
                           self.epsilon * self.epsilon_decay)

    # ======================================================
    # Learning interface
    # ======================================================
    def learn(self, s, a, r, sp, done):

        self.store(s, a, r, sp, done)
        self.train_step()

        return self.get_action_explore(sp)


