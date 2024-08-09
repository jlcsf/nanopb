#include "requests.h"

vaccel_VaccelRequest create_session_request(int flags)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_CreateSessionRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_CreateSessionRequest_tag;
    request.function_args.CreateSessionRequest.flags = flags;

    return request;
}

vaccel_VaccelRequest update_session_request(int session_id, int flags)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_UpdateSessionRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_UpdateSessionRequest_tag;
    request.function_args.UpdateSessionRequest.session_id = session_id;
    request.function_args.UpdateSessionRequest.flags = flags;

    return request;
}

vaccel_VaccelRequest destroy_session_request(int session_id)
{
    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_DestroySessionRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_DestroySessionRequest_tag;
    request.function_args.DestroySessionRequest.session_id = session_id;

    return request;
}

vaccel_VaccelRequest image_classification(int session_id, const char* image_path)
{
    size_t image_size;
    uint8_t* image_data = read_image_data(image_path, &image_size);
    if (!image_data) {
        fprintf(stderr, "Error reading image data\n");
    }

    vaccel_VaccelRequest request = vaccel_VaccelRequest_init_zero;
    request.function_type = vaccel_VaccelRequest_ImageClassificationRequest_tag;
    request.which_function_args = vaccel_VaccelRequest_ImageClassificationRequest_tag;

    request.function_args.ImageClassificationRequest.session_id = session_id;

    size_t copy_size = image_size > sizeof(request.function_args.ImageClassificationRequest.image) ?
                       sizeof(request.function_args.ImageClassificationRequest.image) :
                       image_size;
    memcpy(request.function_args.ImageClassificationRequest.image, image_data, copy_size);
    printf("Size of image %zu\n", image_size);
    free(image_data);

    return request;
}

uint8_t* read_image_data(const char* image_path, size_t* image_size) {
    FILE* file = fopen(image_path, "rb");
    if (!file) {
        fprintf(stderr, "Error opening image file: %s\n", image_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(file_size);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Error allocating memory for image data\n");
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != file_size) {
        free(buffer);
        fprintf(stderr, "Error reading image data from file\n");
        return NULL;
    }

    *image_size = (size_t)file_size;
    return buffer;
}
