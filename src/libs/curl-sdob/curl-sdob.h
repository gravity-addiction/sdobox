#ifndef _CURL_SDOB_H_
#define _CURL_SDOB_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int curl_sdob_submit_scorecard(char *data, int dataLen, int finalized);
int curl_sdob_slave_video(char *data, int dataLen, char* host);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _CURL_SDOB_H_