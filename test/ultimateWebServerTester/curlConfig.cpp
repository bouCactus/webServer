#include "curlConfig.hpp"


static size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

ResponseData PerformHttpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string responseBody;
    long responseCode;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            FAIL("HTTP request failed: " << curl_easy_strerror(res));
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode); // Move this line here

        curl_easy_cleanup(curl);
    } else {
        FAIL("Failed to initialize libcurl.");
    }

    return {responseCode, responseBody};
}

