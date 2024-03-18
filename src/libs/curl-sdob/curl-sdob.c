
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int curl_sdob_submit_scorecard(char *data, int dataLen, int finalized) {
  // Post using CURL
  CURL *curl;
  CURLcode res;

  // printf("Data: %s\n", data);
  curl = curl_easy_init();
  if (curl) {
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "Content-Type: application/json; charset=utf-8");
    chunk = curl_slist_append(chunk, "Connection: close");
    chunk = curl_slist_append(chunk, "Host: dev.omniskorehd.com");
    chunk = curl_slist_append(chunk, "Authorization: Bearer eyJpZCI6MSwibmFtZSI6IkdhcnkiLCJ1c2VybmFtZSI6ImdhcnkiLCJyb2xlIjpbInVzZXIiLHsicm9sZSI6ImFkbWluIiwiYXJlYSI6IiJ9XSwianRpIjoiZDIzYzU0ZjQtODExYS00NGYwLTJhYWYtMWFhOTUxYzhmZWE1IiwiaWF0IjoxNzEwNDc1MDU5LCJleHAiOjE3MTA0NzUxMTl9.-kEKL8JTE2zmJqyRUiQ3sPs21kZxRUvuw-sGdfDCQ1E");

    if (finalized) {
      curl_easy_setopt(curl, CURLOPT_URL, "http://dev.omniskorehd.com/api/latest/events/2024_perris_fresh_meet/scores/finalized");
    } else {
      curl_easy_setopt(curl, CURLOPT_URL, "http://dev.omniskorehd.com/api/latest/events/2024_perris_fresh_meet/scores/submit");
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dataLen);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    res = curl_easy_perform(curl);
    
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
    printf("RES %d\n", res);
    return res;
  }
  return 1;
}

int curl_sdob_slave_video(char *data, int dataLen, char* host) {
  // Post using CURL
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    // Create host and url strings
    int hostLen = snprintf(NULL, 0, "Host: %s", host) + 1;
    int urlLen = snprintf(NULL, 0, "http://%s:4004/p/skydiveorbust/newvideo", host) + 1;
    char *hostStr = (char*)malloc(hostLen * sizeof(char));
    char *urlStr = (char*)malloc(urlLen * sizeof(char));
    snprintf(hostStr, hostLen, "Host: %s", host);
    snprintf(urlStr, urlLen, "http://%s:4004/p/skydiveorbust/newvideo", host);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "Content-Type: application/json; charset: utf-8");
    chunk = curl_slist_append(chunk, "Connection: close");
    chunk = curl_slist_append(chunk, hostStr);

    curl_easy_setopt(curl, CURLOPT_URL, urlStr);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dataLen);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    res = curl_easy_perform(curl);
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
    free(hostStr);
    free(urlStr);
    return res;
  }
  return 1;
}