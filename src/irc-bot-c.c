#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* libcurl - curl.haxx.se/libcurl/ */
#include <curl/curl.h>

/* libsrsirc - https://github.com/fstd/libsrsirc */
#include <libsrsirc/irc.h>
#include <libsrsirc/irc_ext.h>

#include "events.h"
#include "responses.h"
#include "structs.h"

#define MAXLENGTH 255
#define STRLENGTH(x) STRLENGTHVAL(x)
#define STRLENGTHVAL(x) #x

/* global variable with settings */
user_config ucfg;

void cleanupcfg () {
    free(ucfg.botNick);
    free(ucfg.channel);
    free(ucfg.server);
}

int main(int argc, char **argv) {
    irc *ircs = irc_init();

    if (argc != 2) {
        fprintf(stdout, "Usage: %s server.cfg\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* open cfg file */
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Config file %s loaded.\n", argv[1]);

    char *checkCfgParameter = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.botNick = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.server = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.channel = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.nickservPassword = (char *)calloc(MAXLENGTH, sizeof(char));

    while (!feof(f)) {
        fscanf(f, "%" STRLENGTH(MAXLENGTH) "s", checkCfgParameter);

        if (!strcmp(checkCfgParameter, "bot_nick")) {
            fscanf(f, " %" STRLENGTH(MAXLENGTH) "s", ucfg.botNick);
        } else if (!strcmp(checkCfgParameter, "server")) {
            fscanf(f, " %" STRLENGTH(MAXLENGTH) "s", ucfg.server);
        } else if (!strcmp(checkCfgParameter, "port")) {
            fscanf(f, " %hu", &ucfg.port);
        } else if (!strcmp(checkCfgParameter, "ssl")) {
            fscanf(f, " %c", &ucfg.sslActivated);
        } else if (!strcmp(checkCfgParameter, "channel")) {
            fscanf(f, " %" STRLENGTH(MAXLENGTH) "s", ucfg.channel);
        } else if (!strcmp(checkCfgParameter, "nickserv_auth")) {
            fscanf(f, " %" STRLENGTH(MAXLENGTH) "s", ucfg.nickservPassword);
        }
    }
    /* clear cfg stuff, especially to not have the password in memory */
    free(checkCfgParameter);
    fclose(f);

    irc_set_server(ircs, ucfg.server, (uint16_t)ucfg.port);
    irc_set_nick(ircs, ucfg.botNick);
    irc_set_pass(ircs, ucfg.nickservPassword);

    if (ucfg.sslActivated == 'y') {
        irc_set_ssl(ircs, true);
    }

    /* clear the password from memory */
    free(ucfg.nickservPassword);

    fprintf(stdout, "bot nick: %s\nserver: %s\nport: %u\nchannel(s): %s\nnickserv auth: ***\n",
                    ucfg.botNick, ucfg.server, ucfg.port, ucfg.channel);

    /* connect to the IRC network */
    if (!irc_connect(ircs)) {
        fprintf(stderr, "Could not connect or logon. Maybe check config file? %s\n", argv[1]);
        cleanupcfg();
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "IRC server connection successfully created.\n");

    irc_printf(ircs, "JOIN %s", ucfg.channel);

    /* and run into *forever* loop */
    while (irc_online(ircs)) {
        tokarr msg;
        int r = irc_read(ircs, &msg, 500000);

        /* read failure, possible connection closed ? */
        if (r < 0) {
            break;
        }
        /* read timeout */
        if (r == 0) {
            continue;
        }
        interpret_message(msg);
    }

    cleanupcfg();
    irc_dispose(ircs);
    exit(EXIT_SUCCESS);
}
