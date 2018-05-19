import sys
import os
import time
import subprocess
import signal
import pickle
import argparse
from pprint import pprint

IPC_CWD_MSG_QUEUE_DIR = "benchmarks/msg_queue/"
IPC_CWD_NAMED_PIPE_DIR = "benchmarks/named_pipe/"
IPC_CWD_PIPE_DIR = "benchmarks/pipe/"
IPC_CWD_SOCKET_PAIR_DIR = "benchmarks/socketpair/"
IPC_CWD_TCP_DIR = "benchmarks/tcp/"
IPC_CWD_UDS_DIR = "benchmarks/uds/"

IPC_TOTAL_SIZE = 104857600 # 100Mbyte
IPC_TRIAL_ATTEMPTS = 25

IPC_BLOCK_SIZE_1024 = 1024
IPC_BLOCK_SIZE_2048 = 2048
IPC_BLOCK_SIZE_4096 = 4096
IPC_BLOCK_SIZE_8192 = 8192

ipcResults = {
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

def benchmark_test(block_size, total_size, base_dir, single_proc, delay=0, fliporder=False, killattempt=True):
    global ipcResults
    result = benchmark_test_run(block_size, total_size, base_dir, single_proc, delay, fliporder, killattempt)

    resultSplit = result.split(',')
    nameSplit = resultSplit[0].split('/')

    print 'name = {}'.format(nameSplit[1])
    print 'rate = {}'.format(resultSplit[1])

    name = nameSplit[1]
    ipcResults[name][block_size].append(float(resultSplit[1]))

def benchmark_test_run(block_size, total_size, base_dir, single_proc, delay=0, fliporder=False, killattempt=True):
    if single_proc:
        both_proc = subprocess.Popen('./main -b {} -c {}'.format(block_size, total_size),
                                     cwd=base_dir,
                                     shell=True,
                                     stdout=subprocess.PIPE,
                                     preexec_fn=os.setsid)

        both_proc.wait()
    
        benchRow = base_dir + ','
        for line in both_proc.stdout:
            benchRow += line
    else:
        if fliporder:
            client_proc = subprocess.Popen('./client -b {} -c {}'.format(block_size, total_size),
                                           cwd=base_dir + 'client/',
                                           shell=True,
                                           stdout=subprocess.PIPE,
                                           preexec_fn=os.setsid)
            time.sleep(delay)

            server_proc = subprocess.Popen('./server -b {} -c {}'.format(block_size, total_size),
                                           cwd=base_dir + 'server/',
                                           shell=True,
                                           stdout=subprocess.PIPE,
                                           preexec_fn=os.setsid)
            
        else:
            server_proc = subprocess.Popen('./server -b {} -c {}'.format(block_size, total_size),
                                           cwd=base_dir + 'server/',
                                           shell=True,
                                           stdout=subprocess.PIPE,
                                           preexec_fn=os.setsid)

            time.sleep(delay)
            client_proc = subprocess.Popen('./client -b {} -c {}'.format(block_size, total_size),
                                           cwd=base_dir + 'client/',
                                           shell=True,
                                           stdout=subprocess.PIPE,
                                           preexec_fn=os.setsid)

        client_proc.wait()

        benchRow = base_dir + ','
        for line in client_proc.stdout:
            benchRow += line

        if killattempt:
            os.killpg(os.getpgid(server_proc.pid), signal.SIGTERM)

    print 'benchRow = {}'.format(benchRow)
    return benchRow

def ipc_benchmark_run(save_file, trial_runs, verbose):
    blockSizes = [ IPC_BLOCK_SIZE_1024, IPC_BLOCK_SIZE_2048,
                   IPC_BLOCK_SIZE_4096, IPC_BLOCK_SIZE_8192 ]

    for trialRun in xrange(int(trial_runs)):
        for bsize in blockSizes:
            benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_MSG_QUEUE_DIR, False)
            benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_NAMED_PIPE_DIR, False)
            benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_PIPE_DIR, True)
            benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_SOCKET_PAIR_DIR, True)
            benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_TCP_DIR, False, delay=2.0, killattempt=False)
            benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_UDS_DIR, False, fliporder=True)

    if verbose:
        global ipcResults
        print 'ipcResults'
        pprint(ipcResults)

    with open(save_file, 'wb') as handle:
        pickle.dump(ipcResults, handle, protocol=pickle.HIGHEST_PROTOCOL)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Test the DAC, ADC, and PWM function of the Buddy DAQ device')

    parser.add_argument('-t,--trials', dest='trial_runs',
                        nargs='?', const=IPC_TRIAL_ATTEMPTS, type=int,
                        help='number of trials to be run for each IPC test')
    parser.add_argument('-v,--verbose', action='store_true', dest='verbose',
                        help='enable verbose printing')
    parser.add_argument('-f,--file', dest='save_file',
                        help='filename to save pickle results to')

    args = parser.parse_args()
    
    if (not args.save_file) or (not args.trial_runs):
        parser.print_help()
        sys.exit(1)

    ipc_benchmark_run(args.save_file, args.trial_runs, args.verbose)