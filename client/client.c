#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

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
#define BUFFER_SIZE 8192

int create_socket_and_connect();
void send_request(int client_socket, const uint8_t *request_buffer, size_t request_size);
ssize_t receive_response(int client_socket, uint8_t *response_buffer, size_t buffer_size);
void handle_response(const uint8_t *response_buffer, size_t response_size);

vaccel_VaccelRequest create_session_request(int flags);
vaccel_VaccelRequest update_session_request(int session_id, int flags);
vaccel_VaccelRequest destroy_session_request(int session_id);
vaccel_VaccelResponse return_decoded_response(const uint8_t *response_buffer, size_t response_size);

void print_vaccel_response(const vaccel_VaccelResponse *response);
bool decode_vaccel_response(const uint8_t *buffer, size_t size, vaccel_VaccelResponse *response);




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
    vaccel_VaccelRequest update_request = update_session_request(session_id,2);
    ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    pb_encode(&ostream, vaccel_VaccelRequest_fields, &update_request);
    send_request(client_socket, request_buffer, ostream.bytes_written);

    // Receive and handle update session response
    bytes_received = receive_response(client_socket, response_buffer, sizeof(response_buffer));
    handle_response(response_buffer, bytes_received);

    // Create and send update session request
    vaccel_VaccelRequest destory_request = destroy_session_request(session_id);
    ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    pb_encode(&ostream, vaccel_VaccelRequest_fields, &destory_request);
    send_request(client_socket, request_buffer, ostream.bytes_written);

    // Receive and handle update session response
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

    print_vaccel_response(&response);
    printf("---------------------------------------------------------------\n");
}

vaccel_VaccelResponse return_decoded_response(const uint8_t *response_buffer, size_t response_size) {
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    if (!decode_vaccel_response(response_buffer, response_size, &response)) {
        printf("Decoding VaccelRequest failed\n");
    }

    return response;
}



vaccel_VaccelRequest create_session_request(int flags)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_CreateSessionRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_CreateSessionRequest_tag;
    request.function_args.CreateSessionRequest.flags = flags;

    return request;
}

vaccel_VaccelRequest update_session_request(int session_id, int flags)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_UpdateSessionRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_UpdateSessionRequest_tag;
    request.function_args.UpdateSessionRequest.flags = flags;
    request.function_args.UpdateSessionRequest.session_id = session_id;

    return request;
}

vaccel_VaccelRequest destroy_session_request(int session_id)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_DestroySessionRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_DestroySessionRequest_tag;
    request.function_args.DestroySessionRequest.session_id = session_id;

    return request;
}


bool image_data_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    uint8_t *image_data = (uint8_t*)*arg;
    if (!pb_read(stream, image_data, stream->bytes_left)) {
        return false;
    }
    return true;
}

vaccel_VaccelRequest image_classification(int session_id, const uint8_t* image_data, size_t image_size) {
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_ImageClassificationRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_ImageClassificationRequest_tag;

    request.function_args.ImageClassificationRequest.session_id = session_id;

    pb_byte_t image_encoded[64];
    memset(image_encoded, 0, sizeof(image_encoded));
    memcpy(image_encoded, image_data, image_size > sizeof(image_encoded) ? sizeof(image_encoded) : image_size);

    memcpy(request.function_args.ImageClassificationRequest.image, image_encoded, sizeof(image_encoded));

    return request;
}

void print_vaccel_response(const vaccel_VaccelResponse *response) {
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
        default:
            printf("Invalid function type\n");
    }
}

bool decode_vaccel_response(const uint8_t *buffer, size_t size, vaccel_VaccelResponse *response) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, size);
    return pb_decode(&istream, vaccel_VaccelResponse_fields, response);
}
