#ifndef _WEBSOCKET_API_H_
#define _WEBSOCKET_API_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct libUlfiusSDOBNewVideo {
  int cnt;
  char *local_folder; // Local File path excluding filename
  char *video_file; // Filename for local file
  char *url; // Website URL for video
  char *team; // teamNumber for event
  char *rnd; // Round number
  char *videoId; // Database Video Id
  char *compId; // Database Comp Id
  char *compStr; // Display Value for current comp
  char *slug; // Event slug for URL

  char *host; // device is a host, default 1
};
struct libUlfiusSDOBNewVideo *libUlfiusSDOBNewVideoInfo;

// Thread
int websocketApiThreadKill;
int websocketApiThreadRunning;

int WEBSOCKET_API_PORT;

int websocket_api_start();
void websocket_api_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WEBSOCKET_API_H_