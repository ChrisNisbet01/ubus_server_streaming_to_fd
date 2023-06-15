#include "ubus_utils.h"

#include <libubox/blobmsg_json.h>

#include <unistd.h>

void
blob_buf_full_init(struct blob_buf * const b, int const id)
{
    memset(b, 0, sizeof *b);
    blob_buf_init(b, id);
}

bool
blobmsg_array_is_type(
    struct blob_attr const * const array_blob, enum blobmsg_type const type)
{
    return array_blob != NULL && blobmsg_check_array(array_blob, type) >= 0;
}

uint32_t
blobmsg_get_u32_or_default(
    struct blob_attr * const attr, uint32_t const default_value)
{
    return (attr != NULL) ? blobmsg_get_u32(attr) : default_value;
}

bool
blobmsg_get_bool_or_default(
    struct blob_attr * const attr, bool const default_value)
{
    return (attr != NULL) ? blobmsg_get_bool(attr) : default_value;
}

char const *
blobmsg_get_string_or_default(
    struct blob_attr * const attr, char const * const default_value)
{
    return (attr != NULL) ? blobmsg_get_string(attr) : default_value;
}


size_t
blobmsg_array_length(struct blob_attr const * const attr)
{
    size_t len = 0;
    struct blob_attr * cur;
    int rem;

    blobmsg_for_each_attr(cur, attr, rem)
    {
        len++;
    }

    return len;
}

