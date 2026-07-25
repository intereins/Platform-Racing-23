//#include <curl/curl.h>
#include <stdbool.h>
#include "internet.h"

//CURL* curl;
//struct curl_slist* list;

void init_curl()
{
    /*curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    list = curl_slist_append(list, "User-Agent: browser/1.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list); */
}

bool download_file(const char* url, const char* filename)
{

}

void end_curl()
{
    /*curl_slist_free_all(list);
    curl_easy_cleanup(curl);*/
}
