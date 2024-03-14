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
    pb_callback_t image;
} vaccel_ImageClassificationRequest;

typedef struct _vaccel_ImageClassificationResponse {
    pb_callback_t tags;
} vaccel_ImageClassificationResponse;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define vaccel_ImageClassificationRequest_init_default {0, {{NULL}, NULL}}
#define vaccel_ImageClassificationResponse_init_default {{{NULL}, NULL}}
#define vaccel_ImageClassificationRequest_init_zero {0, {{NULL}, NULL}}
#define vaccel_ImageClassificationResponse_init_zero {{{NULL}, NULL}}

/* Field tags (for use in manual encoding/decoding) */
#define vaccel_ImageClassificationRequest_session_id_tag 1
#define vaccel_ImageClassificationRequest_image_tag 2
#define vaccel_ImageClassificationResponse_tags_tag 1

/* Struct field encoding specification for nanopb */
#define vaccel_ImageClassificationRequest_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   session_id,        1) \
X(a, CALLBACK, SINGULAR, BYTES,    image,             2)
#define vaccel_ImageClassificationRequest_CALLBACK pb_default_field_callback
#define vaccel_ImageClassificationRequest_DEFAULT NULL

#define vaccel_ImageClassificationResponse_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, BYTES,    tags,              1)
#define vaccel_ImageClassificationResponse_CALLBACK pb_default_field_callback
#define vaccel_ImageClassificationResponse_DEFAULT NULL

extern const pb_msgdesc_t vaccel_ImageClassificationRequest_msg;
extern const pb_msgdesc_t vaccel_ImageClassificationResponse_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define vaccel_ImageClassificationRequest_fields &vaccel_ImageClassificationRequest_msg
#define vaccel_ImageClassificationResponse_fields &vaccel_ImageClassificationResponse_msg

/* Maximum encoded size of messages (where known) */
/* vaccel_ImageClassificationRequest_size depends on runtime parameters */
/* vaccel_ImageClassificationResponse_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
