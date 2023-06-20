#include "Server.h"


int initListenFd(unsigned short port)
{
    //创建监听fd
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket open");
        return -1;
    }
    //设置端口复用
    int opt = 1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if (ret == -1)
    {
        perror("setsockopt");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }
    

    return lfd;
}

int epollRun(int lfd)
{
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll_create");
        return -1;
    }
    struct epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }
    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(struct epoll_event);
    while (1)
    {
        int num = epoll_wait(epfd, evs, size, -1);
        for (int i = 0; i < num; i++)
        {
            int fd = evs[i].data.fd;
            if (fd == lfd)
            {
                acceptClient(lfd, epfd);
            }
            else
            {
                recvHttpRequest(fd, epfd);
            }
            
        }
        
    }
    return 0;
}

int acceptClient(int lfd, int epfd)
{
    int cfd = accept(lfd, NULL, NULL);
    if (cfd == -1)
    {
        perror("accept");
        return -1;
    }
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }
    return cfd;
}

int recvHttpRequest(int cfd, int epfd)
{
    int len = 0, totle = 0;
    char tmp[1024] = {0};
    char buf[4096] = {0};
    while ((len = recv(cfd, tmp, sizeof tmp, 0)) > 0)
    {
        if (totle + len < sizeof buf)
        {
            memcpy(buf + totle, tmp, len);
        }
        totle += len;
    }
    if (len == -1 && errno == EAGAIN)
    {
        char* pt = strstr(buf, "\r\n");
        int reqLen = pt - buf;
        buf[reqLen] = '\0';
        parseRequestLine(buf, cfd);
    }
    else if (len == 0)
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
        close(cfd);
    }
    else
    {
        perror("recv");
    }
    
    return 0;
}

int parseRequestLine(const char* line, int cfd)
{
    char method[12];
    char path[1024];
    sscanf(line, "%[^ ] %[^ ]", method, path);
    printf("method: %s, path: %s\n", method, path);
    if (strcasecmp(method, "get") != 0)
    {
        return -1;
    }
    char* file = NULL;
    if (strcmp(path, "/") == 0)
    {
        file = "./";
    }
    else
    {
        file = path + 1;
    }

    struct stat st;
    int ret = stat(file, &st);
    if (ret == -1)
    {
        return 0;
    }
    if (S_ISDIR(st.st_mode))
    {
        sendDir(file, cfd);
    }
    
    
    return 0;
}


int sendDir(const char* dirName, int cfd)
{
    char buf[4096] = {0};
    sprintf(buf, "<html><head><title>%s<title></head><body><table>", dirName);
    dirent** namelist;
    int num = scandir(dirName, &namelist, NULL, alphasort);
    for (size_t i = 0; i < num; i++)
    {
        char* name = namelist[i]->d_name;
        struct stat st;
        char subPath[1024] = {0};
        sprintf(subPath, "%s/%s", dirName, name);
        stat(subPath, &st);
        if (S_ISDIR(st.st_mode))
        {
            sprintf(buf + strlen(buf), 
                "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>", 
                name, name, st.st_size);
        }
        else
        {
            sprintf(buf + strlen(buf),
                "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
                name, name, st.st_size);
        }
        send(cfd, buf, strlen(buf), 0);
        memset(buf, 0, sizeof(buf));
        free(namelist[i]);
    }
    sprintf(buf, "</table></body></html>");
    send(cfd, buf, strlen(buf), 0);
    free(namelist);
    return 0;
}

int sendHeadMsg()
{

}