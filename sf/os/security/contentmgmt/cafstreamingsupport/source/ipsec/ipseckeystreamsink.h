/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
@file

@internalComponent
@released
*/

#include <caf/streaming/keystreamsink.h>
#include <caf/streaming/keyassociation.h>

#include <in_sock.h>
#include <lib_pfkey.h>
#include <pfkey_send.h>
#include <ipsecpolapi.h>

#ifndef __IPSECKEYSTREAMSINK_H__
#define __IPSECKEYSTREAMSINK_H__

class RDesReadStream;

namespace StreamAccess
{
	NONSHARABLE_CLASS(CIpSecKeyStreamSink) : public CKeyStreamSink
	{
	public:
		 static CIpSecKeyStreamSink* NewLC(RReadStream& aReadStream);
		 static CIpSecKeyStreamSink* NewLC(const TInetAddr& aSrcAddr, const TInetAddr& aDstAddr);
		 ~CIpSecKeyStreamSink();
		 		 
		 // CKeyStreamSink interface
		 CKeyStreamSink* CloneLC() const;
		 void ProcessNewKeyAssociationL(const CKeyAssociation& aKeyAssociation);
		 void SetEncryptionAlgorithmL(const TEncryptionAlgorithm& aEncryptionAlgorithm);
		 void SetAuthenticationAlgorithmL(const TAuthenticationAlgorithm& aAuthenticationAlgorithm);	 
	protected:
		 void DoExternalizeL(RWriteStream& aStream) const;
	private:
		 void ConstructL();
		 void SetPolicyL();
		 void RemoveSaL(TUint32 aSpi);
		 void SynchronousSendAndVerifyMessageL(TPfkeySendMsg& aMessage, TInt aMessageType, TUint32 aSpi);
		 TBool CompareReceivedMessageExtensionsL(TPfkeyRecvMsg &aReceivedReply, TUint32 aSpi) const;
		 void VerifyAssociationsNotSentL() const;
		 CIpSecKeyStreamSink(const TInetAddr& aSrcAddr, const TInetAddr& aDstAddr);
		 void AddAssociationL(TPfkeySendMsg& aMessage, TUint32 aSpi);
	private:
		 RSocketServ iSocketServ;
		 /** Handle to policy server */
		 RIpsecPolicyServ iPolicyServer;
		 /** Handle to SADB socket */
		 RSADB iSADB;
 		 TInetAddr iSourceAddr;
		 TInetAddr iDestinationAddr;
		 TUint8 iAuthAlg;
		 TUint8 iEncAlg;
		 TUint32 iSequenceNumber;
		 /** Signifies whether a security policy has already been set */
		 TBool iPolicySet;
		 TPolicyHandlePckg iPolicyHandle;
		 /** Used for removing old SA-s, managed as a FIFO queue */
		 RArray<TUint32> iSubmittedSpiList; 
		 /** Number of supported concurrent SA-s */
		 TInt iMaxSpiNumber; 
	};
} // namespace StreamAccess
#endif /* __IPSECKEYSTREAMSINK_H__ */


