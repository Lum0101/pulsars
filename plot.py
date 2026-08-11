import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

df = pd.read_csv("signal.csv")

# Reconstruire une grille 2D (canal x i) à partir du format long
pivot = df.pivot(index="canal", columns="i", values="amplitude")

plt.imshow(pivot.values, aspect="auto", origin="lower", cmap="viridis",
           extent=[pivot.columns.min(), pivot.columns.max(), pivot.index.min(), pivot.index.max()]
)
plt.xlabel("i (temps)")
plt.ylabel("canal")
plt.colorbar(label="amplitude")
plt.show()
