import os
import pandas as pd
import matplotlib.pyplot as plt

# Output folder for the figures (created if missing)
os.makedirs("result", exist_ok=True)

# Load the DFT spectrum (columns: freq, magnitudes)
df = pd.read_csv("magnitudes.csv")

fig, ax = plt.subplots(figsize=(10, 4))

# Magnitude as a function of frequency
ax.plot(df["freq"], df["magnitudes"], linewidth=0.8)

# Labels and title
ax.set_xlabel("Frequency (Hz)")
ax.set_ylabel("Magnitude")
ax.set_title("DFT spectrum")

# Light grid to read the harmonics more easily
ax.grid(alpha=0.3)

fig.tight_layout()   # Avoid clipped labels

# Save before show(), otherwise the saved image can be blank
fig.savefig("result/dft_spectrum.png", dpi=200)
plt.show()