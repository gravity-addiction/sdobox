#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jsmn.h" // JSON Parsing

#include "json.h"

int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

int ta_json_parse(char *json, char* prop, char ** ret_var) {
  int i;
  int r;
  jsmn_parser p;
  jsmntok_t t[128]; /* We expect no more than 128 tokens */
  char * buffer = NULL;
  char * new_buffer;

  jsmn_init(&p);
  r = jsmn_parse(&p, json, strlen(json), t,
                 sizeof(t) / sizeof(t[0]));
  if (r < 0) {
    // printf("Failed to parse JSON: %d\n", r);
    return 0;
  }

  /* Assume the top-level element is an object */
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    // printf("Object expected\n");
    return 0;
  }

  // // debug_print("JSON PARSE, %d - %s", r, json);
  /* Loop over all keys of the root object */
  for (i = 1; i < r; i++) {
    if (jsoneq(json, &t[i], prop) == 0) {
      int len = t[i + 1].end - t[i + 1].start;
      new_buffer = realloc(buffer, len + 2);
      if (!new_buffer) {
        if (buffer != NULL) { free(buffer); }
        i++;
        continue;
      }
      buffer = new_buffer;
      /* We may use strndup() to fetch string value */
      size_t bufferSz = snprintf(NULL, 0, "%.*s",  t[i + 1].end - t[i + 1].start, json + t[i + 1].start);
      *ret_var = (char*)malloc(bufferSz + 1 * sizeof(char));
      snprintf(*ret_var, bufferSz + 1, "%.*s",  t[i + 1].end - t[i + 1].start, json + t[i + 1].start);
      free(buffer);
      return bufferSz;
    }
    i++;
  }
return 0;
}

