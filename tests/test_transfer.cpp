//
//  Hello World server
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
//  Changes for 2.1:
//  - added socket close before terminating
//
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void transfer(/*const*/ char* data, const char *transport);
void receive(const char *transport);

struct args_t {
	int is_client;
	char *data;
	char *transport;
};

int parse_args(int argc_, char *argv_[], args_t *args_);
void usage();

int main (int argc, char *argv[]) 
{
	args_t args = {0};

	if (!parse_args(argc, argv, &args)) {
		usage();
		exit(EXIT_FAILURE);
	}

	if (args.is_client) {
		transfer(args.data, args.transport);
	}
	else {
		receive(args.transport);
	}

	exit(EXIT_SUCCESS);
}

void transfer(/*const*/ char* data, const char *transport) {
	void *context = zmq_init (1);

    //  Socket to talk to server
    printf ("Connecting to hello world server %s...\n", transport);
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, transport);

	if (!strncmp(data, "file://", 7)) {
		FILE* fd = fopen(data + 7, "r");
		char chunk[256] = {0};
		int nbytes = fread(chunk, sizeof(*chunk), sizeof(chunk), fd);
		int nchunk = 0;
		while (nbytes > 0) {
			++nchunk;
			zmq_msg_t request;
        	zmq_msg_init_data (&request, chunk, nbytes, NULL, NULL);
        	printf ("Sending file chunk %d...\n", nchunk);
			zmq_send (requester, &request, 0);
        	zmq_msg_close (&request);

			zmq_msg_t reply;
    		zmq_msg_init (&reply);
    		zmq_recv (requester, &reply, 0);
    		printf ("Server replied with %d bytes:\r\n", (int)zmq_msg_size (&reply));
    		printf ("%s\r\n", (char *)zmq_msg_data (&reply));
    		zmq_msg_close (&reply);

			nbytes = fread(chunk, sizeof(*chunk), sizeof(chunk), fd);
		}
		fclose(fd);
	}
	else {
		zmq_msg_t request;
        zmq_msg_init_data (&request, data, strlen(data), NULL, NULL);
        printf ("Sending data...\r\n");
        zmq_send (requester, &request, 0);
        zmq_msg_close (&request);

		zmq_msg_t reply;
    	zmq_msg_init (&reply);
    	zmq_recv (requester, &reply, 0);
    	printf ("Server replied with %d bytes:\r\n", (int)zmq_msg_size (&reply));
    	printf ("%s\r\n", (char *)zmq_msg_data (&reply));
    	zmq_msg_close (&reply);
	}

	zmq_close (requester);
    zmq_term (context);
}

void receive(const char *transport) {
	void *context = zmq_init (1);

    //  Socket to talk to clients
    void *responder = zmq_socket (context, ZMQ_REP);
    zmq_bind (responder, transport);

	char response[256] = {0};

    while (1) {
        //  Wait for next request from client
        zmq_msg_t request;
        zmq_msg_init (&request);
        zmq_recv (responder, &request, 0);
		sprintf(response, "Received %d bytes", (int)zmq_msg_size (&request));
        printf ("%s:\r\n", response);
		printf ("%s\r\n", (char *)zmq_msg_data (&request));
        zmq_msg_close (&request);

        //  Do some 'work'
        sleep (1);

        //  Send reply back to client
        zmq_msg_t reply;
        zmq_msg_init_data (&reply, response, strlen(response), NULL, NULL);
        zmq_send (responder, &reply, 0);
        zmq_msg_close (&reply);
    }
    //  We never get here but if we did, this would be how we end
    zmq_close (responder);
    zmq_term (context);
}

int parse_args(int argc_, char *argv_[], args_t *args_) {

	memset(args_, 0, sizeof(args_t));

	while (--argc_) {
		if (!strncmp(argv_[argc_], "--client", 8)) {
			args_->is_client = true;
		}
		else if (!strncmp(argv_[argc_], "--data", 6)) {
			args_->data = argv_[argc_] + 7;
		}
		else if (!strncmp(argv_[argc_], "--transport", 11)) {
			args_->transport = argv_[argc_] + 12;
		}
	}

	if (!args_->transport) { return 0; }

	if (args_->is_client) {
		if (!args_->data) { return 0; }		
	}

	return 1;
}

void usage() {
	printf("%s",
		"Transfers specified data from client to server over the 0MQ\r\n" \
        "--client\r\n" \
        "    if this flag is not provided waiting for incoming data\r\n" \
        "--data=[file://<file-path>|some-message]\r\n" \
        "    file o message to transfer\r\n" \
        "--transport=[http://0.0.0.0:5556]\r\n" \
        "    specifies transport to be used\r\n");
}
