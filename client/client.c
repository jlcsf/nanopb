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

vaccel_VaccelRequest create_session_request(int flags)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = 2;
    request.which_function_args = vaccel_VaccelRequest_CreateSessionRequest_tag;
    request.function_args.CreateSessionRequest.flags = flags;

    return request;
}


int main() {

    vaccel_VaccelRequest request;
    pb_byte_t request_buffer[vaccel_VaccelRequest_size];
    ssize_t bytes_sent;

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

    request = create_session_request(1);

    pb_ostream_t ostream = pb_ostream_from_buffer(request_buffer, sizeof(request_buffer));
    if(!pb_encode(&ostream, vaccel_VaccelRequest_fields, &request)){
        return 1;
    }

    bytes_sent = send(client_socket, request_buffer, ostream.bytes_written, 0);

    close(client_socket);

    return 0;
}