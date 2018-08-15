#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>

/* libcurl - curl.haxx.se/libcurl/ */
#include <curl/curl.h>

/* libircclient - ulduzsoft.com/libircclient/ */
#include <libircclient/libircclient.h>
#include <libircclient/libirc_rfcnumeric.h>

/* oniguruma - github.com/kkos/oniguruma */
#include <oniguruma.h>

#include "structs.h"

enum domains_i {
    Youtube,
    Imgur,
    Twitter,
    Imdb
};

/* initialize the string */
void init_string_s(string_c *);

size_t write_response(void *, size_t, size_t, string_c *);

/* check if the message is a URL and contains a domain
 * returns NULL if no /url/domain/title match
 * otherwise return the title for the URL
 */
char *check_message_for_url(const char *);

/* check_regex - returns 0 if the regexpression matched, if no match than it returns 1 */
int check_regex(UChar *, UChar *[], unsigned int *);

/* get the data from the URL */
char *grab_url_data(const char *, unsigned int *);

/* find the <title> tag, extract the title and return it */
char *find_title_tag(char *, unsigned int);