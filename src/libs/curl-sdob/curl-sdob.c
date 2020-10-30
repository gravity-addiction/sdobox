
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int submit_scorecard(char *data, int dataLen) {
  // Post using CURL
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "Content-Type: application/json; charset: utf-8");
    chunk = curl_slist_append(chunk, "Host: dev.skydiveorbust.com");

    curl_easy_setopt(curl, CURLOPT_URL, "https://dev.skydiveorbust.com/api/latest/events/2020_cf_ghost_nationals/scores/submit");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dataLen);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    res = curl_easy_perform(curl);
    // ErrorCheck
    if (res != CURLE_OK) {
      return res;
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
    return 0;
  }
  return 1;
}

int slave_video(char *data, int dataLen, char* host) {
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
    chunk = curl_slist_append(chunk, hostStr);

    curl_easy_setopt(curl, CURLOPT_URL, urlStr);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dataLen);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    res = curl_easy_perform(curl);
    // ErrorCheck
    if (res != CURLE_OK) {
      return res;
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
    return 0;
  }
  return 1;
}