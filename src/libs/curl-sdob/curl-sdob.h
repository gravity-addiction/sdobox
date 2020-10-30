#ifndef _CURL_SDOB_H_
#define _CURL_SDOB_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int submit_scorecard(char *data, int dataLen);
int slave_video(char *data, int dataLen, char* host);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _CURL_SDOB_H_