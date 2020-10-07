#ifndef _NIX_SOCKET_H_
#define _NIX_SOCKET_H_
#include <string.h>
#include <stdio.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct platform_socket
{
	int socket;
	bool connected;
};


#endif
