import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

df = pd.read_csv("signal.csv")

fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection="3d")

for canal in sorted(df["canal"].unique()):
    sub = df[df["canal"] == canal]
    ax.plot(sub["i"], [canal]*len(sub), sub["amplitude"], color="steelblue", linewidth=0.8)

ax.set_xlabel("i (temps)")
ax.set_ylabel("canal")
ax.set_zlabel("amplitude")
plt.show()
