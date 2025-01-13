#include "toralize.h"

Req* Request(const char* dstip_, const int dstport_) {
    Req* req = malloc(reqsize);
    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport_);
    req->dstip = inet_addr(dstip_);
    strncpy(req->userid, USERNAME, 8);
    return req;
};

void CloseConnection(int s, Req* req) {
    close(s);
    free(req);
}

int main(int argc, char** argv) {
    char* host;
    int port, s;
    struct sockaddr_in sock;
    int success = 0;
    char tmp[512];

    Req* req;
    Res* res;
    char buf[ressize];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>", argv[0]);
        return -1;
    }

    host = argv[1];
    port = atoi(argv[2]);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");

        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXY_PORT);
    sock.sin_addr.s_addr = inet_addr(PROXY_ADDR);

    if (connect(s, (struct sockaddr*)&sock, sizeof(sock))) {
        printf("err: can't connect to tor proxy with %s:%d\n", PROXY_ADDR, PROXY_PORT);
        perror("connect");
        return -1;
    }

    printf("connected to proxy\n");
    printf("connected to tor proxy with %s:%d\n", PROXY_ADDR, PROXY_PORT);
    req = Request(host, port);

    write(s, req, reqsize);     // to send request
                                // (writing a pre-filled request to socket "s")

    memset(buf, 0, ressize);            // to clean buffer


    printf("waiting for response... \n");
    int response_code = read(s, buf, ressize);
    printf("response received\n");
    if (response_code == 0) {
        printf("connection closed by server\n");
        return -1;
    }
    if (response_code < 1) {    // to get response
        perror("read");
        close(s);
        free(req);
        return -1;
    }

    
    res = (Res*)buf;
    success = (res->cd == 90);  // 90: request granted
                                // 91: request rejected or failed
                                // 92: request rejected becasue SOCKS server cannot connect to
                                //     identd on the client
                                // 93: request rejected because the client program and identd
                                //     report different user-ids
    
    if (!success) {
        fprintf(stderr, "Unable to traverse\nthe proxy, error code: %d\n", res->cd);
        close(s);
        free(req);
        return -1;
    }
    
    printf("Successfully connected through the proxy to %s:%d\n", host, port);

    memset(tmp, 0, 512);
    snfprintf(tmp, 511, "HEAD / HTTP/1.0\r\n");
    write(s, tmp, strlen(tmp));
    memset(tmp, 0, 512);
    read(s, tmp, 511);
    printf("'%s'\n", tmp);

    close(s);
    free(req);

    return 0;
}
