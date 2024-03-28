#include <Python.h>
#include <session.h>
#include <stdio.h>
#include <vaccel.h>
#include "server.h"
#include <stdio.h>



int vaccel_python_create_session(int session_id) {

    int response;

    response = 1;

    return response;
}

int create_vaccel_session(struct vaccel_session *sess, int flags)
{
    int ret;

    ret = vaccel_sess_init(sess, flags);

	if (ret != VACCEL_OK) {
		fprintf(stderr, "Could not initialize session\n");
		return 1;
	}

    ret = sess->session_id;

    return ret;

}

int update_vaccel_session(struct vaccel_session *sess, int flags)
{
    int ret;
    ret = vaccel_sess_update(sess, flags);

	if (ret != VACCEL_OK) {
		fprintf(stderr, "Could not update session\n");
		return 1;
	}

    return ret;

}

vaccel_VaccelResponse image_classify(struct vaccel_session *sess, pb_byte_t *image, vaccel_VaccelResponse response) {
    char out_text[512], out_imagename[512];

    int image_size = 512;

    int ret = vaccel_image_classification(sess, image, (unsigned char *)out_text, (unsigned char *)out_imagename,
                                           image_size, sizeof(out_text), sizeof(out_imagename));
    if (ret) {
        fprintf(stderr, "Could not run op: %d\n", ret);
        return response;
    }

    printf("Classification tags: %s\n", out_text);

    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t*)response.function_args.ImageClassificationResponse.tags,
                                                sizeof(response.function_args.ImageClassificationResponse.tags));
    if (!pb_encode_string(&stream, (uint8_t*)out_text, strlen(out_text))) {
        fprintf(stderr, "Error encoding string: %s\n", PB_GET_ERROR(&stream));
        return response;
    }

    return response;
}