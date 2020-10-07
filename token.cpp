#include "token.h"

struct token MoveToken(struct tokenizer* tokenizer)
{
	struct token result = {};

	result.length = 1;
	result.data = tokenizer->at;
	result.token = tokenizer->at[0];


	if (result.token != '\0')
	{
		++tokenizer->count;
		++tokenizer->at;
	}

	return result;
}
