import socket
import struct
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
import threading
import csv
import os
import time
from datetime import datetime

# Configuration
UDP_IP = "127.0.0.1"
UDP_PORT = 9998
HISTORY_LEN = 200  # live window: show last 200 points (~20 seconds)

# Log file — saves the ENTIRE run for post-hoc plotting
LOG_DIR = os.path.dirname(os.path.abspath(__file__))
LOG_FILE = os.path.join(LOG_DIR, f"throughput_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv")

# Data storage
tput_history = deque([0.0] * HISTORY_LEN, maxlen=HISTORY_LEN)
ru0_history = deque([0.0] * HISTORY_LEN, maxlen=HISTORY_LEN)
ru1_history = deque([0.0] * HISTORY_LEN, maxlen=HISTORY_LEN)
ru2_history = deque([0.0] * HISTORY_LEN, maxlen=HISTORY_LEN)

all_times = []       # full session timestamps (seconds since start)
all_tputs = []       # full session throughput values
start_time = time.time()
log_lock = threading.Lock()

import glob

def csv_listener():
    # Find the most recent CSV file
    files = sorted(glob.glob(os.path.join(LOG_DIR, "throughput_log_*.csv")))
    if not files:
        print("No telemetry CSV found! Start the simulation first.")
        return
    
    log_file = files[-1]
    print(f"Tailing live telemetry from: {log_file}")
    
    with open(log_file, 'r') as f:
        # Skip header
        f.readline()
        while True:
            line = f.readline()
            if not line:
                time.sleep(0.1)
                continue
            
            row = line.strip().split(',')
            if len(row) < 6:
                continue
            try:
                tput_mbps = float(row[1])
                tput_history.append(tput_mbps)
            except ValueError:
                pass

# Start background listener thread
listener_thread = threading.Thread(target=csv_listener, daemon=True)
listener_thread.start()

# Setup Live Plot (Single Panel)
fig, ax1 = plt.subplots(1, 1, figsize=(13, 6))
fig.patch.set_facecolor('#1e1e1e')

# Subplot: Throughput
line_tput, = ax1.plot(list(tput_history), color='cyan', lw=2, label='Throughput')
ax1.set_facecolor('black')
ax1.tick_params(colors='white')
ax1.spines['bottom'].set_color('gray')
ax1.spines['left'].set_color('gray')
ax1.spines['top'].set_color('#1e1e1e')
ax1.spines['right'].set_color('#1e1e1e')
ax1.set_xlim(0, HISTORY_LEN)
ax1.set_ylim(0, 10)
ax1.set_title("OAI MAC Live DL Aggregate Throughput (DRL Agent)", color='white', fontsize=12)
ax1.set_ylabel("Aggregate Throughput (Mbps)", color='white')
ax1.set_xlabel(f"Time (0.1s ticks) — Full log saved to: {os.path.basename(LOG_FILE)}", color='#aaaaaa', fontsize=9)
ax1.grid(color='gray', linestyle='--', linewidth=0.5, alpha=0.5)

plt.tight_layout()


def update(frame):
    # Update Throughput Plot
    current_tput = list(tput_history)
    line_tput.set_ydata(current_tput)
    pos_tput = [v for v in current_tput if v > 0]
    if pos_tput:
        max_t = max(pos_tput)
        min_t = min(pos_tput)
        ax1.set_ylim(max(0.0, min_t * 0.8), max_t * 1.15)
        
    return line_tput,


ani = animation.FuncAnimation(fig, update, interval=100, blit=False, cache_frame_data=False)

try:
    plt.show()
finally:
    print("\nLive plotting session ended.")
