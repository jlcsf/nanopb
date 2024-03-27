#include <Python.h>
#include <session.h>
#include <stdio.h>
#include <vaccel.h>

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