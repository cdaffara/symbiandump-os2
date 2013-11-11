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
* Description:   Contains the source for Descriptor16 to string conversions
 *
*/



#include "libutils.h"
  


 /**
   * Converts a descriptor of type TBuf16 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored ,
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory)
   */

EXPORT_C int Tbuf16ToString(TDes16& aSrc, string& aDes)
{
    unsigned int ilen = aSrc.Length(); 
    int retval = ESuccess;
    int minusone = -1;
    char* charString = new char[ilen*2+1];
    
    if (!charString)
    {
    	return EInsufficientSystemMemory;
    }
    
    wchar_t *wcharString = new wchar_t[ilen+1];
    
    if (!wcharString)
    {
    	delete []charString;
    	return EInsufficientSystemMemory;
    }
    
    wmemcpy(wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
    wcharString[ilen] = L'\0';
    
    if(minusone != wcstombs(charString, wcharString, ilen*2))
    {
    	charString[ilen*2] = '\0';
        aDes.assign(charString);
    }
    else 
    {
    	retval = EInvalidWCSSequence;
    }
    
    delete []charString;
	delete []wcharString;
	
	return retval;
}

 /**
   * Converts a descriptor of type TBufC16 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored ,
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory)
   */

EXPORT_C int Tbufc16ToString(TDesC16& aSrc, string& aDes)
{
    int ilen = aSrc.Length(), retval = ESuccess;
    int minusone = -1;
    char* charString = new char[ilen*2+1];
    
    if (!charString)
    {
    	return EInsufficientSystemMemory;
    }
    
    wchar_t *wcharString = new wchar_t[ilen+1];
    
    if (!wcharString)
    {
    	delete []charString;
    	return EInsufficientSystemMemory;
    }
    
    wmemcpy(wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
    wcharString[ilen] = L'\0';
   
    if(minusone != wcstombs(charString, wcharString, ilen*2))
    {
    	charString[ilen*2] = '\0';
        aDes.assign(charString);
    }
    else 
    {
    	retval = EInvalidWCSSequence;
    }
    
    delete []charString;
	delete []wcharString;
	
	return retval;
}

 /**
   * Converts a descriptor of type TPtr16 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory)
   */

EXPORT_C int Tptr16ToString (TDes16& aSrc, string& aDes)
{
    int retval = ESuccess;
    unsigned int ilen= aSrc.Length();
	  int minusone = -1;
	  char* charString = new char[ilen*2+1];
	  
	  if (!charString)
    {
    	return EInsufficientSystemMemory;
    }
    wchar_t *wcharString = new wchar_t[ilen+1];
    
    if (!wcharString)
    {
    	delete []charString;
    	return EInsufficientSystemMemory;
    }
    
    wmemcpy(wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
    wcharString[ilen] = L'\0';
   
    
	if(minusone != wcstombs(charString, wcharString, ilen*2))
	{
	    charString[ilen*2] = '\0';
		aDes.assign(charString);  	
	}
	else
	{
		retval = EInvalidWCSSequence;
	}
    
	delete []charString;
	delete []wcharString;
	
	return retval;
}

 /**
   * Converts a descriptor of type TPtrC16 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory)
   */

EXPORT_C int Tptrc16ToString (const TDesC16& aSrc, string& aDes)
{
    int retval = ESuccess;	
	  int ilen= aSrc.Length();
	  int minusone = -1;
	  char* buf = new char[ilen*2 +1];
	  
	  if (!buf)
    {
    	return EInsufficientSystemMemory;
    }
    
    wchar_t *wcharString = new wchar_t[ilen+1];
    
    if (!wcharString)
    {
    	delete []buf;
    	return EInsufficientSystemMemory;
    }
    
    wmemcpy(wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
    wcharString[ilen] = L'\0';
   
	if(minusone != wcstombs(buf, wcharString, ilen*2))
	{
	    buf[ilen*2] = '\0';
		aDes.assign(buf);  	
	}
	else
	{
		retval =  EInvalidWCSSequence;
	}
    
	delete []buf;
	delete []wcharString;
	
	return retval;
}

 /**
   * Converts a descriptor of type HbufC16 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , n_size specifies the conversion size of the string 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory)
   * -2 is EInvalidSize , -5 is EDescriptorNoData)
   */

EXPORT_C int Hbufc16ToString(HBufC16* aSrc, string& aDes, int& n_size)
{
    int retval = ESuccess;
    unsigned int ilen=0;
    int minusone = -1;
    
  	if(!aSrc)
    {
    	return EInvalidPointer;
    }
    else
    {
    	ilen = aSrc->Size();
    	if (0 == ilen)
    	{
    		return EDescriptorNoData;
    	}
    	else if (n_size < ilen)
    	{
    		n_size = ilen; 
    		return EInvalidSize; 		
    	}
    }
    
	  char* buf = new char[ilen*2 +1];
	
	  if (!buf)
    {
    	return EInsufficientSystemMemory;
    }
    
    wchar_t *wcharString = new wchar_t[ilen+1];
    
    if (!wcharString)
    {
    	delete []buf;
    	return EInsufficientSystemMemory;
    }
    
    wmemcpy(wcharString, (const wchar_t*)aSrc->Ptr(), ilen);
    wcharString[ilen] = L'\0';
   
	
	if(minusone != wcstombs(buf, wcharString, ilen*2))
	{
	    buf[ilen*2] = '\0';
	    aDes.assign(buf, ilen*2);		
		
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
	delete []buf;
	delete []wcharString;
		
	return retval;
}

 /**
   * Converts a descriptor of type RBuf16 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory
   * -5 is EDescriptorNoData)
   */

EXPORT_C int Rbuf16ToString(TDes16& aSrc, string& aDes)
{
    unsigned int ilen = aSrc.Length();
    int retval = ESuccess ;
    int minusone = -1;
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    
    char* buf = new char[ilen*2 +1];
        
    if (!buf)
    {
    	return EInsufficientSystemMemory;
    }
    
    wchar_t *wcharString = new wchar_t[ilen+1];
    
    if (!wcharString)
    {
    	delete []buf;
    	return EInsufficientSystemMemory;
    }
    
    wmemcpy(wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
    wcharString[ilen] = L'\0';
   

    if(minusone != wcstombs(buf, wcharString, ilen*2))
	{
	    buf[ilen*2] = '\0';
	    aDes.assign(buf, ilen*2);		
		
	}
	else 
	{
		retval = EInvalidWCSSequence;
	}
	
    delete []buf;
    delete []wcharString;
    
	return retval;
}

 /**
   * Converts a descriptor of type TLit16 to string datatype
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the string to which the result of conversion 
   * is stored , 
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory
   * -5 is EDescriptorNoData)
   */

EXPORT_C int Tlit16ToString(const TDesC16& aSrc, string& aDes)
{   
   unsigned int ilen = 0; 
   int retval = ESuccess;
   ilen = aSrc.Length();
   int minusone = -1;
   
   if (0 == ilen)
   {
   		return EDescriptorNoData;
   }

   char* buf = new char[ilen*2 +1];
   
   if (!buf)
   {
       return EInsufficientSystemMemory;
   }
   
   wchar_t *wcharString = new wchar_t[ilen+1];
    
   if (!wcharString)
   {
   	   delete []buf;
       return EInsufficientSystemMemory;
   }
    
   wmemcpy(wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
   wcharString[ilen] = L'\0';
   

   if(minusone != wcstombs(buf, wcharString, ilen*2))
   {
 	    buf[ilen*2] = '\0';
 	    aDes.assign(buf);
 	    
   }
   else 
   {
	    retval = EInvalidWCSSequence;
   }

   delete []buf;
   delete []wcharString;	
   return retval; 
}
