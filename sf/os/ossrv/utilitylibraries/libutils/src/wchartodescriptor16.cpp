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
* Description:   Contains the source for wchar * to Descriptor16 conversions
 *
*/



#include "libutils.h"
  

 /**
   * Converts a wchar to a descriptor of type TBufc16
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */
   

EXPORT_C int WcharToTbuf16(const wchar_t* aSrc, TDes16& aDes)
{
    unsigned int ilen = 0;
	if ( !aSrc )
	{
		return EInvalidPointer;
	}
	else
	{
		ilen = wcslen(aSrc);
		if(ilen > aDes.MaxLength())
		{
			return EInsufficientMemory; 
		}	
	}
	
	aDes.Copy((const TUint16*)aSrc, ilen);
	return ESuccess;
}
 /**
   * Converts a wchar to a descriptor of type HBufc16
   *
   * @param aSrc is the Wstring to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData, -6 is EUseNewMaxL )
   */

EXPORT_C  int WcharToHbufc16 (const wchar_t* aSrc ,HBufC16* aDes )
{
   
    unsigned int ilendes = 0;
	if ( !aSrc || !aDes)
	{
		return EInvalidPointer;
	}
	
	ilendes = aDes->Length();
	
	if (0 == ilendes)
	{
		return EUseNewMaxL;
	}
	else if (ilendes < wcslen(aSrc))
	{
		return EInsufficientMemory;
	}
	
	*aDes = (const TUint16*)aSrc;		
	return ESuccess;
}

 /**
   * Converts a wchar to a descriptor of type TPtr16
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */

EXPORT_C int WcharpToTptr16(const wchar_t* aSrc, TPtr16& aDes)
{
    unsigned int ilen = 0, ilendes = 0;
	if ( !aSrc )
	{
		return EInvalidPointer;
	}
	
	ilen = wcslen(aSrc);
	ilendes =  aDes.MaxLength();
	
	if (ilendes < ilen)
	{
		return EInsufficientMemory;
	}
	
	aDes.Set((unsigned short*) aSrc, ilen, ilendes);
	
	return ESuccess;
}

 /**
   * Converts a wchar to a descriptor of type TPtrC16
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */
EXPORT_C int WcharpToTptrc16(const wchar_t* aSrc, TPtrC16& aDes)

{
	if (!aSrc)
	{
		return EInvalidPointer;
	}
	
	aDes.Set((TUint16 *)(aSrc), wcslen(aSrc));
	
	return ESuccess;
}

 /**
   * Converts a wchar to a descriptor of type RBuf16
   *
   * @param aSrc is the wchar to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess ,-1 is EInsufficientMemory,
   * -3 is EStringNoData, -4 is EInvalidPointer )
   */

EXPORT_C int WcharToRbuf16(const wchar_t* aSrc, RBuf16& aDes)
{
   
    int retval = ESuccess;
    unsigned int wlen = 0; 
    if ( !aSrc )
	{
		return EInvalidPointer;
	}
	
	wlen = wcslen(aSrc);

  aDes.Copy((const unsigned short *)aSrc, wlen);	

	return retval;
}
