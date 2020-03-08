#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Thread
int websocketThreadKill;
int websocketThreadRunning;

int WEBSOCKET_PORT;

int websocket_start();
void websocket_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WEBSOCKET_H_