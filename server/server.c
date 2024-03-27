#include <session.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 12346
#define BUFFER_SIZE 8192

#include "server.h"
#include "python_bindings.h"
#include "vaccel.h"


int main() {

    int server_socket = setup_server_socket();
    if (server_socket == -1) {
        return 1;
    }

    int client_socket = accept_client_connection(server_socket);
    if (client_socket == -1) {
        close(server_socket);
        return 1;
    }

    char buffer[BUFFER_SIZE] = {0};
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    vaccel_VaccelResponse response;
    ssize_t bytes_received;
    pb_byte_t response_buffer[BUFFER_SIZE];
    pb_ostream_t ostream;

    struct vaccel_session sess;
    printf("---------------------------------------------------------------\n");
    // recieved request to create a session 
    bytes_received = receive_request(client_socket, buffer, sizeof(buffer));
    process_request_and_send_response(client_socket, (const uint8_t *)buffer, bytes_received, sess);
    printf("---------------------------------------------------------------\n");
    // recieved request to update a session 
    bytes_received = receive_request(client_socket, buffer, sizeof(buffer));
    process_request_and_send_response(client_socket, (const uint8_t *)buffer, bytes_received, sess);
    printf("---------------------------------------------------------------\n");
    // recieved request to destroy a session 
    bytes_received = receive_request(client_socket, buffer, sizeof(buffer));
    process_request_and_send_response(client_socket, (const uint8_t *)buffer, bytes_received, sess);
    printf("---------------------------------------------------------------\n");

    close(client_socket);
    close(server_socket);

    return 0;

}

int setup_server_socket() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        return -1;
    }

    printf("Server listening on port %d...\n", PORT);
    return server_socket;
}

int accept_client_connection(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("Accept failed");
        close(server_socket);
        return -1;
    }

    printf("Client connected\n");
    return client_socket;
}

ssize_t receive_request(int client_socket, uint8_t *request_buffer, size_t buffer_size) {
    ssize_t bytes_received = recv(client_socket, request_buffer, buffer_size, 0);
    if (bytes_received == -1) {
        close(client_socket);
        return 1;
    }

    printf("Received %zd bytes from client\n", bytes_received);
    return bytes_received;
}

bool handle_request(const uint8_t *request_buffer, size_t request_size, vaccel_VaccelRequest *request) {
    pb_istream_t istream = pb_istream_from_buffer(request_buffer, request_size);
    return pb_decode(&istream, vaccel_VaccelRequest_fields, request);
}

void send_response(int client_socket, const uint8_t *response_buffer, size_t response_size) {
    ssize_t bytes_sent = send(client_socket, response_buffer, response_size, 0);
    if (bytes_sent == -1) {
        perror("Failed to send response to client");
    }
}


bool decode_vaccel_request(const uint8_t *buffer, size_t size, vaccel_VaccelRequest *request) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, size);
    return pb_decode(&istream, vaccel_VaccelRequest_fields, request);
}

void print_vaccel_request(const vaccel_VaccelRequest *request) {
    switch (request->which_function_args) {
        case vaccel_VaccelRequest_CreateSessionRequest_tag:
            printf("Function Arguments (CreateSessionRequest):\n");
            printf("Flags: %u\n", request->function_args.CreateSessionRequest.flags);
            break;
        case vaccel_VaccelRequest_UpdateSessionRequest_tag:
            printf("Function Arguments (UpdateSessionRequest):\n");
            printf("Flags: %u\n", request->function_args.UpdateSessionRequest.flags);
            printf("SessionID: %u\n", request->function_args.UpdateSessionRequest.session_id);
            break;
        case vaccel_VaccelRequest_DestroySessionRequest_tag:
            printf("Function Arguments (DestroySessionRequest):\n");
            printf("SessionID: %u\n", request->function_args.DestroySessionRequest.session_id);
            break;
        default:
            printf("Invalid function type\n");
    }
}

vaccel_VaccelResponse create_session_response(struct vaccel_session sess ,int flags)
{
    
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelResponse_CreateSessionResponse_tag;
    response.which_function_args = vaccel_VaccelResponse_CreateSessionResponse_tag;

    int output;

    printf("Creating a session with the flags:%u\n" ,flags);
    printf("Creating a session_id with the id:%u\n" ,sess.session_id);

    //output = vaccel_python_create_session(session_id);

    output = create_vaccel_session(&sess, flags);  // use vaccel...

    response.function_args.CreateSessionResponse.session_id = output;

    printf("Output is :%u\n" ,output);

    return response;
}

vaccel_VaccelResponse update_session_response(struct vaccel_session sess, int flags)
{
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelResponse_UpdateSessionResponse_tag;
    response.which_function_args = vaccel_VaccelResponse_UpdateSessionResponse_tag;

    printf("Updating the session with the flags:%u\n" ,flags);

    int output = update_vaccel_session(&sess, flags);

    response.function_args.UpdateSessionResponse.success = 1;
    return response;
}

vaccel_VaccelResponse destroy_session_response(struct vaccel_session sess)
{   
    int dummy;
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelResponse_DestroySessionResponse_tag;
    response.which_function_args = vaccel_VaccelResponse_DestroySessionResponse_tag;

    printf("Destroying the session with the session_id:%u\n" ,sess.session_id);

    int output = vaccel_sess_free(&sess);

    response.function_args.DestroySessionResponse.success = 1;
    return response;
}

void process_request_and_send_response(int client_socket, const uint8_t *request_buffer, size_t request_size, struct vaccel_session sess) {
    vaccel_VaccelRequest request;
    if (!handle_request(request_buffer, request_size, &request)) {
        perror("Failed to handle request");
        return;
    }

    print_vaccel_request(&request);

    vaccel_VaccelResponse response = generate_response(sess, &request);

    pb_byte_t response_buffer[BUFFER_SIZE];
    pb_ostream_t ostream = pb_ostream_from_buffer(response_buffer, sizeof(response_buffer));
    if (!pb_encode(&ostream, vaccel_VaccelResponse_fields, &response)) {
        perror("Failed to encode response");
        return;
    }

    send_response(client_socket, response_buffer, ostream.bytes_written);
}

vaccel_VaccelResponse generate_response(struct vaccel_session sess,const vaccel_VaccelRequest *request) {
    vaccel_VaccelResponse response;

    switch (request->which_function_args) {
        case vaccel_VaccelRequest_CreateSessionRequest_tag:
            response = create_session_response(sess, request->function_args.CreateSessionRequest.flags);
            break;
        case vaccel_VaccelRequest_UpdateSessionRequest_tag:
            response = update_session_response(sess, request->function_args.UpdateSessionRequest.flags);
            break;
        case vaccel_VaccelRequest_DestroySessionRequest_tag:
            response = destroy_session_response(sess);
            break;
        default:
            printf("Invalid function type");
            break;
    }

    return response;
}