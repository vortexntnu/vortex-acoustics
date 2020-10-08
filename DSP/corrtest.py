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


def lag_finder(y1, y2, sr):
    n = len(y1)

    corr = signal.correlate(y2, y1, mode='same') / np.sqrt(signal.correlate(
        y1, y1, mode='same')[int(n/2)] * signal.correlate(y2, y2, mode='same')[int(n/2)])

    delay_arr = np.linspace(-0.5*n/sr, 0.5*n/sr, n)
    delay = delay_arr[np.argmax(corr)]
    print('y2 is ' + str(delay) + ' behind y1')

    plt.figure()
    plt.plot(delay_arr, corr)
    plt.title('Lag: ' + str(np.round(delay, 3)) + ' s')
    plt.xlabel('Lag')
    plt.ylabel('Correlation coeff')
    plt.show()


# Number of samplepoints
N = 1024
# sample spacing
T = 1.0 / 3000
# lager punkter tilhørende sinusbølgene. Har 600 målinger.
# punktene i x retning vil ha avstand 1/800 *600.
x1 = np.linspace(0.0, N*T, N)
x2 = np.linspace(0.0, N*T, N)
# Konstruerer begge signalene. Begge har frekvens på 80 og 50 hz.
y1 = np.sin(2*np.pi*30*x1)
# + 0.5*np.sin(80.0 * 2*np.pi*x1)
# Signal 2 har en shift på 3. ( kun det på 50Hz)
y2 = np.sin(2*np.pi*(x2)*30+2)
# + 0.5*np.sin(80.0 * 2*np.pi*x2)
# Fouriertransform

yf1 = scipy.fftpack.fft(y1)
yf2 = scipy.fftpack.fft(y2)

xf = np.linspace(0.0, 1.0/(2.0*T), N/2)


# Invers fourier transform
yifft1 = scipy.fftpack.ifft(yf1)
yifft2 = scipy.fftpack.ifft(yf2)


# Finner krysskorrelasjonen

lag_finder(y1, y2, 1024)
