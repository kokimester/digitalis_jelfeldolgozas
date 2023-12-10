import numpy as np
import matplotlib.pyplot as plt
import sys

if(len(sys.argv) < 2):
    print("not enough input parameters")
    quit()

a = np.fromfile(sys.argv[1],dtype='float32')

#keparany [4,3]
# plt.figure(1,[8,6])
plt.figure(1,[16,9])
# plt.subplot(211)
fs=14238
if(len(sys.argv) > 2):
    fs=int(sys.argv[2])
datasize=np.size(a)
t=np.arange(0,datasize)/fs
plt.plot(t,np.real(a),'.-',label='signal')
# plt.plot(a,'.-',label='signal')
plt.title('signal in time')
# plt.xlabel('time [sample index]')
plt.xlabel('time [s]')
# plt.xlim(0,fs*60)
# plt.ylim(0,0.1)
plt.ylabel('amplitude')
plt.grid()
plt.legend()

# plt.subplot(212)
# A=20*np.log10(np.abs(np.fft.fft(a)/np.size(a)))
# A=A-max(A)
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
