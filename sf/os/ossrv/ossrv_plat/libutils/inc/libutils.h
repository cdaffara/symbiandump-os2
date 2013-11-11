/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains all the API signatures
*
*/



#ifndef __LIBUTILS_H__
#define __LIBUTILS_H__

#include <e32cmn.h>
#include <string>

using namespace std;

enum TErrorCodes 
{
    ESuccess = 0,
	EInsufficientMemory = -1,
    EInvalidSize = -2,
    EStringNoData = -3,
    EInvalidPointer = -4,
    EDescriptorNoData = -5,
    EUseNewMaxL = -6,
    EInvalidMBSSequence = -7,
    EInvalidWCSSequence = -8,
    EInsufficientSystemMemory = -9
};

IMPORT_C int CharToTbuf16(const char* aSrc, TDes16& aDes);

IMPORT_C int CharToTbuf8(const char* aSrc, TDes8& aDes);

IMPORT_C int CharToHbufc16(const char* aSrc, HBufC16* aDes);

IMPORT_C int CharToHbufc8(const char* aSrc, HBufC8* aDes);

IMPORT_C int CharpToTptr8(const char* aSrc, TPtr8& aDes);

IMPORT_C int CharpToTptr16(const char* aSrc, TPtr16& aDes);

IMPORT_C int CharpToTptrc16(char* aSrc, wchar_t* cPtr, TPtrC16& aDes);

IMPORT_C int CharpToTptrc8(const char* aSrc, TPtrC8& aDes);

IMPORT_C int CharToRbuf16(const char* aSrc, RBuf16& aDes);

IMPORT_C int WcharToTbuf16(const wchar_t* aSrc, TDes16& aDes);

IMPORT_C int WcharToTbuf8(const wchar_t* aSrc, TDes8& aDes);

IMPORT_C int WcharToHbufc8(const wchar_t* aSrc, HBufC8* aDes);

IMPORT_C int WcharToHbufc16(const wchar_t* aSrc, HBufC16* aDes);

IMPORT_C int WcharpToTptr16(const wchar_t* aSrc, TPtr16& aDes);

IMPORT_C int WcharpToTptr8 (const wchar_t* aSrc, char *cPtr, TPtr8& aDes);

IMPORT_C int WcharpToTptrc8 (const wchar_t* aSrc, char* cPtr, TPtrC8& aDes);

IMPORT_C int WcharpToTptrc16 (const wchar_t* aSrc, TPtrC16& aDes);

IMPORT_C int WcharToRbuf8 (const wchar_t* aSrc, RBuf8& aDes);

IMPORT_C int WcharToRbuf16 (const wchar_t* aSrc, RBuf16& aDes);

IMPORT_C  int StringToTbuf8 (string& aString, TDes8& aDes);

IMPORT_C  int StringToTbuf16 (string& aString, TDes16& aDes);

IMPORT_C  int StringToTptrc16 (string& aString, wchar_t *wptr, TPtrC16& aDes);

IMPORT_C int StringToTptrc8 (string& aString, TPtrC8& aDes);

IMPORT_C  int StringToTptr8 (string& aString, TPtr8& aDes);

IMPORT_C int StringToTptr16 (string& aSrc, wchar_t *wPtr, TPtr16& aDes);

IMPORT_C int StringToHbufc16 (string& aString, HBufC16* aDes);

IMPORT_C int StringToHbufc8 (string& aString, HBufC8* aDes);

IMPORT_C int StringToRbuf8 (const string& aSrc, RBuf8& aDes);

IMPORT_C int StringToRbuf16 (const string& aSrc, RBuf16& aDes);

IMPORT_C int WstringToTbuf8 (wstring& aString, TDes8& aDes);

IMPORT_C int WstringToTptr8 (wstring& aString, char* cPtr, TPtr8& aDes);

IMPORT_C int WstringToTptr16 (wstring& aString, TPtr16& aDes);

IMPORT_C int WstringToTptrc8 (wstring& aString, char* cPtr, TPtrC8& aDes);

IMPORT_C int WstringToTptrc16 (wstring& aString , TPtrC16& aDes);

IMPORT_C int WstringToRbuf8 (const wstring& aSrc, RBuf8& aDes);

IMPORT_C int WstringToRbuf16 (const wstring& aSrc, RBuf16& aDes);

IMPORT_C int Tbuf8ToString (TDes8& aSrc, string& aDes);

IMPORT_C int Tptr8ToString (TDes8& aSrc, string& aDes);

IMPORT_C int Tptrc8ToString (const TDesC8& aSrc, string& aDes);

IMPORT_C int Tptr16ToString (TDes16& aDes, string& astring);

IMPORT_C int Tptrc16ToString (const TDesC16& aDes, string& aString);

IMPORT_C int Hbufc16ToString (HBufC16* aSrc, string& aString, int& n_size);

IMPORT_C int Hbufc8ToString (HBufC8* aSrc, string& aString);

IMPORT_C int Rbuf8ToString (TDes8& aSrc, string& aDes);

IMPORT_C int Tbuf8ToChar (TDes8& aSrc, char* aDes, int& n_size);

IMPORT_C int Tbuf16ToChar(TDes16& aSrc, char* aDes, int& n_size);

IMPORT_C int Tbufc8ToChar (TDesC8& aSrc, char* aDes, int &n_size);

IMPORT_C int Tbufc16ToChar (TDesC16& aSrc, char* aDes, int &n_size);

IMPORT_C int Tlitc8ToChar (const TDesC8& aDes, char* aSrc, int& n_size);

IMPORT_C int Tlitc16ToChar (const TDesC16& aDes, char* aSrc,int& n_size);

IMPORT_C int Tptr8ToChar (const TDes8& aSrc, char* aDes, int& n_size);

IMPORT_C int Tptr16ToCharp (const TDes16& aSrc, char* aDes, int& n_size);

IMPORT_C int Tptrc8ToCharp (TPtrC8& aSrc, char* aDes, int& n_size);

IMPORT_C int Tptrc16ToCharp (TPtrC16& aSrc, char* aDes, int& n_size);

IMPORT_C int Rbuf8ToChar (TDes8& aSrc, char * aDes, int& n_size);

IMPORT_C int Rbuf16ToChar (TDes16& aSrc, char* aDes, int& n_size);

IMPORT_C int Tbuf8ToWchar (TDes8& aSrc , wchar_t* aDes, int& n_size);

IMPORT_C int Tbuf16ToWchar (const TDes16& aSrc, wchar_t* aDes, int &n_size);

IMPORT_C int Tbufc8ToWchar (TDesC8& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Tbufc16ToWchar (TDesC& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Tlitc8ToWchar (const TDesC8& aDes, wchar_t* aSrc, int& n_size);

IMPORT_C int Tlitc16ToWchar (const TDesC16& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Tptr8ToWcharp (const TPtr8& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Tptr16ToWcharp (const TPtr16& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Tptrc8ToWcharp (TPtrC8& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Tptrc16ToWcharp (TPtrC16& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Rbuf8ToWchar (TDes8& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Rbuf16ToWchar (TDes16& aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int WcharpToTptrc8(const wchar_t* aSrc, TDes16& aDes);

IMPORT_C int WcharpToTptr16(const wchar_t* aSrc, TDes16& aDes);

IMPORT_C int Tptrc8ToWstring (TPtrC8& aSrc, wstring& aDes);

IMPORT_C int Tptrc16ToWstring (TPtrC16& aSrc, wstring& aDes);

IMPORT_C int Tptr8ToWstring (TPtr8& aSrc, wstring& aDes);

IMPORT_C int Tptr16ToWstring (TPtr16& aSrc, wstring& aDes);

IMPORT_C int Rbuf8ToWstring (TDes8& aSrc, wstring& aDes);

IMPORT_C int Rbuf16ToWstring (TDes16& aSrc, wstring& aDes);

IMPORT_C int Rbuf16ToString(TDes16& aSrc, string& aDes);

IMPORT_C int Tbufc8ToString(TDesC8& aSrc, string& aDes);

IMPORT_C int Tbufc16ToString(TDesC16& aSrc, string& aDes);

IMPORT_C int Tlit8ToString(const TDesC8& aSrc, string& aDes);

IMPORT_C int Tlit16ToString(const TDesC16& aSrc, string& aDes);

IMPORT_C int Hbufc16ToWstring(HBufC16* aSrc, wstring& aDes);

IMPORT_C int Tlitc16ToWstring(TDesC16* aSrc, wstring& aDes);

IMPORT_C int Tlitc8ToWstring(TDes8& aSrc, wstring& aDes);

IMPORT_C int Hbufc8ToWstring(HBufC8* aSrc, wstring& aDes);

IMPORT_C int WstringToHbufc8(wstring& aSrc, HBufC8* aDes);

IMPORT_C int WstringToTbuf16(wstring& aSrc, TDes16& aDes);

IMPORT_C int WstringToHbufc16(wstring& aSrc, HBufC16* aDes);

IMPORT_C int CharToRbuf8(const char* aSrc, RBuf8& aDes);

IMPORT_C int Tbuf8ToWstring(TDes8& aSrc, wstring& aDes);

IMPORT_C int Tbufc8ToWstring(TDesC8& aSrc, wstring& aDes);

IMPORT_C int HBufc8ToWchar(HBufC8* aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Tbuf16ToWstring(TDes16& aSrc, wstring& aDes);

IMPORT_C int Hbufc16ToWchar(const HBufC16* aSrc, wchar_t* aDes, int& n_size);

IMPORT_C int Hbufc8ToChar(HBufC8 *aSrc, char* aDes, int& n_size);

IMPORT_C int Tbuf16ToString(TDes16& aSrc, string& aDes);

#endif  // __LIBUTILS_H__


