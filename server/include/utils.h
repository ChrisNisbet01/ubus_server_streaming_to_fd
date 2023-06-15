#pragma once

#include <libubox/ustream.h>

#include <stdbool.h>

void
initialise_pipe(int * pipe_fd, bool will_read_pipe);

void
close_output_stream(struct ustream_fd * stream);

