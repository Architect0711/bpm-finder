import matplotlib.pyplot as plt
import csv

import os

# Path relative to the script file
script_dir = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(script_dir, "../../cmake-build-debug/waveform.csv")

with open(csv_path) as f:
    lines = f.readlines()
    samples = [float(x.strip()) for x in lines]

plt.plot(samples)
plt.show()
