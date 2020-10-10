#include "nix_socket.h"

struct platform_socket CreateSocket(char* hostname, int port)
{
	//TODO(): Implement IPv6 -- Not sure how to do this on nix machines yet. Need to do more research on this because gethostbyname doesn't support ipv6!!.
	//NOTE(): IPv6 Does not work yet.
	struct platform_socket result = {};
	addrinfo hints = {};
	addrinfo* addrinfoResults = NULL;
	int addrInfoRet = 0;
	char * host = NULL;
	struct sockaddr_in dest_addr={};
	struct hostent *hostEnt = NULL;
	/*
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol |= AI_CANONNAME;
	*/

	result.connected = true;
	host = StripToHostName(hostname);

	hostEnt = gethostbyname(host);

	if (addrInfoRet != 0)
	{
		printf( "error getting address info\n");

		result.connected = false;
	} else {
		
		result.socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (result.socket != -1) 
		{
			struct sockaddr_in *sockin = NULL;


			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port = htons(port);
			sockin = (struct sockaddr_in*) &addrinfoResults->ai_addr;

			memcpy(&dest_addr.sin_addr, hostEnt->h_addr_list[0], hostEnt->h_length);
		
			memset(&(dest_addr.sin_zero),'\0',8);

			if (connect(result.socket, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1)
			{
				result.connected = false;
				printf("could not connect to %s\n", host);
			}
		} else {

			result.connected = false;
			printf("socket failed\n");
		}

		if (host)
		{
			Free(host);
			host=NULL;
		}
		
	}

	return result;
}


int SendToSocket(struct platform_socket* platformSocket, char* message, int length)
{
	int result = 0;

	result = send(platformSocket->socket,message,length,0);

	return result;
}

int RecvFromSocket(struct platform_socket* platformSocket, char* dest, int len)
{
	int result = 0;

	result = recv(platformSocket->socket,dest,len,MSG_DONTWAIT);

	return result;
}
