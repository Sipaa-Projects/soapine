#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <trdparty/jsmn.h>

#define JSMNEX_SUCCESS 0
#define JSMNEX_NOT_PARSED 1


typedef bool(*JSMN_PARSE_CALLBACK)(jsmntok_t, jsmntok_t); // first arg: name token, second arg: value token

int jsmn_equals(const char *json, jsmntok_t *tok, const char *s);
int jsmn_parsetoks(char *contents, int tok_count, jsmntok_t *tokens, JSMN_PARSE_CALLBACK callback);
char *jsmn_get_tok_value(char *json, jsmntok_t *tok);