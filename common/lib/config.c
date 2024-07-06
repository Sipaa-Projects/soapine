#include <lib/jsmnex.h>
#include <trdparty/jsmn.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *config;
jsmn_parser p;
jsmntok_t t[8192];
int i;
int r;

char *config_parser_result;
int config_initialized = 0;

bool config_callback(jsmntok_t nm, jsmntok_t val)
{
    // If you wanna add new options, it's here to get their value.
    
    int namesz = nm.end - nm.start;
    char *name = config + nm.start;

    if (jsmn_equals(config, &nm, "user") == 0)
    {
        printf("* User: %s\n", jsmn_get_tok_value(config, &val));
        return true;
    }

    return false;
}

void config_init(char *cfg)
{
    config_parser_result = malloc(4096);
    config = cfg;

    jsmn_init(&p);
    
    r = jsmn_parse(&p, config, strlen(config), t, sizeof(t) / sizeof(t[0]));
    if (r < 0)
    {
        snprintf(config_parser_result, 4096, "Failed to parse JSON: %d\n", r);
        return;
    }
    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT)
    {
        snprintf(config_parser_result, 4096, "Failed to parse JSON: %d\n", r);
        return;
    }

    jsmn_parsetoks(config, r, t, (JSMN_PARSE_CALLBACK)config_callback);

    config_initialized = 1;
}