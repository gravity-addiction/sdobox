#ifndef __JSON_H_
#define __JSON_H_

#include <stddef.h>
#define JSMN_HEADER
#include "jsmn.h" // JSON Parsing

#ifdef __cplusplus
extern "C" {
#endif

int jsoneq(const char *json, jsmntok_t *tok, const char *s);
int ta_json_parse(char *json, char* prop, char ** ret_var);

#ifdef __cplusplus
}
#endif

#endif /* __JSON_H_ */