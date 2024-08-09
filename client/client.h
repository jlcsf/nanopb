#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include "../nanopb/pb_encode.h"
#include "../nanopb/pb_decode.h"
#include "../headers/agent.pb.h"
#include "../headers/error.pb.h"
#include "../headers/genop.pb.h"
#include "../headers/image.pb.h"
#include "../headers/profiling.pb.h"
#include "../headers/resources.pb.h"
#include "../headers/session.pb.h"
#include "../headers/tensorflow.pb.h"
#include "../headers/torch.pb.h"
#include "requests.h"

#define PORT 12346
#define BUFFER_SIZE 8192

// Function declarations
int create_socket_and_connect();
void send_request(int client_socket, const uint8_t *request_buffer, size_t request_size);
ssize_t receive_response(int client_socket, uint8_t *response_buffer, size_t buffer_size);
void handle_response(const uint8_t *response_buffer, size_t response_size);
vaccel_VaccelResponse return_decoded_response(const uint8_t *response_buffer, size_t response_size);
void print_vaccel_response(const vaccel_VaccelResponse *response, const uint8_t *buffer, size_t size);
bool decode_vaccel_response(const uint8_t *buffer, size_t size, vaccel_VaccelResponse *response);

#endif // CLIENT_H
