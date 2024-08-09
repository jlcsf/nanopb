#include "responses.h"
#include "vaccel.h"
#include "python_bindings.h"


vaccel_VaccelResponse create_session_response(struct vaccel_session sess ,int flags)
{
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelResponse_CreateSessionResponse_tag;
    response.which_function_args = vaccel_VaccelResponse_CreateSessionResponse_tag;

    int output;

    printf("Creating a session with the flags:%u\n", flags);
    printf("Creating a session_id with the id:%u\n", sess.session_id);

    output = create_vaccel_session(&sess, flags);

    response.function_args.CreateSessionResponse.session_id = output;

    printf("Output is :%u\n", output);

    return response;
}

vaccel_VaccelResponse update_session_response(struct vaccel_session sess, int flags)
{
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelResponse_UpdateSessionResponse_tag;
    response.which_function_args = vaccel_VaccelResponse_UpdateSessionResponse_tag;

    printf("Updating the session with the flags:%u\n", flags);

    int output = update_vaccel_session(&sess, flags);

    response.function_args.UpdateSessionResponse.success = 1;
    return response;
}

vaccel_VaccelResponse destroy_session_response(struct vaccel_session sess)
{
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelResponse_DestroySessionResponse_tag;
    response.which_function_args = vaccel_VaccelResponse_DestroySessionResponse_tag;

    printf("Destroying the session with the session_id:%u\n", sess.session_id);

    int output = vaccel_sess_free(&sess);

    response.function_args.DestroySessionResponse.success = 1;
    return response;
}

vaccel_VaccelResponse image_classification_response(struct vaccel_session sess, pb_byte_t image)
{
    vaccel_VaccelResponse response = vaccel_VaccelResponse_init_zero;
    response.function_type = vaccel_VaccelResponse_ImageClassificationResponse_tag;
    response.which_function_args = vaccel_VaccelResponse_ImageClassificationResponse_tag;

    printf("Image classification on the image received:\n");

    response = image_classify(&sess, image, response);  // Ensure image_classify returns a vaccel_VaccelResponse

    return response;
}