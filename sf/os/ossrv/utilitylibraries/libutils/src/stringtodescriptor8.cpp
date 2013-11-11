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
* Description:   Contains the source for string to Descriptor8 conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a string to a descriptor of type TBuf8
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData)
   */

EXPORT_C  int StringToTbuf8(string& aSrc, TDes8& aDes)
{
    const char* charString = aSrc.c_str();
    
    if('\0' == charString[0])
    {
    	return EStringNoData;
    }
    
    if (aDes.MaxLength() < strlen(charString))
    {
    	return EInsufficientMemory;
    }
    
	aDes = (const TUint8*)(charString);
	     
	return ESuccess;
}

 /**
   * Converts a string to a descriptor of type Tptrc8
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData)
   */

EXPORT_C int StringToTptrc8(string& aSrc, TPtrC8& aDes)
{    
    const char* charString = aSrc.c_str();
    if ('\0' == charString[0])
    {
        return EStringNoData;
    }
  
    aDes.Set(TPtrC8((TUint8 *)(charString)));
    
    return ESuccess; 
} 

 /**
   * Converts a string to a descriptor of type TPtr8
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored
   * @return Status code (0 is ESuccess, -3 is EStringNoData, -1 is EInsufficientMemory)
   */

EXPORT_C  int StringToTptr8 (string& aSrc, TPtr8& aDes)
{
    const char* charString = aSrc.c_str();
    
    if ('\0' == charString[0])
    {
    	return EStringNoData;
    }
    
    unsigned int ilen = strlen(charString);
    unsigned int ideslen = aDes.MaxLength();
    if(ilen > ideslen)
    {
        return EInsufficientMemory;
    }
    
    aDes.Set((unsigned char *)charString, ilen, ideslen);
	
  	return ESuccess;	
} 

 /**
   * Converts a string to a descriptor of type HBufc8
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -3 is EStringNoData , -5 is EUseNewMaxL , -4 is EInvalidPointer )
   */

EXPORT_C int StringToHbufc8(string& aSrc , HBufC8* aDes)
{
    unsigned int ilendes = 0;
     
    const char* charString = aSrc.c_str();
    if ('\0' == charString[0])
    {
    	return EStringNoData;
    }
    else if (!aDes)
    {
    	return EInvalidPointer;
    }
	
	ilendes = aDes->Length();
	if(0 == ilendes)
	{
		return EUseNewMaxL;
	}
	
	if (ilendes < strlen(charString))
	{
		return EInsufficientMemory;
	}
	
	*aDes = (const TUint8*)charString;
	return ESuccess;
}

 /**
   * Converts a string to a descriptor of type RBuf8
   *
   * @param aSrc is the string to be converted , aDes is the 
   * reference to the descriptor where the result of conversion 
   * is stored
   * @return Status code (0 is ESuccess,-3 is EStringNoData, -9 is EInsufficientSystemMemory)
   */  

EXPORT_C int StringToRbuf8(const string& aSrc, RBuf8& aDes)
{
    int retval = ESuccess;
    const char* charString = aSrc.c_str();
    if ('\0' == charString[0])
    {
        return EStringNoData;
    }
    
    int ilen = strlen(charString);

    aDes.Copy((const unsigned char *)charString, ilen);	
	
	  return retval;
}
