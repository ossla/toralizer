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

    Req* req;
    Res* res;
    char buf[ressize];

    if (argc < 3) {
        // the error occurs if we have incorrect usage
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
        perror("connect");

        return -1;
    }

    printf("Connected to proxy\n");

    req = Request(host, port);
    write(s, req, reqsize);

    memset(buf, 0, ressize); // to clean buffer
    if (read(s, buf, ressize) < 1) {
        perror("read");
        CloseConnection(s, req);
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
        CloseConnection(s, req);
        return -1;
    }
    
    printf("Successfully connected through the proxy to %s:%d\n", host, port);

    close(s);
    free(req);

    return 0;
}
