#ifndef _WEBSOCKET_SERVER_H_
#define _WEBSOCKET_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Thread
int websocketServerThreadKill;
int websocketServerThreadRunning;

int WEBSOCKET_SERVER_PORT;
char* WEBSOCKET_SERVER_URL;
char* WEBSOCKET_SERVER_STATIC_PATH;
char* WEBSOCKET_SERVER_STATIC;

int websocket_server_start();
void websocket_server_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _WEBSOCKET_SERVER_H_