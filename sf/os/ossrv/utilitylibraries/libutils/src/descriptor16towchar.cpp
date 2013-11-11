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
* Description:   Contains the source for Descriptor16 to wchar * conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a descriptor of type TBuf16 to WChar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the WChar stream where the result of conversion 
   * is stored , n_size specifies the conversion size of the WChar 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize , 
   * -4 is EInvalidPointer)
   */

EXPORT_C int Tbuf16ToWchar(const TDes16& aSrc, wchar_t* aDes, int& n_size)
{	
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if (!aDes)
    {
    	return EInvalidPointer;
    }
	else if(n_size < (ilen+1))
	{
		n_size = ilen + 1;
		return EInvalidSize;
	}
		
	wmemcpy(aDes , (const wchar_t*)aSrc.Ptr(), ilen);
	aDes[ilen] = L'\0';
		
	return ESuccess;	
}
		
/**
   * Converts a descriptor of type HBufC16 to WChar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the WChar stream where the result of conversion 
   * is stored , n_size specifies the conversion size of the WChar 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize , 
   * -4 is EInvalidPointer)
   */

EXPORT_C int Hbufc16ToWchar(const HBufC16* aSrc, wchar_t* aDes, int& n_size)
{	
    unsigned int ilen = 0;
    if (!aSrc || !aDes)
    {
    	return EInvalidPointer;
    }
    else 
    {
        ilen = aSrc->Length();
    
        if (0 == ilen)
        {
    	    return EDescriptorNoData;
        }
        else if(n_size < (ilen+1))
	    {
		    n_size = ilen + 1;
		    return EInvalidSize;
	    }
    }
    	
	wmemcpy(aDes , (const wchar_t*)aSrc->Ptr(), ilen);
	aDes[ilen] = L'\0';
		
	return ESuccess;	
}


 /**
   * Converts a descriptor of type TBufC16 to WChar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the WChar stream where the result of conversion 
   * is stored , n_size specifies the conversion size of the WChar 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize , 
   * -4 is EInvalidPointer)
   */

EXPORT_C int Tbufc16ToWchar(TDesC& aSrc, wchar_t* aDes, int& n_size)
{
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if (!aDes)
    {
    	return EInvalidPointer;
    }
	if (n_size < (ilen+1))
	{
		n_size = ilen+1; 
		return EInvalidSize;
	}
    
	wmemcpy(aDes, (wchar_t *)aSrc.Ptr(), ilen);
	*(aDes + ilen) = L'\0';
	
	return ESuccess;	
}


 /**
   * Converts a descriptor of type TLitC16 to WChar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the WChar stream where the result of conversion 
   * is stored , n_size specifies the conversion size of the WChar 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize , 
   * -4 is EInvalidPointer)
   */
EXPORT_C int Tlitc16ToWchar(const TDesC16& aSrc, wchar_t* aDes, int& n_size)
{
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if ( !aDes )
    {
    	return EInvalidPointer;
    }
    else if (n_size < (ilen+1))
    {
		n_size = ilen+11 ;
		return EInvalidSize;    	
    }
    	
	wmemcpy(aDes , (const wchar_t*)aSrc.Ptr(), ilen);
	aDes[ilen] = L'\0';
    
    return ESuccess;
	
}

 /**
   * Converts a descriptor of type TPtr16 to WChar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the WChar stream where the result of conversion 
   * is stored , n_size specifies the conversion length of the WChar 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize , 
   * -4 is EInvalidPointer)
   */

EXPORT_C int Tptr16ToWcharp(const TPtr16& aSrc ,wchar_t* aDes, int& n_size)
{
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if(!aDes)
    {
    	return EInvalidPointer;
    }
	else if(n_size < ilen)
	{   
		n_size = ilen;
		return EInvalidSize;
	}   
	
	wmemcpy(aDes, (const wchar_t*)aSrc.Ptr(), ilen);
	aDes[ilen] = L'\0';
	
	return ESuccess;
}

 /**
   * Converts a descriptor of type TPtrC16 to WChar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the WChar stream where the result of conversion 
   * is stored , n_size specifies the conversion length of the WChar 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize , 
   * -4 is EInvalidPointer)
   */

EXPORT_C int Tptrc16ToWcharp(TPtrC16& aSrc, wchar_t* aDes, int& n_size)
{
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if (!aDes)
    {
    	return EInvalidPointer;
    }
	else if (n_size < (ilen+1))
	{
		n_size = ilen+1;
		return EInvalidSize;
	}
	
	wmemcpy(aDes , (const wchar_t*)aSrc.Ptr(),ilen);
	aDes[ilen] = L'\0';
	
    return ESuccess;
}

 /**
   * Converts a descriptor of type RBuf16 to WChar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the WChar stream where the result of conversion 
   * is stored , n_size specifies the conversion length of the WChar 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize , 
   * -4 is EInvalidPointer , -5 is EDESCRIPTORNDATA)
   */

EXPORT_C int Rbuf16ToWchar(TDes16& aSrc, wchar_t* aDes, int& n_size)
{
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if(!aDes)
    {
    	return EInvalidPointer; 
    }
    else if (n_size < (ilen+1))
	{
		n_size = ilen + 1;
		return EInvalidSize;
	}

    wmemcpy (aDes,(const wchar_t *)aSrc.Ptr(), ilen);

    aDes[ilen] = L'\0';

	return ESuccess;   
}
