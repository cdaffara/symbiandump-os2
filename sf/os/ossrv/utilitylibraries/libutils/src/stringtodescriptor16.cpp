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
* Description:   Contains the source for string to Descriptor16 conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a string to a descriptor of type TBuf16
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored,n_size specifies the conversion size of the char array 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData, -9 is EInsufficientSystemMemory)
   */
EXPORT_C  int StringToTbuf16(string& aSrc, TDes16& aDes)
{
  int retval = ESuccess;
	const char* charString = aSrc.c_str();
	int ilen = 0; 
	int minusone = -1;
	
    if('\0' == charString[0])
    {
    	return EStringNoData;
    }
        
    ilen = strlen(charString);
    
	if (ilen > aDes.MaxLength())
	{
		return EInsufficientMemory;
	}
	
	wchar_t* WcharString = new wchar_t[ilen];
	
	if(!WcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	if(minusone == mbstowcs(WcharString, (const char*)charString, ilen))
	{
		retval = EInvalidMBSSequence;
	}
	else
	{
			WcharString[ilen] = L'\0';
		aDes.Copy((unsigned short*)WcharString,ilen);
	}
		
	delete []WcharString;	
	return retval;	
}

 /**
   * Converts a string to a descriptor of type Tptr16
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData , -4 is EInvalidPointer)
   */

EXPORT_C  int StringToTptr16 (string& aSrc, wchar_t *wPtr, TPtr16& aDes)
{
    int retval = ESuccess;
    int ilen = 0, ideslen = 0;
    const char* charString = aSrc.c_str();
    int minusone = -1;
    
    if('\0' == charString[0])
    {
    	return EStringNoData;
    }
    else if(!wPtr)
    {
    	return EInvalidPointer;
    }
    
	ilen = strlen(charString);
	ideslen = aDes.MaxLength();
	
	if (ilen > ideslen)
	{
		return EInsufficientMemory;
	}
	
	if(minusone != mbstowcs(wPtr, (const char*)charString, ilen))
	{
		aDes.Set((unsigned short *)wPtr, ilen, ideslen);
	}		
	else
	{
		retval = EInvalidMBSSequence;
	}
	
	return retval;	     
}

 /**
   * Converts a string to a descriptor of type Tptrc16
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor  where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData , -4 is EInvalidPointer)
   */

EXPORT_C  int StringToTptrc16(string& aSrc, wchar_t* wptr, TPtrC16& aDes)
{
    unsigned int ilen = 0;
    int retval = ESuccess;
    const char* charString = aSrc.c_str();
    int minusone = -1;
    
    if (!wptr)
    {
    	return EInvalidPointer;
    }
    
    if('\0' == charString[0])
    {
    	return EStringNoData;
    }
    
	ilen = strlen(charString);
		
	if(minusone != mbstowcs(wptr, (const char*)charString, ilen ))
	{
		aDes.Set((TUint16 *)(wptr), ilen);	
	}
	else
	{
		retval = EInvalidMBSSequence;
	}
	
	return retval;	
}

 /**
   * Converts a string to a descriptor of type HBufc16
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData, -4 is EInvalidPointer, -6 is EUseNewMaxL,
   * -9 is EInsufficientSystemMemory)
   */

EXPORT_C int StringToHbufc16(string& aSrc , HBufC16* aDes)
 {
    int retval = ESuccess;
    unsigned int ilen = 0, ilendes = 0; 
    const char* charString = aSrc.c_str();
    int minusone = -1;
   
    if('\0' == charString[0])
    {
    	return EStringNoData;
    }
    else if (!aDes)
    {
    	return EInvalidPointer;
    }
    
    ilen = strlen(charString);
    ilendes = aDes->Length();
    
    if (0 == ilendes)
    {
    	return EUseNewMaxL;
    }
    else if (ilendes < ilen)
    {
    	return EInsufficientMemory;
    }
    
    wchar_t* WcharString = new wchar_t[ilen];
	
	if(!WcharString)
	{
		return EInsufficientSystemMemory;
	}
	 
	if(minusone == mbstowcs(WcharString, (const char*)charString, ilen))
	{
		retval = EInvalidMBSSequence;
	}
	else
	{
	    TPtrC16 temp((unsigned short*)WcharString, ilen);
		*aDes = temp;
	} 
	
	delete[] WcharString;
	return retval;
}
 
 /**
   * Converts a string to a descriptor of type RBufc16
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData, -7 is EInvalidMBSSequence, -9 is EInsufficientSystemMemory)
   */

EXPORT_C int StringToRbuf16(const string& aSrc, RBuf16& aDes)
{
    int retval = ESuccess;
    unsigned int ilen = 0;
    int minusone = -1;
    
    const char* charString = aSrc.c_str();
    
    if('\0' == charString[0])
    {
    	return EStringNoData;
    }
    
    ilen = strlen(charString);
    
	wchar_t* buf = new wchar_t[ilen];
    
    if (!buf)
    {
    	return EInsufficientMemory;
    }

	if(minusone != mbstowcs(buf, charString, ilen))
	{
	  aDes.Copy((const unsigned short *)buf, ilen);		
	}
	else
	{
	  retval = EInvalidMBSSequence;		
	}
	
	delete []buf;
  return retval;	
}
