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
* Description:   Contains the source for char * to Descriptor8 conversions
 *
*/



#include "libutils.h"
  



/**
   * Converts a character stream to TBuf8
   * @param aSrc is char* which is to be converted to TBuf8
   * @param aDes is the pointer to the descriptor of type TBuf8 which will hold
   * the result of conversion.aDes needs to be allocated with sufficient amount 
   * of memory before being passed to function. This Descriptor should have 
   * a allocation that is equal to or greater than char*
   *
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer)
   */
   
EXPORT_C  int CharToTbuf8 (const char* aSrc, TDes8& aDes)
{
	if (!aSrc)
	{
	    return EInvalidPointer;	
	}
	else 
	{
	    if (strlen (aSrc) > aDes.MaxLength())
	    {
	    	return EInsufficientMemory;	
	    }	
	}
	
	aDes.Copy ((const TUint8*)aSrc);
		
	return ESuccess;
}

 /**
   * Converts a character stream to HBufC8\\
   * @param aSrc is char* which is to be converted to HBufC8
   * @param aDes is the pointer to the descriptor of type HBufC8 which will hold
   * the result of conversion.aDes needs to be allocated with sufficient amount 
   * of memory before being passed to function. This Descriptor should have 
   * a allocation that is equal to or greater than char*
   *
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer, -6 is EUseNewMaxL)
   */
   
EXPORT_C int CharToHbufc8(const char* aSrc, HBufC8* aDes)
{
    unsigned int ilendes = 0;
	if ( !aSrc || !aDes )
	{
		return EInvalidPointer;
	}
	else 
	{
	    ilendes = aDes->Length();
	    if(0 == ilendes)
	    {
	    	return EUseNewMaxL;
	    }
	    else if (strlen (aSrc) > ilendes)
	    {
	    	return EInsufficientMemory;	
	    }	
	}
		
	*aDes = (const TUint8*)aSrc;
	return ESuccess;
	
}

 /**
   * Converts a character stream to Tptr8
   * @param aSrc is char* which is to be converted to TPtr8
   * @param aDes is the pointer to the descriptor of type TPtr8 which will hold
   * the result of conversion.aDes needs to be allocated with sufficient amount 
   * of memory before being passed to function. This Descriptor should have 
   * a allocation that is equal to or greater than char*
   *
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer)
   */
   
EXPORT_C int CharpToTptr8( const char* aSrc, TPtr8& aDes )
{
    unsigned int ilen = 0, ilendes = 0;
	if (!aSrc)
	{
		return EInvalidPointer;
	}
	
	ilen = strlen(aSrc);
	ilendes = aDes.MaxLength();
	
	if (ilendes < ilen)
	{
		return EInsufficientMemory;
	}
	
	aDes.Set((TUint8 *)(aSrc), ilen, ilendes);
	
	return ESuccess;
}

 /**
   * Converts a character stream to TPtrc8
   * @param aSrc is char* which is to be converted to TPtrc8
   * @param aDes is the pointer to the descriptor of type TPtrc8 which will hold
   * the result of conversion.aDes needs to be allocated with sufficient amount 
   * of memory before being passed to function. This Descriptor should have 
   * a allocation that is equal to or greater than char*
   *
   * @return Status code (0 is ESuccess, -4 is EInvalidPointer)  
   */
   
EXPORT_C int CharpToTptrc8(const char* aSrc, TPtrC8& aDes)
{    
	if ( !aSrc )
	{
		return EInvalidPointer;
	}
	
	aDes.Set((TUint8 *)(aSrc), strlen(aSrc));
	
	return ESuccess;
}

 /**
   * Converts a character stream to RBuf8
   * @param aSrc is char* which is to be converted to RBuf8
   * @param aDes is the pointer to the descriptor of type RBuf8 which will hold
   * the result of conversion.aDes needs to be allocated with sufficient amount 
   * of memory before being passed to function. This Descriptor should have 
   * a allocation that is equal to or greater than char*
   *
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer)
   */

EXPORT_C int CharToRbuf8(const char* aSrc, RBuf8& aDes)
{    
  if ( !aSrc )
  {
    return EInvalidPointer;
  }
        
  aDes.Copy((const unsigned char *)aSrc, strlen(aSrc));
	
	return ESuccess;	
}

