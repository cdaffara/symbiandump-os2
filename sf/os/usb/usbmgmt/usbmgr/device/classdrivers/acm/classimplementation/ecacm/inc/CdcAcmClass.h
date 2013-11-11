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

#ifndef __CDCACMCLASS_H__
#define __CDCACMCLASS_H__

#include <d32comm.h>
#include "CdcControlInterfaceRequestHandler.h"
#include "BreakController.h"
 
class MHostPushedChangeObserver;
class MBreakObserver;
class MReadObserver;
class MReadOneOrMoreObserver;
class MNotifyDataAvailableObserver;
class MWriteObserver;
class CCdcControlInterface;
class CCdcDataInterface;

NONSHARABLE_CLASS(CCdcAcmClass) :	public CBase, 
						public MCdcCommsClassRequestHandler
/**
 * The Communication Device Class Abstract Control Model class.
 * This class owns and controls the classes that handle the ACM data and 
 * control interfaces.
 * Instantiation of this class instantiates an ACM function.
 */
	{
public:
	enum TUsbParity ///< Serial port emulated parity setting. Taken directly 
		// from the spec.
		{
		EUsbParityNone	= 0,
		EUsbParityOdd	= 1,
		EUsbParityEven	= 2,
		EUsbParityMark	= 3,
		EUsbParitySpace = 4
		};

	enum TUsbStopBits ///< Serial port emulated stop bit setting. Taken 
		// directly from the spec.
		{
		EUsbStopBitOne		 = 0,
		EUsbStopBitOnePtFive = 1,
		EUsbStopBitTwo		 = 2
		};

	enum TUsbDataBits ///< Serial port emulated data bit setting. Taken 
		// directly from the spec.
		{
		EUsbDataBitsFive	= 5,
		EUsbDataBitsSix 	= 6,
		EUsbDataBitsSeven	= 7,
		EUsbDataBitsEight	= 8,
		EUsbDataBitsSixteen = 16
		};

	enum TUsbCommFeatureSelector
		{
		EUsbCommFeatureSelectAbstractState = 0x01,
		EUsbCommFeatureSelectCountryCode   = 0x02
		};

	enum TUsbAbstractStateBits
		{
		EUsbAbstractStateNoBits 	   = 0x0000,
		EUsbAbstractStateIdleSetting   = 0x0001,
		EUsbAbstractStateDataMultiplex = 0x0002
		};

	NONSHARABLE_CLASS(TUsbConfig)	///< Serial port emulated configuration
		{
	public:
		TUint		 iChangedMembers;
		TUint32 	 iRate; 			///< The line rate
		TUsbDataBits iDataBits; 		///< The number of data bits
		TUsbStopBits iStopBits; 		///< The number of stop bits
		TUsbParity	 iParity;			///< The parity
		};

public:
	static CCdcAcmClass* NewL(const TUint8 aProtocolNum, const TDesC16& aAcmControlIfcName, const TDesC16& aAcmDataIfcName);
	~CCdcAcmClass();

public:
	void SetCallback(MHostPushedChangeObserver* aCallback);
	void SetBreakCallback(MBreakObserver* aBreakCallback);
	void Read(MReadObserver& aObserver, TDes8 &aDes);
	void Read(MReadObserver& aObserver, TDes8 &aDes,TInt aLength);
	void ReadOneOrMore(MReadOneOrMoreObserver& aObserver, TDes8 &aDes);
	void ReadOneOrMore(MReadOneOrMoreObserver& aObserver, TDes8 &aDes, TInt aLength);
	void ReadCancel();
	void NotifyDataAvailable(MNotifyDataAvailableObserver& aObserver);
	void NotifyDataAvailableCancel();

	void Write(MWriteObserver& aObserver, const TDesC8& aDes);
	void Write(MWriteObserver& aObserver, const TDesC8& aDes, TInt aLength);
	void WriteCancel();

	TInt SendSerialState(TBool aRing, TBool aDsr, TBool aDcd);

public: // API for requesting a timed or locked break, and for turning the 
	// break off.
	TInt BreakRequest(CBreakController::TRequester aRequester, 
		CBreakController::TState aState,
		TTimeIntervalMicroSeconds32 aDelay = 0);

public: // used by the break controller
	TBool RingState() const;
	TBool DsrState() const;
	TBool DcdState() const;

	// Accessor for break state
	TBool BreakActive() const;
	void SetBreakActive(TBool aBreakActive);
	
	MHostPushedChangeObserver* Callback();
	MBreakObserver* BreakCallback();

private:
	CCdcAcmClass();
	void ConstructL(const TUint8 aProtocolNum);
	void ConstructL(const TUint8 aProtocolNum, const TDesC16& aControlIfcName, const TDesC16& aDataIfcName);

private: // from MCdcCommsClassRequestHandler
	virtual TInt HandleSendEncapCommand(const TDesC8& aData);
	virtual TInt HandleGetEncapResponse(TDes8& aReturnData);
	virtual TInt HandleSetCommFeature(const TUint16 aSelector, 
		const TDes8& aData);
	virtual TInt HandleGetCommFeature(const TUint16 aSelector, 
		TDes8& aReturnData);
	virtual TInt HandleClearCommFeature(const TUint16 aSelector);
	virtual TInt HandleSetLineCoding(const TDesC8& aData);
	virtual TInt HandleGetLineCoding(TDes8& aReturnData);
	virtual TInt HandleSetControlLineState(TBool aRtsState,TBool aDtrState);
	virtual TInt HandleSendBreak(TUint16 aDuration);

private: // utility
	void SetDefaultAcm(void);
	static void ConvertUsbConfigCodingToEpoc(const TUsbConfig& aUsbConfig, 
		TCommConfigV01& aEpocConfig);
	TInt HandleNewAbstractState(const TUint16 aAbstractState);
	TInt HandleNewCountryCode(const TUint16 aCountryCode);

private: // unowned
	MHostPushedChangeObserver* iCallback;
	MBreakObserver* iBreakCallback;

private: // owned
	CBreakController* iBreakController; ///< The BREAK state machine
	CCdcControlInterface* iControl; ///< The control interface
	CCdcDataInterface* iData; ///< The data interface

	TUint16 		iAcmState; ///< The multiplexed and idle state
	TUint16 		iCountryCode; ///< The country code from ISO 3166
	TUsbConfig		iUsbConfig; ///< The USB Configuration
	TBool			iRtsState; ///< The emulated RTS state
	TBool			iDtrState; ///< The eumlated DTR state

	TBool			iAcmDataMultiplex; ///< derived from bit D1 of iACMState
	TBool			iAcmIdleSetting; ///< derived from bit D0 of iACMState

	TBool			iRingState; 	///< Saved RING state
	TBool			iDsrState;		///< Saved DSR state
	TBool			iDcdState;		///< Saved DCD state

	TBool			iBreakActive;		///< Locally controlled BREAK state
	};

#endif // __CDCACMCLASS_H__
