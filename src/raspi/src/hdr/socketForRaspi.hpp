#ifndef __FOR_RASPI_HPP__
#define __FOR_RASPI_HPP__

#include "ros/ros.h"
#include "cmd_vel_pub/Cmd_vel_msg.hpp"

#include <pthread.h> 
#include <signal.h> 
#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>


#define BUF_SIZE 100 
#define NAME_SIZE 15 
#define ARR_CNT 5 
#define PUB_NODE_NAME "cmd_vel_pub" // name of node
#define SUB_NODE_NAME "cmd_vel_sub" // name of node
#define TOPIC_NAME "cmd_vel" // name of topic : cmd_vel_pub

void* RX_thread(void* arg);

extern pthread_mutex_t g_mutex;
extern int XToken;
extern int YToken;

#endif
