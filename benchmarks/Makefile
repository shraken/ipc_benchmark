MSG_QUEUE = msg_queue
NAMED_PIPE = named_pipe
PIPE = pipe
SOCKETPAIR = socketpair
TCP = tcp
UDS = uds
ZMQ = zmq

.PHONY: all

all:
	$(MAKE) -C $(MSG_QUEUE)/client
	$(MAKE) -C $(MSG_QUEUE)/server
	$(MAKE) -C $(NAMED_PIPE)/client
	$(MAKE) -C $(NAMED_PIPE)/server
	$(MAKE) -C $(PIPE)/
	$(MAKE) -C $(SOCKETPAIR)/
	$(MAKE) -C $(TCP)/client
	$(MAKE) -C $(TCP)/server
	$(MAKE) -C $(UDS)/client
	$(MAKE) -C $(UDS)/server
	$(MAKE) -C $(ZMQ)/client
	$(MAKE) -C $(ZMQ)/server

clean:
	$(MAKE) -C $(MSG_QUEUE)/client clean
	$(MAKE) -C $(MSG_QUEUE)/server clean
	$(MAKE) -C $(NAMED_PIPE)/client clean
	$(MAKE) -C $(NAMED_PIPE)/server clean
	$(MAKE) -C $(PIPE)/ clean
	$(MAKE) -C $(SOCKETPAIR)/ clean
	$(MAKE) -C $(TCP)/client clean
	$(MAKE) -C $(TCP)/server clean
	$(MAKE) -C $(UDS)/client clean
	$(MAKE) -C $(UDS)/server clean
	$(MAKE) -C $(ZMQ)/client clean
	$(MAKE) -C $(ZMQ)/server clean