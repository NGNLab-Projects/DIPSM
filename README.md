# Power Allocation O-RAN xApp (DRL-based)

This repository contains a Deep Reinforcement Learning (DRL) based xApp designed to perform distortion-aware power control within the OpenAirInterface (OAI) 5G framework. 

The xApp acts as an external intelligent agent that dynamically adjusts per-PRB transmit power based on real-time channel state telemetry, operating via a custom E2 Service Model integrated with FlexRIC.

## Repository Structure

Because this project modifies the core OAI MAC layer and the FlexRIC near-RT RIC, this repository is designed to be a patch/overlay on top of the official OAI repositories.

*   **`xApp_Agent/`**: Contains the Python-based DRL agent, environment wrappers, telemetry collection scripts, and plotting utilities.
*   **`fdpa_sm/`**: The custom Frequency Domain Power Allocation (FDPA) E2 Service Model (E2SM). Contains the C code for both the RIC and the gNB agent.
*   **`custom_mac/`**: Modified OAI MAC layer files (C/C++) responsible for real-time inference, CQI UDP listening, and interacting with the `libtorch` backend. Includes modified downlink scheduler files (`gNB_scheduler*.c`, `nr_mac_gNB.h`).
*   **`configs/`**: Custom `.conf` configuration files for running the OAI RF Simulator with multiple UEs.
*   **`patches/`**: Contains `my_full_diff.patch` for quickly applying all OAI core modifications at once.
*   **`install_dependencies.sh`**: Helper script to download the required C++ PyTorch backend.

## Prerequisites

1.  **Ubuntu 22.04 LTS**
3.  **[OpenAirInterface5g](https://gitlab.eurecom.fr/oai/openairinterface5g)**: Clone and build the official OAI `develop` branch.
4.  **[FlexRIC](https://gitlab.eurecom.fr/mosaic5g/flexric)**: Clone and build the official FlexRIC `develop` branch.
5.  **LibTorch**: The OAI MAC layer requires `libtorch` to run the ONNX/PyTorch models natively in C++.

### Installing LibTorch
The heavy `libtorch` binary is not included. Instead. Run the provided script inside your OAI directory:

```bash
chmod +x install_dependencies.sh
./install_dependencies.sh
```
This will download and extract the required `libtorch` version to `./libtorch/`.

## Deployment Instructions

### 1. Apply the OAI MAC Modifications
You can either apply the patch file directly to your OAI clone, or manually copy the C/C++ files.

**Using the patch:**
```bash
cd /path/to/your/openairinterface5g
git apply /path/to/Power-Allocation_O-RAN_xApp/patches/my_full_diff.patch
```

**Manual Copy:**
Copy the MAC inference and listener files:
```bash
cp custom_mac/torch_infer.* /path/to/your/openairinterface5g/openair2/LAYER2/MAC/
cp custom_mac/cqi_udp_listener.* /path/to/your/openairinterface5g/openair2/LAYER2/MAC/
```

Copy the modified downlink scheduler files:
```bash
cp custom_mac/gNB_scheduler* /path/to/your/openairinterface5g/openair2/LAYER2/NR_MAC_gNB/
cp custom_mac/nr_mac_gNB.h /path/to/your/openairinterface5g/openair2/LAYER2/NR_MAC_gNB/
```

### 2. Install the Custom FDPA Service Model
The custom E2 Service Model must be compiled into FlexRIC.
1. Copy the contents of `fdpa_sm/src/` to your FlexRIC source folder: `flexric/src/sm/fdpa_sm/`
2. Copy the contents of `fdpa_sm/test/` to your FlexRIC test folder: `flexric/test/sm/fdpa_sm/`
3. Re-compile FlexRIC following the official Mosaic5G build instructions.

### 3. Pre-trained Weights
Place your trained `.pt` or `.pth` model weights inside the `xApp_Agent/` directory before running inference. *(Note: Pre-trained weights are ignored by Git due to file size restrictions. Download them from the designated cloud storage link if provided).*

## Running the Simulation

Use the provided shell script to start the inference simulation in OAI:
```bash
cd xApp_Agent
./run_100ue_inference.sh
```

## Citation
If you use this repository in your research, please cite the corresponding paper or the NGNLab project.
