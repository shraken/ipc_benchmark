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

resultLoad = plotResults('results/ubuntu_linux.dat')

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
({'msg_queue': {1024: 8.798911134695354, 2048: 9.538600898803018,
4096: 9.47603233372039, 8192: 11.858737904629152}, 'named_pipe':
{1024: 202.41809057422245, 2048: 291.7806386269114, 4096:
435.73767205343455, 8192: 460.7201508586354}, 'pipe': {1024:
156.0427177122514, 2048: 230.21303687907638, 4096: 416.5159900140646,
8192: 429.77413406861064}, 'socketpair': {1024: 229.15686868503704,
2048: 312.86300807512583, 4096: 481.99908887778867, 8192:
682.6553650140983}, 'tcp': {1024: 432.47982031438124, 2048:
714.6610286980286, 4096: 1015.8406970506977, 8192: 974.1517034887681},
'uds': {1024: 10.822894620130272, 2048: 10.173674666703645, 4096:
15.911787677957298, 8192: 13.008448711320966}}, {'msg_queue': {1024:
8.798911134695354, 2048: 9.538600898803018, 4096: 9.47603233372039,
8192: 11.858737904629152}, 'named_pipe': {1024: 202.41809057422245,
2048: 291.7806386269114, 4096: 435.73767205343455, 8192:
460.7201508586354}, 'pipe': {1024: 156.0427177122514, 2048:
230.21303687907638, 4096: 416.5159900140646, 8192:
429.77413406861064}, 'socketpair': {1024: 229.15686868503704, 2048:
312.86300807512583, 4096: 481.99908887778867, 8192:
682.6553650140983}, 'tcp': {1024: 432.47982031438124, 2048:
714.6610286980286, 4096: 1015.8406970506977, 8192: 974.1517034887681},
'uds': {1024: 10.822894620130272, 2048: 10.173674666703645, 4096:
15.911787677957298, 8192: 13.008448711320966}})
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
