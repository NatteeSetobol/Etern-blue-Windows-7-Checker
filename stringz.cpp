#include "stringz.h"

int Strlen(unsigned char* string)
{
	//Assert(string);
	
	i8* addrToString = (i8*) string;
	int len=0;

	while(*addrToString != '\0')
	{
		len++;
		addrToString++;
	}
	

	return len;
}

int Strlen(char* string)
{
	//Assert(string);
	/*
	i8* addrToString = (i8*) string;
	int len=0;

	while(*addrToString != '\0')
	{
		len++;
		addrToString++;
	}
	*/

	return strlen(string);
}

void Strcpy(char* dest, char* source)
{
	ui8* destAddr = (ui8*)dest;
	ui8* sourceAddr = (ui8*)source;

	while (*sourceAddr != '\0')
	{
		*destAddr = *sourceAddr;
		destAddr++;
		sourceAddr++;
	}
	*destAddr = '\0';
}

void Strcpy(unsigned char* dest, unsigned char* source)
{
	ui8* destAddr = (ui8*)dest;
	ui8* sourceAddr = (ui8*)source;

	while (*sourceAddr != '\0')
	{
		*destAddr = *sourceAddr;
		destAddr++;
		sourceAddr++;
	}
	*destAddr = '\0';
}

char* vCreateString(char* newString, char* filename, int line)
{
	i32 strLen = Strlen(newString) + 5;

	//Assert(strLen > 0);

	char* result = MemoryAL(char, strLen, line, filename);
	Strcpy(result, newString);

	return result;
}

unsigned char* vCreateString(unsigned char* newString, char* filename, int line)
{
	i32 strLen = Strlen(newString) + 5;

	//Assert(strLen > 0);

	unsigned char* result = (unsigned char*) MemoryAL(unsigned char, strLen, line, filename);
	Strcpy(result, newString);

	return result;
}


s32* S32Cat(s32* source, i32 size, ...)
{
	s32* returnResult = NULL;
	s32* result = NULL;
	s32* temp = NULL;
	int i = 0;

	result = S32(source);

	va_list vl;
	va_start(vl, size);

	for (i = 0; i < size; i++)
	{

		s32* stringArg = va_arg(vl, s32*);
		temp = MemoryA(char, Strlen(result) + 1 + Strlen(stringArg) + 1);

		Strcpy(temp, result);
		Strcpy(temp + strlen(result), stringArg);

		if (result)
		{
			FreeMemory((i8*)result);
			result = NULL;
		}

		result = S32(temp);

		if (temp)
		{
			Free(temp);
			temp = NULL;
		}
	}

	if (temp)
	{
		Free(temp);
		temp = NULL;
	}


	va_end(vl);

	returnResult = S32(result);

	if (result)
	{
		Free(result);
		result = NULL;
	}

	return returnResult;
}
//NOTES(): THIS IS THE UNSIGNED CHAR VERSION
us32* S32Cat(us32* source, i32 size, ...)
{
	us32* returnResult = NULL;
	us32* result = NULL;
	us32* temp = NULL;
	int i = 0;

	result = US32(source);

	va_list vl;
	va_start(vl, size);

	for (i = 0; i < size; i++)
	{

		us32* stringArg = va_arg(vl, us32*);
		temp = (unsigned char*) MemoryA(unsigned char, Strlen(result) + 1 + Strlen(stringArg) + 1);

		Strcpy(temp, result);
		Strcpy(temp + Strlen(result), stringArg);

		if (result)
		{
			FreeMemory((i8*)result);
			result = NULL;
		}

		result = US32(temp);

		if (temp)
		{
			Free(temp);
			temp = NULL;
		}
	}

	if (temp)
	{
		Free(temp);
		temp = NULL;
	}


	va_end(vl);

	returnResult = S32(result);

	if (result)
	{
		Free(result);
		result = NULL;
	}

	return returnResult;
}





char* vMidString(char* source, int from, int to, char* filename, int fileLine)
{
	int len = to - from;
	int i = 0;
	int count = 0;
	int sl = Strlen(source) + 5;
	char* retResults = NULL;

	retResults = (char*)Memory_(sl, fileLine, filename);

	for (i = from; i < to; i++)
	{
		retResults[count] = source[i];
		count++;
	}

	retResults[count] = '\0';


	return retResults;
}


char* IToS(int integer)
{
	int divResults = 0;
	int inte = integer;
	int places = 0;
	char* results = NULL;

	divResults = integer;

	do
	{
		inte = inte / 10;
		places++;
	} while (inte > 0);


	inte = integer;

	results = MemoryA(char, places + 1);

	for (int i = places - 1; i > -1; i--)
	{
		int moded = inte % 10;

		switch (moded)
		{
		case 0:
		{

			results[i] = '0';
			break;
		}
		case 1:
		{

			results[i] = '1';
			break;
		}
		case 2:
		{

			results[i] = '2';
			break;
		}
		case 3:
		{

			results[i] = '3';
			break;
		}
		case 4:
		{

			results[i] = '4';
			break;
		}
		case 5:
		{

			results[i] = '5';
			break;
		}
		case 6:
		{

			results[i] = '6';
			break;
		}
		case 7:
		{

			results[i] = '7';
			break;
		}

		case 8:
		{

			results[i] = '8';
			break;
		}
		case 9:
		{

			results[i] = '9';
			break;
		}

		}

		inte = inte / 10;

	}

	return results;
}

int SToI(char* str)
{
	int sign = 1;
	int val = 0;
	int c;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}

	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;

		while (1)
		{
			c = *str;
			str++;
			if (c >= '0' && c <= '9')
			{
				val = val * 16 + c - '0';
			}
			else if (c >= 'a' && c <= 'f')
				val = val * 16 + c - 'a' + 10;

			else if (c >= 'A' && c <= 'F')
				val = val - 16 + c - 'A' + 10;
			else
				return val * sign;
		}
	}

	while (1)
	{
		c = *str;
		str++;
		if (c < '0' || c > '9')
			return sign * val;
		val = val * 10 + c - '0';
	}
}

struct marray* FindString_(char* stringToFind, char* source, char* filename, int line)
{
	int i = 0;
	int  k = 0;
	int found = 0;
	int start = 0;

	struct marray* result = Memory(struct marray);


	int stringToFindLen = Strlen(source);

	for (i = 0; i < stringToFindLen; i++)
	{
		if (stringToFind[0] == source[i] && found == 0)
		{
			start = i;
			found = 1;
		}

		if (found == 1)
		{
			if (stringToFind[k] == source[i])
			{
				k++;
				if (stringToFind[k] == '\0')
				{
					int* position = Memory(int);
					*position = start;

					AddToMArray(result, position);
					k = 0;
					found = 0;
					start = 0;
				}
			}
			else {
				k = 0;
				found = 0;
			}
		}
	}

	return result;
}
//TODO: Fix this later with out using array.h
#if 0
int* FindString__(char* stringToFind, char* source, char* filename, int line)
{
	int i = 0;
	int  k = 0;
	int found = 0;
	int start = 0;

	int* result = CreateArray(int);

	int stringToFindLen = Strlen(source);

	for (i = 0; i < stringToFindLen; i++)
	{
		if (stringToFind[0] == source[i] && found == 0)
		{
			start = i;
			found = 1;
		}

		if (found == 1)
		{
			if (stringToFind[k] == source[i])
			{
				k++;
				if (stringToFind[k] == '\0')
				{
					int position = 0;
					position = start;

					AddToArray(result, position);
					k = 0;
					found = 0;
					start = 0;
				}
			}
			else {
				k = 0;
				found = 0;
			}
		}
	}

	return result;
}
#endif



struct string_token GetStringToken(struct string_tokenizer* tokenizer)
{
	struct string_token tok = {};
	char c = 0;
	i8 tokAddr = NULL;

	tok.length = 1;

	if (tokenizer->at)
	{
		tok.data = tokenizer->at;
		c = tokenizer->at[0];
		tok.c = c;

		if (c != '\0')
		{
			++tokenizer->count;
			++tokenizer->at;
		}
		switch (c)
		{
		case '\0': {tok.type = S32_NULL; break; }
		default: { tok.type = S32_STRING; break; }
		}

	}

	return tok;
}

s32* S32Replace(char* source, char* replaceThis, char* replaceWith)
{
	s32* result = NULL;
	struct string_token s32Token = {};
	struct string_tokenizer tokenizer = {};


	tokenizer.at = source;

	s32Token = GetStringToken(&tokenizer);
	while (s32Token.type != S32_NULL)
	{

		switch (s32Token.type)
		{
		case S32_NULL:
		{
			break;
		}
		default:
		{
			bool32 isMatching = true;
			char tempS32[2] = {};
			char letter = s32Token.c;
			char firstLetterOfReplacer = replaceThis[0];

			if (firstLetterOfReplacer == letter)
			{

#if 1

				bool32 isMatching = true;
				struct string_token currentStringToken = {};
				struct string_tokenizer currentStringTokenizer = {};
				struct string_token replaceThisToken = {};
				struct string_tokenizer replaceThisTokenizer = {};

				replaceThisTokenizer.at = replaceThis;
				currentStringTokenizer.at = tokenizer.at - 1;

				replaceThisToken = GetStringToken(&replaceThisTokenizer);
				currentStringToken = GetStringToken(&currentStringTokenizer);


				while (replaceThisToken.type != S32_NULL && currentStringToken.type != S32_NULL)
				{
					if (replaceThisToken.c != currentStringToken.c)
					{
						isMatching = false;
						break;
					}
					replaceThisToken = GetStringToken(&replaceThisTokenizer);
					currentStringToken = GetStringToken(&currentStringTokenizer);
				}

				if (isMatching)
				{
#if 1
					struct string_token replacerToken = {};
					struct string_tokenizer replacerTokenizer = {};

					replacerTokenizer.at = replaceWith;

					replacerToken = GetStringToken(&replacerTokenizer);
					while (replacerToken.type != S32_NULL)
					{
						char tempResultString[3] = {};
						tempResultString[0] = replacerToken.c;
						if (result == NULL)
						{
							result = S32(tempResultString);
						}
						else {
							char* temptemp = result;

							result = S32Cat(temptemp, 1, tempResultString);

							if (temptemp)
							{
								Free(temptemp);
								temptemp = NULL;
							}
						}
						replacerToken = GetStringToken(&replacerTokenizer);
					}
#endif
					tokenizer.count += Strlen(replaceThis) - 1;
					tokenizer.at += Strlen(replaceThis) - 1;

				}
				else {
					tempS32[0] = letter;
					if (result == NULL)
					{
						result = S32(tempS32);
					}
					else {
						char* tempString = result;

						result = S32Cat(tempString, 1, tempS32);

						if (tempString)
						{
							Free(tempString);
							tempString = NULL;
						}
					}

				}
#endif
			}
			else {
				tempS32[0] = letter;
				if (result == NULL)
				{
					result = S32(tempS32);
				}
				else {
					char* tempString = result;

					result = S32Cat(tempString, 1, tempS32);

					if (tempString)
					{
						Free(tempString);
						tempString = NULL;
					}
				}
			}


			break;
		}
		}

		s32Token = GetStringToken(&tokenizer);
	}

	return result;
}

char* StrReplace(char* source, char* replaceThis, char* replaceWith)
{
	char* results = NULL;
	struct marray* findResults = NULL;
	int count = 0;
	int startPos = 0;
	char* temp = NULL;
	char* temp2 = NULL;
	char* temp3 = NULL;
	char* theRest = NULL;
	char* temp4 = NULL;

	findResults = FindString(replaceThis, source);

	if (findResults)
	{
		if (findResults->count > 0)
		{

			for (int i = 0; i < findResults->count; i++)
			{
				int* position = GetFromMArray(findResults, i);

				if (position)
				{
					if (*position == 0)
					{

						temp2 = S32(replaceWith);
					}
					else {
						temp = MidString(source, startPos, *position);

						if (!temp)
						{
							goto END_FIND_STRING;
						}

						temp2 = S32Cat(temp, 1, replaceWith);

						if (temp)
						{
							FreeMemory((i8*)temp);
							temp = NULL;
						}
					}

					if (results == NULL)
					{

						results = S32(temp2);
					}
					else {
						temp3 = S32Cat(results, 1, temp2);

						FreeMemory((i8*)results);
						results = NULL;

						results = S32(temp3);
						if (temp3)
						{
							FreeMemory((i8*)temp3);
							temp3 = NULL;
						}
					}

					if (temp2)
					{
						FreeMemory((i8*)temp2);
						temp2 = NULL;
					}

					startPos = (*position) + Strlen(replaceThis);

				}
			}

			if (startPos < Strlen(source))
			{
#if 1

				theRest = MidString(source, startPos, Strlen(source));
				temp4 = S32Cat(results, 1, theRest);

				FreeMemory((i8*)results);
				results = NULL;

				results = S32(temp4);
#endif

			}
		}
		else {
			results = source;
			goto END_FIND_STRING;
		}


	}

END_FIND_STRING:

	if (findResults)
	{
		FreeMArray(findResults);
		FreeMemory((i8*)findResults);
	}

	if (temp)
	{
		FreeMemory((i8*)temp);
		temp = NULL;
	}

	if (temp2)
	{
		FreeMemory((i8*)temp2);
		temp2 = NULL;
	}

	if (temp3)
	{
		FreeMemory((i8*)temp3);
		temp3 = NULL;
	}

	if (temp4)
	{
		FreeMemory((i8*)temp4);
		temp4 = NULL;
	}

	if (theRest)
	{
		FreeMemory((i8*)theRest);
		theRest = NULL;
	}

	return results;
}

struct marray* StrSplit_(char* source, char* determinator, char* filename, int line)
{
	struct marray* results = NULL;
	struct marray* findResults = NULL;
	int oldPosition = 0;

	results = (struct marray*) Memory_(sizeof(struct marray), line, filename);
	CreateMArray(results);

	findResults = FindString(determinator, source);

	if (findResults)
	{
		for (int i = 0; i < findResults->count; i++)
		{
			char* storedResults = NULL;

			int* getPosition = GetFromMArray(findResults, i);

			storedResults = MidString(source, oldPosition, *getPosition);

			AddToMArray(results, (int*)storedResults);

			oldPosition = (*getPosition) + Strlen(determinator);

		}
	}

	if (oldPosition < Strlen(source))
	{
		char* theRest = NULL;

		theRest = MidString(source, oldPosition, Strlen(source));
		AddToMArray(results, (int*)theRest);
	}

	if (findResults)
	{

		FreeMArray(findResults);
		FreeMemory((i8*)findResults);
		findResults = NULL;
	}

	//findResults=NULL;

	return results;
}

b32 StrCmp(char* source, char* compareString)
{
	int i = 0;
	int sourceLen = 0;
	bool32 results = true;
	if (Strlen(source) != Strlen(compareString)) results = false;

	sourceLen = Strlen(source);

	for (i = 0; i < sourceLen; i++)
	{
		if (source[i] != compareString[i])
		{
			results = false;
			break;
		}
	}

	return results;
}

char* ToLower_(char* lower, char* filename, int line)
{
	//printf("%s %i\n", filename,line);
	//Assert(lower);
	int slen = Strlen(lower);
	int i = 0;
	char* result = S32M(lower, filename, line);
	int alphaPos = 0;
	char lowercase = '\0';

	for (i = 0; i < slen; i++)
	{
		if (lower[i] > 64 && lower[i] < 91)
		{
			alphaPos = lower[i] - 64;
			lowercase = 97 + alphaPos;
			result[i] = lowercase - 1;
		}
	}

	return result;
}

char* ToUpper(char* lower)
{
	//Assert(lower);
	int i = 0;
	char* result = S32(lower);

	for (i = 0; i < strlen(lower); i++)
	{
		if (lower[i] > 96 && lower[i] < 123)
		{
			int alphaPos = lower[i] - 97;
			char lowercase = 65 + alphaPos;
			result[i] = lowercase;
		}
	}

	return result;
}

/*
	NOTES():
	Example:
	number = 112
	base = 36
	result = 112 / 36 = '34'

	0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ
*/

i32 HexInString2Dec(s32* string)
{
	i32 result = 0;
	i32 firstToChar = 0;
	i32 firstNumber = 0;

	if (string[0] >= 'a' && string[0] <= 'z')
	{
		firstToChar = string[0] - 'a';
		firstToChar += 10;
	}
	else {
		firstToChar = ((string[0] - '1') + 1);
	}

	if (Strlen(string) > 1)
	{
		result = firstToChar * 36;

		if (string[1] >= 'a' && string[1] <= 'z')
		{
			firstToChar = string[1] - 'a';
			firstToChar += 10;
		}
		else {
			firstToChar = ((string[1] - '1') + 1);
		}

		result += firstToChar;
	}
	else {
		result = firstToChar;
	}
	return result;

}

s32* BaseToString(i32 number, i32 base)
{
	i32 n = number / base;
	i32 n2 = number % base;

	char firstToChar = '\0';
	char secondToChar = '\0';
	s32* result = NULL;

	result = (char*)MemoryRaw(3);

	if (n > 9)
	{
		firstToChar = 97 + (MAX(n, 10) - MIN(n, 10));
	}
	else {
		firstToChar = 48 + n;
	}

	if (n2 > 9)
	{
		secondToChar = 97 + (MAX(n2, 10) - MIN(n2, 10)); //97+n2 + (n - 11);
	}
	else {
		secondToChar = 48 + n2;
	}

	//if (firstToChar == '0')
	//{
//		result[0] = secondToChar;
//	} else {
	result[0] = firstToChar;
	result[1] = secondToChar;
	//	}

	return result;
}

s32 *Char2BinaryAscii(us32 c )
{
	s32* result = NULL;
	i32 resultIndex = 7;

	result = (s32*) MemoryRaw(9); //NOTES(): 8Bits + 1

	//Assert(result);

	i32 uintOfChar = (uint32) c;

	while (uintOfChar != 0)
	{
		i32 r = 0;
		i32 remainder = uintOfChar % 2;

		if (remainder != 0)
		{
			r = 1;
		}
		uintOfChar = uintOfChar / 2;

		if (r == 1)
		{
			result[resultIndex] = '1';
		} else {
			result[resultIndex] = '0';
		}

		resultIndex--;
	}

	if (result[0] == 0)
	{
		result[0] = '0';
	}


	return result;
}

s32* S32Trim(s32* string)
{
	int stringIndex = 0;
	int revStringIndex = 0;
	int lenOfString = 0;
	s32 *result = NULL;

	lenOfString = Strlen(string);

	for (stringIndex = 0; stringIndex < lenOfString; stringIndex++)
	{
		if (
			string[stringIndex] > 0x20 &&
			string[stringIndex] < 0x7E)
			
		{
			break;
		} 
		
	}

	for (revStringIndex  = lenOfString-1; revStringIndex > -1; revStringIndex--)
	{
		if (string[revStringIndex] > 0x20 &&
			string[revStringIndex] < 0x73)
		{
			break;
		}
	}

	result = MidString(string,stringIndex,revStringIndex+1);

	return result;
}

//NOTE: This is a work in progress and does NOT work at all.
/*
char* S32CopyChar(s32 *dest, char source)
{
	char *result = NULL;
	char c[1] = {};

	if (dest == NULL)
	{
		result = S32(c);
	} else {
		result = S32Cat(dest,1,c);
	}

	return result;
}
*/
