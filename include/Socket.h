#pragma once

#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


namespace w{
    class Socket
    {
    public:
        Socket();
        ~Socket();

        bool bind(const std::string &ip, int port);
        bool listen(int backlog);
        bool connect(const std::string &ip, int port);
        bool close();

        int accept();
        int recv(char * buf, int len);
        int send(const char * buf, int len);
        
        bool set_non_blocking();
        bool set_send_buffer(int size);
        bool set_recv_buffer(int size);
        bool set_linger(bool active, int seconds);
        bool set_keep_alive();
        bool set_reuse_addr();
        bool set_reuse_port();
    private:
        int m_port;
        int m_sockfd;
        std::string m_ip;
    };
    
    Socket::Socket(/* args */)
    {
    }
    
    Socket::~Socket()
    {
    }
    
}