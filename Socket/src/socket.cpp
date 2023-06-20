#include "socket.h"


namespace w{

    Socket::Socket(int family, int type, int protocol)
    : m_family(family), m_type(type), m_protocol(protocol)
    {
    }

    Socket::~Socket()
    {
    }

}