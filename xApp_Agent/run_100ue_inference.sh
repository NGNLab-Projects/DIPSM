#!/bin/bash

# Script to launch the gNB with PyTorch Inference and UEs in noS1 mode

# Number of UEs (default to 2 if not provided)
NUM_UES=${1:-2}
echo "Configuring simulation for $NUM_UES UEs in noS1 mode..."

# 1. Clean up old logs and processes
rm -f /home/ngn-6g/OAI/openairinterface5g/*.log

# 2. Launch Telemetry Collector
echo "Launching Telemetry Collector..."
python3 /home/ngn-6g/OAI/openairinterface5g/collect_telemetry.py > /home/ngn-6g/OAI/openairinterface5g/telemetry.log 2>&1 &
TELEMETRY_PID=$!

# 3. Launch OAI gNB in rfsim Mode (noS1)
echo "Launching OAI gNB MAC Scheduler in noS1 Mode..."
cd /home/ngn-6g/OAI/openairinterface5g/cmake_targets/ran_build/build
./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.24PRB.usrpb210.conf --gNBs.[0].min_rxtxtime 6 --rfsim --telnetsrv --noS1 > /home/ngn-6g/OAI/openairinterface5g/gnb.log 2>&1 &
GNB_PID=$!

# 4. Launch UEs via rfsim (noS1)
echo "Launching $NUM_UES UEs..."
UE_PIDS=""
for i in $(seq 1 $NUM_UES); do
    IMSI=$(printf "00101%010d" $i)
    ./nr-uesoftmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/ue.conf -r 24 --numerology 1 --band 78 -C 3604800000 --ssb 24 --rfsim --noS1 --uicc0.imsi $IMSI > /home/ngn-6g/OAI/openairinterface5g/ue_${i}.log 2>&1 &
    UE_PIDS="$UE_PIDS $!"
    sleep 6
done

echo "Simulation running in noS1 mode with automatic full-buffer traffic."
echo "Press Ctrl+C to stop the simulation and write the final plot."

# Cleanup trap
trap "echo 'Stopping processes...'; kill -9 $GNB_PID $UE_PIDS $TELEMETRY_PID; killall nr-softmodem nr-uesoftmodem python3; exit 0" SIGINT SIGTERM

# Stream the ENTIRE gNB log live to the terminal
tail -n +1 -f /home/ngn-6g/OAI/openairinterface5g/gnb.log
