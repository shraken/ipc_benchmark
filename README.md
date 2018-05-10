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
4. tcp
6. shm
7. message queue

# results

TBD