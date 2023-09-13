#!/usr/bin/python3
import numpy as np
from numpy import fft
import matplotlib.pyplot as plt
#import matplotlib as plt
np.set_printoptions(precision=3)


x = [0,2,1,2,1,0]
X = fft.fft(x) * 1/len(x)
X = fft.fftshift(X)
#print(X.real)
#print(X.imag)

k = np.arange(100)
s = np.sin(0.15*2*3*k)
plt.plot(s,'.-')

S = fft.fft(s) * 1/len(s)
plt.plot(S,'.-')
plt.show()
