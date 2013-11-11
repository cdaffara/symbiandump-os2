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
* Description:   Contains the source for Descriptor to wstring conversions
 *
*/


 		
#include "libutils.h"
  

 		
 /**
   * Converts a descriptor of type Tptrc8 to Wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -5 is EDescriptorNoData)
   */ 	

EXPORT_C int Tptrc8ToWstring(TPtrC8& aSrc, wstring& aDes)
{
    int retval = ESuccess;
    int ilen = aSrc.Length();
    int minusone = -1;
    
	if (0 == ilen)
	{
		return EDescriptorNoData;
	}
	
	wchar_t* wcharString = new wchar_t[ilen+1];
	
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	if(minusone != mbstowcs (wcharString, (const char*)aSrc.Ptr(), ilen))
	{
	    wcharString[ilen] = L'\0';
		aDes.assign(wcharString); 	
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
    delete []wcharString;	
	return retval;
}

 /**
   * Converts a descriptor of type TBuf8 to wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -5 is EDescriptorNoData)
   */

EXPORT_C int Tbuf8ToWstring(TDes8& aSrc, wstring& aDes)
{
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
	if (0 == ilen)
	{
		return EDescriptorNoData;
	}
	
	wchar_t* wcharString = new wchar_t[ilen+1];
	
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	if(minusone != mbstowcs (wcharString, (const char*)aSrc.Ptr(), ilen))
	{
	    wcharString[ilen] = L'\0';
		aDes.assign(wcharString); 	
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
    delete []wcharString;	
	return retval;
}

 /**
   * Converts a descriptor of type TBufC8 to wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -5 is EDescriptorNoData)
   */

EXPORT_C int Tbufc8ToWstring(TDesC8& aSrc, wstring& aDes)
{
 
    int retval = ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
	if (0 == ilen)
	{
		return EDescriptorNoData;
	}
	
	wchar_t* wcharString = new wchar_t[ilen+1];
	
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	if(minusone != mbstowcs (wcharString, (const char*)aSrc.Ptr(), ilen))
	{
	    wcharString[ilen] = L'\0';
		aDes.assign (wcharString); 	
	}
	else 
	{
		retval = EInvalidMBSSequence;
	}
	
    delete []wcharString;	
	return retval;
}

 /**
   * Converts a descriptor of type Tptr8 to wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -5 is EDescriptorNoData)
   */

EXPORT_C int Tptr8ToWstring(TPtr8& aSrc, wstring& aDes)
{
    int retval= ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
	if (0 == ilen)
	{
		return EDescriptorNoData;
	}

	wchar_t* wcharString = new wchar_t[ilen+1];
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	if(minusone != mbstowcs(wcharString, (const char*)aSrc.Ptr(), ilen))
	{
	    wcharString[ilen] = L'\0';
	    aDes.assign(wcharString);
	}
	else
	{
		retval= EInvalidMBSSequence;
	}
	
    delete []wcharString;
	return retval;
}

 /**
   * Converts a descriptor of type RBuf8 to wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -5 is EDescriptorNoData)
   */

EXPORT_C int Rbuf8ToWstring(TDes8& aSrc, wstring& aDes)
{
    int retval = ESuccess; 
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    
    wchar_t* buf = new wchar_t[ilen+1];
    if(!buf)
    {
    	return EInsufficientSystemMemory;
    }
    
    if(minusone != mbstowcs(buf, (char *)aSrc.Ptr(), ilen))
    {
        buf[ilen] = L'\0';
    	aDes.assign(buf);
    }
    else
    {
        retval = EInvalidMBSSequence;
    }
    
    delete []buf;
    return retval;
}

 /**
   * Converts a descriptor of type HBuf8 to wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -4 is EInvalidPointer , -5 is EDescriptorNoData, -7 is EInvalidMBSSequence)
   */

EXPORT_C int Hbufc8ToWstring(HBufC8* aSrc, wstring& aDes)
{	
    unsigned int ilen =  0;
    int retval = ESuccess;
    int minusone = -1;
    	
    if (!aSrc)
    {
    	return EInvalidPointer;
    }
    else
    {
        ilen = aSrc->Length();
        if (0 == ilen )
        {
        	return EDescriptorNoData;	
        }  	
    }
    			
	wchar_t* buff = new wchar_t[ilen+1];
	if (!buff)
	{
		return EInsufficientSystemMemory;
	}
    
    if(minusone != mbstowcs(buff, (char *)aSrc->Ptr(), ilen))
    {
        buff[ilen] = L'\0';
    	aDes.assign(buff);
    }
    else
    {
        retval = EInvalidMBSSequence;
    }
    
    delete []buff;
    return retval;
}

 /**
    * Converts a descriptor of type Tlitc8 to wstring
    *
    * @param aSrc is the descriptor to be converted , aDes is the 
    * reference to the wstring array where the result of conversion 
    * is stored  
    * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
    * -5 is EDescriptorNoData, -7 is EInvalidMBSSequence)
    */

EXPORT_C int Tlitc8ToWstring(TDes8& aSrc, wstring& aDes)
{
    int retval= ESuccess;
    unsigned int ilen = aSrc.Length();
    int minusone = -1;
    
	if (0 == ilen)
	{
		return EDescriptorNoData;
	}

	wchar_t* wcharString = new wchar_t[ilen+1];
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	if(minusone != mbstowcs(wcharString, (const char*)aSrc.Ptr(), ilen))
	{
	    wcharString[ilen] = L'\0';
	    aDes.assign(wcharString);
	}
	else
	{
		retval= EInvalidMBSSequence;
	}
	
    delete []wcharString;
	return retval;
}

