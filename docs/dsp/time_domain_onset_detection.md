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