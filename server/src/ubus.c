#include "ubus.h"
#include "ubus_utils.h"


#include <fcntl.h>
#include <inttypes.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static char const ubus_service_name[] = "stream_debug";
static char const debug_method_name[] = "stream_debug_fd";

static int streamer_handle_set_debug_fd_request(
    struct ubus_context * ctx,
    struct ubus_object * obj,
    struct ubus_request_data * req,
    const char * method,
    struct blob_attr * msg)
{
    UNUSED_ARG(obj);
    UNUSED_ARG(method);
    UNUSED_ARG(msg);

    int res;
    struct streamerd_context_st * const context =
        container_of(ctx, struct streamerd_context_st, ubus_state.ubus_connection.context);
    int const debug_fd = debug_subscriber_init(context);

    if (debug_fd < 0)
    {
        res = UBUS_STATUS_UNKNOWN_ERROR;
        goto done;
    }

    /*
     * This is all a bit odd. The response to the caller must use a different
     * request than the one that was supplied (else ubus disconnects the daemon).
     * To make this work, defer this request and respond to the caller with
     * the new_req set up when the request was deferred.
     */
    struct ubus_request_data new_req;

    ubus_defer_request(ctx, req, &new_req);
    ubus_request_set_fd(ctx, &new_req, debug_fd);
    ubus_complete_deferred_request(ctx, &new_req, UBUS_STATUS_OK);
    fprintf(stderr, "sent response\n");
    res = UBUS_STATUS_OK;

done:
    return res;
}

static struct ubus_method main_object_methods[] =
{
    UBUS_METHOD_NOARG(debug_method_name, streamer_handle_set_debug_fd_request),
};

static struct ubus_object_type main_object_type =
    UBUS_OBJECT_TYPE(ubus_service_name, main_object_methods);

static struct ubus_object main_object =
{
    .name = ubus_service_name,
    .type = &main_object_type,
    .methods = main_object_methods,
    .n_methods = ARRAY_SIZE(main_object_methods),
};

static void
ubus_init_service(struct ubus_context * const ctx)
{
    struct streamerd_context_st * const context =
        container_of(ctx, struct streamerd_context_st, ubus_state.ubus_connection.context);

    ubus_add_object(ctx, &main_object);
    start_count_timer(context);
}

static void
ubus_reconnected(struct ubus_connection_ctx_st * const connection_context)
{
    struct ubus_context * const ubus_ctx = &connection_context->context;
    struct ubus_state * const ubus_state =
        container_of(connection_context, struct ubus_state, ubus_connection);

    fprintf(stderr, "reconnected\n");
    ubus_state->connected = true;
    ubus_add_uloop(ubus_ctx);
}

static void
ubus_connected(struct ubus_connection_ctx_st * const connection_context)
{
    struct ubus_context * const ubus_ctx = &connection_context->context;
    struct ubus_state * const ubus_state =
        container_of(connection_context, struct ubus_state, ubus_connection);

    fprintf(stderr, "connected\n");

    ubus_state->connected = true;
    ubus_init_service(ubus_ctx);
    ubus_add_uloop(ubus_ctx);
}

static void
ubus_disconnected(struct ubus_connection_ctx_st * const connection_context)
{
    struct ubus_state * const ubus_state =
        container_of(connection_context, struct ubus_state, ubus_connection);

    fprintf(stderr, "ubus disconnected\n");

    ubus_state->connected = false;
}

void
streamer_ubus_init(
    struct streamerd_context_st * const context, char const * const ubus_path)
{
    ubus_connection_init(
        &context->ubus_state.ubus_connection,
        ubus_path,
        ubus_connected,
        ubus_reconnected,
        ubus_disconnected);
}

