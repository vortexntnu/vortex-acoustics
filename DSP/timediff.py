# -*- coding: utf-8-sig -*-
"""
Skript for å finne tidsforskjellen i signalene

"""
import scipy.fftpack
import matplotlib.pyplot as plt
import numpy as np
import sys
import math
import scipy
import pandas as pd


import csv

import unicodecsv

mydata = []
with open('TestDATAFFT.csv', 'rb') as f_input:
    input_file2 = unicodecsv.reader(f_input, encoding='utf-8-sig')
    for row in input_file2:
        if(len(row) == 1):
            mydata.append(row[0])

print(mydata)


# Number of samplepoints
N = 1024
# sample spacing
T = 1.0 / 100000
x = np.linspace(0.0, N*T, N)
yfft = scipy.fftpack.fft(mydata)
xfft = np.linspace(0.0, 1.0/(2.0*T), N/2)


for value in yfft:
    if(value.real > 10000 or np.abs(value) < 20000):
        yfft[np.where(yfft == value)] = 0
print(max(np.abs(yfft)))

for i in range(0, len(yfft)):
    if(i < 284 or i > 289):
        yfft[i] = 0

    if(abs(yfft[i]) < 100):
        yfft[i] = 0
# fig, ax = plt.subplots()
# ax.plot(xfft, 2.0/N * np.abs(yfft[:N//2]))
# plt.show()

yfilt = scipy.fftpack.ifft(yfft)
print(len(yfilt))

corr = np.correlate(yfilt, yfilt, mode=True)

timediff = (len(corr))/2 - np.argmax(corr)
print(timediff)
# Plotting the frequency domain
plt.figure(1)
plt.suptitle('Frequency domain')
plt.plot(xfft, 2.0/N * np.abs(yfft[:N//2]))

# Plotting the signal
plt.figure(2)
plt.suptitle('Signal')
plt.plot(yfilt)

plt.figure(3)
plt.suptitle('Signal')
plt.plot(corr)
plt.show()
