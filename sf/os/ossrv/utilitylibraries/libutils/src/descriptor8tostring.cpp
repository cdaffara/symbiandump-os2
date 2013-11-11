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
* Description:   Contains the source for Descriptor8 to string conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a descriptor of type TBuf8 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored ,
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory
   * -4 is EDescriptorNoData)
   */

EXPORT_C int Tbuf8ToString(TDes8& aSrc, string& aDes)
{
    unsigned int ilen = aSrc.Length();
    if (ilen == 0)
    {
    	return EDescriptorNoData;
    }
    
    char* charString = new char[ilen +1];
    if (!charString)
    {
    	return EInsufficientSystemMemory;
    }
    	
    memcpy(charString, (char*)aSrc.Ptr(), ilen);
    charString[ilen] = '\0';
    
	aDes.assign(charString);
	
	delete []charString;
	return ESuccess;
}

  /**
    * Converts a descriptor of type TBufC8 to string datatype
    *
    * @param aSrc is the descriptor to be converted , aDes is the 
    * reference to the string to which the result of conversion 
    * is stored ,
    * @return Status code (0 is ESuccess, -1 is EInsufficientMemory
    * -4 is EDescriptorNoData)
    */

EXPORT_C int Tbufc8ToString(TDesC8& aSrc, string& aDes)
{
    unsigned int ilen = aSrc.Length();
    if(0 == ilen)
    {
    	return EDescriptorNoData;
    }
    
    char* charString = new char[ilen +1];
    if (!charString)
    {
    	return EInsufficientSystemMemory;
    }
    	
    memcpy(charString, (char*)aSrc.Ptr(), ilen);
    charString[ilen] = '\0';
    
	aDes.assign(charString);
	
	delete []charString;
	return ESuccess;
}

 /**
   * Converts a descriptor of type TPtr8 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored ,
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory
   * -4 is EDescriptorNoData)
   */

EXPORT_C int Tptr8ToString (TDes8& aSrc, string& aDes)
{
    unsigned int ilen = aSrc.Length();
    if(0 == ilen )
    {
    	return EDescriptorNoData;
    }
	
	char* charString = new char[ilen +1];
	
	if (!charString)
    {
    	return EInsufficientSystemMemory;
    }
    
    memcpy(charString, (char*)aSrc.Ptr(), ilen);
    charString[ilen] = '\0';
    
	aDes.assign (charString);
	
	delete []charString;
	
	return ESuccess;
}

 /**
   * Converts a descriptor of type TPtrC8 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored ,
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory
   * -4 is EDescriptorNoData)
   */

EXPORT_C int Tptrc8ToString (const TDesC8& aSrc, string& aDes)
{
    unsigned int ilen = aSrc.Length();
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    
	char* charString = new char[ilen +1];
	if(!charString)
	{
		return EInsufficientSystemMemory;
	}
	
    memcpy(charString, (char*)aSrc.Ptr(), ilen);
    charString[ilen] = '\0';
    
    aDes.assign(charString);
	
	delete []charString;
	return ESuccess;
}

 /**
   * Converts a descriptor of type HBufC8 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , n_size specifies the conversion size of the string
   * @return Status code (0 is ESuccess, -2 is EInvalidSize
   * -4 is EInvalidPointer)
   */

EXPORT_C int Hbufc8ToString(HBufC8* aSrc, string& aDes)
{
    unsigned int ilen = 0;
	if(!aSrc)
    {
        return EInvalidPointer;
    }
    else 
    {
        ilen = aSrc->Length();
        if ( 0 == ilen)
        {
        	return EDescriptorNoData;	
        }
    	
    }
	
	char* charString = new char[ilen +1];
	if(!charString)
	{
		return EInsufficientSystemMemory;
	}
	
	memcpy(charString, (char*)aSrc->Ptr(), ilen);
    charString[ilen] = '\0';
    
	aDes.assign(charString);
	delete[] charString;
	return ESuccess;
}

 /**
   * Converts a descriptor of type RBuf8 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , n_size specifies the conversion size of the string
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -5 is EDescriptorNoData)
   */

EXPORT_C int Rbuf8ToString(TDes8& aSrc, string& aDes)
{
    unsigned int ilen = aSrc.Length();
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    
    char* buf = new char [ilen+1];
    if (!buf)
    {
    	return EInsufficientSystemMemory;
    }

    memcpy (buf,(char *)aSrc.Ptr(), ilen);
    buf[ilen]='\0';
   
    aDes.assign(buf);
    
    delete []buf;
	return ESuccess;
}

 /**
   * Converts a descriptor of type TLit8 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , n_size specifies the conversion size of the string
   * @return Status code (0 is ESuccess,  -5 is EDescriptorNoData
   * -9 is EInsufficientSystemMemory)
   */

EXPORT_C int Tlit8ToString(const TDesC8& aSrc, string& aDes)
{   
    unsigned int ilen = aSrc.Length();
    
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    
    char* buf = new char [ilen+1];
    if (!buf)
    {
    	return EInsufficientSystemMemory;
    }

    memcpy (buf,(char *)aSrc.Ptr(), ilen);
    buf[ilen]='\0';
   
    aDes.assign(buf);
    delete [] buf;
    
	return ESuccess;
}
