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
* Description:   Contains the source for Descriptor8 to wchar * conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a descriptor of type TBuf8 to wchar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wchar array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   *  -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */
   
EXPORT_C int Tbuf8ToWchar(TDes8& aSrc, wchar_t* aDes, int& n_size)
{	
    int retval = ESuccess;	
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
    if(0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if(!aDes)
    {
    	return EInvalidPointer;
    }
    else if(n_size < ilen+1)
    {	
        n_size = ilen+1; 
    	return EInvalidSize;
    }	
	
	if(minusone != mbstowcs(aDes, (const char*)aSrc.Ptr(), ilen))
	{
	    *(aDes + ilen) = L'\0';	
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
	return retval;			
}		
		
 /**
   * Converts a descriptor of type TBufc8 to wchar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wchar array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   *  -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */

EXPORT_C int Tbufc8ToWchar(TDesC8& aSrc, wchar_t* aDes, int& n_size)
{
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
    if(0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if(!aDes)
    {
    	return EInvalidPointer;
    }
    else if (n_size < ilen+1)
	{
		n_size = ilen+1;
		return EInvalidSize;
	}
	
	if(minusone != mbstowcs(aDes, (const char *)aSrc.Ptr(), ilen))
	{
		aDes[ilen] = L'\0';
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
	return retval;
}

 /**
   * Converts a descriptor of type TLitc8 to wchar
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wchar array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   *  -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */

EXPORT_C int Tlitc8ToWchar( const TDesC8& aSrc, wchar_t* aDes, int& n_size)
{ 
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
    if (0 == ilen )
    {
    	return EDescriptorNoData;
    }
    else if ( !aDes )
    {
    	return EInvalidPointer;
    }
    else if (n_size < ilen+1)
    {
		n_size = ilen+1 ;
		return EInvalidSize;    	
    }
	
	if(minusone != mbstowcs(aDes, (const char*)aSrc.Ptr(), ilen ))
	{
	    aDes[ilen] = L'\0'; 
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
	return retval;	
}

 /**
   *Converts a descriptor of type TPtr8 to Wchar
   *@param aSrc is the descriptor to be converted , aDes is the 
   *reference to the wchar array where the result of conversion 
   *is stored  
   *@return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */

EXPORT_C int Tptr8ToWcharp(const TPtr8& aSrc, wchar_t* aDes, int& n_size)
{
   
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    else if(!aDes)
    {
    	return EInvalidPointer;
    } 
	else if(n_size < ilen+1)
    {
        n_size = ilen+1 ;
    	return EInvalidSize;
    }
	
	if(minusone != mbstowcs(aDes, (const char*)aSrc.Ptr(), ilen))
	{
	    aDes[ilen] = L'\0';
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
	return retval;
}

 /**
   *Converts a descriptor of type TPtrc to Wchar
   *
   *@param aSrc is the descriptor to be converted , aDes is the 
   *reference to the wchar array where the result of conversion 
   *is stored  
   *@return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */

EXPORT_C int Tptrc8ToWcharp(TPtrC8& aSrc, wchar_t* aDes, int& n_size)
{
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
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
	
	if(minusone != mbstowcs(aDes, (const char*)aSrc.Ptr(), ilen))
	{
	    aDes[ilen] = L'\0';
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
	return retval;	
}

 /**
   *Converts a descriptor of type Rbuf8 to Wchar
   *
   *@param aSrc is the descriptor to be converted , aDes is the 
   *reference to the wchar array where the result of conversion 
   *is stored  
   *@return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */

EXPORT_C int Rbuf8ToWchar(TDes8& aSrc, wchar_t* aDes, int& n_size)
{
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
    if (0 == ilen )
    {
    	return EDescriptorNoData;
    }
    else if(!aDes)
    {
    	return EInvalidPointer;
    }
    else if (n_size < ilen)
	{
		n_size = ilen + 1;
		return EInvalidSize;
	}
	
	if(minusone != mbstowcs(aDes, (const char*)aSrc.Ptr(), ilen))
	{
		*(aDes + ilen) = L'\0';
	}
	else
	{
		retval = EInvalidMBSSequence;
	}
			
	return retval;
}

/**
   *Converts a descriptor of type Rbuf8 to Wchar
   *
   *@param aSrc is the descriptor to be converted , aDes is the 
   *reference to the wchar array where the result of conversion 
   *is stored  
   *@return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */

EXPORT_C int HBufc8ToWchar(HBufC8* aSrc, wchar_t* aDes, int& n_size)
{
    int retval = ESuccess;
    unsigned int ilen = 0;
    int minusone = -1;
    
    if(!aDes || !aSrc)
    {
    	return EInvalidPointer;
    }
    else
    {
    	int ilen = aSrc->Length();
    	if (0 == ilen )
        {
    	    return EDescriptorNoData;
        }
        else if (n_size < ilen)
	    {
		    n_size = ilen + 1;
		    return EInvalidSize;
	    }
    }
    
	if(minusone != mbstowcs(aDes, (const char*)aSrc->Ptr(), ilen))
	{
		*(aDes + ilen) = L'\0';
	}
	else
	{
		retval = EInvalidMBSSequence;
	}
			
	return retval;
}
