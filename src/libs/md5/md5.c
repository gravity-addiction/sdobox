
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

#include "md5.h"

int md5HashFile(char* filepath, char **md5hash) {
  unsigned char c[MD5_DIGEST_LENGTH];
  FILE *inFile = fopen(filepath, "rb");
  MD5_CTX mdContext;
  int bytes;
  unsigned char data[1024];

  if (inFile == NULL) {
    printf("%s can't be opened.\n", filepath);
    return 1;
  }

  MD5_Init(&mdContext);
  while ((bytes = fread(data, 1, 1024, inFile)) != 0) {
    MD5_Update(&mdContext, data, bytes);
  }
  MD5_Final(c, &mdContext);

  *md5hash = (char*)malloc(sizeof c * 2 + 1);
  for (int i = 0, j = 0; i < MD5_DIGEST_LENGTH; i++, j+=2) {
    sprintf(*md5hash + j, "%02x", c[i]);
  }

  fclose(inFile);
  return 0;
}
