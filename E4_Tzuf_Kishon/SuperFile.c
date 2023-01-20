#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"
#include "myMacros.h"

typedef unsigned char BYTE;

int	saveSuperMarketToFile(const SuperMarket* pMarket, int fileType, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp, "Error open supermarket file to write\n");
	//if (!fp)
	//{
	//	printf("Error open supermarket file to write\n");
	//	return 0;
	//}

	if (fileType == 0)
	{
		if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
		{
			CLOSE_RETURN_0(fp);
			//fclose(fp);
			//return 0;
		}

		if (!saveAddressToFile(&pMarket->location, fp))
		{
			CLOSE_RETURN_0(fp);
			//fclose(fp);
			//return 0;
		}
		int count = getNumOfProductsInList(pMarket);

		if (!writeIntToFile(count, fp, "Error write product count\n"))
		{
			CLOSE_RETURN_0(fp);
			//fclose(fp);
			//return 0;
		}

		Product* pTemp;
		NODE* pN = pMarket->productList.head.next; //first Node
		while (pN != NULL)
		{
			pTemp = (Product*)pN->key;
			if (!saveProductToFile(pTemp, fp))
			{
				fclose(fp);
				return 0;
			}
			pN = pN->next;
		}
	}
	else
	{
		BYTE data1[2] = { 0 };
		int count = getNumOfProductsInList(pMarket);
		int superNameLen = (int)strlen(pMarket->name);
		data1[0] = count >> 2;
		data1[1] = count << 6 | superNameLen;
		if (fwrite(&data1, sizeof(BYTE), 2, fp) != 2)
		{
			return 0;
		}
		if (fwrite(pMarket->name, sizeof(BYTE), superNameLen, fp) != superNameLen)
		{
			return 0;
		}
		BYTE data2[1] = { 0 };
		data2[0] = pMarket->location.num;
		if (fwrite(&data2, sizeof(BYTE), 1, fp) != 1)
		{
			return 0;
		}
		if (!writeStringToCompressFile(pMarket->location.street, fp, "Error write supermarket name\n"))
		{
			fclose(fp);
			return 0;
		}
		if (!writeStringToCompressFile(pMarket->location.city, fp, "Error write supermarket name\n"))
		{
			fclose(fp);
			return 0;
		}

		Product* pTemp;
		NODE* pN = pMarket->productList.head.next;

		while (pN != NULL)
		{
			pTemp = (Product*)pN->key;
			if (!saveProductToCompressFile(pTemp, fp))
			{
				fclose(fp);
				return 0;
			}
			pN = pN->next;
		}
	}

	fclose(fp);

	saveCustomerToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}
#define BIN
#ifdef BIN
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	CHECK_MSG_RETURN_0(fp, "Error open company file\n");
	//if (!fp)
	//{
	//	printf("Error open company file\n");
	//	return 0;
	//}

	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		CLOSE_RETURN_0(fp);
		//fclose(fp);
		//return 0;
	}


	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		//	free(pMarket->name);
		//	fclose(fp);
		//	return 0;
	}

	int count;

	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		//	free(pMarket->name);
		//	fclose(fp);
		//	return 0;
	}

	if (count > 0)
	{
		Product* pTemp;
		for (int i = 0; i < count; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
			if (!loadProductFromFile(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
		}
	}
	fclose(fp);
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	return	1;
}

int loadSuperMarketFromCompressFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	int numOfProducts;
	int companyNameLen;

	if (!readNumOfProdnCompNameLenFromCompressFile(&numOfProducts, &companyNameLen, fp, "Error reading data from file\n"))
	{
		fclose(fp);
		return 0;
	}

	pMarket->name = (char*)calloc(companyNameLen + 1, sizeof(char));
	if (!pMarket->name)
	{
		puts("Error\n");
		return 0;
	}

	strcpy(pMarket->name, readStringFromCompressFile(fp, companyNameLen, "Error reading supermarket name\n"));


	if (!pMarket->name)
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		//	free(pMarket->name);
		//	fclose(fp);
		//	return 0;
	}

	pMarket->location = *loadAddressFromCompressFile(fp);
	if (!(&pMarket->location))
	{
		fclose(fp);
		return 0;
	}

	if (numOfProducts > 0)
	{
		Product* pTemp;
		for (int i = 0; i < numOfProducts; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
			if (!loadProductFromCompressFile(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
		}
	}
	fclose(fp);
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	return	1;
}

#else
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		//	free(pMarket->name);
		//	fclose(fp);
		//	return 0;
	}

	fclose(fp);

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#endif

int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	int count;
	fscanf(fp, "%d\n", &count);


	//Product p;
	Product* pTemp;
	for (int i = 0; i < count; i++)
	{
		pTemp = (Product*)calloc(1, sizeof(Product));
		myGets(pTemp->name, sizeof(pTemp->name), fp);
		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		insertNewProductToList(&pMarket->productList, pTemp);
	}

	fclose(fp);
	return 1;
}