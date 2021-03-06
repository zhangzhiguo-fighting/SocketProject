#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H
#include "datatype.h"
void add_event(int epollfd, int fd, int events);
void del_event(int epollfd, int fd); 
void add_event_ptr(int epollfd, int fd, int events, struct User *user);
void del_event_ptr(int epollfd, int fd, int events, struct User *user);
//int udp_connect(int epollfd, struct sockaddr_in *serveraddr);
int udp_accept(int epollfd, int fd, struct User *user);
void add_to_sub_reactor(struct User *user);
#endif
