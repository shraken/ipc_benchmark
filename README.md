# ipc_benchmark

Standard IPC benchmark examples.  Binaries allow specification of block size and
total transfer size.  A randomized dataset is always transferred to minimize
cache influence ensuring worst-cse performance for the test.

An evaluation python test script to serially run the IPC benchmarks collecting
the results into a report is included.  

The following IPC tests are provided:
* pipe
* named_pipe
* unix domain sockets (uds)
* tcp
* udp
* shm
* message queue

# results

TBD