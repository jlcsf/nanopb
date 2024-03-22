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

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("Accept failed");
        close(server_socket);
        return 1;
    }

    printf("Client connected\n");

    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("Failed to receive data from client");
        close(client_socket);
        close(server_socket);
        return 1;
    }

    printf("Received %zd bytes from client\n", bytes_received);

    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, bytes_received);
    bool decode_status = pb_decode(&istream, vaccel_VaccelRequest_fields, &request);

    if (!decode_status) {
        printf("Decoding VaccelRequest failed: %s\n", PB_GET_ERROR(&istream));
        close(client_socket);
        close(server_socket);
        return 1;
    }


    if (request.which_function_args == vaccel_VaccelRequest_CreateSessionRequest_tag) {
        printf("Function Arguments (CreateSessionRequest):\n");
        printf("Flags: %u\n", request.function_args.CreateSessionRequest.flags);
    } else if (request.which_function_args == vaccel_VaccelRequest_UpdateSessionRequest_tag) {
        printf("Do nothing\n");
    } else {
        printf("Do nothing\n");
    }

    close(client_socket);
    close(server_socket);

    return 0;

}


