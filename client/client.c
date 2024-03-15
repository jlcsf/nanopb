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

#define PORT 12345
#define REQUEST_BUFFER_SIZE 8192

int main(int argc, char **argv)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelFunctionType_CREATE_SESSION;

    vaccel_CreateSessionRequest createSessionRequest = vaccel_CreateSessionRequest_init_zero;
    createSessionRequest.flags = 0;

    uint8_t createSessionRequestBuffer[vaccel_CreateSessionRequest_size];
    pb_ostream_t createSessionRequestStream = pb_ostream_from_buffer(createSessionRequestBuffer, sizeof(createSessionRequestBuffer));
    if (!pb_encode(&createSessionRequestStream, vaccel_CreateSessionRequest_fields, &createSessionRequest)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&createSessionRequestStream));
        return 1;
    }

    size_t message_length = createSessionRequestStream.bytes_written;
    request.function_request.arg = &createSessionRequestStream;
    //request.function_request.funcs.encode = (bool (*)(pb_ostream_t *, const pb_field_t *, void * const*)) &pb_encode_string;


    /// connect to server

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == -1) {
        perror("Socket creation failed");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
    }

    const char* initial_message = "Connecting to server";
    if (send(client_socket, initial_message, strlen(initial_message), 0) == -1) {
        perror("Send failed");
    }

    char response_buffer[REQUEST_BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, response_buffer, sizeof(response_buffer), 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        return 1;
    } else if (bytes_received == 0) {
        printf("Connection closed by server\n");
    } else {
        response_buffer[bytes_received] = '\0'; 
        printf("Received response from server: %s\n", response_buffer);
    }

    // Send the vaccel request object to the server
    uint8_t request_buffer[REQUEST_BUFFER_SIZE];
    pb_ostream_t request_stream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));

    // Encode the VaccelRequest object into the buffer
    if (!pb_encode(&request_stream, vaccel_VaccelRequest_fields, &request)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&request_stream));
        return 1;
    }

    // Calculate the length of the serialized message
    size_t request_length = request_stream.bytes_written;

    // Send the serialized VaccelRequest to the server
    if (send(client_socket, request_buffer, request_length, 0) == -1) {
        perror("Send failed");
        return 1;
    }

    return 0;
}