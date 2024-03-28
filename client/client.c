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
vaccel_VaccelRequest image_classification(int session_id, const char* image_path);
vaccel_VaccelResponse return_decoded_response(const uint8_t *response_buffer, size_t response_size);

void print_vaccel_response(const vaccel_VaccelResponse *response,const uint8_t *buffer, size_t size);
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

    // Create and send image classification request
    const char *image_path = "example.jpg";
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

    print_vaccel_response(&response,response_buffer, response_size);
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


// Function to read an image file and return its byte data
uint8_t* read_image_data(const char* image_path, size_t* image_size) {
    FILE* file = fopen(image_path, "rb");
    if (!file) {
        fprintf(stderr, "Error opening image file: %s\n", image_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(file_size);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Error allocating memory for image data\n");
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != file_size) {
        free(buffer);
        fprintf(stderr, "Error reading image data from file\n");
        return NULL;
    }

    *image_size = (size_t)file_size;
    return buffer;
}

vaccel_VaccelRequest image_classification(int session_id, const char* image_path) {
    size_t image_size;
    uint8_t* image_data = read_image_data(image_path, &image_size);
    if (!image_data) {
        fprintf(stderr, "Error reading image data\n");
    }


    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_ImageClassificationRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_ImageClassificationRequest_tag;

    request.function_args.ImageClassificationRequest.session_id = session_id;

    size_t copy_size = image_size > sizeof(request.function_args.ImageClassificationRequest.image) ?
                       sizeof(request.function_args.ImageClassificationRequest.image) :
                       image_size;
    memcpy(request.function_args.ImageClassificationRequest.image, image_data, copy_size);
    printf("Size of image %zu\n", image_size);
    free(image_data);

    return request;
}

void print_vaccel_response(const vaccel_VaccelResponse *response,const uint8_t *buffer, size_t size) {
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