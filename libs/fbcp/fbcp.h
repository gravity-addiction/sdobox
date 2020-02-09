#ifndef _FBCP_H_
#define _FBCP_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Thread
int fbcpThreadKill;
int fbcpThreadRunning;

int fbcp_toggle();
int fbcp_start();
void fbcp_stop();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _FBCP_H_