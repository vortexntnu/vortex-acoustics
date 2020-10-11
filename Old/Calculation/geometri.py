# -*- coding: utf-8 -*-
"""
Skript for å finne geometrisk posisjon til pinger

"""
import numpy as np
import sys
import math
"""
tenker sammensetning slik hvor orgio er markert med o
         /   /     /
      HF1   /     /_ Her er theta, vinkelen mellom HF3 og Pingerens lydbølge
           /   HF3 
          /          
       HF2


Ideen er å bruke rho = t31/t32 = cos(30-theta)/sin(theta)
Løser men for theta får man resultatet under i elif helvete.

Eneste varablene man trenger er t3-t2, t3-t1 og t2-t1.
Ut kommer vinkelen i radianer.
Kan lage script for å finne geometrisk posisjon med å bruke:
ikke lineær minste kvadraters metode, men fuck det.
"""
# Må lage noe elegant for importering av tallene.
t32 = 0
t31 = 0.002
t21 = 0.002


if(t31 > 1/2 * (t32) and t32 != 0):
    rho = (t31)/(t32)
    angle = math.atan(math.sqrt(3) / (2*rho - 1))
elif(t31 < 1/2 * (t32) and t32 != 0):
    rho = (t31)/(t32)
    angle = math.atan(math.sqrt(3) / (2*rho - 1)) + math.pi
elif(t32 == 0 and t31 >= 0):
    angle = math.pi/2
elif(t32 == 0 and t31 < 0):
    angle = -1*math.pi/2
elif(t31 == 1/2 * t32 and t31 >= 0):
    angle = 0
elif(t31 == 1/2 * t32 and t31 < 0):
    angle = math.pi

print(angle)
