#include "Socket.h"

namespace w
{
    bool Socket::bind(const std::string &ip, int port)
    {
        sockaddr_in ser_addr;
        memset(&ser_addr, 0, sizeof(ser_addr));
        ser_addr.sin_family = AF_INET;
        ser_addr.sin_port = htons(port);
        ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        int ret = ::bind(m_sockfd, (sockaddr *)&ser_addr, 0);
        if (ret == -1)
        {
            return false;
        }
        return true;
    }
}