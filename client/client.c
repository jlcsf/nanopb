#include "client.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main() {
    int client_socket = create_socket_and_connect();

    if (client_socket == -1) {
        return 1;
    }

    // Create and send create session request
    vaccel_VaccelRequest create_request = create_session_request(1);
    uint8_t request_buffer[vaccel_VaccelRequest_size];
    pb_ostream_t ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    pb_encode(&ostream, vaccel_VaccelRequest_fields, &create_request);
    send_request(client_socket, request_buffer, ostream.bytes_written);

    // Receive and handle create session response
    uint8_t response_buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_received = receive_response(client_socket, response_buffer, sizeof(response_buffer));
    handle_response(response_buffer, bytes_received);

    vaccel_VaccelResponse response = return_decoded_response(response_buffer, bytes_received);

    int session_id = response.function_args.CreateSessionResponse.session_id;

    // Create and send update session request
    vaccel_VaccelRequest update_request = update_session_request(session_id, 2);
    ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    pb_encode(&ostream, vaccel_VaccelRequest_fields, &update_request);
    send_request(client_socket, request_buffer, ostream.bytes_written);

    // Receive and handle update session response
    bytes_received = receive_response(client_socket, response_buffer, sizeof(response_buffer));
    handle_response(response_buffer, bytes_received);

    // Create and send image classification request
    const char *image_path = "/usr/local/share/images/example.jpg";
    vaccel_VaccelRequest image_request = image_classification(session_id, image_path);
    ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    pb_encode(&ostream, vaccel_VaccelRequest_fields, &image_request);
    send_request(client_socket, request_buffer, ostream.bytes_written);

    // Receive and handle image classification response
    bytes_received = receive_response(client_socket, response_buffer, sizeof(response_buffer));
    handle_response(response_buffer, bytes_received);

    // Create and send destroy session request
    vaccel_VaccelRequest destroy_request = destroy_session_request(session_id);
    ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    pb_encode(&ostream, vaccel_VaccelRequest_fields, &destroy_request);
    send_request(client_socket, request_buffer, ostream.bytes_written);

    // Receive and handle destroy session response
    bytes_received = receive_response(client_socket, response_buffer, sizeof(response_buffer));
    handle_response(response_buffer, bytes_received);

    close(client_socket);

    return 0;
}

int create_socket_and_connect() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        return -1;
    }

    return client_socket;
}

void send_request(int client_socket, const uint8_t *request_buffer, size_t request_size) {
    ssize_t bytes_sent = send(client_socket, request_buffer, request_size, 0);
    if (bytes_sent == -1) {
        perror("Failed to send request to server");
        close(client_socket);
        exit(1);
    }
}

ssize_t receive_response(int client_socket, uint8_t *response_buffer, size_t buffer_size) {
    ssize_t bytes_received = recv(client_socket, response_buffer, buffer_size, 0);
    if (bytes_received == -1) {
        perror("Failed to receive data from server");
        close(client_socket);
        exit(1);
    }
    return bytes_received;
}

void handle_response(const uint8_t *response_buffer, size_t response_size) {
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    if (!decode_vaccel_response(response_buffer, response_size, &response)) {
        printf("Decoding VaccelRequest failed\n");
        return;
    }

    print_vaccel_response(&response, response_buffer, response_size);
    printf("---------------------------------------------------------------\n");
}

vaccel_VaccelResponse return_decoded_response(const uint8_t *response_buffer, size_t response_size) {
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    if (!decode_vaccel_response(response_buffer, response_size, &response)) {
        printf("Decoding VaccelRequest failed\n");
    }

    return response;
}

void print_vaccel_response(const vaccel_VaccelResponse *response, const uint8_t *buffer, size_t size) {
    switch (response->which_function_args) {
        case vaccel_VaccelResponse_CreateSessionResponse_tag:
            printf("Function Arguments (CreateSessionResponse):\n");
            printf("Session_ID: %u\n", response->function_args.CreateSessionResponse.session_id);
            break;
        case vaccel_VaccelResponse_UpdateSessionResponse_tag:
            printf("Updated Session flags\n");
            break;
        case vaccel_VaccelResponse_DestroySessionResponse_tag:
            printf("Destroyed Session flags\n");
            break;
        case vaccel_VaccelResponse_ImageClassificationResponse_tag:
            printf("Image classification complete\n");
            printf("Output tag: \n");
            char decode_into_here[512];
            pb_istream_t stream_decoded = pb_istream_from_buffer((pb_byte_t*)response->function_args.ImageClassificationResponse.tags,
                                                                 sizeof(response->function_args.ImageClassificationResponse.tags));
            pb_read(&stream_decoded, (pb_byte_t*)decode_into_here, stream_decoded.bytes_left);
            printf("Decoded Classification tags: %s\n", decode_into_here);
            break;
        default:
            printf("Invalid function type\n");
    }
}

bool decode_vaccel_response(const uint8_t *buffer, size_t size, vaccel_VaccelResponse *response) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, size);
    return pb_decode(&istream, vaccel_VaccelResponse_fields, response);
}