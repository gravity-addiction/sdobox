#ifndef _WEBSOCKET_API_H_
#define _WEBSOCKET_API_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

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