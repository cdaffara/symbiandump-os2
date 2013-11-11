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
* Description:   Contains the source for Descriptor16 to wstring conversions
 *
*/

	
 		
#include "libutils.h"
  


 		
 /**
   * Converts a descriptor of type TPtrC16 to Wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the Wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */
   		
EXPORT_C int Tptrc16ToWstring(TPtrC16& aSrc, wstring& aDes)
{	
   
    unsigned int ilen = aSrc.Length();
    if (0 == ilen)
	{
		return EDescriptorNoData;
	}
	
	wchar_t* wcharString = new wchar_t[ilen+1];
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy((wchar_t*)wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
	wcharString[ilen] = L'\0';
	
	aDes.assign(wcharString);
	
	delete []wcharString;
	
	return ESuccess;
}

 /**
   * Converts a descriptor of type Tbuf16 to Wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the Wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */		

EXPORT_C int Tbuf16ToWstring(TDes16& aSrc, wstring& aDes)
{	
   
    unsigned int ilen = aSrc.Length();
    if (0 == ilen)
	{
		return EDescriptorNoData;
	}
	
	wchar_t* wcharString = new wchar_t[ilen+1];
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy((wchar_t*)wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
	wcharString[ilen] = L'\0';
	
	aDes.assign(wcharString);
	
	delete []wcharString;
	return ESuccess;
}

 /**
   * Converts a descriptor of type HBufc16 to Wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the Wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   *  -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */
EXPORT_C int Hbufc16ToWstring(HBufC16* aSrc, wstring& aDes)
{	
    unsigned int ilen =  0;	
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
    			
	wchar_t* wcharString = new wchar_t[ilen+1];
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy((wchar_t*)wcharString, (const wchar_t*)aSrc->Ptr(), ilen);
	wcharString[ilen] = L'\0';
	
	aDes.assign(wcharString);
	
	delete []wcharString;
	return ESuccess;
}

 /**
   * Converts a descriptor of type TPtr16 to Wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the Wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   *  -5 is EDescriptorNoData)
   */
EXPORT_C int Tptr16ToWstring(TPtr16& aSrc, wstring& aDes)
{
   
    unsigned int ilen = aSrc.Length();
    
	if (0 == ilen)
	{
		return EDescriptorNoData;
	}
		
	wchar_t* wcharString = new wchar_t[ilen+1];
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy(wcharString, (wchar_t *)aSrc.Ptr(), ilen);
	wcharString[ilen] = L'\0';
	
	aDes.assign(wcharString);
	
	delete []wcharString;
	return ESuccess;
}


 /**
   * Converts a descriptor of type RBuf16 to Wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the Wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   *  -2 is EInvalidSize , -4 is EInvalidPointer , -5 is EDescriptorNoData)
   */
EXPORT_C int Rbuf16ToWstring(TDes16& aSrc, wstring& aDes)
{
    unsigned int ilen = aSrc.Length();
    if (0 == ilen)
    {
    	return EDescriptorNoData;
    }
    
    wchar_t* buf = new wchar_t[ilen+1];
    if(!buf)
    {
    	return EInsufficientSystemMemory;
    }
    
   	wmemcpy (buf,(wchar_t *)aSrc.Ptr(), ilen);
    buf[ilen]=L'\0';
    
    aDes.assign(buf);
	delete[] buf;
	
	return ESuccess;
}

 /**
   * Converts a descriptor of type TLitc16 to Wstring
   *
   * @param aSrc is the descriptor to be converted , aDes is the 
   * reference to the Wstring array where the result of conversion 
   * is stored  
   * @return Status code (0 is ESuccess, -1 is EInsufficientMemory, 
   * -5 is EDescriptorNoData)
   */
EXPORT_C int Tlitc16ToWstring(TDesC16& aSrc, wstring& aDes)
{	
 
    unsigned int ilen =  aSrc.Length();	
   	if (0 == ilen)
    {
    	return EDescriptorNoData;
    }		
	
	wchar_t* wcharString = new wchar_t[ilen+1];
	if (!wcharString)
	{
		return EInsufficientSystemMemory;
	}
	
	wmemcpy((wchar_t*)wcharString, (const wchar_t*)aSrc.Ptr(), ilen);
	wcharString[ilen] = L'\0';
	
	aDes.assign(wcharString);
	
	delete []wcharString;
	return ESuccess;
}
