#ifndef _TOUCHAPP_SDOB_PLAYER_H_
#define _TOUCHAPP_SDOB_PLAYER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int pg_sdob_player_chaptersUpdating;

void pg_sdob_player_video_chapterMarks(gslc_tsGui *pGui);
void pg_sdob_player_video_chapterList(int len);
void pg_sdob_player_video_chapters();
void pg_sdob_player_sliderTicks(gslc_tsGui *pGui, double *tickMarks, int tickCnt);
// void setVideoCurrentTime(long long int video_pos);
void setSliderPos(gslc_tsGui *pGui, int16_t nPercent);
void setSliderPosByTime(gslc_tsGui *pGui);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _TOUCHAPP_SDOB_PLAYER_H_