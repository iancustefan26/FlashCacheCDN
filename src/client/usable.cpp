#include "usable.h"

#include <curl/curl.h>

size_t call_back(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void clear_screen()
{
    system("clear");
}

string get_public_ipv4()
{
    CURL* curl = curl_easy_init();
    std::string public_ip;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://ifconfig.me");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_back);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &public_ip);

        const CURLcode response = curl_easy_perform(curl); // Curl request

        if (response != CURLE_OK)
            throw runtime_error("curl_easy_perform() failed");
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return public_ip;
}
