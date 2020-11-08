# -*- coding: utf-8-sig -*-
"""
Testing av krysskorrelasjon

Konstruerere to signaler

Hvert signal skal bestå av to kombinerte sinusbølger.
De kombinerte sinusbølgene skal ha forskjellig frekvens.

Idéen er å teste hvordan man kan først
1. Filtrere ut den ene bølgen med annen frekvens på begge signalene.
2. Bruke krysskorrelasjon for å finne tidsforskjellen.
"""


import scipy.fftpack
import matplotlib.pyplot as plt
import numpy as np
import sys
import math
import scipy
import pandas as pd
from scipy import signal
import random


# Number of samplepoints
N = 64000
# sample spacing
T = 1.0 / 100000
# lager punkter tilhørende sinusbølgene. Har 600 målinger.
# punktene i x retning vil ha avstand 1/800 *600.
x1 = np.linspace(0.0, N*T, N)
x2 = np.linspace(0.0, N*T, N)
# Konstruerer begge signalene. Begge har frekvens på 80 og 50 hz.
y1 = np.sin(2*np.pi*x1*40000+3) + random.random() * \
    x1 + np.sin(2*np.pi*(x1*28000))
# Signal 2 har en shift på 3. ( kun det på 50Hz)
y2 = np.sin(2*np.pi*(x2*40000)) + random.random() * \
    x2 + np.sin(2*np.pi*(x2*28000))

# + 0.5*np.sin(80.0 * 2*np.pi*x2)
# Fouriertransform


yf1 = scipy.fftpack.fft(y1)
yf2 = scipy.fftpack.fft(y2)
xf = np.linspace(0.0, 1.0/(2.0*T), N/2)

# Filtering the signals
for i in range(0, len(yf1)):
    if(i > 39900 and i < 40100):
        print(yf1[i])
    if(i < 25000):
        yf1[i] = 0
        yf2[i] = 0


print(yf1[40000], 'hz')
yifft1 = scipy.fftpack.ifft(yf1)
yifft2 = scipy.fftpack.ifft(yf2)


# Cross correlation
corr = np.correlate(yifft1, yifft2, mode=True)

time = (len(corr))/2 - np.argmax(corr)
print(np.argmax(corr))
print(time)

# Plot fouriertransform
plt.figure(1)
plt.subplot(211)
plt.suptitle("Frekvensdoemenet")
plt.plot(xf, 2.0/N * np.abs(yf1[:N//2]))

# Plotting the correlation
plt.figure(2)
plt.suptitle('Korrelasjon')
plt.subplot(211)
plt.plot(corr)

# Plotting the original signal and the filtered signal
fig, axs = plt.subplots(4, sharex=True, sharey=True)
fig.suptitle('Filtrert og ufiltrert signaler')
axs[0].plot(yifft1, 'b')

axs[1].plot(yifft2, 'b')
axs[2].plot(y1, 'r')
axs[3].plot(y2, 'r')

# Finner krysskorrelasjonen

plt.show()
