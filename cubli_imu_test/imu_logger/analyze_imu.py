# analyze_imu.py
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("imu_log.csv")
df["t"] = (df["device_t_ms"] - df["device_t_ms"].iloc[0]) / 1000.0  # seconds since first sample

fig, axes = plt.subplots(2, 1, sharex=True, figsize=(10, 6))

for col in ["ax", "ay", "az"]:
    axes[0].plot(df["t"], df[col], label=col)
axes[0].set_ylabel("Accel (m/s²)")
axes[0].legend()
axes[0].grid(True)

for col in ["gx", "gy", "gz"]:
    axes[1].plot(df["t"], df[col], label=col)
axes[1].set_ylabel("Gyro (rad/s)")
axes[1].set_xlabel("Time (s)")
axes[1].legend()
axes[1].grid(True)

plt.tight_layout()
plt.show()