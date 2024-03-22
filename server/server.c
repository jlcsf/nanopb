#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 12346
#define BUFFER_SIZE 8192

#include "../nanopb/pb_encode.h"
#include "../nanopb/pb_decode.h"

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

int main() {
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

    // Receive data from client
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("Failed to receive data from client");
        close(client_socket);
        close(server_socket);
        return 1;
    }

    printf("Received %zd bytes from client\n", bytes_received);

    // Decode the received data into a VaccelRequest object
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, bytes_received);
    bool decode_status = pb_decode(&istream, vaccel_VaccelRequest_fields, &request);

    printf("Decoded VaccelRequest:\n");
    printf("Function Type: %d\n", request.function_type);

    // Decode the request.function_request object

    vaccel_CreateSessionRequest createSessionRequest = vaccel_CreateSessionRequest_init_zero;
    pb_istream_t args_stream = pb_istream_from_buffer(request.function_request, sizeof(request.function_request));
    bool args_status = pb_decode(&args_stream, vaccel_CreateSessionRequest_fields, &createSessionRequest);

    // Check if decoding the function arguments was successful
    if (!args_status) {
        printf("Decoding function arguments failed: %s\n", PB_GET_ERROR(&args_stream));
        close(client_socket);
        close(server_socket);
        return 1;
    }

    // Print additional fields in the create session request
    printf("Flags in CreateSessionRequest: %d\n", createSessionRequest.flags);


    // Close sockets
    close(client_socket);
    close(server_socket);

    return 0;
}

