#ifndef __SOCKET_H__
#define __SOCKET_H__

char* StripToHostName(char* host);

#ifdef COMPILER_MSVC
#include "win_socket.h"
#else
#include "nix_socket.h"
#endif

#include "token.h"

enum Method { GET, POST,OPTIONS };

enum search_token_type
{
	SEARCH_DATA_NONE,
	FORWARD_SLASH,
	SEARCH_DATA,
	SEARCH_COLON
};

struct platform_socket CreateSocket(char* hostname, int port);
int SendToSocket(struct platform_socket* platormSocket, char* message, int length);
int RecvFromSocket(struct platform_socket* platformSocket, char* dest, int len);
enum search_token_type GetSearchToken(struct tokenizer* tokenizer);
#endif
