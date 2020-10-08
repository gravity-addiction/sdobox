#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/shared.h"

// ---------------------------
// Hashing Routines
// ---------------------------
// Hashing sha256sum
char*     m_sha256sum = "/usr/bin/sha256sum -z %s | cut -d \" \" -f1";


int hash_test(char* filepath, char *retHash, int retSize) {
  char *fullpath = malloc(strlen(m_sha256sum) + strlen(filepath) - 1);
  if (fullpath == NULL) { return -1; }
  sprintf(fullpath, m_sha256sum, filepath);
  return 0;// run_system_cmd_with_return(fullpath, retHash, retSize);
}

void fileinfo_create(char* filepath) {
  // debug_print("Hashing: %s'\n", filepath);
  // Fetch File Hash
  char retHash[68];
  int hashOk = hash_test(filepath, retHash, sizeof(retHash));
  if (hashOk == 0) {
    // debug_print("Video Hash: %s\n", retHash);
  } else {
    // debug_print("Video Hash Error Status: %d, %s\n", hashOk, retHash);
  }

  // Lookup Hash for previous info
}