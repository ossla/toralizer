#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PROXY_ADDR  "127.0.0.1"
#define PROXY_PORT  9050
#define USERNAME    "toraliz" // 7 bytes max
#define reqsize sizeof(struct proxy_request)
#define ressize sizeof(struct proxy_response)

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;

struct proxy_request {
    // 		request SOCKS4
    //      +----+----+----+----+----+----+----+----+----+----+....+----+
    // 		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
    // 		+----+----+----+----+----+----+----+----+----+----+....+----+
    //  	   1    1      2              4           variable(64)   1

    int8 vn;
    int8 cd;
    int16 dstport;
    int32 dstip;
    unsigned char userid[8];
};
typedef struct proxy_request Req;

struct proxy_response {
    //     +----+----+----+----+----+----+----+----+
    //     | VN | CD | DSTPORT |      DSTIP        |
    //     +----+----+----+----+----+----+----+----+
    //        1    1      2              4

    int8 vn;
    int8 cd;
    int16 _;  // not imprtnt
    int32 __; // not imprtnt
};
typedef struct proxy_response Res;

Req* request(const char*, const int); //dstip, dstport
int main(int, char**);