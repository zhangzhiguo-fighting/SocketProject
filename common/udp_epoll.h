#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H
void add_event(int epollfd, int fd, int events);
void add_del(int epollfd, int fd, int events);
int udp_connect(int epollfd, struct sockaddr_in *serveraddr);
int udp_accept(int epollfd, int fd);
#endif
