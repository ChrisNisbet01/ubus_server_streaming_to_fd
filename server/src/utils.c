#include "utils.h"

#include <unistd.h>

void
initialise_pipe(int * const pipe_fd, bool const will_read_pipe)
{
    if (!will_read_pipe || pipe(pipe_fd) != 0)
    {
        pipe_fd[0] = -1;
        pipe_fd[1] = -1;
    }
}

void
close_output_stream(struct ustream_fd * const stream)
{
    if (stream->fd.fd > -1)
    {
        ustream_free(&stream->stream);
        close(stream->fd.fd);
        stream->fd.fd = -1;
    }
}

