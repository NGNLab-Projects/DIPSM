#!/usr/bin/env python3
"""
plot_full_session.py — Plot the complete throughput and RU power history from a saved CSV log.
Usage:  python3 plot_full_session.py [logfile.csv]
        If no file given, uses the most recent throughput_log_*.csv in the current directory.
"""

import sys
import os
import glob
import csv
import matplotlib.pyplot as plt
import numpy as np

# ── Find log file ──────────────────────────────────────────────────────────────
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
if len(sys.argv) > 1:
    log_file = sys.argv[1]
else:
    files = sorted(glob.glob(os.path.join(SCRIPT_DIR, "throughput_log_*.csv")))
    if not files:
        print(f"No throughput log files found in {SCRIPT_DIR}. Run plot_throughput.py first.")
        sys.exit(1)
    log_file = files[-1]
    print(f"Using most recent log: {log_file}")

# ── Load data ──────────────────────────────────────────────────────────────────
times, tputs, ues = [], [], []
ru0_p, ru1_p, ru2_p = [], [], []

with open(log_file, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        try:
            t = float(row['time_s'])
            v = float(row['tput_mbps'])
            u = int(row['num_ues']) if row.get('num_ues') else 0
            times.append(t)
            tputs.append(v)
            ues.append(u)
            
            # Read RU powers if present, otherwise default to 0.0
            ru0_p.append(float(row['ru0_power']) if row.get('ru0_power') else 0.0)
            ru1_p.append(float(row['ru1_power']) if row.get('ru1_power') else 0.0)
            ru2_p.append(float(row['ru2_power']) if row.get('ru2_power') else 0.0)
        except (ValueError, KeyError):
            continue

if not times:
    print("Log file is empty.")
    sys.exit(1)

times = np.array(times)
tputs = np.array(tputs)
ru0_p = np.array(ru0_p)
ru1_p = np.array(ru1_p)
ru2_p = np.array(ru2_p)

n_pts  = len(times)
dur    = times[-1] - times[0]
avg    = np.mean(tputs)
peak   = np.max(tputs)
num_ue = max(ues) if any(u > 0 for u in ues) else "N/A"

print(f"Duration   : {dur:.1f} s  ({n_pts} samples)")
print(f"Avg tput   : {avg:.2f} Mbps")
print(f"Peak tput  : {peak:.2f} Mbps")
print(f"Max UEs    : {num_ue}")
if len(ru0_p) > 0:
    print(f"Mean Powers: RU0={np.mean(ru0_p):.2f}W, RU1={np.mean(ru1_p):.2f}W, RU2={np.mean(ru2_p):.2f}W")

# ── Plot Setup (Single Panel: Throughput Timeline) ────────────────
fig, ax1 = plt.subplots(1, 1, figsize=(14, 6))
fig.patch.set_facecolor('#1e1e1e')

# Panel 1: Throughput Timeline
ax1.set_facecolor('black')
ax1.plot(times, tputs, color='cyan', lw=1.2, label='Aggregate DL Throughput')

# 200-step convergence window markers (every ~20s at 10-frame intervals)
episode_len_s = 20.0
for ep_start in np.arange(0, dur, episode_len_s):
    ax1.axvline(x=ep_start, color='yellow', alpha=0.25, lw=0.8, linestyle='--')

# Rolling 10-sample mean
if n_pts >= 10:
    window = 10
    rolling_mean = np.convolve(tputs, np.ones(window)/window, mode='valid')
    rolling_times = times[window-1:]
    ax1.plot(rolling_times, rolling_mean, color='orange', lw=2, label=f'{window}-sample rolling avg', alpha=0.85)

ax1.axhline(y=avg, color='red', lw=1, linestyle=':', alpha=0.7, label=f'Session avg: {avg:.2f} Mbps')
ax1.set_xlim(times[0], times[-1])
ax1.set_ylim(0, peak * 1.15)
ax1.set_xlabel("Time (seconds)", color='white')
ax1.set_ylabel("Throughput (Mbps)", color='white')
ax1.set_title(f"OAI DRL Agent — Full Session Telemetry Analysis\n"
              f"File: {os.path.basename(log_file)} | "
              f"{n_pts} samples | Duration: {dur:.0f}s | Peak: {peak:.1f} Mbps | Avg: {avg:.1f} Mbps",
              color='white', fontsize=11)
ax1.tick_params(colors='white')
ax1.spines['bottom'].set_color('gray')
ax1.spines['left'].set_color('gray')
ax1.spines['top'].set_color('#1e1e1e')
ax1.spines['right'].set_color('#1e1e1e')
# ax1.legend(facecolor='#333', edgecolor='gray', labelcolor='white', fontsize=9)
ax1.grid(color='gray', linestyle='--', linewidth=0.4, alpha=0.5)

plt.tight_layout()

# Save PNG next to the CSV
png_path = log_file.replace('.csv', '.png')
plt.savefig(png_path, dpi=150, bbox_inches='tight', facecolor='#1e1e1e')
print(f"Plot saved to: {png_path}")
plt.show()
