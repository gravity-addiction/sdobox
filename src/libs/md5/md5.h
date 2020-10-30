#ifndef _SDOB_MD5_H_
#define _SDOB_MD5_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int md5HashFile(char* filepath, char **md5hash);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _SDOB_MD5_H_