import numpy as np
import matplotlib.pyplot as plt
import sys

if(len(sys.argv) < 2):
    print("not enough input parameters")
    quit()

a = np.fromfile(sys.argv[1],dtype='complex64')
if(len(sys.argv) > 2):
    b = np.fromfile(sys.argv[2],dtype='complex64')


#keparany [4,3]
plt.figure(1,[8,6])
#plt.figure(1,[16,9])
if(len(sys.argv) > 2):
    plt.subplot(211)
plt.plot(np.real(a),'.-',label='real')
plt.plot(np.imag(a),'.-',label='imag')
if(len(sys.argv) > 2):
    plt.title('first signal in time')
plt.title('signal in time')
plt.xlabel('time [sample index]')
plt.xlim(0,190000)
plt.ylim(-2,2)
plt.ylabel('amplitude')
plt.grid()
plt.legend()
if(len(sys.argv) > 2):
    plt.subplot(212)
    plt.plot(np.real(b),'.-',label='real')
    plt.plot(np.imag(b),'.-',label='imag')
    plt.title('second signal in time')
    plt.xlabel('time [sample index]')
    plt.xlim(0,190000)
    plt.ylim(-2,2)
    plt.ylabel('amplitude')
    plt.grid()
    plt.legend()

# A=20*np.log10(np.abs(np.fft.fft(a)/np.size(a)))
# A=A-max(A)
# fs=128000
# if(len(sys.argv) >= 3):
#     fs=int(sys.argv[2])
# f=np.arange(0,fs,fs/np.size(A))/fs
# plt.plot(f,A,'.-')
# plt.title('signal spectrum')
# plt.xlabel('frequency [normalized frequency]')
# plt.ylabel('normalized amplitude [dB]')
# plt.xlim(0,1/2)
# plt.ylim(-160,0)
# plt.grid()
# plt.subplots_adjust(left=0.125, bottom=0.1, right=0.9, top=0.9, wspace=0.2, hspace=0.4)

if(len(sys.argv) < 4):
    plt.show()
else:
    plt.savefig(sys.argv[3],dpi=200)

#plt.savefig('../pic/tmp.png',dpi=200)
