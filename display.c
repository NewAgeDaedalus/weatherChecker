#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "libs/cJSON/cJSON.h"

void displayWeatherToday(cJSON * weatherList, int n){
        if (n > 24)
                n = 24;
        for (int i = 2; i < n+2; i++){
                cJSON *listElement;
                listElement = cJSON_GetArrayItem(weatherList, i);
                time_t timestamp = (time_t) (cJSON_GetObjectItem(listElement, "dt")
                                ->valueint);
                struct tm *today;
                today = gmtime(&timestamp);
                double temp = convertToC(cJSON_GetObjectItem(listElement, "temp")
                                ->valuedouble);
                double windSpeed = cJSON_GetObjectItem(listElement, "wind_speed")
                        ->valuedouble;
                char *weatherDesc = cJSON_GetObjectItem(cJSON_GetObjectItem(listElement,
                                        "weather")->child, "description")->valuestring;
                printf("========================================================\n");
                printf("       |\n");
                if (today->tm_hour < 10)
                        printf("0");
                printf("%d:00  | ", today->tm_hour);
                printf("%s, %.f C, wind speed %.f m/s\n", weatherDesc, temp, windSpeed);
                printf("       |\n");
        }
        printf("========================================================\n");
}
