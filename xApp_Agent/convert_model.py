import torch
import torch.nn as nn

class RealAgent(nn.Module):
    def __init__(self, state_dim):
        super(RealAgent, self).__init__()
        self.feature = nn.Sequential(
            nn.Linear(state_dim, 256),
            nn.ReLU(),
            nn.Linear(256, 256),
            nn.ReLU(),
            nn.Linear(256, 128),
            nn.ReLU()
        )
        self.value = nn.Sequential(
            nn.Linear(128, 64),
            nn.ReLU(),
            nn.Linear(64, 1)
        )
        self.advantage_power = nn.Sequential(
            nn.Linear(128, 64),
            nn.ReLU(),
            nn.Linear(64, 72)
        )
        self.advantage_k = nn.Sequential(
            nn.Linear(128, 64),
            nn.ReLU(),
            nn.Linear(64, 4)
        )

    def forward(self, x):
        feat = self.feature(x)
        val = self.value(feat)
        adv_p = self.advantage_power(feat)
        adv_k = self.advantage_k(feat)
        
        q_power = val + (adv_p - adv_p.mean(dim=-1, keepdim=True))
        q_k = val + (adv_k - adv_k.mean(dim=-1, keepdim=True))
        
        return torch.cat([q_power, q_k], dim=-1)

def convert(pth_file, pt_file, state_dim):
    print(f"Loading {pth_file} (state_dim={state_dim})...")
    model = RealAgent(state_dim)
    data = torch.load(pth_file, map_location='cpu', weights_only=False)
    
    # Handle potentially different key names in state dict
    if 'qnet_state_dict' in data:
        model.load_state_dict(data['qnet_state_dict'])
    elif 'qnet' in data:
        model.load_state_dict(data['qnet'])
    else:
        model.load_state_dict(data)
        
    model.eval()

    example_input = torch.randn(1, state_dim)
    print("Tracing model...")
    traced_script_module = torch.jit.trace(model, example_input)
    traced_script_module.save(pt_file)
    print(f"Successfully saved traced TorchScript model to {pt_file}\n")

if __name__ == "__main__":
    # Case 1: Distortion Aware (102 dims)
    convert('A1_D_SM_Ppa_ckpt.pth', 'agent_aware.pt', 102)
    
    # Case 2: Distortion Unaware (101 dims)
    convert('Unaware4.pth', 'agent_unaware4.pt', 101)
