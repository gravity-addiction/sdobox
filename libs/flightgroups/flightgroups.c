#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "shared.h"

void xml_flightgroups_parseFile(char *filename) {
  int fd = open(filename, O_RDONLY);
  int len = lseek(fd, 0, SEEK_END);
  void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

  if (data)
  {
    char* json_file;
    ta_json_parse(data, "Group1-12_2.mp4", &json_file);
    printf("Groups: %s\n", json_file);
  }
}
