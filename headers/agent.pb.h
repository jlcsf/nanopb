/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9-dev */

#ifndef PB_VACCEL_AGENT_PB_H_INCLUDED
#define PB_VACCEL_AGENT_PB_H_INCLUDED
#include <pb.h>
#include "session.pb.h"
#include "resources.pb.h"
#include "image.pb.h"
#include "tensorflow.pb.h"
#include "torch.pb.h"
#include "genop.pb.h"
#include "profiling.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _vaccel_VaccelFunctionType {
    vaccel_VaccelFunctionType_UNUSED = 0,
    vaccel_VaccelFunctionType_CREATE_SESSION = 1,
    vaccel_VaccelFunctionType_UPDATE_SESSION = 2,
    vaccel_VaccelFunctionType_DESTROY_SESSION = 3,
    vaccel_VaccelFunctionType_CREATE_RESOURCE = 4,
    vaccel_VaccelFunctionType_REGISTER_RESOURCE = 5,
    vaccel_VaccelFunctionType_UNREGISTER_RESOURCE = 6,
    vaccel_VaccelFunctionType_DESTROY_RESOURCE = 7,
    vaccel_VaccelFunctionType_IMAGE_CLASSIFICATION = 8,
    vaccel_VaccelFunctionType_TENSORFLOW_MODEL_LOAD = 9,
    vaccel_VaccelFunctionType_TENSORFLOW_MODEL_UNLOAD = 10,
    vaccel_VaccelFunctionType_TENSORFLOW_MODEL_RUN = 11,
    vaccel_VaccelFunctionType_TORCH_JITLOAD_FORWARD = 12,
    vaccel_VaccelFunctionType_GENOP = 13,
    vaccel_VaccelFunctionType_GET_TIMERS = 14,
    vaccel_VaccelFunctionType_VACCEL_FUNCTION = 15
} vaccel_VaccelFunctionType;

/* Struct definitions */
typedef struct _vaccel_VaccelEmpty {
    char dummy_field;
} vaccel_VaccelEmpty;

typedef struct _vaccel_VaccelRequest {
    vaccel_VaccelFunctionType function_type;
    pb_byte_t function_request[64];
} vaccel_VaccelRequest;

typedef struct _vaccel_VaccelResponse {
    vaccel_VaccelFunctionType function_type;
    pb_byte_t function_response[64];
} vaccel_VaccelResponse;


#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _vaccel_VaccelFunctionType_MIN vaccel_VaccelFunctionType_UNUSED
#define _vaccel_VaccelFunctionType_MAX vaccel_VaccelFunctionType_VACCEL_FUNCTION
#define _vaccel_VaccelFunctionType_ARRAYSIZE ((vaccel_VaccelFunctionType)(vaccel_VaccelFunctionType_VACCEL_FUNCTION+1))


#define vaccel_VaccelRequest_function_type_ENUMTYPE vaccel_VaccelFunctionType

#define vaccel_VaccelResponse_function_type_ENUMTYPE vaccel_VaccelFunctionType


/* Initializer values for message structs */
#define vaccel_VaccelEmpty_init_default          {0}
#define vaccel_VaccelRequest_init_default        {_vaccel_VaccelFunctionType_MIN, {0}}
#define vaccel_VaccelResponse_init_default       {_vaccel_VaccelFunctionType_MIN, {0}}
#define vaccel_VaccelEmpty_init_zero             {0}
#define vaccel_VaccelRequest_init_zero           {_vaccel_VaccelFunctionType_MIN, {0}}
#define vaccel_VaccelResponse_init_zero          {_vaccel_VaccelFunctionType_MIN, {0}}

/* Field tags (for use in manual encoding/decoding) */
#define vaccel_VaccelRequest_function_type_tag   1
#define vaccel_VaccelRequest_function_request_tag 2
#define vaccel_VaccelResponse_function_type_tag  1
#define vaccel_VaccelResponse_function_response_tag 2

/* Struct field encoding specification for nanopb */
#define vaccel_VaccelEmpty_FIELDLIST(X, a) \

#define vaccel_VaccelEmpty_CALLBACK NULL
#define vaccel_VaccelEmpty_DEFAULT NULL

#define vaccel_VaccelRequest_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    function_type,     1) \
X(a, STATIC,   SINGULAR, FIXED_LENGTH_BYTES, function_request,   2)
#define vaccel_VaccelRequest_CALLBACK NULL
#define vaccel_VaccelRequest_DEFAULT NULL

#define vaccel_VaccelResponse_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    function_type,     1) \
X(a, STATIC,   SINGULAR, FIXED_LENGTH_BYTES, function_response,   2)
#define vaccel_VaccelResponse_CALLBACK NULL
#define vaccel_VaccelResponse_DEFAULT NULL

extern const pb_msgdesc_t vaccel_VaccelEmpty_msg;
extern const pb_msgdesc_t vaccel_VaccelRequest_msg;
extern const pb_msgdesc_t vaccel_VaccelResponse_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define vaccel_VaccelEmpty_fields &vaccel_VaccelEmpty_msg
#define vaccel_VaccelRequest_fields &vaccel_VaccelRequest_msg
#define vaccel_VaccelResponse_fields &vaccel_VaccelResponse_msg

/* Maximum encoded size of messages (where known) */
#define VACCEL_AGENT_PB_H_MAX_SIZE               vaccel_VaccelRequest_size
#define vaccel_VaccelEmpty_size                  0
#define vaccel_VaccelRequest_size                68
#define vaccel_VaccelResponse_size               68

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
