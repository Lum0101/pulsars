import matplotlib.pyplot as plt
import csv

x, y = [], []
with open("signal.csv") as f:
    for row in csv.reader(f):
        x.append(int(row[0]))
        y.append(float(row[1]))

plt.plot(x, y)
plt.show()
