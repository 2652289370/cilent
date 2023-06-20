#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <ostream>
#include <memory>

namespace w{
    class Socket : public std::enable_shared_from_this<Socket>
    {
    public:
        typedef std::shared_ptr<Socket> Ptr;

        enum Type{
            TCP = SOCK_STREAM,
            UDP = SOCK_DGRAM
        };

        enum Family{
            IPv4 = AF_INET,
            IPv6 = AF_INET6,
            UNIX = AF_UNIX
        };
        Socket(int family, int type, int protocol = 0);
        virtual ~Socket();
    private:
        /* data */
    protected:
        int m_sock;
        int m_family;
        int m_type;
        int m_protocol;
    };

  


}
