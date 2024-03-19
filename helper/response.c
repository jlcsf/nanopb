/// this function sends a response after recieving a response
/// first creates/decodes the protobuf object which is being sent and then seralises the data

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "../nanopb/pb_encode.h"
#include <../nanopb/pb_decode.h>

#include "../headers/agent.pb.h"
#include "../headers/error.pb.h"
#include "../headers/genop.pb.h"
#include "../headers/image.pb.h"
#include "../headers/genop.pb.h"
#include "../headers/image.pb.h"
#include "../headers/profiling.pb.h"
#include "../headers/resources.pb.h"
#include "../headers/session.pb.h"
#include "../headers/tensorflow.pb.h"
#include "../headers/torch.pb.h"

#define BUFFER_SIZE 8192


int main(int argc, char *argv[])
{
    int client_socket = atoi(argv[1]);

    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelFunctionType_CREATE_RESOURCE;

    vaccel_CreateSessionResponse vaccel_args = vaccel_CreateSessionResponse_init_zero;
    vaccel_args.session_id = 11;

    uint8_t response_buffer[BUFFER_SIZE];

    pb_ostream_t response_stream = pb_ostream_from_buffer(response_buffer, sizeof(response_buffer));
    if (!pb_encode(&response_stream, vaccel_CreateSessionResponse_fields, &vaccel_args)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&response_stream));
        return 1;
    }

    size_t response_length = response_stream.bytes_written;
    
    response.function_response.arg = &response_stream;


    uint8_t response_message_buffer[BUFFER_SIZE];
    pb_ostream_t response_message_stream = pb_ostream_from_buffer(response_message_buffer, sizeof(response_message_buffer));

    if (!pb_encode(&response_message_stream, vaccel_VaccelResponse_fields, &response)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&response_message_stream));
        return 1;
    }

    size_t response_msg_length = response_message_stream.bytes_written;

    if (send(client_socket, response_message_buffer, response_msg_length, 0) == -1) {
        perror("Send failed");
        return 1;
    }


}
