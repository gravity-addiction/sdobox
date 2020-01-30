#ifndef _TOUCHAPP_H_
#define _TOUCHAPP_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define JSMN_HEADER
#include "jsmn.h" // JSON Parsing

char*    config_path = "/home/pi/.config/touchapp/touchapp.conf";
// int      m_bQuit = 0; // Runs main() while loop while(!m_bQuit) {}
int      m_bSigInt = 0; // placeholder for returning from app same sigint value that the app received


void signal_sigint(int sig);
void get_config_settings();
int main( int argc, char* args[] );


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _TOUCHAPP_H_