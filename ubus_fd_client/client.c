
#include <sys/time.h>
#include <unistd.h>

#include <libubox/ustream.h>

#include <libubus.h>

static char const ubus_service_name[] = "stream_debug";
static char const debug_method_name[] = "stream_debug_fd";
static struct ubus_context * ctx;

static void test_client_fd_data_cb(struct ustream * s, int bytes)
{
    (void)bytes;

    int len;
    char * const data = ustream_get_read_buf(s, &len);

    if (len < 1)
    {
        goto done;
    }

    fprintf(stderr, "Debug:: %s\n", data);
    ustream_consume(s, len);

done:
    return;
}

static void
test_client_fd_notify_state_cb(struct ustream * s)
{
    if (s->eof || s->write_error)
    {
        ustream_free(s);

        struct ustream_fd * const test_fd = container_of(s, struct ustream_fd, stream);

        close(test_fd->fd.fd);
        uloop_end();
    }
}

static void test_client_fd_cb(struct ubus_request * req, int fd)
{
    (void)req;

    static struct ustream_fd test_fd;

    fprintf(stderr, "Debug enabled\n");

    test_fd.stream.notify_read = test_client_fd_data_cb;
    test_fd.stream.notify_state = test_client_fd_notify_state_cb;
    test_fd.stream.string_data = true;

    ustream_fd_init(&test_fd, fd);
}

static void client_main(void)
{
    uint32_t id;

    if (ubus_lookup_id(ctx, ubus_service_name, &id))
    {
        fprintf(stderr, "Failed to look up service\n");
        goto done;
    }
    /* Order is important here. ubus_invoke_async() memsets 'req'. */
    struct ubus_request req;

    ubus_invoke_async(ctx, id, debug_method_name, NULL, &req);
    req.fd_cb = test_client_fd_cb;
    /* Note that ubus_complete_request_async() doesn't work if setting things
     * up in a connected callback, as might be the case if this was a server-
     * style application, where objects etc are added once ubus connects.
     */
    ubus_complete_request(ctx, &req, 0);

    uloop_run();

done:
    return;
}

int main(int argc, char ** argv)
{
    const char * ubus_socket = NULL;
    int ch;

    while ((ch = getopt(argc, argv, "cs:")) != -1)
    {
        switch (ch)
        {
        case 's':
            ubus_socket = optarg;
            break;

        default:
            break;
        }
    }

    argc -= optind;
    argv += optind;

    uloop_init();

    ctx = ubus_connect(ubus_socket);
    if (!ctx)
    {
        fprintf(stderr, "Failed to connect to ubus\n");
        return -1;
    }

    ubus_add_uloop(ctx);

    client_main();

    ubus_free(ctx);
    uloop_done();

    return 0;
}

