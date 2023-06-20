#include <sys/timerfd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/epoll.h>
 
 
#define handle_error(msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)
 
void print_elapsed_time(void);
 
static void msec2tspec(int msec, struct timespec *ts)
{
	if (msec) {
		ts->tv_sec  =  msec / 1000;
		ts->tv_nsec = (msec % 1000) * 1000000;
	} else {
		ts->tv_sec  = 0;
		ts->tv_nsec = 0;
	}
}
 
int main(void)
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd == -1)
    {
        handle_error("timerfd_create");
    }
 
    struct itimerspec new_value = {};
	msec2tspec(10, &new_value.it_value);
	msec2tspec(10, &new_value.it_interval);
    if (timerfd_settime(timerfd, 0, &new_value, NULL) == -1)
    {
        handle_error("timerfd_settime");
    }
 
    print_elapsed_time();
    printf("timer started\n");
 
 
    int epollfd = epoll_create(1); // or epoll_create(1)
    if (epollfd == -1)
    {
        handle_error("epoll_create1");
    }
 
    struct epoll_event ev;
    ev.events = EPOLLIN; // 表示该文件描述符可以读的时候就触发
    ev.data.fd = timerfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &ev);
    const int maxEvents = 5; // 也可以设置为1
    struct epoll_event events[maxEvents]; 
    while (1)
    {
        int nfd = epoll_wait(epollfd, events, maxEvents, -1);
        if (nfd > 0)
        {
            for (int i = 0; i < nfd; ++i)
            {
                if (events[i].data.fd == timerfd)
                {
                    uint64_t exp = 0;
                    int ret = read(timerfd, &exp, sizeof(uint64_t));
                    if (ret != sizeof(uint64_t)) 
                    {
                        handle_error("read timerfd");
                    }
                    
                    print_elapsed_time();
                }
            }
        }
    }
    return 0;
}
 
void print_elapsed_time(void)
{
    static struct timeval start = {};
    static int first_call = 1;
 
    if (first_call == 1)
    {
        first_call = 0;
        if (gettimeofday(&start, NULL) == -1)
        {
            handle_error("gettimeofday");
        }
    }
 
    struct timeval current = {};
    if (gettimeofday(&current, NULL) == -1)
    {
        handle_error("gettimeofday");
    }
 
    static int old_secs = 0, old_usecs = 0;
 
    int secs  = current.tv_sec - start.tv_sec;
    int usecs = current.tv_usec - start.tv_usec;
    if (usecs < 0)
    {
        --secs;
        usecs += 1000000;
    }
 
    usecs = (usecs + 500)/1000; // 四舍五入
 
    if (secs != old_secs || usecs != old_usecs)
    {
    	printf("%d.%03d\n", secs, usecs);
    	old_secs = secs;
    	old_usecs = usecs;
    }
 
}