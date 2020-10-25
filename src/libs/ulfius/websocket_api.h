#ifndef _WEBSOCKET_API_H_
#define _WEBSOCKET_API_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct libUlfiusSDOBNewVideo {
  int cnt;

  char *host; // device is a host, default 1
  
  char *local_folder; // Local File path excluding filename
  char *video_file; // Filename for local file
  char *url; // Website URL for video

  char *team; // teamNumber for event
  char *rnd; // Round number
  char *videoId; // Database Video Id
  char *eventId; // Database Event Id
  char *eventStr; // Display Value for current event
  char *compId; // Database Comp Id
  char *compStr; // Display Value for current comp
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