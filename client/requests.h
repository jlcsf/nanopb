#ifndef VACCEL_REQUESTS_H
#define VACCEL_REQUESTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <arpa/inet.h>

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

vaccel_VaccelRequest create_session_request(int flags);
vaccel_VaccelRequest update_session_request(int session_id, int flags);
vaccel_VaccelRequest destroy_session_request(int session_id);
uint8_t* read_image_data(const char* image_path, size_t* image_size);
vaccel_VaccelRequest image_classification(int session_id, const char* image_path);

#endif // VACCEL_REQUESTS_H
