#include "streamer.h"
#include "ubus.h"
#include "utils.h"
#include "ubus_utils.h"

#include <fcntl.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void
count_timer_cb(struct uloop_timeout * const t)
{
    struct streamerd_context_st * const context =
        container_of(t, struct streamerd_context_st, count_timer);

    debug_output_write_to_debug_apps(
        context->debug_output_context,
        "debug stream: count: %d\n", context->count);
    context->count++;

    uloop_timeout_set(&context->count_timer, 1000);
}

void
start_count_timer(streamerd_context_st * const context)
{
    context->count_timer.cb = count_timer_cb;
    uloop_timeout_set(&context->count_timer, 1000);
}

int
debug_subscriber_init(struct streamerd_context_st * const context)
{
    return debug_output_fd_init(context->debug_output_context);
}

void
streamerd_deinit(streamerd_context_st * const context)
{
    if (context == NULL)
    {
        goto done;
    }

    ubus_connection_shutdown(&context->ubus_state.ubus_connection);

    /*
     * Leave the debug output until the end so users can see as much debug as
     * possible.
     */
    debug_output_deinit(context->debug_output_context);

    free(context);

done:
    return;
}

streamerd_context_st *
streamerd_init(char const * const ubus_path)
{
    struct streamerd_context_st * const context = calloc(1, sizeof *context);

    if (context == NULL)
    {
        goto done;
    }

    context->debug_output_context = debug_output_init();

    streamer_ubus_init(context, ubus_path);

done:
    return context;
}

