#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 12345
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

    // Receive initial message from client
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        close(client_socket);
        close(server_socket);
        return 1;
    }

    printf("Received initial message from client: %s\n", buffer);

    // Send response to initial message
    const char *response_message = "Server received your initial message";
    ssize_t bytes_sent = send(client_socket, response_message, strlen(response_message), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        close(client_socket);
        close(server_socket);
        return 1;
    }

    // Receive protocol buffer message from client
    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        close(client_socket);
        close(server_socket);
        return 1;
    }

    // Decode the received protocol buffer message
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    pb_istream_t stream = pb_istream_from_buffer((uint8_t *)buffer, bytes_received);
    if (!pb_decode(&stream, vaccel_VaccelRequest_fields, &request)) {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        close(client_socket);
        close(server_socket);
        return 1;
    }

    // Print decoded message
    printf("Decoded message:\n");
    printf("Function Type: %d\n", request.function_type);

    // Send response back to client
    const char *response_message_pb = "Server received your protocol buffer message";
    bytes_sent = send(client_socket, response_message_pb, strlen(response_message_pb), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        close(client_socket);
        close(server_socket);
        return 1;
    }

    printf("Response sent to client\n");

    // Close sockets
    close(client_socket);
    close(server_socket);

    return 0;
}
