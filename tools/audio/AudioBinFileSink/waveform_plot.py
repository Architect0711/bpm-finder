import matplotlib.pyplot as plt
import numpy as np
import os

# Path relative to the script file
script_dir = os.path.dirname(os.path.abspath(__file__))
bin_path = os.path.join(script_dir, "../../../cmake-build-debug/waveform.bin")

# Read binary file containing raw 32-bit floats
samples = np.fromfile(bin_path, dtype=np.float32)

plt.plot(samples)
plt.xlabel("Sample")
plt.ylabel("Amplitude")
plt.title("Audio Waveform")
plt.show()
