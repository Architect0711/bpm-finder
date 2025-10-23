import subprocess
import json
import os
import sys

# Determine the executable path based on the platform
script_dir = os.path.dirname(os.path.realpath(__file__))
print(f"Script dir: {script_dir}")
repository_root = os.path.dirname(os.path.dirname(os.path.dirname(script_dir)))
print(f"Repository root: {repository_root}")

if sys.platform == 'win32':
    executable = os.path.join(repository_root, 'cmake-build-debug', 'tools', 'PipelineResultInitialization.exe')
else:
    executable = os.path.join(repository_root, 'cmake-build-debug', 'tools', 'PipelineResultInitialization')

print(f"Executable: {executable}")

proc = subprocess.Popen(
    [executable, '44100', '1024', '40', '120', '1.0'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True,
    bufsize=1
)

# Wait for ready signal
while True:
    status = proc.stdout.readline()
    if not status:
        print("Error: Process ended before ready signal")
        proc.terminate()
        sys.exit(1)
    print(status.strip())
    if "[LOG] ready" in status:
        break

# Send audio chunk
audio_data = {"rawAudio": [0.1] * 1024}
proc.stdin.write(json.dumps(audio_data) + '\n')
proc.stdin.flush()

while True:
    status = proc.stdout.readline()
    if not status:
        print("Error: Process ended before process returned data")
        proc.terminate()
        sys.exit(1)
    print(status.strip())
    if "[LOG] exit" in status:
        break
result = json.loads(proc.stdout.readline())

# Stop
proc.stdin.write('exit\n')
proc.stdin.flush()
