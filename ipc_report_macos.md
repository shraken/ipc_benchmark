# IPC Report

## Requirements

python 3.6.x
pweave

## Usage

```console
shraken@mint-vbox ~/shraken_code/ipc_benchmark $ python benchmark.py 
usage: benchmark.py [-h] [-t,--trials [TRIAL_RUNS]] [-v,--verbose]
                    [-f,--file SAVE_FILE]

Test the DAC, ADC, and PWM function of the Buddy DAQ device

optional arguments:
  -h, --help            show this help message and exit
  -t,--trials [TRIAL_RUNS]
                        number of trials to be run for each IPC test
  -v,--verbose          enable verbose printing
  -f,--file SAVE_FILE   filename to save pickle results to
```

## Running It

```console
shraken@mint-vbox ~/shraken_code/ipc_benchmark $ python benchmark.py -v -t 100 -f result.dat
```

## Benchmark Result

The figure below compares the throughput (MB/sec) for the different IPC methods when varying the block
size of transmission.  The transfer size for each test is fixed at 100 Megabytes.  

The best performance was TCP with a block size of 8192 but the socketpair
method with block size of 8192 was almost as much with better repeatability.

The worst case performance was msg_queue and the unix domain socket.

These results are interesting as unix domain sockets are often advocated
for as a superior 


```python
import numpy as np
import matplotlib.pyplot as plt
import pickle
from pprint import pprint

IPC_BLOCK_SIZE_1024 = 1024
IPC_BLOCK_SIZE_2048 = 2048
IPC_BLOCK_SIZE_4096 = 4096
IPC_BLOCK_SIZE_8192 = 8192

def getMaxMinValue(results, key):
    return [ max(results), min(results) ]

def plotResults(filename):
    with open(filename, "rb") as handle:
        result = pickle.load(handle)
    
        #print(result)

    typesToParse = [ 'msg_queue', 'named_pipe', 'pipe', 'socketpair', 'tcp', 'uds' ]
    blockSizeList = [ IPC_BLOCK_SIZE_1024, IPC_BLOCK_SIZE_2048, 
                     IPC_BLOCK_SIZE_4096, IPC_BLOCK_SIZE_8192 ]
    colorTypes = [ 'r', 'b', 'g', 'c', 'm', 'y' ]

    n_groups = 4
    fig, ax = plt.subplots()

    index = np.arange(n_groups)
    bar_width = 0.1

    opacity = 0.4
    error_config = {'ecolor': '0.3'}

    #resultMean = {key: None for key in typesToParse}
    #blankList = {int(key): [] for key in blockSizeList}
    #resultMean = {key: blankList for key in typesToParse}
    #resultStd = {key: blankList for key in typesToParse}

    resultMean = {
        'msg_queue' : {
            IPC_BLOCK_SIZE_1024: [],
            IPC_BLOCK_SIZE_2048: [],
            IPC_BLOCK_SIZE_4096: [],
            IPC_BLOCK_SIZE_8192: [],
        },
        'named_pipe': {
            IPC_BLOCK_SIZE_1024: [],
            IPC_BLOCK_SIZE_2048: [],
            IPC_BLOCK_SIZE_4096: [],
            IPC_BLOCK_SIZE_8192: [],
        },
        'pipe': {
            IPC_BLOCK_SIZE_1024: [],
            IPC_BLOCK_SIZE_2048: [],
            IPC_BLOCK_SIZE_4096: [],
            IPC_BLOCK_SIZE_8192: [],
        },
        'socketpair': {
            IPC_BLOCK_SIZE_1024: [],
            IPC_BLOCK_SIZE_2048: [],
            IPC_BLOCK_SIZE_4096: [],
            IPC_BLOCK_SIZE_8192: [],
        },
        'tcp': {
            IPC_BLOCK_SIZE_1024: [],
            IPC_BLOCK_SIZE_2048: [],
            IPC_BLOCK_SIZE_4096: [],
            IPC_BLOCK_SIZE_8192: [],
        },
        'uds': {
            IPC_BLOCK_SIZE_1024: [],
            IPC_BLOCK_SIZE_2048: [],
            IPC_BLOCK_SIZE_4096: [],
            IPC_BLOCK_SIZE_8192: [],
        }
    }

    resultStd = resultMean

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

        for blockMean, blockStd, blockSize in zip(means_temp, stds_temp, blockSizeList):
            resultMean[typeParse][blockSize] = blockMean
            resultStd[typeParse][blockSize] = blockStd

    '''
    print('resultMean')
    print(resultMean)

    print('resultStd')
    print(resultStd)
    '''

    plt.xlabel('Type')
    plt.ylabel('Rate (MB/sec)')
    plt.title('IPC Throughput v. Block size')
    plt.xticks(index + bar_width / 2, ('1024', '2048', '4096', '8192'))
    plt.legend()

    plt.tight_layout()
    plt.show()

    return (resultMean, resultStd)

resultLoad = plotResults('results/macos_result.dat')

print('resultLoad')
print(resultLoad)

#[maxMsgQueue, minMsgQueue] = getMaxMinValue(result, 'msg_queue')
#print('maxMsgQueue')
#print(maxMsgQueue)
#print('minMsgQueue')
#print(minMsgQueue)
```

```
resultLoad
({'msg_queue': {1024: 0.18118567047379883, 2048: 0.18235194188548226,
4096: 0.19908482617026252, 8192: 0.1757957907156152}, 'named_pipe':
{1024: 21.012239081098894, 2048: 46.397233834701346, 4096:
103.51947911361107, 8192: 42.66460417764383}, 'pipe': {1024:
13.82129519452535, 2048: 11.874667213458352, 4096: 31.776701830117037,
8192: 39.61117425020482}, 'socketpair': {1024: 12.340511457934737,
2048: 33.34672295877549, 4096: 60.10491976612, 8192:
18.288449946932616}, 'tcp': {1024: 10.114057656468749, 2048:
33.43107208229263, 4096: 63.172315718937114, 8192: 50.74285829184467},
'uds': {1024: 1.8545226652113418, 2048: 2.73681156255785, 4096:
2.4932181318241873, 8192: 2.9463887329250893}}, {'msg_queue': {1024:
0.18118567047379883, 2048: 0.18235194188548226, 4096:
0.19908482617026252, 8192: 0.1757957907156152}, 'named_pipe': {1024:
21.012239081098894, 2048: 46.397233834701346, 4096:
103.51947911361107, 8192: 42.66460417764383}, 'pipe': {1024:
13.82129519452535, 2048: 11.874667213458352, 4096: 31.776701830117037,
8192: 39.61117425020482}, 'socketpair': {1024: 12.340511457934737,
2048: 33.34672295877549, 4096: 60.10491976612, 8192:
18.288449946932616}, 'tcp': {1024: 10.114057656468749, 2048:
33.43107208229263, 4096: 63.172315718937114, 8192: 50.74285829184467},
'uds': {1024: 1.8545226652113418, 2048: 2.73681156255785, 4096:
2.4932181318241873, 8192: 2.9463887329250893}})
```

![](figures/ipc_report_figure1_1.png)\


<div style="max-height:1000px;max-width:1500px;overflow:auto;">
<table border="1" class="dataframe">
<thead>
    <tr style="text-align: right;">
        <th>Block Size / Type</th>
        <th>1024</th>
        <th>2048</th>
        <th>4096</th>
        <th>8192</th>
    </tr>
</thead>
<tbody>
    <tr>
        <th>msg_queue</th>
        <td></td>
        <td>x</td>
        <td>x</td>
        <td>x</td>
    </tr>
    <tr>
        <th>named_pipe</th>
        <td>x</td>
        <td>x</td>
        <td>x</td>
        <td>x</td>
    </tr>
    <tr>
        <th>pipe</th>
        <td>x</td>
        <td>x</td>
        <td>x</td>
        <td>x</td>
    </tr>
    <tr>
        <th>socketpair</th>
        <td>x</td>
        <td>x</td>
        <td>x</td>
        <td>x</td>
    </tr>
    <tr>
        <th>tcp</th>
        <td>x</td>
        <td>x</td>
        <td>x</td>
        <td>x</td>
    </tr>
    <tr>
        <th>uds</th>
        <td>x</td>
        <td>x</td>
        <td>x</td>
        <td>x</td>
    </tr>
</tbody>
</table>
</div>
