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
* Description:   Contains the source for wstring to Descriptor16 conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a wstring to a descriptor of type TBufc16
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData )
   */

EXPORT_C int WstringToTbuf16(wstring& aSrc, TDes16& aDes)
{
    int retval = ESuccess;
    const wchar_t* wcharString = aSrc.c_str();
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    
    int len= wcslen(wcharString);
    if ((wcslen(wcharString)) > aDes.MaxLength())
    if (len > aDes.MaxLength())
    {
    	return EInsufficientMemory;
    }
    
	aDes.Copy((const TUint16*)wcharString);
	
    return retval;	
}

 /**
   * Converts a wstring to a descriptor of type TPtr16
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -3 is EStringNoData, -1 is EInsufficientMemory)
   */

EXPORT_C int WstringToTptr16(wstring& aSrc, TPtr16& aDes )
{
    const wchar_t* wcharString = aSrc.c_str();
    unsigned int wlen = 0 , maxlen =0; 
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    
    wlen = wcslen(wcharString);
    maxlen = aDes.MaxLength();
    
    if (maxlen < wlen)
    {
    	return EInsufficientMemory;
    }
    
	aDes.Set((unsigned short *)wcharString, wlen, maxlen);
	return ESuccess;
}

 /**
   * Converts a wstring to a descriptor of type TPtrC16
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-3 is EStringNoData  )
   */

EXPORT_C int WstringToTptrc16(wstring& aSrc, TPtrC16& aDes)
{
  
    const wchar_t* wcharString = aSrc.c_str();
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    
	aDes.Set((TUint16 *)(wcharString));
	return ESuccess;
}

 /**
   * Converts a wstring to a descriptor of type HBufc16
   *
   * @param aSrc is the Wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */

EXPORT_C int WstringToHbufc16(wstring& aSrc, HBufC16* aDes)
{
    unsigned int ilendes = 0; 
    const wchar_t* wcharString = aSrc.c_str();
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    else if (!aDes)
    {
    	return EInvalidPointer;
    }
    
    ilendes = aDes->Length();
    
    if (0 == ilendes)
    {
    	return EUseNewMaxL;
    }
    else if (ilendes < wcslen(wcharString))
    {
    	return EInsufficientMemory;
    }

	*aDes = (TUint16*)wcharString;
	
	return ESuccess;	
}
 
 /**
   * Converts a wstring to a descriptor of type RBufc16
   *
   * @param aSrc is the wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess , -3 is EStringNoData )
   */

EXPORT_C int WstringToRbuf16(const wstring& aSrc, RBuf16& aDes)
{
    unsigned int wlen =0;
    int retval = ESuccess;
    const wchar_t* wcharString = aSrc.c_str();
    
    if (L'\0' == wcharString[0])
    {
    	return EStringNoData;
    }
    
    wlen = wcslen(wcharString);

    aDes.Copy((const unsigned short *)wcharString, wlen);	

	  return retval;
}
