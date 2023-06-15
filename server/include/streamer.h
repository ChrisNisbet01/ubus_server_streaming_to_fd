#pragma once

#include "debug_output.h"
#include "ubus_connection.h"

#include <libubus.h>
#include <libubox/avl.h>
#include <libubox/ustream.h>

typedef struct streamerd_context_st streamerd_context_st;

struct ubus_state
{
    bool connected;
    struct ubus_connection_ctx_st ubus_connection;
};

struct streamerd_context_st
{
    int count;
    struct uloop_timeout count_timer;

    struct ubus_state ubus_state;
    debug_output_context_st * debug_output_context;
};

void
streamerd_deinit(streamerd_context_st *context);

streamerd_context_st *
streamerd_init(char const * ubus_path);

void
start_count_timer(streamerd_context_st * const context);


/*
 * Get a file descriptor that the subscriber can read from. This file will
 * contain the debug output from streamerd.
 */
int
debug_subscriber_init(streamerd_context_st * const context);

