import sys
import os
import time
import subprocess
import signal

def msg_queue_test():
    print 'msg_queue_test'

    server_proc = subprocess.Popen('./server -a -b 1024 -c 1048576000',
                                   cwd='benchmarks/msg_queue/server/',
                                   shell=True,
                                   stdout=subprocess.PIPE)

    client_proc = subprocess.Popen('./client -a -b 1024 -c 1048576000',
                                   cwd='benchmarks/msg_queue/client/',
                                   shell=True,
                                   stdout=subprocess.PIPE)

    client_proc.wait()
    
    for line in client_proc.stdout:
        sys.stdout.write(line)

    os.killpg(os.getpgid(server_proc.pid), signal.SIGTERM)

def ipc_benchmark_run():
    msg_queue_test()

if __name__ == '__main__':
    ipc_benchmark_run()