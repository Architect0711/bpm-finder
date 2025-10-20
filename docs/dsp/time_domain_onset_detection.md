# Onset Detection in the Time Domain

This is the first and very naive approach to beat detection. I know that there are more sophisticated approaches, but
this app is about learning C++ and signal processing, so I will create multiple dsp pipeline implementations anyway.

## TLDR

The core idea for a time-domain beat detector is to measure the instantaneous volume (energy) of the audio and detect
sudden, significant increases in that energy. These sudden increases, or onsets, often correlate with a beat, especially
with kick drums in electronic music.

## Stages

### 1. Band Pass Filter

The kick drum in electronic music is usually between two thresholds. We can use a band pass filter with adaptive
thresholds to filter out the frequencies we are not interested in.

**Input:** AudioChunk (1024 float samples)

**Output:** AudioChunk (1024 float samples, filtered)

#### Algorithm

The most common and simplest digital filter is a second-order digital Infinite Impulse Response (IIR)
filter, which can be configured as a band-pass filter using specific coefficients.

$y[n]=b_0x[n]+b_1x[n−1]+b_2x[n−2]−a_1y[n−1]−a_2y[n−2]$

where:

- $y[n]$ is the output sample
- $x[n]$ is the input sample
- $b_0$ is the first coefficient
- $b_1$ is the second coefficient
- $b_2$ is the third coefficient
- $a_1$ is the first coefficient of the second order filter
- $a_2$ is the second coefficient of the second order filter

##### Coefficients

There are several online calculators for the coefficients of a biquad filter.

- https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3/
- https://arachnoid.com/BiQuadDesigner/index.html

These require 4 input parameters:

##### Sample Rate

The sample rate $f_s$ is the number of samples per second. It is read from the WASAPI audio source when it connects to
the audio device once, then you can read it.

##### Gain

The gain $G$ is usually 1 (=0 dB) for a band-pass filter, since it is a passive filter that does not change the
amplitude of the signal. It only cuts out frequencies that are outside the desired range.

##### Center Frequency

The center frequency $f_c$ is calculated from the desired frequencies $f_1$ (min) and $f_2$ (max) as:

$f_c = \sqrt{f_1 \cdot f_2}$

##### Q Factor

The Q factor $Q$ is a crucial parameter for a band-pass filter, as it defines the selectivity or "sharpness" of the
filter.

$Q = \frac{f_c}{f_s} = \frac{\sqrt{f_1 \cdot f_2}}{f_2 - f_1}$

A higher $Q$ (e.g., $Q>10$) means a narrow band-pass filter with high selectivity.  
A lower $Q$ (e.g., $Q<1$) means a wider band-pass filter.

### 2. Energy Calculation

The core idea of the time-domain Onset Detection Function (ODF) is to generate a new signal (the Onset Strength Signal -
OSS) that clearly highlights the moments of sudden, percussive energy. This is a crucial step in beat detection.

**Input:** AudioChunk (1024 float samples, filtered)

**Output:** Energy (1 float value)

#### Algorithm

Energy for each frame is calculated as:

$E = \sum_{n=0}^{N-1} s[n]^2$

where:

- $E$ is the energy
- $s[n]$ is the signal amplitude at sample n
- $N$ is the number of samples

### 3. Onset Detection

After calculating the energy for each frame, we can detect onsets by looking for sudden, percussive energy increases.

**Input:** Energy (1 float value)

**Output:** Onset Signal (1 float value)

#### Algorithm

Onset strength signal (OSS) is calculated as:

$OSS[k] = max(0, Ecurrent - Eprevious)$

where:

- $OSS[k]$ is the onset strength signal at sample k
- $Ecurrent$ is the current energy
- $Eprevious$ is the previous energy

### 4. Peak Index Detection

The final step is to calculate the BPM (beats per minute) from the OSS signal.

**Input:** Onset Signal (1 float value)

**Output:** List of Indices of Peaks (n integer values)

#### Algorithm

This algorithm is buffering the OSS signals from the previous stage over a sliding window and then finding the local
maxima in the collected OSS signals.

##### 1. Sliding Window

$B[k] = \{ s[k-N+1], \dots, s[k] \}$

where:

- $B[k]$ is the buffer at sample k
- $N$ is the size of the buffer
- $s[k]$ is the OSS signal at sample k

##### 2. Finding Peaks

This algorithm finds all points in a signal where:

* The value is higher than both the previous and next sample, *and*
* It’s greater than a threshold that depends on the signal’s overall maximum.

Threshold:  
$T[k] = \alpha \cdot \max_{s \in B[k]} s$

Peak condition:
$x[n]$ is a peak if  
$x[n] > T$,  
$x[n] > x[n-1]$, and  
$x[n] > x[n+1]$

Detected peaks:
$\mathcal{P} = \{ n : x[n] \text{ satisfies above conditions} \}$

### 5. Inter-Onset Interval Calculation

This stage calculates the temporal spacing between the peaks that were accumulated over the sliding window in the
previous stage.

**Input:** List of Indices of Peaks (n integer values)

**Output:** List of Spaces between Peaks (n float values)

#### Algorithm

This step converts discrete peak positions into temporal spacing, which is exactly the information needed to estimate
the beat period.

Let $\mathcal{P} = \{ p_0, p_1, \dots, p_{M-1} \}$ be the detected peaks.

Intervals between peaks:  
$I[m] = p_m - p_{m-1}, \quad m = 1, \dots, M-1$

Optionally in seconds:  
$I_{\text{sec}}[m] = \frac{p_m - p_{m-1}}{f_s}$

### 6. Calculate Dominant Interval

Remember, everything happens over the data that was present in the sliding window created in the peak detection stage.
So now we are looking for the dominant interval in this sliding window. If one interval between peaks occurs often, it
is likely that this is a regular rhythmic pattern like the kick drum.

**Input:** List of Spaces between Peaks (n float values)

**Output:** Dominant Interval (1 float value)

#### Algorithm

The dominant interval represents the typical spacing between beats in the signal.

Using the median makes it robust to outliers:

* If a peak is missed (interval too large)
* Or a spurious peak is detected (interval too small)

…the median still gives a good estimate of the typical beat spacing.

Let $I = \{ I[1], I[2], \dots, I[M-1] \}$ be the intervals between consecutive peaks.

Dominant interval:
$D = \text{median}(I) = \text{median} \big( I[1], I[2], \dots, I[M-1] \big)$

##### 7. Calculate BPM

For this last step, we convert the dominant interval to seconds.

**Input:** Dominant Interval (1 float value)

**Output:** BPM (1 float value)

#### Algorithm

The dominant interval is expressed in buffer indices right now, where each index represents a chunk of audio of size
the selected chunk size. Calculating chunks per second = sample rate / chunk size converts this interval into seconds.

Then we can compute the BPM by inverting the interval in seconds and multiplying it by 60.

Let $D$ be the dominant interval in onsetBuffer indices (samples/chunks).

Convert to seconds:  
$C_\text{per\_sec} = \frac{f_s}{N_c}$  
$D_\text{sec} = \frac{D}{C_\text{per\_sec}} = D \cdot \frac{N_c}{f_s}$

Convert to BPM:  
$\text{BPM} = \frac{60}{D_\text{sec}}$
