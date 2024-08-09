#ifndef RESPONSES_H
#define RESPONSES_H

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
#include "vaccel.h"      // Assuming this contains the definition of vaccel_VaccelResponse and pb_byte_t

// Function prototypes
vaccel_VaccelResponse create_session_response(struct vaccel_session sess, int flags);
vaccel_VaccelResponse update_session_response(struct vaccel_session sess, int flags);
vaccel_VaccelResponse destroy_session_response(struct vaccel_session sess);
vaccel_VaccelResponse image_classification_response(struct vaccel_session sess, pb_byte_t image);

#endif // RESPONSES_H