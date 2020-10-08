import scipy.fftpack
import matplotlib.pyplot as plt
import numpy as np
import sys
import math
import scipy
import pandas as pd
from scipy import signal
import random

# Samples
N = 1024
# Sampling time
T = 1 / 50000
# Frequency filter variables
freqHigh = 45
freqLow = 35

# Constructing the signals
x1 = np.linspace(0, N*T, N)
x2 = np.linspace(0, N*T, N)
x3 = np.linspace(0, N*T, N)

y1 = np.sin(2*np.pi*2000*x1) + np.random.normal(0, 3, 1024) + \
    np.sin(30000*2*np.pi*x3) + np.sin(40000*2*np.pi*x3)

y2 = np.sin(2*np.pi*2000*x2+2) + np.random.normal(0, 3, 1024) + \
    np.sin(13245*2*np.pi*x3) + np.sin(25000*2*np.pi*x3)

y3 = np.sin(2*np.pi*2000*x3+3) + np.sin(20000*2*np.pi*x3) + \
    np.random.normal(0, 3, 1024) + np.sin(145*2*np.pi*x3) + \
    np.sin(5400*2*np.pi*x3)

# Performing FFT
yf1 = scipy.fftpack.fft(y1)
yf2 = scipy.fftpack.fft(y2)
yf3 = scipy.fftpack.fft(y3)

# Filtering the signals, i.e removing noise & band pass filter.

for i in range(0, len(yf1)):
    if(abs(yf1[i]) < 300):
        yf1[i] = 0
    if(abs(yf2[i]) < 300):
        yf2[i] = 0
    if(abs(yf3[i]) < 300):
        yf3[i] = 0

    if(i > freqHigh or i < freqLow):
        yf1[i] = 0
        yf2[i] = 0
        yf3[i] = 0


# Performing inverse FFT
yif1 = scipy.fftpack.ifft(yf1)
yif2 = scipy.fftpack.ifft(yf2)
yif3 = scipy.fftpack.ifft(yf3)

# Performing cross correlation
corr21 = np.correlate(yif1, yif2, mode=True)
corr31 = np.correlate(yif1, yif3, mode=True)
corr32 = np.correlate(yif2, yif3, mode=True)

# Calculating the time differences

timediff21 = (len(corr21))/2 - np.argmax(corr21)
timediff31 = (len(corr31))/2 - np.argmax(corr31)
timediff32 = (len(corr32))/2 - np.argmax(corr32)

# Printing the differences
print("Difference between signal")
print("1 and 2:", timediff21)
print("3 and 1:", timediff31)
print("3 and 2:", timediff32)

# Unfiltered signal

ucorr21 = np.correlate(y1, y2, mode=True)
ucorr31 = np.correlate(y1, y3, mode=True)
ucorr32 = np.correlate(y2, y3, mode=True)
utimediff21 = (len(ucorr21))/2 - np.argmax(ucorr21)
utimediff31 = (len(ucorr31))/2 - np.argmax(ucorr31)
utimediff32 = (len(ucorr32))/2 - np.argmax(ucorr32)

# Printing the differences
print("Difference between signal ( unfiltered)")
print("1 and 2:", utimediff21)
print("3 and 1:", utimediff31)
print("3 and 2:", utimediff32)

# Plotting the original signals
fig, axs = plt.subplots(3, sharex=True, sharey=True)
fig.suptitle('The unfiltered signals')
axs[0].plot(y1, 'r')
axs[1].plot(y2, 'r')
axs[2].plot(y3, 'r')

# Plotting the filtered signals
fig, axs2 = plt.subplots(3, sharex=True, sharey=True)
fig.suptitle('The filtered signals')
axs2[0].plot(yif1, 'b')
axs2[1].plot(yif2, 'b')
axs2[2].plot(yif3, 'b')

# Plotting the frequency domain
xf = np.linspace(0.0, 1.0/(2.0*T), N/2)
fig, axs3 = plt.subplots(3, sharex=True, sharey=True)
fig.suptitle('Frequency domain')
axs3[0].plot(xf, 2.0/N * np.abs(yf1[:N//2]))
axs3[1].plot(xf, 2.0/N * np.abs(yf2[:N//2]))
axs3[2].plot(xf, 2.0/N * np.abs(yf3[:N//2]))

# Plotting the crosscorrelation
fig, axs4 = plt.subplots(3, sharex=True, sharey=True)
fig.suptitle('Crosscorrelation')
axs4[0].plot(corr21, 'r')
axs4[1].plot(corr31, 'r')
axs4[2].plot(corr32, 'r')

plt.show()
