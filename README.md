# ipc_benchmark

Standard IPC benchmark examples.  Binaries allow specification of block size and
total transfer size.  A randomized dataset is always transferred to minimize
cache influence ensuring worst-cse performance for the test.

An evaluation python test script to serially run the IPC benchmarks collecting
the results into a report is included.  

The following IPC tests are provided:

1. pipe
2. named_pipe
3. unix domain sockets (uds)
4. socketpair
5. tcp
6. message queue

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

# Results

Results for Linux, FreeBSD, and mac OS are provided in the results/ directory.  