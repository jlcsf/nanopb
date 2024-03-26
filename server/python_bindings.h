#ifndef PYTHON_BINDINGS_H
#define PYTHON_BINDINGS_H

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

int vaccel_python_create_session(int session_id);

#endif /* PYTHON_BINDINGS_H */
