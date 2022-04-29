#include <unistd.h>
ssize_t readn(int , void*, size_t );
ssize_t writen(int , void *, size_t );
int readHttpResponse(int, char*);
int tcpConnect(const char *hostname);
char *ftoa(double number);
double convertToC(double temp);

struct coords{
        double lat;
        double lon;
};
