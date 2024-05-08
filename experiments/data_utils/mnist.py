"""
Code from PECANN-DPC
"""

import numpy as np

import torchvision.datasets as datasets

mnist_train = datasets.MNIST(root="/ssd2/kishen/centroidHAC/mnist", train=True, download=True)
mnist_test = datasets.MNIST(root="/ssd2/kishen/centroidHAC/mnist", train=False, download=True)

all_data = np.vstack([mnist_train.data.numpy(), mnist_test.data.numpy()]).reshape(
    70000, 28 * 28
)
all_data = all_data.astype("float32")
all_data /= 256
all_labels = np.vstack(
    [
        mnist_train.targets.numpy().reshape(60000, 1),
        mnist_test.targets.numpy().reshape(10000, 1),
    ]
)

# np.save("/ssd2/kishen/centroidHAC/mnist/mnist.npy", all_data)
# np.save("/ssd2/kishen/centroidHAC/mnist/mnist_gt.npy", all_labels)
np.savetxt("/ssd2/kishen/centroidHAC/mnist/mnist.txt", all_data, fmt="%.5f", delimiter=" ", header = "70000 784",comments='')
np.savetxt("/ssd2/kishen/centroidHAC/mnist/mnist.gt", all_labels, fmt="%i")
