import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("magnitudes.csv")


plt.plot(df["frec"]df)
plt.xlabel("frec")
plt.ylabel("magnitude")
plt.title("DFT spectrum")
plt.show()
