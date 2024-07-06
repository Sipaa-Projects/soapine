#include <lib/jsmnex.h>
#include <trdparty/jsmn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int jsmn_equals(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}

int jsmn_parsetoks(char *contents, int tok_count, jsmntok_t *tokens, JSMN_PARSE_CALLBACK callback)
{
    if (contents == NULL || tok_count < 1 || tokens == NULL || callback == NULL)
        return 0;
    
    for (int i = 1; i < tok_count; i++)
    {
        if (!callback(tokens[i], tokens[i+1]))
        {
            printf("Unexpected token: %.*s\n", tokens[i].start - tokens[i].end, contents + tokens[i].start);
        }

        i++;
    }
}

char *jsmn_get_tok_value(char *json, jsmntok_t *tok)
{
    int sz = tok->end - tok->start;
    char *start = json + tok->start;

    char *buf = malloc(sz + 1);

    strncpy(buf, start, sz);
    buf[sz] = '\0';

    return buf;
}