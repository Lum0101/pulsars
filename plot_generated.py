import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("signal.csv")
pivot = df.pivot(index="channel", columns="i", values="amplitude")

plt.imshow(pivot.values, aspect="auto", origin="lower", cmap="viridis", interpolation="nearest")
plt.xlabel("i (time)")
plt.ylabel("channel")
plt.colorbar(label="amplitude")
plt.show()
