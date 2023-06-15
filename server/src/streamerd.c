#include "ubus.h"

#include <libubox/uloop.h>
#include <libubox/ulog.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

static void
ignore_sigpipe(void)
{
    struct sigaction sa;

    if (sigaction(SIGPIPE, NULL, &sa) == 0)
    {
        sa.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &sa, NULL);
    }
}

static bool
run(char const * const ubus_path)
{
    bool success;

    ignore_sigpipe();

    uloop_init();

    streamerd_context_st * const context = streamerd_init(ubus_path);

    if (context != NULL)
    {
        uloop_run();
        success = true;
        streamerd_deinit(context);
    }
    else
    {
        success = false;
    }

    uloop_done();

    return success;
}

static void
usage(FILE * const fp, char const * const program_name)
{
    fprintf(fp,
            "usage: %s [-u ubus_path]\n\n"
            "options\n"
            "\t-h\thelp      - this help\n"
            "\t-u\tubus path - UBUS socket path\n",
            program_name);
}

int
main(int argc, char * * argv)
{
    int exit_code;
    char const * ubus_path = NULL;
    int opt;
    char const * const program_name = argv[0];

    while ((opt = getopt(argc, argv, "hu:")) != -1)
    {
        switch (opt)
        {
            case 'u':
                ubus_path = optarg;
                break;
            case 'h':
                usage(stdout, program_name);
                exit_code = EXIT_SUCCESS;
                goto done;
            default:
                usage(stderr, program_name);
                exit_code = EXIT_FAILURE;
                goto done;
        }
    }

    ULOG_NOTE("%s started\n", program_name);

    exit_code = run(ubus_path) ? EXIT_SUCCESS : EXIT_FAILURE;

done:
    if (exit_code == EXIT_SUCCESS)
    {
        ULOG_NOTE("%s: exiting\n", program_name);
    }
    else
    {
        ULOG_ERR("%s: exiting\n", program_name);
    }

    return exit_code;
}

