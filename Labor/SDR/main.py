import numpy as np
import matplotlib as mp

B1 = np.fromfile('valami.bin', dtype=float)

mp.plot(B1)

print(B1)