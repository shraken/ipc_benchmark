import sys
import os
import time
import subprocess
import signal

IPC_CWD_MSG_QUEUE_DIR = "benchmarks/msg_queue/"
IPC_CWD_NAMED_PIPE_DIR = "benchmarks/named_pipe/"
IPC_CWD_PIPE_DIR = "benchmarks/pipe/"

IPC_TOTAL_SIZE = 104857600 # 100Mbyte

def benchmark_test_run(block_size, total_size, base_dir, single_proc):
    if single_proc:
        both_proc = subprocess.Popen('./main -b {} -c {}'.format(block_size, total_size),
                                     cwd=base_dir,
                                     shell=True,
                                     stdout=subprocess.PIPE,
                                     preexec_fn=os.setsid)

    else:
        client_proc = subprocess.Popen('./client -b {} -c {}'.format(block_size, total_size),
                                       cwd=base_dir + 'client/',
                                       shell=True,
                                       stdout=subprocess.PIPE,
                                       preexec_fn=os.setsid)

        server_proc = subprocess.Popen('./server -b {} -c {}'.format(block_size, total_size),
                                       cwd=base_dir + 'server/',
                                       shell=True,
                                       stdout=subprocess.PIPE,
                                       preexec_fn=os.setsid)

        client_proc.wait()
    
        sys.stdout.write(base_dir + ',')
        for line in client_proc.stdout:
            sys.stdout.write(line)

        os.killpg(os.getpgid(server_proc.pid), signal.SIGTERM)

def ipc_benchmark_run():
    blockSizes = [1024, 2048, 4095, 8192]

    for bsize in blockSizes:
        benchmark_test_run(bsize, IPC_TOTAL_SIZE, IPC_CWD_MSG_QUEUE_DIR, False)
        benchmark_test_run(bsize, IPC_TOTAL_SIZE, IPC_CWD_NAMED_PIPE_DIR, False)

if __name__ == '__main__':
    ipc_benchmark_run()