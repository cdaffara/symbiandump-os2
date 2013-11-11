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

#ifndef __CDCCONTROLINTERFACEREADER_H__
#define __CDCCONTROLINTERFACEREADER_H__

#include <e32base.h>
#include "RequestHeader.h"
#include "acmcapability.h"

class MCdcCommsClassRequestHandler;
class RDevUsbcClient;

NONSHARABLE_CLASS(CCdcControlInterfaceReader) : public CActive
/**
 * Deals with reading control information from the LDD.
 */
	{
public:
	static CCdcControlInterfaceReader* NewL(
		MCdcCommsClassRequestHandler& aParent, 
		RDevUsbcClient& aLdd);
	~CCdcControlInterfaceReader();

private:
	CCdcControlInterfaceReader(MCdcCommsClassRequestHandler& aParent, 
		RDevUsbcClient& aLdd);

private: // utility
	void HandleReadCompletion(TInt aError);
	
	void DecodeMessageHeader();
	void DecodeMessageDataWithResponseRequired();
	void DecodeMessageData();

	void ReadMessageHeader();
	void ReadMessageData(TUint aLength);

private: // from CActive
	void RunL();
	void DoCancel();

private: // unowned
	MCdcCommsClassRequestHandler& iParent;
	RDevUsbcClient& iLdd;			

private: // owned
	TBuf8<KUsbRequestHdrSize> iMessageHeader; ///< Raw message header

	enum
		{
		KAcmControlReadBufferLength = 256, ///< Size of the ACM control read 
		///< buffer

		// Request IDs- see p16 of UBSCDC1.1 Table 4.
		KGetEncapsulated = 0x01, ///< GET_ENCAPSULATED_RESPONSE
		KGetCommFeature  = 0x03, ///< GET_COMM_FEATURE
		KGetLineCoding	 = 0x21, ///< GET_LINE_CODING

		KSendEncapsulated	 = 0x00, ///< SEND_ENCAPSULATED_COMMAND
		KSetCommFeature 	 = 0x02, ///< SET_COMM_FEATURE
		KClearCommFeature	 = 0x04, ///< CLEAR_COMM_FEATURE
		KSetLineCoding		 = 0x20, ///< SET_LINE_CODING
		KSetControlLineState = 0x22, ///< SET_CONTROL_LINE_STATE
		KSendBreak			 = 0x23, ///< SEND_BREAK
		};

	enum TReaderState
		{
		EWaitingForHeader = 0,	///< Waiting for message header
		EWaitingForData = 1,	///< Waiting for message data
		};

	TBuf8<KAcmControlReadBufferLength> iMessageData;	///< Message data
	///< Decoded message header
	TUsbRequestHdr iRequestHeader;						
	TReaderState iState;								///< Reader state
	};

#endif // __CDCCONTROLINTERFACEREADER_H__
