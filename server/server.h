#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <stddef.h>

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

int setup_server_socket();
int accept_client_connection(int server_socket);
ssize_t receive_request(int client_socket, uint8_t *request_buffer, size_t buffer_size);
bool handle_request(const uint8_t *request_buffer, size_t request_size, vaccel_VaccelRequest *request);
void send_response(int client_socket, const uint8_t *response_buffer, size_t response_size);
bool decode_vaccel_request(const uint8_t *buffer, size_t size, vaccel_VaccelRequest *request);

vaccel_VaccelResponse create_session_response(int session_id);
vaccel_VaccelResponse update_session_response(int dummy);
vaccel_VaccelResponse destroy_session_response(int dummy);
void print_vaccel_request(const vaccel_VaccelRequest *request);

void process_request_and_send_response(int client_socket, const uint8_t *request_buffer, size_t request_size);
vaccel_VaccelResponse generate_response(const vaccel_VaccelRequest *request); 

#endif /* SERVER_H */
