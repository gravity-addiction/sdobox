#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <jansson.h>
#include <libconfig.h>

#include "gui/skydiveorbust/skydiveorbust.h"
#include "gui/skydiveorbust/skydiveorbust_zmq.h"

#include "libs/shared.h"
#include "libs/dbg/dbg.h"
#include "libs/queue/queue.h"
#include "libs/thpool/thpool.h"
#include "libs/zhelpers/zhelpers-conn.h"
#include "libs/zhelpers/zhelpers-tx.h"
#include "libs/zhelpers/zhelpers.h"

#include "sdob-cache.h"
#include "sdob-zmq.h"

void *libsdob_zmq_scoring = NULL;
void *libsdob_zmq_events = NULL;
threadpool thpool = NULL;

struct libsdob_question * LIBSDOB_QUESTION_INIT(char* question) {
  // if (libsdob_zmq_scoring == NULL) {
    dbgprintf(DBG_DEBUG, "%s\n", "Finding Scoring Server");

    if (libSdobCache->server->scoringserver == NULL) {
      config_t cfg;
      config_init(&cfg);
      // Read the file. If there is an error, report it and exit.
      if (access(config_path, F_OK) == -1 || !config_read_file(&cfg, config_path)) {
        dbgprintf(DBG_DEBUG, "Cannot Find config_path: %s\n", config_path);
        config_destroy(&cfg);
        return NULL;
      }

      const char * retScoringServer;
      if (config_lookup_string(&cfg, "scoringserver", &retScoringServer)) {
        libSdobCache->server->scoringserver = strdup(retScoringServer);
      } else {
        printf("No scoringserver configuration in ~/.config/sdobox/sdobox.conf\n");
        config_destroy(&cfg);
        return NULL;
      }
      config_destroy(&cfg);
    }
        
    dbgprintf(DBG_DEBUG, "%s\n", "Connecting For Scoring");
    int rc = zmq_connect_socket(&libsdob_zmq_scoring, libSdobCache->server->scoringserver, ZMQ_REQ);
    if (rc < 0) {
      printf("Connection Failed: %d\n", rc);
    }
  // }

  struct libsdob_question *sdobQuestion = (struct libsdob_question*)malloc(sizeof(struct libsdob_question));
  sdobQuestion->server = libsdob_zmq_scoring;
  sdobQuestion->question = question;
  return sdobQuestion;
}
void LIBSDOB_QUESTION_DESTROY(struct libsdob_question *sdobQuestion) {
  if (sdobQuestion->question != NULL) {
    free(sdobQuestion->question);
  }
  free(sdobQuestion);
}

int libsdob_zmq_init() {
  dbgprintf(DBG_DEBUG, "SDOB ZMQ Init\n");
  libSdobCache = LIBSDOB_CACHE_INIT();
  libsdob_eventThreadStart();
  thpool = thpool_init(5);
  return 1;
}

void libsdob_zmq_destroy() {
  libsdob_eventThreadStop();
  if (libsdob_zmq_scoring != NULL) {
    zmq_close(libsdob_zmq_scoring);
    libsdob_zmq_scoring = NULL;
  }
  if (libsdob_zmq_events != NULL) {
    zmq_close(libsdob_zmq_events);
    libsdob_zmq_events = NULL;
  }

	thpool_wait(thpool);
	puts("Killing SDOB-ZMQ Threadpool");
	thpool_destroy(thpool);  
}



// Submit Scorecard
static pthread_mutex_t scoringSend = PTHREAD_MUTEX_INITIALIZER;

int libsdob_zmq_scoring_send_th(void* input) {
  pthread_mutex_lock(&scoringSend);
  dbgprintf(DBG_DEBUG, "Scoring Send: %s\n", ((struct libsdob_question *)input)->question);
  int rc = s_send (((struct libsdob_question *)input)->server, ((struct libsdob_question *)input)->question);
  
  if (rc < 0) {
    printf("Unable to send scoring: %d\n", rc);
    zmq_close(((struct libsdob_question *)input)->server);
    ((struct libsdob_question *)input)->server = NULL;
    goto cleanup;
  }

  char *string = s_recv(((struct libsdob_question *)input)->server);
  dbgprintf(DBG_INFO, "ZMQ Return: %s\n", string);
  free(string);

 cleanup:
//  free(((struct libsdob_question *)input)->question);
//  ((struct libsdob_question *)input)->question = NULL;
  LIBSDOB_QUESTION_DESTROY((struct libsdob_question *)input);
  pthread_mutex_unlock(&scoringSend);
  return 1;
}

int libsdob_zmq_scoring_send(char* scorecard) {
  return thpool_add_work(thpool, &libsdob_zmq_scoring_send_th, (void*)(struct libsdob_question*)LIBSDOB_QUESTION_INIT(scorecard));
}


// ------------------------
// SDOB Events Thread 
// ------------------------
void * libsdob_eventThread(void *input) {
  if (libsdob_eventThreadRunning) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting SkydiveOrBust Events Thread, Already Started");
    pthread_exit(NULL);
  }

  void *eventserver = NULL;
  char tokenStr[8];
  libsdob_eventThreadRunning = 1;
  
  // Grab Device token
  FILE *tokenFile;
  tokenFile = fopen("/opt/sdobox/device.token", "r");
  if (tokenFile == NULL) {
    strlcpy(tokenStr, "NOTOKENS", 8);
  } else {
    fgets(tokenStr, 8, tokenFile);
    fclose(tokenFile);
  }

  if (libsdob_eventThreadKill) {
    dbgprintf(DBG_DEBUG, "%s\n", "Not Starting SkydiveOrBust Events Thread, Stop Flag Set");
    goto cleanup;
  }

  if (((struct libsdob_cache *)input)->server->eventserver == NULL) {
    dbgprintf(DBG_DEBUG, "%s\n", "Finding SkydiveOrBust Events Server");

    config_t cfg;
    config_init(&cfg);
    // Read the file. If there is an error, report it and exit.
    if (access(config_path, F_OK) == -1 || !config_read_file(&cfg, config_path)) {
      dbgprintf(DBG_DEBUG, "Cannot Find config_path: %s\n", config_path);
      config_destroy(&cfg);
      goto cleanup;
    }

    const char * retEventServer;
    if (config_lookup_string(&cfg, "eventserver", &retEventServer)) {
      ((struct libsdob_cache *)input)->server->eventserver = strdup(retEventServer);
    } else {
      printf("No eventserver configuration in ~/.config/sdobox/sdobox.conf\n");
      config_destroy(&cfg);
      goto cleanup;
    }
    config_destroy(&cfg);
  }

  dbgprintf(DBG_DEBUG, "Starting SkydiveOrBust Events Thread: %s\n", ((struct libsdob_cache *)input)->server->eventserver);
  int rc = 0;

  // Try starting event server
  printf("Sending: --%s-- %d\n", ((struct libsdob_cache *)input)->server->eventserver, ZMQ_SUB);
  rc = zmq_connect_socket(&eventserver, ((struct libsdob_cache *)input)->server->eventserver, ZMQ_SUB);

  while (rc < 0 && !libsdob_eventThreadKill) {
    zmq_close(eventserver);
    sleep(2);
    rc = zmq_connect_socket(&eventserver, ((struct libsdob_cache *)input)->server->eventserver, ZMQ_SUB);
    printf("Event Connect: %d: %s\n", rc, ((struct libsdob_cache *)input)->server->eventserver);
  }

  // Did connect, try initialize subscriptions
  if (rc > -1 && !libsdob_eventThreadKill) {
    dbgprintf(DBG_DEBUG, "EventServer Connected, %s - %d\n", ((struct libsdob_cache *)input)->server->eventserver, rc);
    const char *filtereventserver = "";
    rc = zmq_setsockopt (eventserver, ZMQ_SUBSCRIBE, filtereventserver, strlen(filtereventserver));

    // Failed to initialize subscriptions;
    if (rc < 0) {
      dbgprintf(DBG_DEBUG, "%s\n", "Cannot Subscribe to Event Server");   
      printf("%s\n", "Shutdown SkydiveOrBust Events Thread");
      goto cleanup;
    }
 
    // 0MQs sockets to poll
    zmq_pollitem_t items [] = {
      { eventserver, 0, ZMQ_POLLIN, 0 }
    };
    
    while (!libsdob_eventThreadKill) {
      rc = zmq_poll (items, 1, -1);
      if (!m_bQuit && rc > -1) {  
        if (items[0].revents & ZMQ_POLLIN) {
          char *str = s_recv(eventserver);
          dbgprintf(DBG_DEBUG, "eventserver: %s\n", str);
          dbgprintf(DBG_DEBUG, "Token String: %s\n", tokenStr);
        
          char *pt;
          // List Array
          // int ptI = 0;
          // while (pt != NULL) {
          //   printf("%d %s\n", ptI, pt);
          //   pt = strtok (NULL, ";");
          //   ptI++;
          // }
          pt = strtok (str,";");
          if (pt != NULL && strcmp(pt, "clear") == 0) {
            pt = strtok(NULL, ";");
            printf("Check: %s - %s\n", pt, tokenStr);
            if (strcmp(pt, tokenStr) == 0) {
              // Clear Scorecard
              struct queue_head *item = malloc(sizeof(struct queue_head));
              INIT_QUEUE_HEAD(item);
              item->action = E_Q_SCORECARD_CLEAR;
              pg_sdob_add_action(&item);

              printf("Also Send Submit\n");
              // Submit Scorecard
              struct queue_head *itemSc = malloc(sizeof(struct queue_head));
              INIT_QUEUE_HEAD(itemSc);
              itemSc->action = E_Q_SCORECARD_SUBMIT_SCORECARD;
              pg_sdob_add_action(&itemSc);              
            }
          } else
          
          if (pt != NULL && strcmp(pt, "driver-add") == 0) {
            pt = strtok(NULL, ";");
            printf("Check: %s - %s\n", pt, tokenStr);
            if (strcmp(pt, tokenStr) == 0) {
              pg_sdobVideoDriver(1);              
            }
          } else
          
          if (pt != NULL && strcmp(pt, "discipline") == 0) {
            pt = strtok(NULL, ";");
            pg_sdobUpdateDiscipline(&m_gui, pt, strtok(NULL, ";"));
          } else

          if (pt != NULL && strcmp(pt, "ping") == 0) {
            pt = strtok(NULL, ";");
            // Submit Scorecard
            struct queue_head *itemSc = malloc(sizeof(struct queue_head));
            INIT_QUEUE_HEAD(itemSc);
            itemSc->action = E_Q_SCORECARD_SUBMIT_SCORECARD;
            pg_sdob_add_action(&itemSc);
          }          



          free(str);
        }
      }
    }
  } else {
    goto cleanup;
  }
  
 cleanup:
  zmq_close(eventserver);
  eventserver = NULL; 
  printf("%s\n", "Closing SkydiveOrBust Events Thread");
  libsdob_eventThreadRunning = 0;
  pthread_exit(NULL);
}


int libsdob_eventThreadStart() {
  dbgprintf(DBG_DEBUG, "%s\n", "libsdob_eventThreadStart()");
  if (libsdob_eventThreadRunning) { return 0; }

  // pg_sdob_pl_sliderForceUpdate = 1;
  dbgprintf(DBG_DEBUG, "SkydiveOrBust Events Thread Spinup: %d\n", libsdob_eventThreadRunning);
  libsdob_eventThreadKill = 0;
  
  pthread_t tid;
  return pthread_create(&tid, NULL, &libsdob_eventThread, libSdobCache);
}

void libsdob_eventThreadStop() {
  dbgprintf(DBG_DEBUG, "%s\n", "libsdob_eventThreadStop()");
  // Shutdown MPV FIFO Thread
  int shutdown_cnt = 0;
  if (libsdob_eventThreadRunning) {
    libsdob_eventThreadKill = 1;
    while (libsdob_eventThreadRunning && shutdown_cnt < 20) {
      usleep(100000);
      shutdown_cnt++;
    }
  }
  dbgprintf(DBG_DEBUG, "SkydiveOrBust Events Thread Shutdown %d\n", shutdown_cnt);
}

