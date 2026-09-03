#!/usr/bin/env python3
"""
plot_episode_throughput.py — Plot the end-of-episode downlink throughput across episodes.
An episode consists of exactly 200 steps (logged at 100ms intervals).
This script monitors the log file and plots the throughput at the 200th step of each episode.
If the simulation is running, the plot updates dynamically in real-time.
"""

import sys
import os
import glob
import csv
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from datetime import datetime

# ── Find log file ──────────────────────────────────────────────────────────────
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
if len(sys.argv) > 1:
    log_file = sys.argv[1]
else:
    files = sorted(glob.glob(os.path.join(SCRIPT_DIR, "throughput_log_*.csv")))
    if not files:
        print(f"No throughput log files found in {SCRIPT_DIR}. Please run the simulation first.")
        sys.exit(1)
    log_file = files[-1]
    print(f"Using log file: {log_file}")

# ── Setup Plot ─────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(10, 6))
fig.patch.set_facecolor('#1e1e1e')
ax.set_facecolor('black')
ax.tick_params(colors='white')
ax.spines['bottom'].set_color('gray')
ax.spines['left'].set_color('gray')
ax.spines['top'].set_color('#1e1e1e')
ax.spines['right'].set_color('#1e1e1e')
ax.grid(color='gray', linestyle='--', linewidth=0.5, alpha=0.5)

line, = ax.plot([], [], color='cyan', marker='o', lw=2, markersize=6, label='End-of-Episode Throughput')
ax.set_title("DRL Power Allocation: End-of-Episode DL Throughput Convergence", color='white', fontsize=12)
ax.set_xlabel("Episode Number", color='white')
ax.set_ylabel("Throughput at Step 200 (Mbps)", color='white')
ax.legend(facecolor='#333', edgecolor='gray', labelcolor='white')

# ── Real-Time Tracking Loop ────────────────────────────────────────────────────
episodes = []
end_of_episode_tputs = []

print("Monitoring log file for completed episodes (200 steps each)... Close window or press Ctrl+C to stop.")

def update(frame):
    global log_file, episodes, end_of_episode_tputs

    # Check if a newer log file exists
    if len(sys.argv) == 1:
        files = sorted(glob.glob(os.path.join(SCRIPT_DIR, "throughput_log_*.csv")))
        if files and files[-1] != log_file:
            print(f"[PLOTTER] Detected new log file: {files[-1]}. Switching...")
            log_file = files[-1]
            episodes = []
            end_of_episode_tputs = []
            line.set_xdata([])
            line.set_ydata([])
            ax.set_xlim(0.5, 10)
            ax.set_ylim(0, 10)

    if not os.path.exists(log_file):
        return line,

    # Check if the log file is active (modified in the last 5 seconds)
    try:
        file_mod_time = os.path.getmtime(log_file)
        if time.time() - file_mod_time > 5.0:
            if frame % 10 == 0:
                print(f"[TIP] Log file '{os.path.basename(log_file)}' has not been updated recently. "
                      f"Ensure that 'python3 plot_throughput.py' is running to capture live telemetry.")
    except Exception:
        pass

    tputs = []
    ru0s = []
    try:
        with open(log_file, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                try:
                    tput_val = float(row['tput_mbps'])
                    ru0_val = float(row['ru0_power'])
                    tputs.append(tput_val)
                    ru0s.append(ru0_val)
                except (ValueError, KeyError):
                    continue
    except Exception:
        return line,

    current_row_count = len(tputs)
    num_completed = current_row_count // 200
    
    if num_completed > len(episodes):
        new_episodes = list(range(len(episodes) + 1, num_completed + 1))
        for ep in new_episodes:
            # The end-of-episode throughput is the 200th step (index ep * 200 - 1)
            idx = ep * 200 - 1
            if idx < len(tputs):
                tput_at_end = tputs[idx]
                ru0_at_end = ru0s[idx]
                episodes.append(ep)
                end_of_episode_tputs.append(tput_at_end)
                print(f"[LOG] Episode {ep} completed. End-of-episode throughput: {tput_at_end:.2f} Mbps, RU0 Power: {ru0_at_end:.2f} W")

        # Update Plot Data
        line.set_xdata(episodes)
        line.set_ydata(end_of_episode_tputs)
        
        # Adjust plot limits dynamically
        ax.set_xlim(0.5, max(10, len(episodes) + 1))
        if end_of_episode_tputs:
            max_val = max(end_of_episode_tputs)
            min_val = min(end_of_episode_tputs)
            ax.set_ylim(max(0.0, min_val * 0.8), max_val * 1.2)
        
    return line,

ani = animation.FuncAnimation(fig, update, interval=1000, blit=False, cache_frame_data=False)
plt.show()
