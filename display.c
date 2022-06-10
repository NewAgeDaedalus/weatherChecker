#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "libs/cJSON/cJSON.h"

const char *sunny = "          \\       /\n"
"      |    \\_____/\n"
"      | ---/      \\ ---\n"
"      |    |       |\n"
"      |  ---\\_____/ ---\n"
"      |    /     \\\n"
"      |   /       \\\n";

const char *cloudy = "              ---\n"
"      |     .(    ).   \n" 
"      |   (   .     ) \n"
"      | (---(---------)\n";

const char * partlyCloudy = "      \\        /\n"         
"      |    \\______/  __( )_       \n " 
"      | ---/      __(      )-.  \n" 
"      |    |     (     .     )\n" 
"      | ---\\___ (-----)-----)\n"
"      |    /     \n"
"      |   /\n";

const char * lightRain = "              ---\n"
"      |     .(    ).    \n"
"      |   (   .     )\n" 
"      | (---(---------)\n"
"      | . . .. . |. .|\n"
"      | .. .. ... . .\n";

const char * rain = "               ---\n"
"      |     .(    ).    \n"
"      |   (   .     )\n" 
"      | (---(---------)\n"
"      | | | | | | | | |\n"
"      | | | | | | | | |\n";

const char *storm = "               ---\n"
"      |     .(    ).    \n"
"      |   (   .     ) \n"
"      | (---(---------)\n"
"      | \\ \\ | | | | | \n"
"      | _\\ \\\n"
"      | \\ _\\ | | | |\n"
"      |  \\ \\\n"
"      |   \\/\n";

void displayWeatherToday(cJSON * weatherList, int n){
        if (n > 24)
                n = 24;
        for (int i = 0; i < n; i++){
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
                const char *weatherIcon = "";
                char curWord[20] = "";
                for (char *c = weatherDesc; ; c++){
                        if (*c == ' ' || *c == '\n' || *c== '\0'){ //*(c+1) hella unsafe
                                if (!strcmp("light",curWord)){
                                        weatherIcon = lightRain;
                                        break;
                                }
                                if(!strcmp("partly", curWord)){
                                        weatherIcon = partlyCloudy;
                                        break;
                                }
                                if(!strcmp("clear",curWord)){
                                        weatherIcon = sunny;
                                        break;
                                }
                                if (!strcmp("strom", curWord)){
                                        weatherIcon = storm;
                                        break;
                                }
                                if (!strcmp("clouds", curWord)){
                                        weatherIcon = cloudy;
                                        break;
                                }
                                strcpy(curWord, "");
                                if (*c == '\0')
                                        break;
                        }
                        else{
                                int len = strlen(curWord);
                                curWord[len] = *c;
                                curWord[len+1] = '\0';
                        }
                }
        printf("%s\n", weatherIcon);
        if ((today->tm_hour+2)%24 < 10)
                printf("0");
        printf("%d:00  | ", (today->tm_hour+2)%24);
        printf("%s, %.1f C, wind speed %.1f m/s", weatherDesc, temp, windSpeed);
        printf("\n========================================================\n");
        }
}
