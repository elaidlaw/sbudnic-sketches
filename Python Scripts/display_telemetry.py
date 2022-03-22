import numpy as np
import matplotlib.pyplot as plt

data = np.load('telemetry.npy')

fig, axs = plt.subplots(1, 1)

axs.scatter(data[:, 0], data[:, 1], marker='.')
axs.plot(data[:, 0], data[:, 1])
axs.plot(data[:, 0], data[:, 2])
# axs[1].scatter(data[:, 0], data[:, 3], marker='.')
# axs[1].plot(data[:, 0], data[:, 3])
plt.show()