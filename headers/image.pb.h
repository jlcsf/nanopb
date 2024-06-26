/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9-dev */

#ifndef PB_VACCEL_IMAGE_PB_H_INCLUDED
#define PB_VACCEL_IMAGE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _vaccel_ImageClassificationRequest {
    uint32_t session_id;
    pb_byte_t image[512];
} vaccel_ImageClassificationRequest;

typedef struct _vaccel_ImageClassificationResponse {
    char tags[512];
} vaccel_ImageClassificationResponse;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define vaccel_ImageClassificationRequest_init_default {0, {0}}
#define vaccel_ImageClassificationResponse_init_default {""}
#define vaccel_ImageClassificationRequest_init_zero {0, {0}}
#define vaccel_ImageClassificationResponse_init_zero {""}

/* Field tags (for use in manual encoding/decoding) */
#define vaccel_ImageClassificationRequest_session_id_tag 1
#define vaccel_ImageClassificationRequest_image_tag 2
#define vaccel_ImageClassificationResponse_tags_tag 1

/* Struct field encoding specification for nanopb */
#define vaccel_ImageClassificationRequest_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   session_id,        1) \
X(a, STATIC,   SINGULAR, FIXED_LENGTH_BYTES, image,             2)
#define vaccel_ImageClassificationRequest_CALLBACK NULL
#define vaccel_ImageClassificationRequest_DEFAULT NULL

#define vaccel_ImageClassificationResponse_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   tags,              1)
#define vaccel_ImageClassificationResponse_CALLBACK NULL
#define vaccel_ImageClassificationResponse_DEFAULT NULL

extern const pb_msgdesc_t vaccel_ImageClassificationRequest_msg;
extern const pb_msgdesc_t vaccel_ImageClassificationResponse_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define vaccel_ImageClassificationRequest_fields &vaccel_ImageClassificationRequest_msg
#define vaccel_ImageClassificationResponse_fields &vaccel_ImageClassificationResponse_msg

/* Maximum encoded size of messages (where known) */
#define VACCEL_IMAGE_PB_H_MAX_SIZE               vaccel_ImageClassificationRequest_size
#define vaccel_ImageClassificationRequest_size   521
#define vaccel_ImageClassificationResponse_size  514

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
