import subprocess
import os
import numpy as np
import matplotlib.pyplot as plt

os.makedirs("figures", exist_ok=True)

# Processing chain: each program reads the CSV written by the previous one
PROGRAMS = ["generate", "dedispersion", "dft"]

def run_pipeline(noise):
    """Compile with NOISE_AMPLITUDE=noise, run the chain, return magnitudes.csv."""
    for p in PROGRAMS:
        subprocess.run(
            ["gcc", "-O2", f"-DNOISE_AMPLITUDE={noise}", f"{p}.c", "-o", p, "-lm"],
            check=True)                                 # check=True: stop if the compilation fails
        subprocess.run([f"./{p}"], check=True)
    return np.loadtxt("magnitudes.csv", delimiter=",", skiprows=1)

def snr(spec):
    """Provisional S/N: maximum peak / median of the spectrum (to be refined later)."""
    mag = spec[1:, 1]                                   # Skip the DC bin
    return mag.max() / np.median(mag)

noises = np.logspace(-1, 3, 12)                         # sigma from 0.1 to 1000
results = []
for a in noises:
    s = snr(run_pipeline(a))
    results.append(s)
    #print(f"sigma = {a:8.3f}   S/N = {s:8.2f}")

# Save the scan so that it can be re-plotted without recomputing
np.savetxt("noise_scan.csv", np.column_stack([noises, results]),
           delimiter=",", header="noise,snr", comments="")

# Log-log plot: S/N ~ 1/sigma is a straight line of slope -1
plt.loglog(noises, results, "o-")
plt.xlabel("NOISE_AMPLITUDE (σ)")
plt.ylabel("S/N")
plt.grid(True, which="both", alpha=0.3)
plt.savefig("result/noise_scan.png")
plt.show()