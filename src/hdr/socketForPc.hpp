#ifndef __SOCKET_FOR_PC_HPP__
#define __SOCKET_FOR_PC_HPP__

#include "Standard.hpp"

#define BUF_SIZE 100
#define MAX_CLNT 32
#define ID_SIZE 10
#define ARR_CNT 5
#define BOT_USER 1
#define STOP_SIG 99999

#define DEBUG
typedef struct {
    char fd;
    char *from;
    char *to;
    char *msg;
    int len;
}MSG_INFO;

typedef struct {
    int index;
    int fd;
    char ip[20];
    char id[ID_SIZE];
    char pw[ID_SIZE];
}CLIENT_INFO;

void* TX_thread(void* arg);
int main();

extern pthread_mutex_t g_mutex;
extern std::string g_string;
extern std::string g_endString;


#endif
