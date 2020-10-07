#include "socket.h"

enum search_token_type GetSearchToken(struct tokenizer* tokenizer)
{
	enum search_token_type result;

	switch (tokenizer->at[0])
	{
		case '\0': { result = SEARCH_DATA_NONE; break; }
		case '/': { result = FORWARD_SLASH; break; }
		case ':': { result = SEARCH_COLON; break; }
		default: { result = SEARCH_DATA; break; }
	}

	return result;
}

char* StripToHostName(char* host)
{

	char* result = NULL;
	bool found = false;
	bool httpFound = false;
	int forwardSlashCount = 0;
	int slashPos = -1;
	char* temp = NULL;

	struct tokenizer sTokenizer = {};
	struct token sToken = {};

	sTokenizer.at = host;

	do
	{

		sToken.type = GetSearchToken(&sTokenizer);

		switch (sToken.type)
		{
			case SEARCH_COLON:
			{
				char* http = MidString((char*)host, 0, sTokenizer.count);

				if (http)
				{
					if (StrCmp(http, (char*) "http") || StrCmp(http,(char*)  "https"))
					{
						httpFound = true;
					}

					if (http)
					{
						Free(http);
						http = NULL;
					}
				}
				break;
			}
			case FORWARD_SLASH:
			{
				forwardSlashCount++;

				if (forwardSlashCount == 1)
				{
					if (slashPos == -1)
					{
						slashPos = sTokenizer.count;
					}
				}
				if (forwardSlashCount == 2 && httpFound == true)
				{
					slashPos = sTokenizer.count;
				}
				if (forwardSlashCount == 3 && httpFound == true)
				{
					found = true;
				}
				break;
			}
			case SEARCH_DATA_NONE:
			{
				break;
			}
		}

		sToken = MoveToken(&sTokenizer);
		sToken.type = GetSearchToken(&sTokenizer);

	} while (sToken.type != SEARCH_DATA_NONE && found == false);
	
	if (found)
	{
		result = MidString(host, slashPos+1, sTokenizer.count - 1);
	}
	else
	if (httpFound && found == false)
	{
		result = MidString(host, slashPos+1, strlen(host));
	}
	else
	{
		if (slashPos != -1)
		{
			result = MidString(host, 0, slashPos - 1);
		}
		else {
			result = MidString(host, 0, strlen(host));
		}
	}

	return result;
}
