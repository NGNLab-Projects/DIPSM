import csv
import sys
import numpy as np

def analyze(log_file):
    print(f"--- Analyzing {log_file} ---")
    tputs = []
    ru0s = []
    try:
        with open(log_file, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                try:
                    tputs.append(float(row['tput_mbps']))
                    ru0s.append(float(row['ru0_power']))
                except (ValueError, KeyError):
                    continue
    except Exception as e:
        print(f"Error reading {log_file}: {e}")
        return

    ep_tputs = []
    ep_ru0s = []
    num_completed = len(tputs) // 200
    for ep in range(1, num_completed + 1):
        idx = ep * 200 - 1
        ep_tputs.append(tputs[idx])
        ep_ru0s.append(ru0s[idx])

    if len(ep_tputs) >= 50:
        regime1_tput = np.mean(ep_tputs[0:25])
        regime1_ru0 = np.mean(ep_ru0s[0:25])
        regime2_tput = np.mean(ep_tputs[25:50])
        regime2_ru0 = np.mean(ep_ru0s[25:50])
        print(f"Regime 1 (25mW, Eps 1-25): Avg Tput = {regime1_tput:.2f} Mbps, Avg RU0 Power = {regime1_ru0:.2f} W")
        print(f"Regime 2 (200mW, Eps 26-50): Avg Tput = {regime2_tput:.2f} Mbps, Avg RU0 Power = {regime2_ru0:.2f} W")
    else:
        print(f"Not enough episodes ({len(ep_tputs)}).")
    print()

analyze('/home/ngn-6g/OAI/openairinterface5g/throughput_log_20260706_105443.csv')
analyze('/home/ngn-6g/OAI/openairinterface5g/throughput_log_20260707_110519.csv')
