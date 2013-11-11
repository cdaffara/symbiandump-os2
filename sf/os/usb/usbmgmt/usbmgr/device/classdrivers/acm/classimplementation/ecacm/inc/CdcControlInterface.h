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

#ifndef __CDCCONTROLINTERFACE_H__
#define __CDCCONTROLINTERFACE_H__

#include "CdcInterfaceBase.h"

class CCdcAcmClass;
class CCdcControlInterfaceReader;

NONSHARABLE_CLASS(CCdcControlInterface) : public CCdcInterfaceBase
/**
 * ACM control interface subclass.
 */
	{
public:
	static CCdcControlInterface* NewL(CCdcAcmClass& aParent, const TUint8 aProtocolNum, const TDesC16& aIfcName);
	~CCdcControlInterface();

public:
	TInt SetupClassSpecificDescriptor(TUint8 aDataInterfaceNumber);
	TInt SendNetworkConnection(TBool aValue);
	TInt SendSerialState(TBool aOverRun, 
		TBool aParity, 
		TBool aFraming, 
		TBool aRing, 
		TBool aBreak,
		TBool aTxCarrier,
		TBool aRxCarrier);

public:
	static inline TReal32 GetF32( TUint8** aPPbuf );
	static inline void	  PutF32( TUint8** aPPbuf, TReal32 aF32 );

	static inline TUint32 GetU32( TUint8** aPPbuf );
	static inline void	  PutU32( TUint8** aPPbuf, TUint32 aU32 );

	static inline TUint16 GetU16( TUint8** aPPbuf );
	static inline void	  PutU16( TUint8** aPPbuf,TUint16 aU16 );

	static inline TUint8 GetU08( TUint8** aPPbuf );
	static inline void	 PutU08( TUint8** aPPbuf, TUint8 aU8 );

private:
	CCdcControlInterface(const TUint8 aProtocolNum, const TDesC16& aIfcName);
	void ConstructL(CCdcAcmClass& aParent);

private: // from CCdcClassBase
	TInt SetUpInterface();

private: // utility
	TInt WriteData(TEndpointNumber aEndPoint, TDes8& aDes, TInt aLength);

private: // owned
	CCdcControlInterfaceReader* iReader;	///< Handle on the read object

	TUint16 iSerialState;				///< local copy of the flagset
	const TUint8 iProtocolNum;

	enum
		{
		///< Max packet size sent on Interrupt endpoint
		KMaxPacketTypeInterrupt = 64,	
		///< Interrupt endpoint poll time in ms
		KPollInterval  = 128, 
		KWriteDataTimeout = 500000,
		};
	};

#include "LittleEndianPacker.INL"

#endif // __CDCCONTROLINTERFACE_H__
