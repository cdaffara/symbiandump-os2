/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * @file
 * @internalComponent
 */

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_essock.h>
#else
#include <es_sock.h> 
#endif // OVERDUMMY_NCMCC

#include "ncmclassdescriptor.h"
// For OST tracing
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmclassdescriptorTraces.h"
#endif


/**
 * This function packs the TNcmClassDescriptor class into a descriptor with 
 * the correct byte alignment for transmission on the USB bus.
 *
 * @return Correctly-aligned buffer. NB The buffer returned is a member of 
 * this class and has the same lifetime.
 */
TDes8& TNcmClassDescriptor::Des()
	{    
	OstTraceFunctionEntry0( TNCMCLASSDESCRIPTOR_DES_ENTRY );
	TUint index = 0;

	iBuffer.SetLength(KUSBNcmClassSpecificBufferSize);

	iBuffer[index++] = iHdrSize;
	iBuffer[index++] = iHdrType;
	iBuffer[index++] = iHdrSubType;
	LittleEndian::Put16(&iBuffer[index], iHdrBcdCDC);
	index += 2;

	iBuffer[index++] = iUnSize;
	iBuffer[index++] = iUnType;
	iBuffer[index++] = iUnSubType;
	iBuffer[index++] = iUnMasterInterface;
	iBuffer[index++] = iUnSlaveInterface;

	iBuffer[index++] = iEthFunLength;
	iBuffer[index++] = iEthFunType;
	iBuffer[index++] = iEthFunSubtype;
	iBuffer[index++] = iMACAddress;
	LittleEndian::Put32(&iBuffer[index], iEthernetStatistics);	
	index += 4;
	LittleEndian::Put16(&iBuffer[index], iMaxSegmentSize);	
	index += 2;
	LittleEndian::Put16(&iBuffer[index], iNumberMCFilters);	
	index += 2;
	iBuffer[index++] = iNumberPowerFilters;

	iBuffer[index++] = iNcmFunLength;
	iBuffer[index++] = iNcmFunType;
	iBuffer[index++] = iNcmFunSubtype;
	LittleEndian::Put16(&iBuffer[index], iNcmVersion);	
	index += 2;
	iBuffer[index++] = iNetworkCapabilities;

	OstTraceFunctionExit0( TNCMCLASSDESCRIPTOR_DES_EXIT );
	return iBuffer;
	}

// End of file

