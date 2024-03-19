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
#define BUFFER_SIZE 8192

vaccel_VaccelResponse decode_response(int client_socket, uint8_t* response_buffer, ssize_t recv_bytes)
{

    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    pb_istream_t stream = pb_istream_from_buffer((uint8_t *)response_buffer, recv_bytes);
    if (!pb_decode(&stream, vaccel_VaccelResponse_fields, &response)) {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        close(client_socket);
    }

    // Decode the create session response
    vaccel_CreateSessionResponse createSessionResponse = vaccel_CreateSessionResponse_init_zero;
    if (!pb_decode(&stream, vaccel_CreateSessionResponse_fields, &createSessionResponse)) {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        close(client_socket);
    }

    printf("Decoded create session response:\n");
    printf("Response type: %d\n", response.function_type);
    printf("Session ID: %d\n", createSessionResponse.session_id);

    return response;


}

void send_request(int client_socket)
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
    }
}



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

    send_request(client_socket);

    printf("Sent bytes to the server\n");

    bytes_received = recv(client_socket, response_buffer, sizeof(response_buffer), 0);
    
    printf("Recieving bytes from server\n");

    vaccel_VaccelResponse response;

    response = decode_response(client_socket, response_buffer, bytes_received);

    close(client_socket);

    return 0;
}