#ifndef __TOKEN__
#define __TOKEN__
#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "stringz.h"

struct token
{
	size_t length;
	i32 type;
	s32 *data;
	char token;
};

struct tokenizer
{
	int count;
	s32 *at;
};

struct token MoveToken(struct tokenizer* tokenizer);

#endif

