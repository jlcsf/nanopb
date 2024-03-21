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

#define PORT 12346
#define REQUEST_BUFFER_SIZE 8192

int main(int argc, char **argv)
{

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

    char response_buffer[REQUEST_BUFFER_SIZE] = {0};
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

    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelFunctionType_CREATE_SESSION;

    vaccel_CreateSessionRequest createSessionRequest = vaccel_CreateSessionRequest_init_zero;
    createSessionRequest.flags = 2;

    pb_byte_t func_args[8];

    // Create a stream to write into the buffer
    pb_ostream_t ostream = pb_ostream_from_buffer(func_args, sizeof(func_args));

    // Encode the createSessionRequest object into the stream
    if (!pb_encode(&ostream, vaccel_CreateSessionRequest_fields, &createSessionRequest)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&ostream));
        return 1;
    }

    memcpy(request.function_request, func_args, ostream.bytes_written);
    printf("%s", request.function_request);

    uint8_t request_buffer[REQUEST_BUFFER_SIZE];
    pb_ostream_t request_stream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));

    // Encode the VaccelRequest object into the buffer
    if (!pb_encode(&request_stream, vaccel_VaccelRequest_fields, &request)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&request_stream));
        return 1;
    }

    size_t request_length = request_stream.bytes_written;

    // Decode the received protocol buffer message
    // Decode the function request arguments
    vaccel_CreateSessionRequest new_createSessionRequest = vaccel_CreateSessionRequest_init_zero;
    pb_istream_t arg_stream = pb_istream_from_buffer(request.function_request, sizeof(request.function_request));
    if (!pb_decode(&arg_stream, vaccel_CreateSessionRequest_fields, &new_createSessionRequest)) {
        fprintf(stderr, "Decoding vaccel_CreateSessionRequest failed: %s\n", PB_GET_ERROR(&arg_stream));
        return 1;
    }

    // Check if the flags field contains 2
    if (new_createSessionRequest.flags == 2) {
        printf("Flag is set to 2\n");
    } else {
        printf("Flag is not set to 2\n");
    }


    // Decode the function request arguments
    vaccel_CreateSessionRequest new_createSessionRequest = vaccel_CreateSessionRequest_init_zero;
    pb_istream_t arg_stream = pb_istream_from_buffer(new_request.function_request, sizeof(new_request.function_request));
    if (!pb_decode(&arg_stream, vaccel_CreateSessionRequest_fields, &new_createSessionRequest)) {
        fprintf(stderr, "Decoding vaccel_CreateSessionRequest failed: %s\n", PB_GET_ERROR(&arg_stream));
        return 1;
    }

    printf("Decoded function request arguments:\n");
    printf("Flags: %d\n", new_createSessionRequest.flags);

    // Send the serialized VaccelRequest to the server
    if (send(client_socket, request_buffer, request_length, 0) == -1) {
        perror("Send failed");
        return 1;

    }

    printf("Sent bytes to the server\n");

    printf("Recieving bytes from server\n");

    bytes_received = recv(client_socket, response_buffer, sizeof(response_buffer), 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        close(client_socket);
        return 1;
    } else if (bytes_received == 0) {
        printf("Connection closed by server\n");
        close(client_socket);
        return 1;
    } else {
        response_buffer[bytes_received] = '\0';
        printf("Received response from server: %s\n", response_buffer);
    }

    // Decode the response message
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    pb_istream_t new_stream = pb_istream_from_buffer((const pb_byte_t*)response_buffer, bytes_received);
    if (!pb_decode(&stream, vaccel_VaccelResponse_fields, &response)) {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        close(client_socket);
        return 1;
    }

    printf("Decoded response message:\n");
    printf("Function Type: %d\n", response.function_type);

    // Decode the create session response
    vaccel_CreateSessionResponse createSessionResponse = vaccel_CreateSessionResponse_init_zero;
    if (!pb_decode(&stream, vaccel_CreateSessionResponse_fields, &createSessionResponse)) {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        close(client_socket);
        return 1;
    }

    printf("Decoded create session response:\n");
    printf("Session ID: %d\n", createSessionResponse.session_id);

    close(client_socket);

    return 0;
}