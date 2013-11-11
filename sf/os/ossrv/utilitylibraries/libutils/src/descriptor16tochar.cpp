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
* Description:   Contains the source for Descriptor to char * conversions
 *
*/


 
#include "libutils.h"
  


 /**
   * Converts a descriptor of type TBuf16 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer, -8 is EInvalidWCSSequence)
   */
EXPORT_C int Tbuf16ToChar(TDes16& aSrc, char* aDes, int& n_size)
{	
    unsigned int ilen = aSrc.Length();
    int retval = ESuccess;
    wchar_t *temp16String = NULL;
    int minusone = -1;
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if(!aDes)
    {
    	return EInvalidPointer;
    }
    
    else if(n_size < ilen*2+1)
    {
    	n_size = ilen*2;
    	return EInvalidSize;
    }
        		
	temp16String = new wchar_t [ilen+1];
	if (!temp16String)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy(temp16String, (const wchar_t *)aSrc.Ptr(), ilen);
	temp16String[ilen] = L'\0'; 	
	
	if(minusone != wcstombs(aDes, (const wchar_t*)temp16String, ilen*2))
	{
	    aDes[ilen*2] = '\0'; 
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
	delete []temp16String;	
	return retval;
}

 /**
   * Converts a descriptor of type TBufC16 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer, -5 is EDescriptorNoData
   * -8 is EInvalidWCSSequence)
   */	
EXPORT_C int Tbufc16ToChar(TDesC& aSrc, char* aDes, int& n_size)
{
    int retval = ESuccess;
    unsigned int ilen = 0 ;
    ilen = aSrc.Length();
    wchar_t* temp16String = NULL;
    int minusone = -1;
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    } 
    else if (!aDes)
	{
		return EInvalidPointer;		
	}
	else if(n_size < ilen*2+1)
    {
    	n_size = ilen*2 + 1;
    	return EInvalidSize;
    }

    temp16String = new wchar_t [ilen+1];
	if (!temp16String)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy(temp16String, (const wchar_t *)aSrc.Ptr(), ilen);
	temp16String[ilen] = L'\0'; 	
		
	if(minusone != wcstombs(aDes, (const wchar_t*)temp16String, ilen*2))
	{
	     aDes[ilen*2] = '\0';    
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
    delete []temp16String;
	return retval;	
}

 /**
   * Converts a descriptor of type TLitc16 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the  conversion size of the string
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -2 is EInvalidSize
   * -3 is EDescriptorNoData , -4 is EInvalidPointer, -8 is EInvalidWCSSequence)
   */

EXPORT_C int Tlitc16ToChar(const TDesC16& aSrc, char* aDes, int& n_size)
{
    unsigned int ilen = 0;
    int retval = ESuccess;
    ilen = aSrc.Length();
    wchar_t* temp16String = NULL;
    int minusone = -1;
    
    if (0 == ilen )
    {
    	return EDescriptorNoData;
    }
    else if ( !aDes )
    {
        return EInvalidPointer;
    }
    else if (n_size < ilen*2 +1)
    {
    	n_size = ilen*2 + 1;
    	return EInvalidSize;
    }
      
  	
  	temp16String = new wchar_t [ilen+1];
	if (!temp16String)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy(temp16String, (const wchar_t*)aSrc.Ptr(), ilen);
	temp16String[ilen] = L'\0'; 
	
    if(minusone != wcstombs(aDes, temp16String, ilen*2))
	{
	     aDes[ilen*2] = '\0';    
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
	delete []temp16String;
	return retval;
}

 /**
   * Converts a descriptor of type TPtr16 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer, -8 is EInvalidWCSSequence)
   */

EXPORT_C int Tptr16ToCharp (const TDes16& aSrc, char* aDes, int& n_size)
{
    unsigned int ilen = 0 ;
    int retval = ESuccess;
    ilen = aSrc.Length();
    wchar_t* temp16String = NULL;
    int minusone = -1;
    
	if ( !aDes )
    {
        return EInvalidPointer;
    }
    else if (n_size < ilen*2+1)
    {
    	n_size = ilen*2 + 1;
    	return EInvalidSize;
    }
    
    temp16String = new wchar_t [ilen+1];
	if (!temp16String)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy(temp16String, (const wchar_t *)aSrc.Ptr(), ilen);
	temp16String[ilen] = L'\0'; 
	
	if(minusone != wcstombs(aDes, temp16String, ilen*2))
	{
	     aDes[ilen*2] = '\0';    
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
	delete []temp16String;
	return retval;
}

 /**
   * Converts a descriptor of type TPtrC16 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the
   * reference to the  character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer)
   */

EXPORT_C int Tptrc16ToCharp(TPtrC16& aSrc, char* aDes, int& n_size)
{
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    wchar_t* temp16String = NULL;
     
	if(!aDes)
    {
        return EInvalidPointer;
    }
    else if (n_size < ilen*2+1)
    {
    	n_size = ilen*2+1;
    	return EInvalidSize;
    }
       
    temp16String = new wchar_t [ilen+1];
	if (!temp16String)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy(temp16String, (const wchar_t *)aSrc.Ptr(), ilen);
	temp16String[ilen] = L'\0'; 
	
	if(minusone != wcstombs(aDes, temp16String, ilen*2))
	{
	     aDes[ilen*2] = '\0';    
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
	delete []temp16String;
	return retval;
}

 /**
   * Converts a descriptor of type RBuf16 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer)
   */

EXPORT_C int Rbuf16ToChar(TDes16& aSrc, char* aDes, int& n_size)
{
    unsigned int ilen = aSrc.Length();
    int retval = ESuccess;
    wchar_t* temp16String = NULL;
    int minusone = -1;
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;	
    } 
    else if(!aDes)
	{
		return EInvalidPointer;
	}
	else if (n_size < ilen*2+1)
	{
		n_size =  ilen*2+1;
		return EInvalidSize;
	}
    
    temp16String = new wchar_t [ilen+1];
	if (!temp16String)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy(temp16String,(const wchar_t *) aSrc.Ptr(), ilen);
	temp16String[ilen] = L'\0'; 
	
	if(minusone != wcstombs(aDes, temp16String, ilen*2))
	{
	     aDes[ilen*2] = '\0';    
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
    delete []temp16String;
	return retval;
}
