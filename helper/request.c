/// this function sends a request
/// first creates/decodes the protobuf object which is being sent and then seralises the data

#include <stdio.h>
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

int request(int client_socket)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelFunctionType_CREATE_SESSION;

    // create the request and then populise the fields
    // for now just use the session create

    vaccel_CreateSessionRequest vaccel_args = vaccel_CreateSessionRequest_init_zero;
    vaccel_args.flags = 0;

    uint8_t request_buffer[vaccel_CreateSessionRequest_size];

    pb_ostream_t request_buffer_stream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));

    if (!pb_encode(&request_buffer_stream, vaccel_CreateSessionRequest_fields, &vaccel_args))
    {
        fprintf(stderr, "Failed to encode: %s\n", PB_GET_ERROR(&request_buffer_stream));
    }

    size_t message_length = request_buffer_stream.bytes_written;
    request.function_request.arg = &request_buffer_stream;

    uint8_t request_buffer_send[BUFFER_SIZE];
    pb_ostream_t request_stream = pb_ostream_from_buffer(request_buffer_send, sizeof(request_buffer_send));

    if (!pb_encode(&request_stream, vaccel_VaccelRequest_fields, &request))
    {
        fprintf(stderr, "Failed to eoncode: %s\n", PB_GET_ERROR(&request_stream));
    }

    // length of request stream

    size_t request_length = request_stream.bytes_written;

    // send request to the server/client

    if (send(client_socket, request_buffer_send, request_length, 0) == -1)
    {
        perror("Failed to send message");
        return 1;
    }
}

int main(){
    return 0;
}