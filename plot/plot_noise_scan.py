import subprocess
import os
import numpy as np
import matplotlib.pyplot as plt

os.makedirs("figures", exist_ok=True)

PROGRAMS = ["generate", "dedispersion", "dft"]

def run_pipeline(noise):
    """Compile avec NOISE_AMPLITUDE=noise, exécute la chaîne, renvoie magnitudes.csv."""
    for p in PROGRAMS:
        subprocess.run(
            ["gcc", "-O2", f"-DNOISE_AMPLITUDE={noise}", f"{p}.c", "-o", p, "-lm"],
            check=True)                                 # check=True : stoppe si la compilation échoue
        subprocess.run([f"./{p}"], check=True)
    return np.loadtxt("magnitudes.csv", delimiter=",", skiprows=1)

def snr(spec):
    """S/N provisoire : pic maximal / médiane du spectre (à raffiner plus tard)."""
    mag = spec[1:, 1]                                   #
    return mag.max() / np.median(mag)

noises = np.logspace(-1, 3, 12)                         # 0.1 to 1000
results = []
for a in noises:
    s = snr(run_pipeline(a))
    results.append(s)
    #print(f"sigma = {a:8.3f}   S/N = {s:8.2f}")

np.savetxt("noise_scan.csv", np.column_stack([noises, results]),
           delimiter=",", header="noise,snr", comments="")

plt.loglog(noises, results, "o-")
plt.xlabel("NOISE_AMPLITUDE (σ)")
plt.ylabel("S/N")
plt.grid(True, which="both", alpha=0.3)
plt.savefig("result/noise_scan.png")
plt.show()