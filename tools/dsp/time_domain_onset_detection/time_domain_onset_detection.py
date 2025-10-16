import matplotlib.pyplot as plt
import numpy as np
import os

# Path relative to the script file
script_dir = os.path.dirname(os.path.abspath(__file__))
waveform_path = os.path.join(script_dir, "../../../cmake-build-debug/waveform.bin")
bandpass_path = os.path.join(script_dir, "../../../cmake-build-debug/bandpass.bin")

# Read binary files containing raw 32-bit floats
waveform_samples = np.fromfile(waveform_path, dtype=np.float32)
bandpass_samples = np.fromfile(bandpass_path, dtype=np.float32)

# Plot both waveforms
plt.figure(figsize=(12, 6))
plt.plot(waveform_samples, label="Original Waveform", color="blue", alpha=0.7)
plt.plot(bandpass_samples, label="Band-Pass Filtered", color="red", alpha=0.7)
plt.xlabel("Sample")
plt.ylabel("Amplitude")
plt.title("Audio Waveform Comparison: Original vs Band-Pass Filtered")
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()
