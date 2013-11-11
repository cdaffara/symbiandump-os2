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
* Description:   Contains the source for wchar * to Descriptor8 conversions
 *
*/


#include "libutils.h"
  


 /**
   * Converts a wchar to a descriptor of type TBuf8
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */

EXPORT_C int WcharToTbuf8(const wchar_t* aSrc, TDes8& aDes)
{
    int retval = ESuccess;
    unsigned int ilen = 0;
    int minusone = -1;
	if ( !aSrc )
	{
		return EInvalidPointer;
	}
	else
	{
	    ilen = wcslen(aSrc);
	    if (ilen*2 > aDes.MaxLength())
	    {
	    	return EInsufficientMemory;	
	    } 
	}
	 
	char *CharString = new char[ilen*2];
	
	if (!CharString)
	{
		return EInsufficientSystemMemory;
	}
	
	if(minusone == wcstombs(CharString, (const wchar_t*)aSrc, ilen*2))
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
   * Converts a wchar to a descriptor of type HBufc8
   * 
   * @param aSrc is the Wstring to be converted, aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory )
   */

EXPORT_C int WcharToHbufc8(const wchar_t* aSrc, HBufC8* aDes)
{
    unsigned int wlen = 0, ilendes = 0;
    int retval = ESuccess;
    int minusone = -1;
	if ( !aSrc || !aDes)
	{
		return EInvalidPointer;
	}
	else
	{
		wlen = wcslen(aSrc);
        ilendes = aDes->Length();
	    if(0 == ilendes)
	    {
	        return EUseNewMaxL;
	    }
	    else if (wlen > ilendes)
	    {
	        return EInsufficientMemory;	
	    }
	}
    
    char *CharString = new char[wlen*2];
	
	if (!CharString)
	{
		return EInsufficientSystemMemory;
	}
	
    	
	if(minusone == wcstombs(CharString, aSrc, wlen*2))
	{
		retval = EInvalidWCSSequence;
	}
	else
	{
	    (*aDes) = (const TUint8 *)CharString;	
	}
	
	delete []CharString;
	return retval;
}

 /**
   * Converts a wchar to a descriptor of type TPtr8
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */

EXPORT_C int WcharpToTptr8(const wchar_t* aSrc, char* cPtr, TPtr8& aDes)
{
    int retval = ESuccess;
    unsigned int wlen = 0, ilendes = 0;
    int minusone = -1;
	if ( !aSrc )
	{
		return EInvalidPointer;
	}
	else 
	{
	    wlen = wcslen(aSrc);
	    ilendes = aDes.MaxLength();
	    if (ilendes < wlen )
	    {
	    	return EInsufficientMemory;
	    }	    	
	}
	
	if(minusone != wcstombs(cPtr, (const wchar_t*)aSrc, wlen))
	{
		aDes.Set((TUint8*)cPtr, wlen, ilendes);	
	}
	else
	{
		retval = EInvalidWCSSequence;
	}
	
	return retval;
}

 /**
   * Converts a wchar to a descriptor of type TPtrC8
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   *  -4 is EInvalidPointer )
   */ 
 
EXPORT_C int WcharpToTptrc8(const wchar_t* aSrc, char* cPtr, TPtrC8& aDes)
{

    int retval = ESuccess;
    unsigned int wlen = 0 ;
    int minusone = -1;
    
	if ( !aSrc  || !cPtr)
	{
		return EInvalidPointer;
	}
	
	wlen = wcslen(aSrc);
	
	if(minusone != wcstombs(cPtr, (const wchar_t*)aSrc, wlen))
	{
		aDes.Set((TUint8*)cPtr,wlen);
	}
	else
	{
		retval = EInvalidWCSSequence;
	}
	
	return retval;
}


 /**
   * Converts a wchar to a descriptor of type RBuf8
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess ,-1 is EInsufficientMemory, -4 is EInvalidPointer )
   */

EXPORT_C int WcharToRbuf8(const wchar_t* aSrc, RBuf8& aDes)
{
    int retval = ESuccess;
    unsigned  int wlen = 0;
    char* buf = NULL;
    int minusone = -1;
    
    if (!aSrc)
	{
		return EInvalidPointer;
	}
	
	wlen = wcslen(aSrc);

	buf = new char[wlen*2];
	if (!buf)
	{
		return EInsufficientSystemMemory;
	}

	if(minusone != wcstombs(buf, (const wchar_t*)aSrc, wlen*2))
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
