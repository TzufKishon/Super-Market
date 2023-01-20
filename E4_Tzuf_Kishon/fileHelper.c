#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileHelper.h"
#include "General.h"

typedef unsigned char BYTE;

int	 writeStringToFile(const char* str, FILE* fp, const char* msg)
{
	int length = (int)strlen(str);
	length++;
	if (!writeIntToFile(length, fp, msg))
		return 0;

	if (!writeCharsToFile(str, length, fp, msg))
		return 0;


	return 1;
}

int	 writeStringToCompressFile(const char* str, FILE* fp, const char* msg)
{
	int length = (int)strlen(str);
	if (!writeIntToFile(length, fp, msg))
		return 0;

	if (!writeCharsToFile(str, length, fp, msg))
		return 0;

	return 1;
}

int	 writeCharsToFile(const char* arr, int size, FILE* fp, const char* msg)
{
	if (fwrite(arr, sizeof(char), size, fp) != size)
	{
		puts(msg);
		return 0;
	}
	return 1;

}

int	 writeIntToFile(int val, FILE* fp, const char* msg)
{
	if (fwrite(&val, sizeof(int), 1, fp) != 1)
	{
		puts(msg);
		return 0;
	}
	return 1;
}

char* readStringFromFile(FILE* fp, const char* msg)
{
	char* str;
	int length;
	if (!readIntFromFile(&length, fp, msg))
		return NULL;
	str = (char*)malloc((length) * sizeof(char));
	if (!str)
	{
		puts(msg);
		return NULL;
	}
	if (fread(str, sizeof(char), length, fp) != length)
	{
		free(str);
		puts(msg);
		return NULL;
	}
	return str;
}

char* readAddressFromCompressFile(FILE* fp, const char* msg)
{
	char* str;
	int length;
	if (!readIntFromFile(&length, fp, msg))
		return NULL;
	str = (char*)calloc((length + 1), sizeof(char));
	if (!str)
	{
		puts(msg);
		return NULL;
	}
	if (fread(str, sizeof(char), length, fp) != length)
	{
		free(str);
		puts(msg);
		return NULL;
	}
	return str;
}

char* readStringFromCompressFile(FILE* fp, int companyNameLen, const char* msg)
{
	char markerName[MAX_STR_LEN];
	if (fread(&markerName, sizeof(char), companyNameLen, fp) != companyNameLen)
	{
		puts(msg);
		return NULL;
	}
	markerName[companyNameLen] = '\0';
	return markerName;
}

int readFixSizeStrFromFile(char* arr, FILE* fp, const char* msg)
{
	int len;
	if (!readIntFromFile(&len, fp, msg))
		return 0;

	if (!readCharsFromFile(arr, len, fp, msg))
		return 0;

	return 1;
}

int	 readCharsFromFile(char* arr, int length, FILE* fp, const char* msg)
{
	if (fread(arr, sizeof(char), length, fp) != length)
	{
		puts(msg);
		return 0;
	}
	return 1;
}

int	  readIntFromFile(int* pVal, FILE* fp, const char* msg)
{
	if (fread(pVal, sizeof(int), 1, fp) != 1)
	{
		puts(msg);
		return 0;
	}
	return 1;

}

int readNumOfProdnCompNameLenFromCompressFile(int* numOfProd, int* compNameLen, FILE* fp, const char* msg)
{
	BYTE data[2];
	if (fread(&data, sizeof(BYTE), 2, fp) != 2)
	{
		puts(msg);
		return 0;
	}
	*numOfProd = data[0] << 2 | (data[1] >> 6) & 0x3;
	*compNameLen = data[1] & 0x3f;

	return 1;
}

char* readDynStringFromTextFile(FILE* fp)
{
	char temp[MAX_STR_LEN];
	myGets(temp, sizeof(temp), fp);
	return getDynStr(temp);
}