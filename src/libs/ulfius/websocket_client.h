#ifndef _WEBSOCKET_CLIENT_H_
#define _WEBSOCKET_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Thread
int websocketClientThreadKill;
int websocketClientThreadRunning;

char* WEBSOCKET_CLIENT_HOST;

int websocket_client_start();
void websocket_client_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WEBSOCKET_CLIENT_H_