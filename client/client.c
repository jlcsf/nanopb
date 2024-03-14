#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

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

int main(int argc, char **argv)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelFunctionType_CREATE_SESSION;

    vaccel_CreateSessionRequest createSessionRequest = vaccel_CreateSessionRequest_init_zero;

    uint8_t createSessionRequestBuffer[vaccel_CreateSessionRequest_size];
    pb_ostream_t createSessionRequestStream = pb_ostream_from_buffer(createSessionRequestBuffer, sizeof(createSessionRequestBuffer));
    if (!pb_encode(&createSessionRequestStream, vaccel_CreateSessionRequest_fields, &createSessionRequest)) {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&createSessionRequestStream));
        return 1;
    }

    request.function_request.arg = &createSessionRequestStream;
    request.function_request.funcs.encode = (bool (*)(pb_ostream_t *, const pb_field_t *, void * const*)) &pb_encode_string;
}