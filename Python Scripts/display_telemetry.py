import numpy as np
import matplotlib.pyplot as plt

data = np.load('telemetry.npy')

fig, axs = plt.subplots(1, 2)

axs[0].scatter(data[:, 0], data[:, 2], marker='.')
axs[0].plot(data[:, 0], data[:, 2])
axs[1].scatter(data[:, 0], data[:, 3], marker='.')
axs[1].plot(data[:, 0], data[:, 3])
plt.show()