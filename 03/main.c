#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>
#include <cjson/cJSON.h> 


#define URLf "http://wttr.in/%s?format=j1"
#define MAX_CITY_LEN 20

typedef struct {
    char *memory;
    size_t size;
} RawJSON;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    RawJSON *mem = (RawJSON *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(int argc, char **argv){
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <city>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *city_name = argv[1];
    size_t city_len = strlen(city_name);
    if (city_len > MAX_CITY_LEN) {
        fprintf(stderr, "error city name too long %ld. max %d\n", city_len, MAX_CITY_LEN);
        return EXIT_FAILURE;
    }
    char url[strlen(URLf) + MAX_CITY_LEN + 1];
    sprintf(url, URLf, city_name);
    printf("URL=%s\n", url);

    CURL *curl;
    CURLcode code;
    RawJSON rawJ;

    rawJ.memory = malloc(1);
    rawJ.size = 0; 

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&rawJ);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        code = curl_easy_perform(curl);
        if(code != CURLE_OK){
            fprintf(stderr, "error curl request: %s\n", curl_easy_strerror(code));
            curl_easy_cleanup(curl);
            return EXIT_FAILURE;
        }
        curl_easy_cleanup(curl);
    }


    cJSON *json = cJSON_Parse(rawJ.memory);
    if (json == NULL) {
        fprintf(stderr, "error parsing JSON '%s'\n", rawJ.memory);
        free(rawJ.memory);
        return EXIT_FAILURE;
    }

    // char *json_str = cJSON_Print(json);
    // printf("%s\n", json_str); 
    // cJSON_free(json_str);

    //JSON fields
    cJSON *current_condition = cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(json, "current_condition"), 0);
    if (current_condition == NULL) {
        fprintf(stderr, "error parsing JSON field\n");
        cJSON_Delete(json);
        free(rawJ.memory);
        return EXIT_FAILURE;
    }
    // температура
    char *cel = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(current_condition, "temp_C"));
    char *far = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(current_condition, "temp_F"));
    char *obs_time = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(current_condition, "observation_time"));

    // описание погоды
    cJSON *weather_desc_item = cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(current_condition, "weatherDesc"), 0);
    char *weather_desc = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(weather_desc_item, "value"));
    
    // ветер
    char *wind_dir = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(current_condition, "winddir16Point"));
    char *wind_speed_kmph = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(current_condition, "windspeedKmph"));

    printf("\tCity: %s\n", city_name);
    printf("\tCelsius: %s\n", cel);
    printf("\tFahrenheit: %s\n", far);
    printf("\tObservation Time: %s\n", obs_time);
    printf("\tWeather Description: %s\n", weather_desc);
    printf("\tWind Direction: %s\n", wind_dir);
    printf("\tWind Speed: %s km/h\n", wind_speed_kmph);

    cJSON_Delete(json);
    free(rawJ.memory);
    curl_global_cleanup();

    return 0;
}