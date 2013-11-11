/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CLASSDESCRIPTOR_H__
#define __CLASSDESCRIPTOR_H__

#include <e32std.h>
#include "acmcapability.h"

#ifdef DISABLE_ACM_CF_COUNTRY_SETTING

	const TUint   KUSBClassSpecificBufferSize = 14;

#elif defined ENABLE_ACM_CF_COUNTRY_SETTING

	const TUint   KUsbCommNumCountries = 1;
	const TUint16 KUsbCommCountryCode0 = (('G'<<8)+'B');
	const TUint   KUSBClassSpecificBufferSize = 18 + (2*KUsbCommNumCountries);

#endif

NONSHARABLE_CLASS(TUsbCsClassDescriptor) 	///< Class-specific descriptor
	{
public:
	TUint8	iHdrSize;			///< Header size
	TUint8	iHdrType;			///< Type
	TUint8	iHdrSubType;		///< Sub-type
	TUint16 iHdrBcdCDC; 		///< CDC version no in binary coded decimal
	TUint8	iAcmSize;			///< ACM descriptor size
	TUint8	iAcmType;			///< ACM type
	TUint8	iAcmSubType;		///< ACM sub-type
	TUint8	iAcmCapabilities;	///< ACM capabilities
	TUint8	iUnSize;			///< Union descriptor size
	TUint8	iUnType;			///< Union descriptor type
	TUint8	iUnSubType; 		///< Union descriptor sub-type
	TUint8	iUnMasterInterface; ///< Master interface number
	TUint8	iUnSlaveInterface;	///< Slave interface number

#if defined(DISABLE_ACM_CF_COUNTRY_SETTING)

	// no functional descriptor needed

#elif defined(ENABLE_ACM_CF_COUNTRY_SETTING)

	// CDC Country Selection Functional Descriptor
	TUint8 iCsSize;    ///< Country Selection descriptor size
	TUint8 iCsType;    ///< Country Selection descriptor type
	TUint8 iCsSubType; ///< Country Selection descriptor sub-type
	TUint8 iCsRelDate; ///< Country Selection (string ddmmyyyy) ISO-3166 date
	TUint16 iCsCountryCode[KUsbCommNumCountries];

#endif

public:
	TDes8& Des();

private:
	enum
		{
		///< Size of the ACM class-specific descriptor
		KUsbClassSpecificBufferSize = 14,
		};

	TBuf8<KUsbClassSpecificBufferSize> iBuffer;
	};

#endif // __CLASSDESCRIPTOR_H__
