#pragma once

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/socket.h>

int initListenFd(unsigned short poort);
int epollRun(int lfd);
int acceptClient(int lfd, int epfd);
int recvHttpRequest(int cfd, int epfd);
int parseRequestLine(const char* line, int cfd);