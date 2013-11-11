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
* Description:   Contains the source for Descriptor8 to char * conversions
 *
*/


 
#include "libutils.h"
  

	
 /**
   * Converts a descriptor of type TBuf8 to char*
   * @param aSrc is the descriptor of type TBuf8 that is to be converted to char*
   * @param aDes is a char* that will hold the result after conversion. Care should be taken to 
   * allocate sufficient amount of memory to char* in the calling function. The amount of memory that is
   * being allocated to the char* is being made known using n_size
   * @param n_size is the size of the char*. This should have a minimum value equal to size of the 
   * descriptor. Incase its less, the conversion is going to fail and returns the corresponding error code
   */

EXPORT_C int Tbuf8ToChar(TDes8& aSrc, char* aDes, int& n_size)
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

	memcpy(aDes , (const char *)aSrc.Ptr(), ilen);
	
	aDes[ilen] = '\0';
		
	return ESuccess;
}

 /**
    * Converts a descriptor of type TBufC8 to char*
   * @param aSrc is the descriptor of type TBufC8 that is to be converted to char*
   * @param aDes is a char* that will hold the result after conversion. Care should be taken to 
   * allocate sufficient amount of memory to char* in the calling function. The amount of memory that is
   * being allocated to the char* is being made known using n_size
   * @param n_size is the size of the char*. This should have a minimum value equal to size of the 
   * descriptor. Incase its less, the conversion is going to fail and returns the corresponding error code
   */

EXPORT_C int Tbufc8ToChar(TDesC8& aSrc, char* aDes, int& n_size)
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
    else if(n_size < ilen+1)
    {
    	n_size = ilen;
    	return EInvalidSize;
    }
    	
	memcpy(aDes, aSrc.Ptr(), ilen);
	aDes[ilen] = '\0';
        
    return ESuccess;
}

/**
   * Converts a descriptor of type TLitC8 to char*
   * @param aSrc is the descriptor of type TLitC8 that is to be converted to char*
   * @param aDes is a char* that will hold the result after conversion. Care should be taken to 
   * allocate sufficient amount of memory to char* in the calling function. The amount of memory that is
   * being allocated to the char* is being made known using n_size
   * @param n_size is the size of the char*. This should have a minimum value equal to size of the 
   * descriptor. Incase its less, the conversion is going to fail and returns the corresponding error code
   */
	
EXPORT_C int Tlitc8ToChar(const TDesC8& aSrc, char* aDes, int& n_size)
{   
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen )
    {
    	return EDescriptorNoData;
    }     
    else if(!aDes)
    {
    	return EInvalidPointer;
    }
    else if (n_size < ilen )
    {
    	n_size = ilen + 1;
    	return EInvalidSize;
    }
    
    memcpy(aDes, (char *)aSrc.Ptr(), ilen);
	aDes[ilen] = '\0';
	
	return ESuccess;	 
}


 /**
   * Converts a descriptor of type TPtr8 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize ,
   *  -4 is EInvalidPointer)
   */
EXPORT_C int Tptr8ToChar(const TDes8& aSrc, char* aDes, int& n_size)
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
    else if (n_size < ilen)
    {
    	n_size = ilen + 1;
    	return EInvalidSize;
    }
    
	memcpy(aDes , (const char *)aSrc.Ptr(), ilen);
	aDes[ilen] = '\0';
    
    return ESuccess;
}

 /**
   * Converts a descriptor of type TPtrC8 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize ,
   *  -4 is EInvalidPointer)
   */
EXPORT_C int Tptrc8ToCharp(TPtrC8& aSrc, char* aDes, int& n_size)
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
    else if (n_size < ilen)
    {
    	n_size = ilen + 1;
    	return EInvalidSize;
    }
	
	memcpy(aDes , aSrc.Ptr(), ilen);
	aDes[ilen] = '\0';
    
    return ESuccess;
}

 /**
   * Converts a descriptor of type RBuf8 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize ,
   *  -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */
   
EXPORT_C int Rbuf8ToChar(TDes8& aSrc, char* aDes, int& n_size)
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
    else if (n_size < ilen)
    {
    	n_size = ilen + 1;
    	return EInvalidSize;
    }
    
	memcpy (aDes,(char *)aSrc.Ptr(), ilen);
	aDes[ilen] = '\0';

	return ESuccess;
}

 /**
   * Converts a descriptor of type HBufC8 to character stream
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the character sream where the result of conversion 
   * is stored , n_size specifies the conversion size of the string 
   * @return Status code (0 is ESuccess, -2 is EInvalidSize ,
   *  -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */

EXPORT_C int Hbufc8ToChar(HBufC8 *aSrc, char* aDes, int& n_size)
{
    unsigned int ilen = 0;
    
    if ( !aDes || !aSrc)
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
        else if (n_size < ilen)
    	{
    		n_size = ilen + 1;
    		return EInvalidSize;
    	}  	
    } 	
    
	memcpy (aDes,(char *)aSrc->Ptr(), ilen);
	aDes[ilen] = '\0';

	return ESuccess;
}
