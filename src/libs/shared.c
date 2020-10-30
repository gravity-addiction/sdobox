
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/kd.h>
#include <assert.h>
#include <time.h>
#include <ctype.h> /* isalpha */
#include "libs/shared.h"
#include "libs/dbg/dbg.h"

#define DIM(x) (sizeof(x)/sizeof((x)))

void free(void *ptr);

static const char *sizes[] = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" };
static const uint64_t exbibytes = 1024ULL * 1024ULL * 1024ULL *
                                  1024ULL * 1024ULL * 1024ULL;

#ifndef HAVE_STRLCAT
/*
 * '_cups_strlcat()' - Safely concatenate two strings.
 */

size_t                  /* O - Length of string */
strlcat(char *dst,        /* O - Destination string */
        const char *src,      /* I - Source string */
        size_t size)      /* I - Size of destination string buffer */
{
  size_t    srclen;         /* Length of source string */
  size_t    dstlen;         /* Length of destination string */


 /*
  * Figure out how much room is left...
  */

  dstlen = strlen(dst);
  size -= dstlen + 1;

  if (!size)
    return (dstlen);        /* No room, return immediately... */

 /*
  * Figure out how much room is needed...
  */

  srclen = strlen(src);

 /*
  * Copy the appropriate amount...
  */

  if (srclen > size)
    srclen = size;

  memcpy(dst + dstlen, src, srclen);
  dst[dstlen + srclen] = '\0';

  return (dstlen + srclen);
}
#endif /* !HAVE_STRLCAT */

#ifndef HAVE_STRLCPY
/*
 * '_cups_strlcpy()' - Safely copy two strings.
 */

size_t                  /* O - Length of string */
strlcpy(char *dst,        /* O - Destination string */
        const char *src,      /* I - Source string */
        size_t      size)     /* I - Size of destination string buffer */
{
  size_t    srclen;         /* Length of source string */


 /*
  * Figure out how much room is needed...
  */

  size --;

  srclen = strlen(src);

 /*
  * Copy the appropriate amount...
  */

  if (srclen > size)
    srclen = size;

  memcpy(dst, src, srclen);
  dst[srclen] = '\0';

  return (srclen);
}
#endif /* !HAVE_STRLCPY */


void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    // debug_print("%s\n", "malloc() failed");
    return;
  }
  s->ptr[0] = '\0';
}
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size * nmemb;
  s->ptr = realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    // debug_print("%s\n", "realloc() failed");
    return (size_t)0;
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

int fd_is_valid(int fd) {
  return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

int fd_is_connected(int fd)
{
    unsigned char buf;
    int err = recv(fd, &buf, 1, MSG_PEEK | MSG_DONTWAIT);
    dbgprintf(DBG_DEBUG, "Connected: %d, Err: %d\n", err, errno);
    return 1;
}

int setnonblock(int sock) {
  int flags;
  flags = fcntl(sock, F_GETFL, 0);
  if (-1 == flags)
    return -1;
  return fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

void clearsocket(int fd) {
  int nread;
  char receive[1024];
  while((nread = read(fd, receive, 1023) > 0)) {
    receive[nread]='\0';
  }
}
int sgetline(int fd, char ** out)
{
  int buf_size = 0;
  int in_buf = 0;
  int ret;
  char ch;
  char * buffer = NULL;
  char * new_buffer;

  do {
    // read a single byte
    ret = read(fd, &ch, 1);
    if (ret < 1)
    {
      if (errno == EAGAIN && in_buf == 0) {
        // ok, no problem, socket is non-blocking and there's nothing to read,
        // we haven't allocated anything yet either.
        assert(!buffer);
        return 0;
      }
      // error or disconnect
      if (in_buf) {
        char* tmp = strndup(buffer, in_buf);
        dbgprintf(DBG_MPV_READ, "sgetline from %d, got error '%s'"
                  " after having read an incomplete line '%s'\n", fd, strerror(errno), tmp);
        free(tmp);
      }
      else
        dbgprintf(DBG_MPV_READ, "sgetline from %d, got error '%s'\n", fd, strerror(errno));

      free(buffer);
      return 0;
    }

    // has end of line been reached?
    if (ch == '\n') { break; } // yes

    // is more memory needed?
    if ((buf_size == 0) || (in_buf == buf_size))
    {
      buf_size += 128;
      new_buffer = realloc(buffer, buf_size);

      if (!new_buffer)
      {
        free(buffer);
        dbgprintf(DBG_MPV_READ, "sgetline from %d, could not reset buffer\n", fd);
        return -1;
      }

      buffer = new_buffer;
    }

    buffer[in_buf] = ch;
    ++in_buf;
  } while (1);

  // if the line was terminated by "\r\n", ignore the
  // "\r". the "\n" is not in the buffer
  if ((in_buf > 0) && (buffer[in_buf-1] == '\r'))
      --in_buf;

  // is more memory needed?
  if ((buf_size == 0) || (in_buf == buf_size))
  {
    ++buf_size;
    new_buffer = realloc(buffer, buf_size);

    if (!new_buffer)
    {
      free(buffer);
      dbgprintf(DBG_MPV_READ, "sgetline from %d, could not reset buffer (2)\n", fd);
      return -1;
    }

    buffer = new_buffer;
  }

// // debug_print("Got Line: %s\n", buffer);
  // add a null terminator
  buffer[in_buf] = '\0';
  // printf("Readline: %s\n", buffer);
  if (in_buf > 0) {
    *out = (char*)malloc((in_buf + 1) * sizeof(char));
    strlcpy(*out, buffer, in_buf + 1);
  }
  free(buffer);
  // *out = buffer; // complete line
  return in_buf; // number of chars in the line, not counting the line break and null terminator
}


void sgetlines_withcb(char *buf, size_t len, void (*function)(char *, size_t sz, size_t cnt)) {
  char newBuf[len + 1];
  CLEAR(newBuf, len + 1);

  int lineLen = 0;
  int lineCnt = 0;
  // split reply up by line
  for (size_t i = 0; i < len; i++) {
    if (buf[i] == '\n') { // found line end
      function(newBuf, lineLen, lineCnt);

      CLEAR(newBuf, lineLen); // clear cache buffer
      lineLen = 0;
      lineCnt++;

    // add next charactor to newBuf
    } else {
      newBuf[lineLen] = buf[i];
      lineLen++;
    }
  }
  if (lineLen > 0) { // Try process last entry that didn't have a line return;
    function(newBuf, lineLen, lineCnt);
  }
}


// Define debug message function for GUIslice

int time_to_secs(char* timestamp) {
  int h, m, s = 0;
  int secs = -1;
  if (sscanf(timestamp, "%d:%d:%d", &h, &m, &s) >= 2) {
    secs = h *3600 + m*60 + s;
  }
  return secs;
}

void secs_to_time(int millisecs, char *retFormat, int retLen) {
  int milliseconds = (long) (millisecs / 1000) % 1000;
  int seconds      = (((int) (millisecs / 1000) - milliseconds)/1000)%60;
  int minutes      = (((((int) (millisecs / 1000) - milliseconds)/1000) - seconds)/60) %60;
  int hours        = ((((((int) (millisecs / 1000) - milliseconds)/1000) - seconds)/60) - minutes)/60;

/*
  while(milliseconds >= 10) {
    milliseconds = milliseconds / 10;
  }
*/
  // Round by > .8
  if (milliseconds >= 800) {
    seconds++;
  }

  // snprintf(ret, retsize, "%02d:%02d:%02d.%d", hours, minutes, seconds, milliseconds);
  snprintf(retFormat, retLen, "%02d:%02d:%02d", hours, minutes, seconds);
}



int parseTabbedData(const char *s, char *data[], size_t n) {
  size_t i = 0;
  const char *Start = s;

  while (1) {
    if ((*s == '\t') || (*s == '\0')) {
      if (i >= n) {
        // printf("%s\n", "Cannot Parse Tabbed Data, More Tabs than N");
        return -1;  // More than n data
      }
      size_t Length = s - Start;
      if (Length > 0) {
        memcpy(data[i], Start, Length);
        data[i][Length] = '\0';
        i++;
        if (*s == '\0') {
          return i;
        }
        s++;
        Start = s;
      } else {
        return i;
      }
    }
    else s++;
  }
  return 0;
}


int cstring_cmp(const void *a, const void *b)
{
  const char **ia = (const char **)a;
  const char **ib = (const char **)b;
  return strcasecmp(*ia, *ib);
}

int cint_cmp(const void *a, const void *b)
{
  const char **ia = (const char **)a;
  const char **ib = (const char **)b;

  int _ia = atoi(*ia);
  int _ib = atoi(*ib);

  return (_ia > _ib) ? 1 : 0;
}


void combineFilePath(char* destination, const char* path1, const char* path2)
{
  if(path1 == NULL && path2 == NULL) {
    strcpy(destination, "");;
  }
  else if(path2 == NULL || strlen(path2) == 0) {
    strcpy(destination, path1);
  }
  else if(path1 == NULL || strlen(path1) == 0) {
    strcpy(destination, path2);
  } 
  else {
    char directory_separator[] = "/";
#ifdef WIN32
    directory_separator[0] = '\\';
#endif
    const char *last_char = path1;
    while(*last_char != '\0')
        last_char++;        
    int append_directory_separator = 0;
    if(strcmp(last_char, directory_separator) != 0) {
        append_directory_separator = 1;
    }
    strcpy(destination, path1);
    if(append_directory_separator)
        strcat(destination, directory_separator);
    strcat(destination, path2);
  }
}

int fileStruct_cmpName(const void *a, const void *b)
{
  const struct fileStruct **ia = (const struct fileStruct **)a, **ib = (const struct fileStruct **)b;
  return strcasecmp((*ia)->name, (*ib)->name);
}

struct fileStruct * INIT_FILESTRUCT(struct dirent *ep, struct stat s) {
  struct fileStruct *file = (struct fileStruct*)malloc(sizeof(struct fileStruct));
  file->name = strdup(ep->d_name);

  file->mode = s.st_mode;
  file->size = s.st_size; // size of file in bytes
  file->atime = s.st_atime; // time of last access
  file->mtime = s.st_mtime; // time of last modification
  file->ctime = s.st_ctime; // time of last status change

  return file;
}

char *file_ext(char *filename) {
  char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

int file_exists(char *filename) {
  if(access(filename, F_OK) != -1) {
    return 1;
  } else {
    return 0;
  }
}

// Create list of files from folder, loop twice first for count and second for data
static size_t file_list_2(const char *path, struct fileStruct ***ls, int type, int with_up) {
  size_t count = 0;
  DIR *dp = NULL;
  struct dirent *ep = NULL;

  size_t lsMax = 128;
  *ls = (struct fileStruct **)malloc(lsMax * sizeof(struct fileStruct*));

  dp = opendir(path);
  if(NULL == dp) {
      dbgprintf(DBG_ERROR, "no such directory: '%s'\n", path);
      return 0;
  }

  ep = readdir(dp);
  while(NULL != ep) {
    // Hide dotfiles
    if (strncmp(ep->d_name, ".", 1) == 0
        &&
        (!with_up || strcmp(ep->d_name, "..") != 0)) {
      ep = readdir(dp);
      continue;
    }

    size_t fullpathSz = snprintf(NULL, 0, "%s/%s", path, ep->d_name) + 1;
    char *fullpath = (char*)calloc(fullpathSz, sizeof(char));
    snprintf(fullpath, fullpathSz, "%s/%s", path, ep->d_name);

    struct stat s;
    if (stat(fullpath, &s) == 0 &&
        (
        (type == -1 && ((s.st_mode & S_IFREG) || (s.st_mode & S_IFDIR))) ||
        (s.st_mode & type)
        )
    ) {
      // Add more space for filenames
      if (count >= lsMax) {
        lsMax += 128;
        struct fileStruct **newLs = (struct fileStruct **)realloc(*ls, lsMax * sizeof(struct fileStruct *));
        *ls = newLs;
      }
      (*ls)[count] = INIT_FILESTRUCT(ep, s);
      (*ls)[count]->path = strdup(path);
      count++;
    }

    free(fullpath);
    ep = readdir(dp);
  }

  closedir(dp);
  return count;
}

size_t file_list(const char *path, struct fileStruct ***ls, int type) {
  return file_list_2(path,ls,type,0);
}
size_t file_list_w_up(const char *path, struct fileStruct ***ls, int type) {
  return file_list_2(path,ls,type,1);
}

char * calculateSize(uint64_t size)
{
    char *result = (char *)malloc(sizeof(char) * 20);
    uint64_t  multiplier = exbibytes;
    int i;

    for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
    {
        if (size < multiplier)
            continue;
        if (size % multiplier == 0)
            sprintf(result, "%" PRIu64 " %s", size / multiplier, sizes[i]);
        else
            sprintf(result, "%.1f %s", (float) size / multiplier, sizes[i]);
        return result;
    }
    strcpy(result, "0");
    return result;
}

void swapInts (int *a, int *b) 
{ 
  int temp = *a; 
  *a = *b; 
  *b = temp; 
} 

// Fisher yates tool to randomize arr[]
void fisherYatesRandomize (int arr[], int n) 
{ 
  // Use a different seed value so that we don't get same 
  // result each time we run this program 
  srand (time(NULL)); 

  // Start from the last element and swap one by one. We don't 
  // need to run for the first element that's why i > 0 
  for (int i = n-1; i > 0; i--) 
  { 
    // Pick a random index from 0 to i 
    int j = rand() % (i+1); 

    // Swap arr[i] with the element at random index 
    swapInts(&arr[i], &arr[j]); 
  } 
}

int isNumeric(char* str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (isalpha(str[i]) != 0) { return 1; }
  }
  return 0;
}
/*
//---------------------
// System Commands
//---------------------
void run_system_cmd(char *fullpath) {
  if (fullpath == NULL) { return; }

  system(fullpath);
}

int run_system_cmd_with_return(char *fullpath, char *ret, int retsize) {
  if (fullpath == NULL) {
    // puts("No Path");
    return -1;
  } else {
    // printf("Running: %s\n", fullpath);
    // Open the command for reading.

    FILE *fd;
    fd = popen(fullpath, "r");
    if (!fd) return 1;

    char buffer[256];
    size_t chread;

    size_t comalloc = 256;
    size_t comlen = 0;
    char  *comout = malloc(comalloc);
    if (comout == NULL) { return 1; }

    // Use fread so binary data is dealt with correctly
    while ((chread = fread(buffer, 1, sizeof(buffer), fd)) != 0) {
      if (comlen + chread >= comalloc) {
        comalloc *= 2;
        comout = realloc(comout, comalloc);
      }
      memmove(comout + comlen, buffer, chread);
      comlen += chread;
    }

    // copy output to return
    strlcpy(ret, comout, retsize - 1);
    if (comout != NULL) { free(comout); }

    int pidC = pclose(fd);
    return WEXITSTATUS(pidC);
  }
}

void fbcp_start() {
  if (!fbcp_running) {
    fbcp_running = 1;
    system("fbcp &");
  }
}
void fbcp_stop() {
  system("killall fbcp &");
  fbcp_running = 0;
}
*/

