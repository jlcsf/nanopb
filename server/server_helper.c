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

vaccel_VaccelRequest decode_request(int client_socket, int server_socket, uint8_t* response_buffer, ssize_t recv_bytes)
{

    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    pb_istream_t stream = pb_istream_from_buffer((uint8_t *)response_buffer, recv_bytes);
    if (!pb_decode(&stream, vaccel_VaccelRequest_fields, &request)) {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        close(client_socket);
        close(server_socket);
    }

    printf("Decoded message:\n");
    printf("Function Type: %d\n", request.function_type);

    vaccel_CreateSessionRequest createSessionRequest = vaccel_CreateSessionRequest_init_zero;
    if (!pb_decode(&stream, vaccel_CreateSessionRequest_fields, &createSessionRequest)) {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        close(client_socket);
        close(server_socket);
    }

    // Print decoded message
    printf("Decoded message:\n");
    printf("Flags: %d\n", createSessionRequest.flags);

    return request;

}

void send_response(int client_socket, int server_socket)
{
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelFunctionType_CREATE_SESSION;

    vaccel_CreateSessionResponse createSessionResponse = vaccel_CreateSessionResponse_init_zero;
    createSessionResponse.session_id = 11;

    uint8_t createSessionResponseBuffer[vaccel_CreateSessionResponse_size];
    pb_ostream_t createSessionResponseStream = pb_ostream_from_buffer(createSessionResponseBuffer, sizeof(createSessionResponseBuffer));
    if (!pb_encode(&createSessionResponseStream, vaccel_CreateSessionResponse_fields, &createSessionResponse)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&createSessionResponseStream));
    }

    size_t message_length = createSessionResponseStream.bytes_written;
    response.function_response.arg = &createSessionResponseStream;
    //request.function_request.funcs.encode = (bool (*)(pb_ostream_t *, const pb_field_t *, void * const*)) &pb_encode_string;

    // Send the vaccel request object to the server
    uint8_t response_buffer[BUFFER_SIZE];
    pb_ostream_t response_stream = pb_ostream_from_buffer(response_buffer, sizeof(response_buffer));

    // Encode the VaccelRequest object into the buffer
    if (!pb_encode(&response_stream, vaccel_VaccelResponse_fields, &response)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&response_stream));
    }

    // Calculate the length of the serialized message
    size_t response_length = response_stream.bytes_written;

    // Send the serialized VaccelResponse to the client
    if (send(client_socket, response_buffer, response_length, 0) == -1) {
        perror("Send failed");
    }

    printf("Sent response back to the client\n");
}


int main(){

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Prepare server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    // Listen for connections
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("Accept failed");
        close(server_socket);
        return 1;
    }

    printf("Client connected\n");

    // Receive protocol buffer message from client
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

    vaccel_VaccelRequest request = decode_request(client_socket,server_socket, buffer, bytes_received);
    
    send_response(client_socket, server_socket);

}