/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#pragma warning (disable: 4514) // unreferenced inline/local function has been removed

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#if defined(__VC32__)
#include <FCNTL.H>
#include <IO.H>
#endif

#undef BIG_ENDIAN

#if (defined(__MSVCRT__) || defined(_MSC_VER))
//#define _stricmp  _stricmp
//#define _strnicmp _strnicmp
#else // linux 
#define _stricmp  strcasecmp
#define _strnicmp strncasecmp
#endif

const int KVersionNumber=025;
const int KLargeNumber=1000000;

extern int Utf8ToUnicode(wchar_t* aUnicode, const char* aUtf8);
extern int UnicodeToUtf8(char* aUtf8, const wchar_t* aUnicode);

enum TByteOrder
	{
	EByteOrderUnspecified,
	EByteOrderBigEndian,
	EByteOrderLittleEndian,
#if defined(BIG_ENDIAN)
	EByteOrderNative=EByteOrderBigEndian,
	EByteOrderForeign=EByteOrderLittleEndian
#else
	EByteOrderNative=EByteOrderLittleEndian,
	EByteOrderForeign=EByteOrderBigEndian
#endif
	};

struct SBuffer
	{
	int iNumberOfBytes;
	void* iData;
	};

void PrintUsage(const char* aProgramName)
    {
	fprintf(stderr, "\nVersion %03d\n\nCharacter set conversion tool\nCopyright (c) 1999 Symbian Ltd\n\n", KVersionNumber);
	fprintf(stderr, "Usage:\n\n\t%s [<options>] <inputspec> <outputspec>\n\nwhere\n\n\t"
				"options    :=  [-big|-little][-byteordermark]\n\t"
				"inputspec  :=  -input=<format> [<input_file>]\n\t"
				"outputspec :=  -output=<format> [<output_file>]\n\t"
				"format     :=  unicode|1252|utf8|...\n\n", aProgramName);
	const char* localeData=setlocale(LC_ALL, "");
	while (*localeData!='.')
		{
		++localeData;
		}
	fprintf(stderr, "(The default encoding is currently \"%s\")\n\n", localeData+1);
	}

void Assert(int aCondition, const char* aErrorMessageFormat, const void* aExtraParameter1=NULL, const void* aExtraParameter2=NULL)
	{
	if (!aCondition)
		{
		char errorMessage[100];
		sprintf(errorMessage, aErrorMessageFormat, aExtraParameter1, aExtraParameter2);
		fprintf(stderr, "Error: %s\n", errorMessage);
		exit(1);
		}
	}

void PrintWarning(const char* aWarningMessage)
	{
	fprintf(stderr, "Warning: %s\n", aWarningMessage);
	}

int TryFileParameter(int aArgc, char* aArgv[], int& aArgIndex, const char* aInputOrOutput, const char*& aEncoding, FILE*& aFile, const char* aFileMode)
	{
	char prefix[100];
	strcpy(prefix, "-");
	strcat(prefix, aInputOrOutput);
	strcat(prefix, "=");
	int lengthOfPrefix=strlen(prefix);
	if (_strnicmp(aArgv[aArgIndex], prefix, lengthOfPrefix)==0)
		{
		Assert(aEncoding==NULL, "\"%s...\" is specified more than once", prefix);
		aEncoding=aArgv[aArgIndex]+lengthOfPrefix;
		++aArgIndex;
		if ((aArgIndex>=aArgc) || (aArgv[aArgIndex][0]=='-'))
			{
			--aArgIndex;
			}
		else
			{
			aFile=fopen(aArgv[aArgIndex], aFileMode);
			Assert(aFile!=NULL, "opening %s-file failed", aInputOrOutput);
			}
		return 1;
		}
	return 0;
	}

void ReadParameters(int aArgc, char* aArgv[], int& aOutputByteOrderMark, TByteOrder& aUnicodeByteOrder, const char*& aInputEncoding, const char*& aOutputEncoding, FILE*& aInputFile, FILE*& aOutputFile)
	{
	if ((aArgc<=1) || (_stricmp(aArgv[1], "?")==0) || (_stricmp(aArgv[1], "/?")==0))
		{
		PrintUsage(aArgv[0]);
		exit(0);
		}
	for (int i=1; i<aArgc; ++i) // start at index 1 to avoid the program name (which is the first parameter)
		{
		if (_stricmp(aArgv[i], "-byteordermark")==0)
			{
			Assert(!aOutputByteOrderMark, "\"-byteordermark\" is specified more than once");
			aOutputByteOrderMark=1;
			}
		else if (_stricmp(aArgv[i], "-big")==0)
			{
			Assert(aUnicodeByteOrder==EByteOrderUnspecified, "the byte order of unicode text (i.e. \"-big\"/\"-little\") is specified more than once");
			aUnicodeByteOrder=EByteOrderBigEndian;
			}
		else if (_stricmp(aArgv[i], "-little")==0)
			{
			Assert(aUnicodeByteOrder==EByteOrderUnspecified, "the byte order of unicode text (i.e. \"-big\"/\"-little\") is specified more than once");
			aUnicodeByteOrder=EByteOrderLittleEndian;
			}
		else
			{
			Assert(TryFileParameter(aArgc, aArgv, i, "input", aInputEncoding, aInputFile, "r") ||
				   TryFileParameter(aArgc, aArgv, i, "output", aOutputEncoding, aOutputFile, "w"), "bad parameter \"%s\"", aArgv[i]);
			}
		}
	Assert(aInputEncoding!=NULL, "no input encoding is specified");
	Assert(aOutputEncoding!=NULL, "no output encoding is specified");
	}

int ReadFromFileReturningNumberOfBytesRead(void* aBuffer, int aNumberOfBytesToRead, FILE* aInputFile)
	{
	int numberOfBytesRead=0;
	int numberOfBytesToReadThisTime=aNumberOfBytesToRead;
	for (;;)
		{
		for (;;)
			{
			const int remainingNumberOfBytesToRead=aNumberOfBytesToRead-numberOfBytesRead;
			if (numberOfBytesToReadThisTime>remainingNumberOfBytesToRead)
				{
				numberOfBytesToReadThisTime=remainingNumberOfBytesToRead;
				}
			const int numberOfBytesReadThisTime=fread(aBuffer, 1, numberOfBytesToReadThisTime, aInputFile);
			const int error=ferror(aInputFile);
			if (error==0)
				{
				aBuffer=((unsigned char*)aBuffer)+numberOfBytesReadThisTime;
				numberOfBytesRead+=numberOfBytesReadThisTime;
				Assert(numberOfBytesRead<=aNumberOfBytesToRead, "internal error (read too many bytes)");
				if ((numberOfBytesRead>=aNumberOfBytesToRead) || feof(aInputFile))
					{
					return numberOfBytesRead;
					}
				break;
				}
			numberOfBytesToReadThisTime/=2;
			Assert(numberOfBytesToReadThisTime>0, "reading from file failed with error number %d", (const void*)error);
			clearerr(aInputFile);
			}
		}
	}

void WriteToFile(const void* aBuffer, int aNumberOfBytesToWrite, FILE* aOutputFile)
	{
	const int numberOfBytesWritten=fwrite(aBuffer, 1, aNumberOfBytesToWrite, aOutputFile);
	Assert(numberOfBytesWritten==aNumberOfBytesToWrite, "only %d out of %d bytes could be written to file", (const void*)numberOfBytesWritten, (const void*)aNumberOfBytesToWrite);
	const int error=ferror(aOutputFile);
	Assert(error==0, "writing to file failed with error number %d", (const void*)error);
	}

void HandleByteOrderMarks(int aOutputByteOrderMark, TByteOrder& aUnicodeByteOrder, const char* aInputEncoding, const char* aOutputEncoding, FILE* aInputFile, FILE* aOutputFile)
	{
	if (_stricmp(aInputEncoding, "unicode")==0)
		{
		unsigned short firstUnicodeCharacter=0;
		const int numberOfBytesRead=ReadFromFileReturningNumberOfBytesRead((void*)&firstUnicodeCharacter, sizeof(unsigned short), aInputFile);
		TByteOrder byteOrderSpecifiedByByteOrderMark=EByteOrderUnspecified;
		if (numberOfBytesRead==sizeof(unsigned short))
			{
			switch (firstUnicodeCharacter)
				{
			case 0xfeff:
				byteOrderSpecifiedByByteOrderMark=EByteOrderNative;
				break;
			case 0xfffe:
				byteOrderSpecifiedByByteOrderMark=EByteOrderForeign;
				break;
			default:
				const int error=fseek(aInputFile, 0, SEEK_SET); // rewind to the start of the file
				Assert(error==0, "could not rewind to the start of the input file");
				break;
				}
			}
		if (byteOrderSpecifiedByByteOrderMark!=EByteOrderUnspecified)
			{
			if ((aUnicodeByteOrder!=EByteOrderUnspecified) && (byteOrderSpecifiedByByteOrderMark!=aUnicodeByteOrder))
				{
				PrintWarning("the byte order specified by the byte-order mark in the unicode input is different from the byte order specified by the parameter - taking the byte-order specified by the byte-order mark in the unicode input");
				}
			aUnicodeByteOrder=byteOrderSpecifiedByByteOrderMark;
			}
		}
	if (aOutputByteOrderMark)
		{
		if (_stricmp(aOutputEncoding, "unicode")!=0)
			{
			PrintWarning("\"-byteordermark\" is only relevant for unicode output");
			}
		else
			{
			Assert(aUnicodeByteOrder!=EByteOrderUnspecified, "the byte order must be specified if a byte-order mark is to be added to the unicode output");
			unsigned short firstUnicodeCharacter=(unsigned short)((aUnicodeByteOrder==EByteOrderNative)? 0xfeff: 0xfffe);
			WriteToFile((const void*)&firstUnicodeCharacter, sizeof(unsigned short), aOutputFile);
			}
		}
	}

void ObeyRequiredByteOrderIfUnicode(TByteOrder& aUnicodeByteOrder, const char* aEncoding, SBuffer& aBuffer)
	{
	if (_stricmp(aEncoding, "unicode")==0)
		{
		Assert(aBuffer.iNumberOfBytes%sizeof(wchar_t)==0, "internal error (bad number of bytes in unicode buffer)");
		if (aUnicodeByteOrder==EByteOrderUnspecified)
			{
			PrintWarning("the byte order of unicode text is unspecified - defaulting to little endian");
			aUnicodeByteOrder=EByteOrderLittleEndian;
			}
		if (aUnicodeByteOrder==EByteOrderForeign)
			{
			for (unsigned char* bytePointer=((unsigned char*)aBuffer.iData)+(aBuffer.iNumberOfBytes-sizeof(wchar_t)); bytePointer>=aBuffer.iData; bytePointer-=sizeof(wchar_t))
				{
				unsigned char temp=*bytePointer;
				*bytePointer=*(bytePointer+1);
				*(bytePointer+1)=temp;
				}
			}
		}
	}

int OtherToUnicode(const char* aInputEncoding, wchar_t* aUnicode, const char* aOther)
// if the output parameter is NULL, it returns the precise size of the would-be output parameter (in terms of number of "wchar_t"s) excluding any trailing '\0', otherwise it returns 0
	{
	if (_stricmp(aInputEncoding, "utf8")==0)
		{
		return Utf8ToUnicode(aUnicode, aOther);
		}
	char localeData[100];
	strcpy(localeData, ".");
	strcat(localeData, aInputEncoding);
	Assert(setlocale(LC_ALL, localeData)!=NULL, "could not convert from encoding \"%s\"", aInputEncoding);
	return mbstowcs(aUnicode, aOther, KLargeNumber);
	}

int UnicodeToOther(const char* aOutputEncoding, char* aOther, const wchar_t* aUnicode)
// if the output parameter is NULL, it returns the precise size of the would-be output parameter (in terms of number of "char"s) excluding any trailing '\0', otherwise it returns 0
	{
	if (_stricmp(aOutputEncoding, "utf8")==0)
		{
		return UnicodeToUtf8(aOther, aUnicode);
		}
	char localeData[100];
	strcpy(localeData, ".");
	strcat(localeData, aOutputEncoding);
	Assert(setlocale(LC_ALL, localeData)!=NULL, "could not convert to encoding \"%s\"", aOutputEncoding);
	return wcstombs(aOther, aUnicode, KLargeNumber);
	}

void DoConversion(TByteOrder& aUnicodeByteOrder, const char* aInputEncoding, const char* aOutputEncoding, FILE* aInputFile, FILE* aOutputFile)
	{
	SBuffer arrayOfBuffers[3];
	arrayOfBuffers[0].iNumberOfBytes=0;
	arrayOfBuffers[0].iData=malloc(KLargeNumber+2); // +2 for the 2 '\0' bytes appended to the data read from file
	Assert(arrayOfBuffers[0].iData!=NULL, "cannot allocate enough memory");
	arrayOfBuffers[1].iNumberOfBytes=0;
	arrayOfBuffers[1].iData=NULL;
	arrayOfBuffers[2].iNumberOfBytes=0;
	arrayOfBuffers[2].iData=NULL;
	SBuffer* currentBuffer=arrayOfBuffers;
	currentBuffer->iNumberOfBytes=ReadFromFileReturningNumberOfBytesRead(currentBuffer->iData, KLargeNumber, aInputFile);
	// append 2 '\0' bytes at the end of the buffer read from file (2 in case it is unicode)
	((char*)currentBuffer->iData)[currentBuffer->iNumberOfBytes]='\0';
	((char*)currentBuffer->iData)[currentBuffer->iNumberOfBytes+1]='\0';
	ObeyRequiredByteOrderIfUnicode(aUnicodeByteOrder, aInputEncoding, *currentBuffer);
	// if the input and output encodings are different, convert from one to the other (via unicode if neither is itself unicode)
	if (_stricmp(aInputEncoding, aOutputEncoding)!=0)
		{
		if (_stricmp(aInputEncoding, "unicode")!=0)
			{
			SBuffer* nextBuffer=currentBuffer+1;
			nextBuffer->iNumberOfBytes=sizeof(wchar_t)*OtherToUnicode(aInputEncoding, NULL, (const char*)currentBuffer->iData);
			Assert(nextBuffer->iNumberOfBytes>=0, "invalid multi-byte character encountered");
			nextBuffer->iData=malloc(nextBuffer->iNumberOfBytes+sizeof(wchar_t)); // "+sizeof(wchar_t)" for terminating '\0'
			Assert(nextBuffer->iData!=NULL, "cannot allocate enough memory");
			OtherToUnicode(aInputEncoding, (wchar_t*)nextBuffer->iData, (const char*)currentBuffer->iData);
			currentBuffer=nextBuffer;
			}
		if (_stricmp(aOutputEncoding, "unicode")!=0)
			{
			SBuffer* nextBuffer=currentBuffer+1;
			nextBuffer->iNumberOfBytes=sizeof(char)*UnicodeToOther(aOutputEncoding, NULL, (const wchar_t*)currentBuffer->iData);
			Assert(nextBuffer->iNumberOfBytes>=0, "unconvertible unicode character encountered");
			nextBuffer->iData=malloc(nextBuffer->iNumberOfBytes+sizeof(char)); // "+sizeof(char)" for terminating '\0'
			Assert(nextBuffer->iData!=NULL, "cannot allocate enough memory");
			UnicodeToOther(aOutputEncoding, (char*)nextBuffer->iData, (const wchar_t*)currentBuffer->iData);
			currentBuffer=nextBuffer;
			}
		}
	ObeyRequiredByteOrderIfUnicode(aUnicodeByteOrder, aOutputEncoding, *currentBuffer);
	WriteToFile((const void*)currentBuffer->iData, currentBuffer->iNumberOfBytes, aOutputFile);
	free(arrayOfBuffers[0].iData);
	free(arrayOfBuffers[1].iData);
	free(arrayOfBuffers[2].iData);
	}

void FlushAndCloseFiles(FILE* aInputFile, FILE* aOutputFile)
	{
	Assert(fflush(aOutputFile)==0, "flushing output-file failed");
	if (aInputFile!=stdin)
		{
		Assert(fclose(aInputFile)==0, "closing input-file failed");
		}
	if (aOutputFile!=stdout)
		{
		Assert(fclose(aOutputFile)==0, "closing output-file failed");
		}
	}

int main(int aArgc, char* aArgv[])
	{
	int outputByteOrderMark=0;
	TByteOrder unicodeByteOrder=EByteOrderUnspecified;
	const char* inputEncoding=NULL;
	const char* outputEncoding=NULL;
	FILE* inputFile=stdin;
	FILE* outputFile=stdout;
	ReadParameters(aArgc, aArgv, outputByteOrderMark, unicodeByteOrder, inputEncoding, outputEncoding, inputFile, outputFile);
#if defined(__VC32__)
	_setmode(_fileno(inputFile), _O_BINARY);
	_setmode(_fileno(outputFile), _O_BINARY);
#endif
	HandleByteOrderMarks(outputByteOrderMark, unicodeByteOrder, inputEncoding, outputEncoding, inputFile, outputFile);
	DoConversion(unicodeByteOrder, inputEncoding, outputEncoding, inputFile, outputFile);
	FlushAndCloseFiles(inputFile, outputFile);
	return 0;
	}


