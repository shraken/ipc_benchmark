import sys
import os
import time
import subprocess
import signal

IPC_TOTAL_SIZE = 1048576000

def top_msg_queue():
    blockSizes = [1024, 2048, 4095, 8192]

    for bsize in blockSizes:
        test_msg_queue(bsize, IPC_TOTAL_SIZE)

def test_msg_queue(block_size, total_size):
    server_proc = subprocess.Popen('./server -b {} -c {}'.format(block_size, total_size),
                                   cwd='benchmarks/msg_queue/server/',
                                   shell=True,
                                   stdout=subprocess.PIPE,
                                   preexec_fn=os.setsid)

    client_proc = subprocess.Popen('./client -b {} -c {}'.format(block_size, total_size),
                                   cwd='benchmarks/msg_queue/client/',
                                   shell=True,
                                   stdout=subprocess.PIPE,
                                   preexec_fn=os.setsid)

    client_proc.wait()
    
    for line in client_proc.stdout:
        sys.stdout.write(line)

    os.killpg(os.getpgid(server_proc.pid), signal.SIGTERM)

def ipc_benchmark_run():
    top_msg_queue()

if __name__ == '__main__':
    ipc_benchmark_run()