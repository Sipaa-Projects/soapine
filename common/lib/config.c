#include <lib/config.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *config;
menu_entry_t *menu_root;
menu_entry_t *parser_mentry;
menu_entry_t *current_menu_entry;

bool config_is_in_entry = false;
bool config_is_in_declaration = false;
bool config_parsing_errors_present = false;

config_declaration_t config_declarations[4096];
int config_declarations_array_pos = 0; // When the parser parsed all declarations, this becomes the declaration count

char config_entry_property_name[2048];

/*------------ Public API ------------*/

config_declaration_t *config_get_value(char *name) {
    for (int i = 0; i < config_declarations_array_pos; i++)
    {
        if (strlen(name) == strlen(config_declarations[i].name) && strcmp(name, config_declarations[i].name) == 0)
            return &config_declarations[i];
    }
    
    return (config_declaration_t *)CONFIG_NOT_FOUND;
}

menu_entry_t *config_get_menu_root() { return menu_root; }

int config_get_mentry_count() { 
    menu_entry_t *ce = menu_root;
    int i = 0;
    while (ce != NULL)
    {
        ce = ce->next;
        i++;
    }
    return i; 
}

/*------------ Utilities ------------*/

char *config_extract_text_in_quotes(const char *str) {
    const char *start = strchr(str, '"');
    if (start == NULL) {
        return NULL;
    }

    start++;
    const char *end = strchr(start, '"');
    if (end == NULL) {
        return NULL;
    }

    size_t length = end - start;
    char *result = (char*)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }

    strncpy(result, start, length);
    result[length] = '\0';

    return result;
}

void config_remove_spaces_tabs(char *str) {
    char *src = str;
    char *dst = str;
    int in_quotes = 0;

    while (*src) {
        if (*src == '\"') {
            in_quotes = !in_quotes; // Toggle the in_quotes flag
            *dst++ = *src++;
        } else if (in_quotes) {
            *dst++ = *src++;
        } else {
            if (*src != ' ' && *src != '\t') {
                *dst++ = *src;
            }
            src++;
        }
    }
    *dst = '\0'; // Null-terminate the destination string
}

bool isdigit(unsigned char c) {
    if (c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9' || c == '0')
        return true;
    return false;
}

bool ishexchar(unsigned char c) {
    if (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F')
        return true;
    return false;
}

bool is_decimal_only(const char *str) {
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

bool is_hex_only(const char *str) {
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            if (!ishexchar((unsigned char)*str))
                return false;
        }
        str++;
    }
    return true;
}

config_valtype_t config_get_value_type(char *vstring)
{
    // Check if there's quotes
    if (vstring[0] == '\"')
    {
        if (vstring[strlen(vstring) - 1] == '\"')
            return STRING;

        printf("config: Expected an ending quote, but got %s", vstring);
        config_parsing_errors_present = true;
        return INVALID;
    }
    else if (is_decimal_only(vstring))
    {
        return DECIMAL;
    }
    else if (is_hex_only(vstring))
    {
        return HEXADECIMAL;
    }
    else if (vstring[0] == '#' && is_hex_only(&vstring[1]))
    {
        return HEX_COLOR;
    }
    else if (!strncmp(vstring, "true", 4) || !strncmp(vstring, "false", 5))
    {
        return BOOL;
    }
}

/*------------ Main config file parsing ------------*/

void config_parse_entry_token(char *tok)
{
    if (!config_is_in_declaration)
    {
        strcpy(config_entry_property_name, tok);
    } else {
        if (!strncmp(config_entry_property_name, "PROTOCOL", 8))
        {
            if (tok[0] != '\"')
            {
                printf("config: Expected a string literal in %s->%s, but got: %s\n", parser_mentry->name, config_entry_property_name, tok);
                config_parsing_errors_present = true;
                return;
            }
            strcpy(parser_mentry->protocol, config_extract_text_in_quotes(tok));
        }
        if (!strncmp(config_entry_property_name, "COMMENT", 8))
        {
            if (tok[0] != '\"')
            {
                printf("config: Expected a string literal in %s->%s, but got: %s\n", parser_mentry->name, config_entry_property_name, tok);
                config_parsing_errors_present = true;
                return;
            }
            strcpy(parser_mentry->comment, config_extract_text_in_quotes(tok));
        }
        if (!strncmp(config_entry_property_name, "RESOLUTION", 10))
        {
            if (tok[0] != '\"')
            {
                printf("config: Expected a string literal in %s->%s, but got: %s\n", parser_mentry->name, config_entry_property_name, tok);
                config_parsing_errors_present = true;
                return;
            }
            strcpy(parser_mentry->resolution, config_extract_text_in_quotes(tok));
        }
        if (!strncmp(config_entry_property_name, "IMAGE_PATH", 10))
        {
            if (tok[0] != '\"')
            {
                printf("config: Expected a string literal in %s->%s, but got: %s\n", parser_mentry->name, config_entry_property_name, tok);
                config_parsing_errors_present = true;
                return;
            }
            strcpy(parser_mentry->image_path, config_extract_text_in_quotes(tok));
        }
        if (!strncmp(config_entry_property_name, "CMDLINE", 10))
        {
            if (tok[0] != '\"')
            {
                printf("config: Expected a string literal in %s->%s, but got: %s\n", parser_mentry->name, config_entry_property_name, tok);
                config_parsing_errors_present = true;
                return;
            }
            strcpy(parser_mentry->cmdline, config_extract_text_in_quotes(tok));
        }
    }
}

void config_process_token(char *tok/*, int line */)
{
    if (strlen(tok) == 1 && tok[0] == '\n') {
        printf("config: INFO: Ignoring new line-only line\n");
        return;
    }

    if (strncmp(tok, "menu_entry", 10) == 0)
    {
        printf("config: INFO: Declaring new entry\n");
        // We're declaring an entry
        int len = 0;

        parser_mentry = malloc(sizeof(menu_entry_t));
        strcpy(parser_mentry->name, config_extract_text_in_quotes(tok));
        strcpy(parser_mentry->protocol, "\0");
        strcpy(parser_mentry->comment, "\0");
        strcpy(parser_mentry->resolution, "\0");
        strcpy(parser_mentry->image_path, "\0");
        strcpy(parser_mentry->cmdline, "\0");
        parser_mentry->next = NULL;

        config_is_in_entry = true;
        
        if (tok[strlen(tok) - 1] != '{')
        {
            printf("config: Starting bracket missing at declaration of %s!\n", parser_mentry->name);
            config_parsing_errors_present = true;
        }

        return;
    } else if (strncmp(tok, "};", 2) == 0)
    {
        if (config_is_in_entry)
        {
            if (!strcmp(parser_mentry->protocol, "\0"))
            {
                printf("config: PROTOCOL not provided for %s!\n", parser_mentry->name);
                config_parsing_errors_present = true;
            } else if (!strcmp(parser_mentry->image_path, "\0"))
            {
                printf("config: IMAGE_PATH not provided for %s!\n", parser_mentry->name);
                config_parsing_errors_present = true;
            }

            if (!menu_root)
            {
                menu_root = parser_mentry;
                current_menu_entry = NULL;
            }
            else if (menu_root && !current_menu_entry)
            {
                current_menu_entry = parser_mentry;
                menu_root->next = current_menu_entry;
            }
            else if (menu_root && current_menu_entry)
            {
                current_menu_entry->next = parser_mentry;
                current_menu_entry = parser_mentry;
            }
            parser_mentry = NULL;
            config_is_in_entry = false;
        }
        else
        {
            goto unexpected_token;
        }

        return;
    } else {
        config_remove_spaces_tabs(tok);

        // Assume this is a declaration
        if (!config_is_in_declaration)
        {
            if (config_is_in_entry)
                config_parse_entry_token(tok);
            else
                strcpy(config_declarations[config_declarations_array_pos].name, tok);

            config_is_in_declaration = true;
            return;
        }
        else
        {
            if (config_is_in_entry)
                config_parse_entry_token(tok);
            else {
                config_declarations[config_declarations_array_pos].type = config_get_value_type(tok);

                switch (config_declarations[config_declarations_array_pos].type)
                {
                case STRING:
                    config_declarations[config_declarations_array_pos].value_str = config_extract_text_in_quotes(tok); // This functions creates a new buffer using malloc.
                    break;
                
                case DECIMAL:
                    uint32_t i = (uint32_t)atoi(tok);
                    config_declarations[config_declarations_array_pos].value = i;
                    break;

                case HEXADECIMAL:
                    uint32_t i2 = (uint32_t)strtol(tok, NULL, 16);
                    config_declarations[config_declarations_array_pos].value = i2;
                    break;

                case HEX_COLOR:
                    uint32_t i3 = (uint32_t)strtol(&tok[1], NULL, 16);
                    config_declarations[config_declarations_array_pos].value = i3;
                    break;

                case BOOL:
                    if (!strncmp(tok, "true", 4))
                    {
                        config_declarations[config_declarations_array_pos].value = 1;
                        break;
                    } else if (!strncmp(tok, "false", 5))
                    {
                        config_declarations[config_declarations_array_pos].value = 0;
                        break;
                    } else
                    {
                        printf("config: Invalid boolean: %s", tok);
                        config_parsing_errors_present = true;
                        config_declarations[config_declarations_array_pos].value = -1;
                        break;
                    }
                    config_declarations[config_declarations_array_pos].value = -2;
                    break;

                default:
                    printf("config: Unknown type for %s", tok);
                    config_declarations[config_declarations_array_pos].value = -1;
                    break;
                }

                config_declarations_array_pos++;
            }
            config_is_in_declaration = false;
            return;
        }
    }

unexpected_token:
    printf("config: Unexpected token \"%s\"\n", tok);
    config_parsing_errors_present = true;
}

void config_display() {
    printf("Declarations: \n");
    for (int i = 0; i < config_declarations_array_pos; i++)
    {
        if (config_declarations[i].type == STRING) {
            printf("- Name: %s, Type: String, Value: %s\n", config_declarations[i].name, config_declarations[i].value_str);
        } else if (config_declarations[i].type == DECIMAL) {
            printf("- Name: %s, Type: Decimal, Value: %d\n", config_declarations[i].name, config_declarations[i].value);
        } else if (config_declarations[i].type == HEXADECIMAL) {
            printf("- Name: %s, Type: Hexadecimal, Value: %x\n", config_declarations[i].name, config_declarations[i].value);
        } else if (config_declarations[i].type == HEX_COLOR) {
            printf("- Name: %s, Type: Hexadecimal RGB color, Value: %x\n", config_declarations[i].name, config_declarations[i].value);
        } else if (config_declarations[i].type == BOOL) {
            printf("- Name: %s, Type: Boolean, Value: %s (%d)\n", config_declarations[i].name, config_declarations[i].value == 1 ? "true" : "false", config_declarations[i].value);
        } else {
            printf("- Name: %s, Type: Unknown, Value: (null)\n", config_declarations[i].name);
        }
    }
    printf("\n");

    printf("Entries: \n");
    menu_entry_t *curentry = menu_root;

    while (curentry != NULL) {
        printf("- %s\n", curentry->name);
        printf("   * Protocol: %s\n", curentry->protocol);
        printf("   * Image path: %s\n", curentry->image_path);

        if (strcmp(curentry->resolution, "\0") != 0) {
            printf("   * Comment: %s\n", curentry->comment);
        } else { printf("   * Comment: (not provided)\n"); }
        if (strcmp(curentry->resolution, "\0") != 0) {
            printf("   * Command line: %s\n", curentry->resolution);
        } else { printf("   * Command line: (not provided)\n"); }
        if (strcmp(curentry->resolution, "\0") != 0) {
            printf("   * Resolution: %s\n", curentry->resolution);
        } else { printf("   * Resolution: (not provided)\n"); }

        curentry = curentry->next;
    }
}

void config_init(char *cfg) {
    config = cfg;

    const char *delim = "=\n";

    char *token;
    //int line_number = 1;

    char *cfg_copy = strdup(cfg); // We don't need to check if strdup returned NULL, since malloc will call abort if the firmware returns an error.

    token = strtok(cfg_copy, delim);
    while (token != NULL) {
        config_process_token(token/*, line_number */);

        token = strtok(NULL, delim);
    }

    config_display();

    free(cfg_copy);
}

/**
void parse_entry(char *entry_json)
{
    parser_mentry = malloc(sizeof(menu_entry_t));

    jsmn_parser ep;
    jsmntok_t t[4096];
    int token_count = 0; 

    jsmn_init(&ep);
    
    token_count = jsmn_parse(&ep, entry_json, strlen(entry_json),
                                        t, sizeof(t) / sizeof(t[0]));
    
    if (r < 0)
    {
        snprintf(config_parser_result, 4096, "Failed to parse JSON: %d\n", r);
        return;
    }
    if (r < 1 || t[0].type != JSMN_OBJECT)
    {
        snprintf(config_parser_result, 4096, "Failed to parse JSON: %d\n", r);
        return;
    }

    if (!menu_root)
    {
        menu_root = current_menu_entry;
    }
    else if (menu_root && !current_menu_entry)
    {
        current_menu_entry = parser_mentry;
        menu_root->next = current_menu_entry;
    }
    else if (menu_root && current_menu_entry)
    {
        current_menu_entry->next = parser_mentry;
        current_menu_entry = parser_mentry;
    }
}
**/