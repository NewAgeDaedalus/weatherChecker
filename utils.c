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
#include <math.h>

ssize_t readn(int sock, void *ptr, size_t n){
        size_t nleft = n;
        ssize_t nread; 
        char *msg = (char*) ptr;
        while ( nleft > 0){
                if((nread = read(sock, msg, nleft)) < 0){
                        if (errno == EINTR)
                                nread = 0;
                        else
                                return -1;
                }else if(nread == 0)
                        break;
                nleft -=nread;
                msg += nread;
        }
        return n -nleft;
}

ssize_t writen(int sock, void *ptr, size_t n){
        size_t nleft = n;
        ssize_t nwritten = 0;
        char *msg = (char*)ptr;
        while (nleft > 0){
                if ((nwritten = write(sock, msg, nleft)) <= 0){
                        if(errno == EINTR && nwritten < 0)
                                nwritten = 0;
                        else{
                                err(1,"write error");
                                return -1;
                        }
                }
                nleft -=nwritten;
                ptr += nwritten;
        }
        return n;
}

int readHttpResponse(int sock, char *response){
        //read the header
        int numberOfNewLines = 0, err;
        char header[1024]="";
        //Weird shit going on. It would be more logical if it were <2, but it doesn't work.
        //int nLen = readn(sock, header, 150);
        //header[nLen] = '\0';
        //printf("%s\n", header);
        while (numberOfNewLines < 3 ){
                char buff[2] = "";
                //printf("%d\n", err);
                readn(sock, buff, 1);
                if (buff[0] == '\n' || buff[0] == '\r'){
                        numberOfNewLines++;
                }
                else
                        numberOfNewLines = 0;
                strcat(header, buff);
        }
        //check the code, if it's 200 read the body otherwise
        //return -1 and write the header in response
        char code[4] = "";
        int i = 0;
        for (i = 0; ' ' != header[i]; i++);
        i++;
        for (int j = 0; header[i] != ' '; ){
                code[j] = header[i];
                i++;
                j++;
        }
        if (strcmp(code, "200")){
                strcpy(response, header);
                printf("Here\n");
                return -1;
        }
        //Lookup content length
        char *word = (char*) malloc(1500);
        int found = 0;
        int contenLen = 0;
        //read body
        for (char *c = header; *c != '\0'; c++){
                if ( *c == '\n' || *c == ' '){
                        if (found && strcmp(word, "")){
                                contenLen = atoi(word);
                                break;
                        }
                        if (!strcmp(word, "Content-Length:"))
                                found = 1;
                        strcpy(word,"");
                        continue;
                }
                int len = strlen(word);
                word[len] = *c;
                word[len+1] = '\0';
        }
	readn(sock, word, 1);
        free(word);
        readn(sock,response, contenLen);
        return 0;
}

//Makes a tcp conneection given a hostname
//returns socket number.
int tcpConnect( const char *hostname){
//         printf("Connecting to %s\n", hostname);
        int sock, error;
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock < 0)
                err(1,"sock error");
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        error = getaddrinfo(hostname,"http", &hints, &res);
        if (error){
                if (error == EAI_SYSTEM)
                        printf( "looking up %s: %s\n",hostname, strerror(errno));
                else
                        printf( "looking up %s: %s\n",hostname, gai_strerror(error));
                return -1;
        }
        struct addrinfo *cur = res;
        while(cur != NULL){
                char addres[100];
                inet_ntop(AF_INET,&((struct sockaddr_in*)cur->ai_addr)->sin_addr,addres,100);
                struct sockaddr_in myAddr;
                memset(&myAddr, 0, sizeof(myAddr));
                myAddr.sin_family = AF_INET;
                myAddr.sin_port = 0;
                if (bind(sock,(struct sockaddr*) &myAddr, sizeof(myAddr)) < 0)
                        err(1, "bind erro");
                struct sockaddr_in apiAddress;
                apiAddress.sin_family= AF_INET;
                apiAddress.sin_port = htons(80);
                apiAddress.sin_addr.s_addr=inet_addr(addres);
                socklen_t sin_size;
                sin_size = sizeof(struct sockaddr);
                if(connect(sock,(struct sockaddr *) &apiAddress, sin_size) == -1){
                        printf("failed to connect to %s\n", addres);
                }
                else{
                        break;
                }
                cur = cur->ai_next;
        } 
       return sock; 
}

double convertToC(double temp){
        return temp - 273.15;
}

char *ftoa(double number){
        int negative = 0, len = 1;
        if (number < 0){
                negative = 1;
                len++;
                number = fabs(number);
        }
        int preDecimal = (int) number;
        char preDecimalStr[100] = "";
        char postDecimalStr[100] = "";
        double postDecimal= number - ((int) number);
        
        if (preDecimal == 0){
                strcpy(preDecimalStr, "0");
                len++;
        } else{
                while (preDecimal){   
                        char znam[2] = "";
                        znam[0] = preDecimal%10 + '0';
                        strcat(preDecimalStr, znam);
                        preDecimal = preDecimal/10;
                        len++;
                }
        }
        while(postDecimal > 0){
                char znam[2] = "";
                znam[0] = (int)(postDecimal*10) +'0';
                strcat(postDecimalStr, znam);
                postDecimal = postDecimal*10;
                postDecimal = (int)postDecimal -postDecimal;
                len++;
        }
        int preDecimalLen = strlen(preDecimalStr);
        for (int i = 0; i < (int)(preDecimalLen/2);i++){
                char tmp;
                tmp = preDecimalStr[i];
                preDecimalStr[i] = preDecimalStr[preDecimalLen-i-1];
                preDecimalStr[preDecimalLen-i-1] = tmp;
        }
        int postDecimalLen= strlen(postDecimalStr);
        for (int i = 0; i < (int)(postDecimalLen/2);i++){
                char tmp;
                tmp = postDecimalStr[i];
                postDecimalStr[i] = postDecimalStr[postDecimalLen-i-1];
                postDecimalStr[postDecimalLen-i-1] = tmp;
        }
        char *res = (char*) malloc(len+1);//+1 za '\0'
        strcpy(res,"");
        if (negative)
                strcat(res, "-");
        strcat(res, preDecimalStr);
        strcat(res, ".");
        strcat(res, postDecimalStr);
        return res;
}
