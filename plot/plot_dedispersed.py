import os
import pandas as pd
import matplotlib.pyplot as plt

# Output folder for the figures (created if missing)
os.makedirs("result", exist_ok=True)

# Load the dedispersed signal (columns: i, amplitude)
df = pd.read_csv("dedispersed.csv")

fig, ax = plt.subplots(figsize=(10, 4))

# Amplitude as a function of the time sample index
ax.plot(df["i"], df["amplitude"], linewidth=0.8)

# Labels and title
ax.set_xlabel("i (time sample)")
ax.set_ylabel("Amplitude")
ax.set_title("Dedispersed signal")

# Light grid to read the pulse positions more easily
ax.grid(alpha=0.3)

fig.tight_layout()   # Avoid clipped labels

# Save before show(), otherwise the saved image can be blank
fig.savefig("result/dedispersed.png", dpi=200)
plt.show()