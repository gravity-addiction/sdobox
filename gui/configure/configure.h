#ifndef _PAGES_CONFIGURE_H_
#define _PAGES_CONFIGURE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include "gui/pages.h"

enum {
  E_CONFIGURE_CLEAN,
  E_CONFIGURE_EL_BOX,
  E_CONFIGURE_EL_CLOSE,

  E_CONFIGURE_EL_MSG,
  E_CONFIGURE_EL_MSGA,
  E_CONFIGURE_EL_MSGB,

  E_CONFIGURE_EL_MAX
};

#define MAX_ELEM_PG_CONFIGURE      E_CONFIGURE_EL_MAX
#define MAX_ELEM_PG_CONFIGURE_RAM  MAX_ELEM_PG_CONFIGURE

gslc_tsElem m_asPgConfigureElem[MAX_ELEM_PG_CONFIGURE_RAM];
gslc_tsElemRef m_asPgConfigureElemRef[MAX_ELEM_PG_CONFIGURE];

gslc_tsElemRef* pg_configureEl[E_CONFIGURE_EL_MAX];

void pg_configure_init(gslc_tsGui *pGui);
void pg_configure_open(gslc_tsGui *pGui);
void pg_configure_destroy(gslc_tsGui *pGui);
void pg_configure(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _PAGES_CONFIGURE_H_
