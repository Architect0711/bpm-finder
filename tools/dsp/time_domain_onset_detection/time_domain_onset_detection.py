import matplotlib.pyplot as plt
import numpy as np
import os

# Configuration
CHUNK_SIZE = 1024

# Path relative to the script file
script_dir = os.path.dirname(os.path.abspath(__file__))
waveform_path = os.path.join(script_dir, "../../../cmake-build-debug/waveform.bin")
bandpass_path = os.path.join(script_dir, "../../../cmake-build-debug/bandpass.bin")
energy_path = os.path.join(script_dir, "../../../cmake-build-debug/energy.bin")

# Read binary files containing raw 32-bit floats
waveform_samples = np.fromfile(waveform_path, dtype=np.float32)
bandpass_samples = np.fromfile(bandpass_path, dtype=np.float32)
energy_samples = np.fromfile(energy_path, dtype=np.float32)

# Expand energy samples: repeat each value CHUNK_SIZE times
energy_expanded = np.repeat(energy_samples, CHUNK_SIZE)

print(
    f"Waveform samples: {len(waveform_samples)}, Bandpass samples: {len(bandpass_samples)}, Energy samples: {len(energy_samples)}, Energy × chunk_size: {len(energy_samples) * CHUNK_SIZE}, Expanded energy samples: {len(energy_expanded)}")

# Normalize energy to match the scale of the original waveform
max_waveform = np.max(np.abs(waveform_samples))
max_energy = np.max(energy_expanded)
energy_normalized = energy_expanded * (max_waveform / max_energy)

# Plot both waveforms
plt.figure(figsize=(12, 6))
plt.plot(waveform_samples, label=f"Original Waveform (n={len(waveform_samples)})", color="blue", alpha=0.7)
plt.plot(bandpass_samples, label=f"Band-Pass Filtered (n={len(bandpass_samples)})", color="red", alpha=0.7)
plt.plot(energy_normalized, label=f"Energy (Normalized) (n={len(energy_normalized)})", color="green", alpha=0.7)
plt.xlabel("Sample")
plt.ylabel("Amplitude")
plt.title("Audio Waveform Comparison: Original vs Band-Pass Filtered")
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()
