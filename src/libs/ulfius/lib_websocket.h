#ifndef _LIB_WEBSOCKET_H_
#define _LIB_WEBSOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <ulfius.h>

char * print_map(const struct _u_map * map);
// static char * read_file(const char * filename);

int websocket_cb(const struct _u_request * request, struct _u_response * response, void * user_data);
int websocket_cbEcho(const struct _u_request * request, struct _u_response * response, void * user_data);
int websocket_cbFile(const struct _u_request * request, struct _u_response * response, void * user_data);


int websocket_cbDefault (const struct _u_request * request, struct _u_response * response, void * user_data);
void websocket_manager_callback(const struct _u_request * request,
                               struct _websocket_manager * websocket_manager,
                               void * websocket_manager_user_data);
void websocket_incoming_message_callback (const struct _u_request * request,
                                         struct _websocket_manager * websocket_manager,
                                         const struct _websocket_message * last_message,
                                         void * websocket_incoming_message_user_data);
void websocket_onclose_callback (const struct _u_request * request,
                                struct _websocket_manager * websocket_manager,
                                void * websocket_onclose_user_data);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _LIB_WEBSOCKET_H_