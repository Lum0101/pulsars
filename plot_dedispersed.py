import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("dedispersed.csv")

plt.plot(df["i"], df["amplitude"])
plt.xlabel("i (tie)")
plt.ylabel("Ampitude")
plt.title("Dedispersed signal")
plt.show()
