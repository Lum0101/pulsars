import os
import pandas as pd
import matplotlib.pyplot as plt

# Output folder for the figures (created if missing)
os.makedirs("result", exist_ok=True)

# Load the multi-channel signal (columns: channel, i, amplitude)
df = pd.read_csv("signal.csv", dtype={"channel": int, "i": int, "amplitude": float})

# Reshape into a 2D grid: one row per channel, one column per time sample
pivot = df.pivot(index="channel", columns="i", values="amplitude")

# Dynamic spectrum: the dispersion appears as a curved track across channels
fig, ax = plt.subplots(figsize=(10, 5))
im = ax.imshow(pivot.values, aspect="auto", origin="lower",
               cmap="viridis", interpolation="nearest")

# Labels and title
ax.set_xlabel("i (time sample)")
ax.set_ylabel("Channel")
ax.set_title("Dispersed signal (dynamic spectrum)")

fig.colorbar(im, ax=ax, label="Amplitude")
fig.tight_layout()   # Avoid clipped labels

# Save before show(), otherwise the saved image can be blank
fig.savefig("result/dynamic_spectrum.png", dpi=200)
plt.show()