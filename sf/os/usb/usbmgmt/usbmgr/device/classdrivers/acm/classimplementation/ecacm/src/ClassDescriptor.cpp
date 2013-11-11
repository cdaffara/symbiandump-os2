/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "ClassDescriptor.h"
#include "AcmPanic.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ClassDescriptorTraces.h"
#endif


TDes8& TUsbCsClassDescriptor::Des()
/**
 * This function packs the TUsbCsClassDescriptor class into a descriptor with 
 * the correct byte alignment for transmission on the USB bus.
 *
 * @return Correctly-aligned buffer. NB The buffer returned is a member of 
 * this class and has the same lifetime.
 */
	{
	OstTraceFunctionEntry0( TUSBCSCLASSDESCRIPTOR_DES_ENTRY );
	
	TUint index = 0;

	iBuffer.SetLength(KUsbClassSpecificBufferSize);

	iBuffer[index++] = iHdrSize;
	iBuffer[index++] = iHdrType;
	iBuffer[index++] = iHdrSubType;
	iBuffer[index++] = (TUint8) ( iHdrBcdCDC & 0x00ff);
	iBuffer[index++] = (TUint8) ((iHdrBcdCDC & 0xff00) >> 8);
	iBuffer[index++] = iAcmSize;
	iBuffer[index++] = iAcmType;
	iBuffer[index++] = iAcmSubType;
	iBuffer[index++] = iAcmCapabilities;
	iBuffer[index++] = iUnSize;
	iBuffer[index++] = iUnType;
	iBuffer[index++] = iUnSubType;
	iBuffer[index++] = iUnMasterInterface;
	iBuffer[index++] = iUnSlaveInterface;

#if defined(DISABLE_ACM_CF_COUNTRY_SETTING)

	// no functional descriptor needed

#elif defined(ENABLE_ACM_CF_COUNTRY_SETTING)

	// CDC Country Selection Functional Descriptor
	iBuffer[index++] = iCsSize;
	iBuffer[index++] = iCsType;
	iBuffer[index++] = iCsSubType;
	iBuffer[index++] = iCsRelDate;

	for ( TUint scan = 0 ; scan < KUsbCommNumCountries ; scan++ )
		{
		iBuffer[index++] = (TUint8) (iCsCountryCode[scan] & 0x00ff);
		iBuffer[index++] = (TUint8) ((iCsCountryCode[scan] & 0xff00) >> 8);
		}

#endif

	if (index != KUsbClassSpecificBufferSize)
		{
		OstTrace1( TRACE_FATAL, TUSBCSCLASSDESCRIPTOR_DES, "TUsbCsClassDescriptor::Des;index=%d", (TInt)index );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	OstTraceFunctionExit0( TUSBCSCLASSDESCRIPTOR_DES_EXIT );
	return iBuffer;
	}

//
// End of file
