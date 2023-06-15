#pragma once

#include "streamer.h"

void
streamer_ubus_init(
    struct streamerd_context_st * context, char const * ubus_path);

