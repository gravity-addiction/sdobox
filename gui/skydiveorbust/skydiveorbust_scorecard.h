#ifndef _TOUCHAPP_SDOB_SCORECARD_H_
#define _TOUCHAPP_SDOB_SCORECARD_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


int pg_sdobInsertMark(int markSelected, double markTime, int mark);
void pg_sdobDeleteMark(int markSelected);
void pg_sdobMoveMark(int markSelected, int moveAmt);
int pg_sdobSOWTSet(double markTime, double workingTime);
int pg_sdobSOWTReset();
int pg_sdobScoringMarkHidden(int markI);
void pg_sdobScoringMarks(gslc_tsGui *pGui);
void pg_sdobScoringSelectionClear(gslc_tsGui *pGui);
void pg_sdobScoringSelectionLastHidden(gslc_tsGui *pGui);
int pg_sdobSubmitScorecard();

double pg_sdobSetRetrieveExternalSOWT(const char* meet, const char* team, const char* round, double sowt);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _TOUCHAPP_SDOB_SCORECARD_H_
