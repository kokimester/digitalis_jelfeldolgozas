import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
np.set_printoptions(precision=3)

N  = 128
n  = np.arange(N)
xn = np.sin(2*np.pi / 15.0 * n)

window = np.blackman(N)
xn = xn * window

# freq = np.fft.fftfreq(N)
# freq_theta = freq * 2 * pi / N

Xk = np.fft.fft(xn)
# Xk = np.abs(Xk)
Xk = np.fft.fftshift(Xk)
#print(Xk)
Xk_ishift = np.fft.ifftshift(Xk)
xn_ifft = np.fft.ifft(Xk_ishift)

### restore unwindowd signal
# xn_ifft = xn_ifft / window

plt.figure(2,[16,9])

plt.subplot(221)
plt.title('signal in time domain')
plt.plot(xn,'.-')

plt.subplot(212)
plt.title('signal in frequency domain')
plt.stem(np.arange(-N/2, N/2), np.abs(Xk))

plt.subplot(222)
plt.title('inverse dft of signal')
plt.plot(xn_ifft, '.-')


plt.show()
###help from thesis
#plt.figure(1,[8,6])
#plt.figure(1,[16,9])
#plt.subplot(211)
#plt.plot(np.real(a),'.-',label='real')
#plt.plot(np.imag(a),'.-',label='imag')
#plt.title('signal in time')
#plt.xlabel('time [sample index]')
#plt.xlim(0,101)
#plt.ylabel('amplitude')
#plt.grid()
#plt.legend()
#plt.subplot(212)

