#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include "utils.h"
#include "display.h"
#include "libs/cJSON/cJSON.h" 

const char *latMy= "45.819601167969466";
const char *lonMy=  "15.884336035489284";
const char *apiKey = ""; //Put your apikey from openweathermap.org here
const char *weatherAPIhostname = "api.openweathermap.org";
const char *geocodingAPIhostname = "api.openweathermap.org";

//I am sending my exact coordinates via unecrypted into the wire
//might not smart to glow in the dark.
int getBasic(const char *lat,const char *lon){
        const char *resource = "/data/2.5/weather";
        int sock;
        //create a tcp connection
        sock = tcpConnect(weatherAPIhostname);
        //construct url
        char url[200], httpRequest[1024];
        strcpy(url, resource);
        strcat(url, "?lat=");
        strcat(url, lat);
        strcat(url, "&lon=");
        strcat(url, lon);
        strcat(url, "&appid=");
        strcat(url,apiKey);
        //construct http request
        strcpy(httpRequest, "GET ");
        strcat(httpRequest, url);
        strcat(httpRequest, " HTTP/1.1\n");
        strcat(httpRequest, "Host: api.openweathermap.org\n");
        strcat(httpRequest, "Accept: */*\n");
        strcat(httpRequest,"\n");
        //send http request
        //printf("%s\n", httpRequest);
        writen(sock, httpRequest, strlen(httpRequest)); 
        //wait for resonse
        char response[4096] = "";
        readHttpResponse(sock, response);
        //printf("%s\n", response);
        cJSON *responseData = cJSON_Parse(response);
        double temp = cJSON_GetObjectItem(cJSON_GetObjectItem(responseData,"main"), 
                        "temp")->valuedouble;
        temp= convertToC(temp);
        cJSON *weather = cJSON_DetachItemFromArray(cJSON_GetObjectItem(responseData,"weather"), 0);
        cJSON *wind = cJSON_GetObjectItem(responseData, "wind");
        double windSpeed = cJSON_GetObjectItem(wind, "speed")->valuedouble;
        char *weatherDesc = cJSON_GetObjectItem(weather, "description")->valuestring;
        printf("%s, %.1f C, wind speed %.1f m/s \n",weatherDesc, temp, windSpeed);
        cJSON_Delete(responseData);
        close(sock);
        return 0; 
}

struct coords* 
getCountryCoords(const char *cityName){
        const char *resource = "/geo/1.0/direct";
        int sock = tcpConnect(geocodingAPIhostname);
        char url[1024];
        strcpy(url, resource);
        strcat(url, "?");
        strcat(url, "q=");
        strcat(url, cityName);
        strcat(url, "&appid=");
        strcat(url, apiKey);
        char header[2048];
        strcpy(header, "GET ");
        strcat(header, url);
        strcat(header, " HTTP/1.1\n");
        strcat(header, "Host: ");
        strcat(header, geocodingAPIhostname);
        strcat(header, "\n");
        strcat(header, "Accept: */*\n");
        strcat(header,"\n");
        //send http request
        writen(sock, header, strlen(header)); 
        char response[4056];
        readHttpResponse(sock, response);
        //printf("%s\n",response);
        cJSON *json = cJSON_DetachItemFromArray(cJSON_Parse(response), 0);
        close(sock);
        struct coords *kordinate = (struct coords*) malloc(sizeof(struct coords));
        kordinate->lat= cJSON_GetObjectItem(json, "lat")->valuedouble;
        kordinate->lon= cJSON_GetObjectItem(json,"lon")->valuedouble;
        return kordinate;
}

cJSON *get48Hours(const char *lat, const char *lon){
        const char *resource = "/data/2.5/onecall";
        int sock = tcpConnect(weatherAPIhostname);
        char url[1024] = "";
        strcpy(url, resource);
        strcat(url, "?lat=");
        strcat(url, lat);
        strcat(url, "&lon=");
        strcat(url, lon);
        strcat(url, "&exclude=");
        strcat(url, "minutely,daily");
        strcat(url, "&appid=");
        strcat(url,apiKey);
        char header[2048];
        strcpy(header, "GET ");
        strcat(header, url);
        strcat(header, " HTTP/1.1\n");
        strcat(header, "Host: ");
        strcat(header,  weatherAPIhostname);
        strcat(header, "\n");
        strcat(header, "Accept: */*\n");
        strcat(header,"\n");
        writen(sock,header, strlen(header));
        char response[20000];
        readHttpResponse(sock, response);
        //printf("%s\n",response);
        cJSON *json = cJSON_Parse(response); 
        cJSON *hourlyWeatherList = cJSON_DetachItemFromObject(json, "hourly"); 
        cJSON_Delete(json);
        close(sock);
        return hourlyWeatherList;
}

int main(int argc, char *argv[]){
        int ch;
        int hourly = 0, now = 1;
        char cityName[100];
        while ((ch = getopt(argc,argv,"h:n")) != -1){
                switch (ch){
                        case 'h':
                                hourly = atoi(optarg);
                                if (hourly > 24)
                                        hourly = 24;
                                now = 0;
                                break;
                        case 'n':
                                now = 1;
                                hourly = 0;
                                break; 
                }
        }
        strcpy(cityName, argv[1]);
        struct coords *kordinate;
        kordinate = getCountryCoords(cityName);
        char *latStr, *lonStr;
        latStr = ftoa(kordinate->lat);
        lonStr = ftoa(kordinate->lon);
        if (now){
               getBasic(latStr, lonStr);
        }else if (hourly){
               cJSON *weatherList;
               weatherList = get48Hours(latStr, lonStr);
               displayWeatherToday(weatherList, hourly);
               cJSON_Delete(weatherList);
        }
        /*
        time_t currentTimestamp;
        struct tm *timeThis;
        time(&currentTimestamp);
        timeThis= gmtime(&currentTimestamp);
        printf("%d\n",timeThis->tm_year+1900); 
        */
        free(kordinate);
        free(latStr);
        free(lonStr);
        return 0;
}
