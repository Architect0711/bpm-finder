import numpy as np
import os

# Path relative to the script file
script_dir = os.path.dirname(os.path.abspath(__file__))
bin_path = os.path.join(script_dir, "../../cmake-build-debug/waveform.bin")
data = np.fromfile(bin_path, dtype=np.float32)
print(data)
print(f"Number of samples: {len(data)}")
