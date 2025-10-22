import subprocess
import json

proc = subprocess.Popen(
    ['./PipelineResultInitialization', '44100', '1024', '40', '120', '1.0'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True,
    bufsize=1
)

# Wait for ready
status = json.loads(proc.stdout.readline())

# Send audio chunk
audio_data = {"samples": [0.1] * 1024}
proc.stdin.write(json.dumps(audio_data) + '\n')
proc.stdin.flush()

# Read result
result = json.loads(proc.stdout.readline())

# Stop
proc.stdin.write('stop\n')
proc.stdin.flush()
