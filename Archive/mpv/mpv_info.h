#ifndef _MPV_INFO_H_
#define _MPV_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct libMpvPlayerInfo {
  unsigned long int cnt;
  int has_file;
  int is_loaded;
  int is_playing;
  int is_seeking;
  int has_seeked;
  double position;
  double duration;
  double fps;
  double pbrate;
  double pbrateUser;
  char *positionStr;
  char *pbrateStr;
  char *pbrateUserStr;
  char *folder;
  char *file;
};
struct libMpvPlayerInfo *libMpvVideoInfo;

struct libMpvPlayerInfo * LIBMPV_EVENTS_INIT_INFO();
void LIBMPV_EVENTS_DESTROY_INFO(struct libMpvPlayerInfo *eventInfo);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MPV_INFO_H_
