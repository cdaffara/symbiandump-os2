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
* Description:   Contains the source for char * to Descriptor16 conversions
 *
*/



#include "libutils.h"
  

/** 
  * Converts a character stream to TBuf16
  * 
  * @param aSrc is char*, aDes is the reference to the descriptor
  * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer, -7 is EInvalidMBSSequence)
  */
     
EXPORT_C  int CharToTbuf16(const char* aSrc, TDes16& aDes)
{		
    int retval = ESuccess;
    unsigned int ilen = 0;
    int minusone = -1;

	    
	if (!aSrc)
	{	
	    return EInvalidPointer;	
	}	
	else 
	{	
	    ilen = strlen(aSrc);
	    if(ilen > aDes.MaxLength())
	    {
	        return EInsufficientMemory;	
	    }	
	}    
    wchar_t *WcharString = new wchar_t[ilen+1];
    if(!WcharString)
    {
    	return EInsufficientSystemMemory;
    }
            
	if(minusone == mbstowcs(WcharString, (const char*)aSrc, ilen))
	{   
		retval = EInvalidMBSSequence;
	}
	else 
	{
		aDes.Copy((unsigned short *)WcharString, ilen);
	}   
        
	delete[] WcharString;
	return retval;
}

   /**
     * Converts a character stream to HBufc16
     * 
     * @param aSrc is char*, aDes is the reference to the descriptor
     * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer, -6 is EUseNewMaxL, -7 is EInvalidMBSSequence)
     */
     
EXPORT_C int CharToHbufc16(const char* aSrc, HBufC16* aDes)
{	
	int retval = ESuccess; 
	unsigned int ilen = 0, ilendes = 0; 
	int minusone = -1;
	if (!aSrc || !aDes)
	{
		return EInvalidPointer;
	}
	else 
	{
	    ilen = strlen (aSrc);
	    ilendes = aDes->Length();
	    
	    if(0 == ilendes)
	    {
	        return EUseNewMaxL;	
	    }     
	    else if (ilen > ilendes)
	    {
	    	return EInsufficientMemory;	
	    }	
	}
	
	wchar_t *wCharString = new wchar_t[ilen+1];
	
	if (!wCharString)
	{
		return EInsufficientMemory;
	}
	
	if(minusone == mbstowcs((wchar_t *)wCharString, (const char*)aSrc, ilen))
	{
		retval = EInvalidMBSSequence;
	}
	else 
	{
	    wCharString[ilen] = (wchar_t)'\0';
		TPtrC16 temp ((unsigned short *)wCharString, ilen);
		*aDes = temp; 
	}
	
	delete[] wCharString;
	return retval;
}

/**
  * Converts a character stream to TPtr16
  *
  * @param aSrc is char*, wPtr is wchar_t*, aDes is the reference to the descriptor
  * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer, -7 is EInvalidMBSSequence)
  */
     
EXPORT_C int CharpToTptr16(const char* aSrc, TPtr16& aDes)
{
    int retval = ESuccess;
    unsigned int ilen =0 , ilendes = 0;
    int minusone = -1;
    
	if (!aSrc)
	{
		return EInvalidPointer;
	}
	else 
	{
	    ilen = strlen(aSrc);
	    ilendes = aDes.MaxLength();
	    
	    if (ilendes < ilen)
	    {
	    	return EInsufficientMemory;
	    }
	}
	
  wchar_t *wPtr = new wchar_t[ilen+1];
	
	if (!wPtr)
	{
		return EInsufficientMemory;
	}
	
	if(minusone != mbstowcs((wchar_t *)wPtr, (const char*)aSrc, ilen ))
	{
		aDes.Copy((unsigned short *)wPtr, ilen);
	}
	else
	{
		retval = EInvalidMBSSequence;
	}
	
	delete[] wPtr;
	return retval;
}

/**
  * Converts a character stream to TPtrc16
  *
  * @param aSrc is char*, cPtr is wchar_t*, aDes is the reference to the descriptor
  * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer, -7 is EInvalidMBSSequence)
  */
     
EXPORT_C int CharpToTptrc16(char* aSrc ,wchar_t* cPtr, TPtrC16& aDes)
{
    int retval = ESuccess; 
    unsigned int ilen = 0;
    int minusone = -1;
    
	if ( !aSrc || !cPtr )
	{
		return EInvalidPointer;
	}
	
	ilen = strlen(aSrc);

	if(minusone != mbstowcs((wchar_t*)cPtr, (const char*)aSrc, strlen(aSrc)))
	{
	    aDes.Set((TUint16*)cPtr, ilen);
	}
	else
	{
	    retval = EInvalidMBSSequence;
	}
	
	return retval; 
}

/**
  * Converts a character stream to RBuf16
  * 
  * @param aSrc is char*, aDes is the reference to the descriptor
  * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, -4 is EInvalidPointer, -7 is EInvalidMBSSequence)
  */
  
  
EXPORT_C int CharToRbuf16(const char* aSrc, RBuf16& aDes)
{
    int retval = ESuccess;
    unsigned int ilen = 0; 
    int minusone = -1;
    if ( !aSrc )
    {
    	return EInvalidPointer;
    }
    ilen = strlen(aSrc);
	
	wchar_t* buf = new wchar_t[ilen];
    if (!buf)
    {
    	return EInsufficientSystemMemory;
    }
    
	if(minusone != mbstowcs(buf, aSrc, ilen))
	{
	    aDes.Copy((const unsigned short *)buf, ilen);	
	}   
	else
	{
		retval = EInvalidMBSSequence;
	}
	
	delete []buf;
	return retval;
}
