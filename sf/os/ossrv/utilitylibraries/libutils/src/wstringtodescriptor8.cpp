/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains the source for wstring to Descriptor8 conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a wstring to a descriptor of type TBufc8
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer, -8 is EInvalidWCSSequence
   * -9 is EInsufficientSystemMemory)
   */

EXPORT_C int WstringToTbuf8(wstring& aSrc, TDes8& aDes)
{	
    int retval = ESuccess;
    unsigned int ilen = 0;
    const wchar_t* wcharString = aSrc.c_str();
    int minusone = -1;
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    
    ilen = wcslen(wcharString);
    
    if (ilen*2 > aDes.MaxLength())
    {
    	return EInsufficientMemory;
    }
    
    char *CharString = new char[ilen*2];
    
    if(!CharString)
    {
    	return EInsufficientSystemMemory;
    }
    
	if(minusone == wcstombs(CharString, (const wchar_t*)wcharString, ilen*2))
	{
		retval = EInvalidWCSSequence;
	}
	else 
	{
		aDes.Copy((unsigned char *)CharString, ilen*2);
	}
	
	delete []CharString;
	return retval;	
}

 /**
   * Converts a wstring to a descriptor of type TPtr8
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */

EXPORT_C int WstringToTptr8(wstring& aSrc, char* cPtr, TPtr8& aDes)
{
    int retval = ESuccess;
    unsigned int wlen = 0, ilendes = 0;
    const wchar_t* wcharString = aSrc.c_str();
    int minusone = -1;
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    
	wlen = wcslen(wcharString);
	ilendes = aDes.MaxLength();
	
	if (ilendes < 2*wlen)
	{
		return EInsufficientMemory;
	}
	
	if(minusone != wcstombs(cPtr, wcharString, wlen*2))
	{
		aDes.Set((unsigned char *)cPtr, wlen*2, ilendes);	
	}
	else
	{
		retval = EInvalidWCSSequence;
	}
		
	return retval;
}

 /**
   * Converts a wstring to a descriptor of type TPtrC8
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */

EXPORT_C int WstringToTptrc8(wstring& aSrc, char* cPtr, TPtrC8& aDes)
{
    int retval = ESuccess;
    unsigned int wlen = 0;
    const wchar_t* wcharString = aSrc.c_str();
    int minusone = -1;
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    else if ( !cPtr )
    {
    	return EInvalidPointer;
    }
    
	wlen = wcslen(wcharString);
		
    if(minusone != wcstombs(cPtr, (const wchar_t*)wcharString, wlen*2 ))
    {
    	aDes.Set((TUint8 *)(cPtr), wlen*2);	
    }
	else
	{
		retval = EInvalidWCSSequence;
	}
	
    return retval;	
}

 /**
   * Converts a wstring to a descriptor of type HBufc8
   *
   * @param aSrc is the Wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData )
   */

EXPORT_C int WstringToHbufc8(wstring& aSrc, HBufC8* aDes)
{
    unsigned int wlen = 0 , ilendes = 0; 
    int retval = ESuccess; 
    const wchar_t* wcharString = aSrc.c_str();
    int minusone = -1;
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }	
    else if (!aDes)
	{
		return EInvalidPointer;
	}
	
	wlen = wcslen(wcharString);
    ilendes = aDes->Length();
    
    if (0 == ilendes)
    {
    	return EUseNewMaxL;
    }
    else if (ilendes < wlen)
    {
    	return EInsufficientMemory;
    }
    
    char *CharString = new char[wlen*2];
    
    if(!CharString)
    {
    	return EInsufficientSystemMemory;
    }
    
	
	if(minusone == wcstombs(CharString, (const wchar_t *)wcharString, wlen*2))
	{
		retval = EInvalidWCSSequence;
	}
	else 
	{
		*aDes = (TUint8 *)CharString;
	}
	
	delete []CharString;
	return retval;
}
 
 /**
   * Converts a wstring to a descriptor of type RBuf8
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess ,-1 is EInsufficientMemory -3 is EStringNoData )
   */

EXPORT_C int WstringToRbuf8(const wstring& aSrc, RBuf8& aDes)
{
    int retval = ESuccess;
    unsigned int wlen = 0;
    const wchar_t* wcharString = aSrc.c_str();
    int minusone = -1;
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }	
    
    wlen = wcslen(aSrc.c_str());
   
    char* buf = new char[wlen*2];
    
    if (!buf)
    {
    	return EInsufficientSystemMemory;
    }
         
	if(minusone != wcstombs(buf, (const wchar_t*)wcharString, wlen*2))
	{
    aDes.Copy((const unsigned char *)buf, wlen*2);	
	}
	else
	{
		retval = EInvalidWCSSequence;
	}
	
	delete []buf;
	return retval;
}
