#include "toralize.h"

Req* request(const char* dstip_, const int dstport_) {
    Req* req = malloc(reqsize);
    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport_);
    req->dstip = inet_addr(dstip_);
    strncpy(req->userid, USERNAME, 7);
    return req;
};

int main(int argc, char** argv) {
    char* host;
    int port, s;
    struct sockaddr_in sock;

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
    close(s);
    return 0;
}
