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
IPC_CWD_ZMQ_DIR = "benchmarks/zmq/"

#IPC_TOTAL_SIZE = 104857600 # 100Mbyte
IPC_TOTAL_SIZE = 10485760000 # 1 GByte
IPC_DEFAULT_TRIAL_ATTEMPTS = 25

BENCHMARK_LOOP_WAIT_PERIOD = 2.0

IPC_BLOCK_SIZE_1024 = 1024
IPC_BLOCK_SIZE_2048 = 2048
IPC_BLOCK_SIZE_4096 = 4096
IPC_BLOCK_SIZE_8192 = 8192
IPC_BLOCK_SIZE_16384 = 16384
IPC_BLOCK_SIZE_32768 = 32768
IPC_BLOCK_SIZE_65536 = 65536

typesToParse = [ 'msg_queue', 'named_pipe', 'pipe', 'socketpair', 'tcp', 'uds', 'zmq' ]
blockSizeList = [ 1024, 2048, 4096, 8192, 16384, 32768, 65536 ]

blankList = {key: [] for key in blockSizeList}
#ipcResults = {key: blankList for key in typesToParse}

ipcResults = {
    'msg_queue' : {
        IPC_BLOCK_SIZE_1024: [],
        IPC_BLOCK_SIZE_2048: [],
        IPC_BLOCK_SIZE_4096: [],
        IPC_BLOCK_SIZE_8192: [],
        IPC_BLOCK_SIZE_16384: [],
        IPC_BLOCK_SIZE_32768: [],
        IPC_BLOCK_SIZE_65536: [],
    },
    'named_pipe': {
        IPC_BLOCK_SIZE_1024: [],
        IPC_BLOCK_SIZE_2048: [],
        IPC_BLOCK_SIZE_4096: [],
        IPC_BLOCK_SIZE_8192: [],
        IPC_BLOCK_SIZE_16384: [],
        IPC_BLOCK_SIZE_32768: [],
        IPC_BLOCK_SIZE_65536: [],
    },
    'pipe': {
        IPC_BLOCK_SIZE_1024: [],
        IPC_BLOCK_SIZE_2048: [],
        IPC_BLOCK_SIZE_4096: [],
        IPC_BLOCK_SIZE_8192: [],
        IPC_BLOCK_SIZE_16384: [],
        IPC_BLOCK_SIZE_32768: [],
        IPC_BLOCK_SIZE_65536: [],
    },
    'socketpair': {
        IPC_BLOCK_SIZE_1024: [],
        IPC_BLOCK_SIZE_2048: [],
        IPC_BLOCK_SIZE_4096: [],
        IPC_BLOCK_SIZE_8192: [],
        IPC_BLOCK_SIZE_16384: [],
        IPC_BLOCK_SIZE_32768: [],
        IPC_BLOCK_SIZE_65536: [],
    },
    'tcp': {
        IPC_BLOCK_SIZE_1024: [],
        IPC_BLOCK_SIZE_2048: [],
        IPC_BLOCK_SIZE_4096: [],
        IPC_BLOCK_SIZE_8192: [],
        IPC_BLOCK_SIZE_16384: [],
        IPC_BLOCK_SIZE_32768: [],
        IPC_BLOCK_SIZE_65536: [],
    },
    'uds': {
        IPC_BLOCK_SIZE_1024: [],
        IPC_BLOCK_SIZE_2048: [],
        IPC_BLOCK_SIZE_4096: [],
        IPC_BLOCK_SIZE_8192: [],
        IPC_BLOCK_SIZE_16384: [],
        IPC_BLOCK_SIZE_32768: [],
        IPC_BLOCK_SIZE_65536: [],
    },
    'zmq': {
        IPC_BLOCK_SIZE_1024: [],
        IPC_BLOCK_SIZE_2048: [],
        IPC_BLOCK_SIZE_4096: [],
        IPC_BLOCK_SIZE_8192: [],
        IPC_BLOCK_SIZE_16384: [],
        IPC_BLOCK_SIZE_32768: [],
        IPC_BLOCK_SIZE_65536: [],
    }
}

def benchmark_test(block_size, total_size, base_dir, single_proc=False, delay=0, fliporder=False, killattempt=True):
    global ipcResults
    
    for attempts in xrange(5):
        result = benchmark_test_run(block_size, total_size, base_dir, single_proc, delay, fliporder, killattempt)
    
        print 'result'
        print result

        try:
            resultSplit = result.split(',')
            nameSplit = resultSplit[0].split('/')
        except ValueError:
            time.sleep(2)
            continue

        print 'name = {}'.format(nameSplit[1])
        print 'rate = {}'.format(resultSplit[1])

        name = nameSplit[1]
        ipcResults[name][block_size].append(float(resultSplit[1]))
        break

    if attempts >= 5:
        print 'FAILURE, bad result'
        return False
    else:
        return True

def benchmark_test_run(block_size, total_size, base_dir, single_proc=False, delay=0, fliporder=False, killattempt=True):
    if single_proc:
        print 'block_size = {}'.format(block_size)
        print 'total size = {}'.format(total_size)

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
            print 'block_size = {}'.format(block_size)
            print 'total size = {}'.format(total_size)

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
            print 'block_size = {}'.format(block_size)
            print 'total size = {}'.format(total_size)

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

        if os.uname()[0] == 'Linux':
            os.killpg(os.getpgid(server_proc.pid), signal.SIGTERM)
        else:
            server_proc.kill()
        
    #print 'benchRow = {}'.format(benchRow)
    return benchRow

def ipc_benchmark_run(save_file, trial_runs, verbose):
    for trialRun in xrange(int(trial_runs)):
        print 'Running trial {}'.format(trialRun)

        for bsize in blockSizeList:
            #benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_MSG_QUEUE_DIR, single_proc=False)
            #benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_NAMED_PIPE_DIR, single_proc=False)
            #benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_PIPE_DIR, single_proc=True)
            #benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_SOCKET_PAIR_DIR, single_proc=True)
            #benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_TCP_DIR, single_proc=False, killattempt=True)
            #benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_UDS_DIR, single_proc=False, fliporder=True)
            benchmark_test(bsize, IPC_TOTAL_SIZE, IPC_CWD_ZMQ_DIR, single_proc=False, fliporder=True)
            time.sleep(BENCHMARK_LOOP_WAIT_PERIOD)

    if verbose:
        global ipcResults
        print 'ipcResults'
        pprint(ipcResults)

    with open(save_file, 'wb') as handle:
        pickle.dump(ipcResults, handle, protocol=pickle.HIGHEST_PROTOCOL)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Test the DAC, ADC, and PWM function of the Buddy DAQ device')

    parser.add_argument('-t,--trials', dest='trial_runs',
                        nargs='?', const=IPC_DEFAULT_TRIAL_ATTEMPTS, type=int,
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
