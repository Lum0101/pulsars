import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("./magnitudes.csv")

plt.plot(df["freq"], df["magnitudes"])
plt.xlabel("frequency (Hz))")
plt.ylabel("magnitude")
plt.title("DFT spectrum")
plt.show()
