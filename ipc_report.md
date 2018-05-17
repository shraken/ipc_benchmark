```python
import numpy as np
import matplotlib.pyplot as plt
import pickle
from pprint import pprint

IPC_BLOCK_SIZE_1024 = 1024
IPC_BLOCK_SIZE_2048 = 2048
IPC_BLOCK_SIZE_4096 = 4096
IPC_BLOCK_SIZE_8192 = 8192

def plotResults(filename):
    with open(filename, "rb") as handle:
        result = pickle.load(handle)

    '''
    typesToParse = [ 'msg_queue', 'named_pipe', 'pipe',
                     'socketpair', 'tcp', 'uds' ]
    '''

    typesToParse = [ 'msg_queue', 'named_pipe', 'pipe', 'socketpair', 'tcp', 'uds' ]
    colorTypes = [ 'r', 'b', 'g', 'c', 'm', 'y' ]

    n_groups = 4
    fig, ax = plt.subplots()

    index = np.arange(n_groups)
    bar_width = 0.1

    opacity = 0.4
    error_config = {'ecolor': '0.3'}

    for idx, typeParse in enumerate(typesToParse):
        means_temp = ( np.mean(result[typeParse][IPC_BLOCK_SIZE_1024]),
                       np.mean(result[typeParse][IPC_BLOCK_SIZE_2048]),
                       np.mean(result[typeParse][IPC_BLOCK_SIZE_4096]),
                       np.mean(result[typeParse][IPC_BLOCK_SIZE_8192]) )

        stds_temp = ( np.std(result[typeParse][IPC_BLOCK_SIZE_1024]),
                      np.std(result[typeParse][IPC_BLOCK_SIZE_2048]),
                      np.std(result[typeParse][IPC_BLOCK_SIZE_4096]),
                      np.std(result[typeParse][IPC_BLOCK_SIZE_8192]) )

        rects_temp = plt.bar(index + (idx * bar_width), means_temp, bar_width,
                             alpha=opacity,
                             color=colorTypes[idx],
                             yerr=stds_temp,
                             error_kw=error_config,
                             label=typeParse)

    plt.xlabel('Type')
    plt.ylabel('Rate (MB/sec)')
    plt.title('IPC Throughput v. Block size')
    plt.xticks(index + bar_width / 2, ('1024', '2048', '4096', '8192'))
    plt.legend()

    plt.tight_layout()
    plt.show()

plotResults('ipc_result2.dat')

```