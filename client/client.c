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

int main() {

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

    // Encoding

    // Prepare the request
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = 2;

    // Set up the create session object with any additional flags
    vaccel_CreateSessionRequest createSessionRequest = vaccel_CreateSessionRequest_init_zero;
    createSessionRequest.flags = 50;

    // Encode the create session object
    pb_byte_t createSessionRequestBuffer[vaccel_CreateSessionRequest_size];
    pb_ostream_t ostream = pb_ostream_from_buffer(createSessionRequestBuffer, sizeof(createSessionRequestBuffer));
    bool success = pb_encode(&ostream, vaccel_CreateSessionRequest_fields, &createSessionRequest);

    if (!success) {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
        return 1;
    }


    // Copy the encoded create session object to the request
    memcpy(request.function_request, createSessionRequestBuffer, ostream.bytes_written);
    printf("Size of function_request: %zu\n", sizeof(request.function_request));

    pb_byte_t request_buffer[vaccel_VaccelRequest_size];
    pb_ostream_t n_ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    success = pb_encode(&n_ostream, vaccel_VaccelRequest_fields, &request);

    if (!success) {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
        return 1;
    }

    // Send the request to the server   
    ssize_t bytes_sent = send(client_socket, request_buffer, ostream.bytes_written, 0);
    if (bytes_sent == -1) {
        perror("Failed to send request to server");
        return 1;
    } else {
        printf("Request sent successfully\n");
        printf("Sent %zd bytes from client\n", bytes_sent);
    }


    close(client_socket);

    return 0;
}