// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "ipseckeystreamsink.h"
#include <caf/streaming/keyassociation.h>
#include <networking/pfkeyv2.h>
#include <s32mem.h>
#include "scaflog.h"

using namespace StreamAccess;

// The last two rules: (inbound = {}, outbound = {}) are catch-all rules - without them, all packets
// which do not match the policy will get rejected
_LIT8( KPolicyFormat,  
"SECURITY_FILE_VERSION: 3\r\n[INFO]\r\n\
Test CAF IpSec Integration Policy\r\n\
[POLICY]\r\n\
sa caf_sas = {\r\n\
esp\r\n\
encrypt_alg %d\r\n\
auth_alg %d\r\n\
src_specific\r\n\
local_port_specific\r\n\
remote_port_specific\r\n\
}\r\n\
inbound local %S 255.255.255.255 local_port %d remote_port %d = { caf_sas() }\r\n\
inbound = {}\r\n\
outbound = {}\r\n" );

// The number is taken as the maximal recommendation from OMA DRM BCAST standard (section 9.1, IPSec management)
const TUint KDefaultMaxSpiNumber = 3; 

CIpSecKeyStreamSink* CIpSecKeyStreamSink::NewLC(const TInetAddr& aSrcAddr, const TInetAddr& aDstAddr)
	{
	CIpSecKeyStreamSink* self = new (ELeave) CIpSecKeyStreamSink(aSrcAddr, aDstAddr);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CIpSecKeyStreamSink::CIpSecKeyStreamSink(const TInetAddr& aSrcAddr, const TInetAddr& aDstAddr) :
						iSourceAddr(aSrcAddr), 
						iDestinationAddr(aDstAddr), 
						iPolicySet(EFalse),
						iMaxSpiNumber(KDefaultMaxSpiNumber)
	{		
	}

CIpSecKeyStreamSink::~CIpSecKeyStreamSink()
	{
	// Remove all SA-s
	TInt submittedSaCount = iSubmittedSpiList.Count();
	for (TInt i = 0; i < submittedSaCount; ++i)
		{
		TRAP_IGNORE(RemoveSaL(iSubmittedSpiList[i]));
		}
	
	if (iPolicySet)
		{
		TRequestStatus status;
		iPolicyServer.UnloadPolicy( iPolicyHandle(), status );
		User::WaitForRequest(status);
		// Impossible to handle the error well in destructor - ignore the status code
		}

	iSADB.Close();
	iSocketServ.Close();
	
	iPolicyServer.Close();		
	iSubmittedSpiList.Close();
	}

void CIpSecKeyStreamSink::ConstructL()
	{
	DEBUG_PRINTF(_L("Constructing an IPSec key stream sink."));
	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iSADB.Open(iSocketServ));
	User::LeaveIfError(iPolicyServer.Connect());	
	DEBUG_PRINTF(_L("Constructed an IPSec key stream sink."));
	}

CKeyStreamSink* CIpSecKeyStreamSink::CloneLC() const
	{
	CIpSecKeyStreamSink *ret = CIpSecKeyStreamSink::NewLC(iSourceAddr, iDestinationAddr);
  	ret->iEncAlg = this->iEncAlg;
 	ret->iAuthAlg = this->iAuthAlg;
 	return ret;
	}

static TUint32 ConvertToNetworkOrder(TUint32 aNum)
    {
    const TInt KMaxTUint32CStringLen = 11;
    TUint8 temp[ KMaxTUint32CStringLen ];   
    LittleEndian::Put32( temp, aNum );
    return BigEndian::Get32( temp );
    }

TBool CIpSecKeyStreamSink::CompareReceivedMessageExtensionsL(TPfkeyRecvMsg &aReceivedReply, TUint32 aSpi) const
	{
	TBool spiVerified(EFalse), destAddrVerified(EFalse);
	
	TPfkeyAnyExt ext;
 
 	// We verify that both the SPI matches and the destination address matches - this should exclude
 	// replies to unrelated messages
	while ( !spiVerified || !destAddrVerified )
         {
         // Both extensions should be found in the reply
         User::LeaveIfError(aReceivedReply.NextExtension(ext));
         TInt extType = ext.ExtType();
         if ( extType == SADB_EXT_ADDRESS_DST )
             {
             const TInetAddr* addr = reinterpret_cast<const TInetAddr *>(ext.Ptr() + sizeof(struct sadb_address));
    		 if (*addr == iDestinationAddr)
    		 	destAddrVerified = ETrue;	
    		 else
    			return EFalse;
             }
         else if (extType == SADB_EXT_SA)
         	{
    		const sadb_sa* saExt = reinterpret_cast<const sadb_sa *>(ext.Ptr());
    		if (saExt->sadb_sa_spi == aSpi)
    			spiVerified = ETrue;
    		else
    			return EFalse;
         	}
         }
    return ETrue;
	}

void CIpSecKeyStreamSink::SynchronousSendAndVerifyMessageL(TPfkeySendMsg& aMessage, TInt aMessageType, TUint32 aSpi) 
	{
	// Wait for the message to be sent
	TRequestStatus status;
	iSADB.FinalizeAndSend(aMessage, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	// Receive a reply - since SADB sends replies to _all_ sockets, we need to filter out replies
	// which do not correspond to our own request
	for(;;)
		{
		TPfkeyRecvMsg receivedReply;
		iSADB.ReadRequest(receivedReply, status);
		User::WaitForRequest(status);
		User::LeaveIfError(status.Int());
		
		// Verify that the message is a reply to the one sent by us
		sadb_msg &msgHeader = receivedReply.MsgHdr();
		if (msgHeader.sadb_msg_pid != RProcess().Id())
			continue;
		if (msgHeader.sadb_msg_seq != iSequenceNumber)
			continue;
		// Do additional validation by checking whether the destination address and the SPI are the same as ours
		TBool isResponseToRequest(ETrue);
		switch (aMessageType)
			{
			case SADB_ADD: 
			case SADB_DELETE:
				isResponseToRequest = CompareReceivedMessageExtensionsL(receivedReply, aSpi);
				break;
			default:
				ASSERT(0); // Unexpected state
			}
		if (!isResponseToRequest)
			continue;
		// If the message types does not match, then the problem is internal in IPSec - it should not answer with a different message type
		if (msgHeader.sadb_msg_type != aMessageType)
			User::Leave(KErrArgument); 
		if (msgHeader.sadb_msg_errno != 0)
			{
			// Mimic the logic in IPSec error handling (see the Update function in key_msg.cpp)		
			TUint16 reservedField = (TUint16)msgHeader.sadb_msg_reserved << 8;
			TUint16 errnoField = msgHeader.sadb_msg_errno;
			User::Leave(-(reservedField + errnoField));
			}		
		break;
		}			
	}
	
void CIpSecKeyStreamSink::AddAssociationL(TPfkeySendMsg& aMessage, TUint32 aSpi) 	
	{
	SynchronousSendAndVerifyMessageL(aMessage, SADB_ADD, aSpi);
	
	// Take care to delete old SA-s, if needed
	iSubmittedSpiList.AppendL(aSpi); 
	if (iSubmittedSpiList.Count() > iMaxSpiNumber)
		{
		RemoveSaL(iSubmittedSpiList[0]);
		iSubmittedSpiList.Remove(0);
		}		
	}
	
void CIpSecKeyStreamSink::ProcessNewKeyAssociationL(const CKeyAssociation& aKeyAssociation) 
	{
	if (!iPolicySet)
		{
		SetPolicyL();
		}
	// No official RTTI support, using static_cast - no validation that it is indeed an IPSec association
	const CIpSecKeyAssociation* ipsecKeyAssociation = static_cast<const CIpSecKeyAssociation *>(&aKeyAssociation);
	
	DEBUG_PRINTF2(_L("IPSec key stream sink - processing new association with SPI %d."), ipsecKeyAssociation->GetSpiL());
		
	// We use ESP, since there is very low probability that AH will be used - it does not provide confidentiality
	TPfkeySendMsg sendMessage(SADB_ADD, SADB_SATYPE_ESP, ++iSequenceNumber, RProcess().Id());
	TUint32 bigEndianSpi(ConvertToNetworkOrder(ipsecKeyAssociation->GetSpiL()));
	sendMessage.Add( Int2Type<SADB_EXT_SA>(), bigEndianSpi, iAuthAlg, iEncAlg); 
	const HBufC8 *encryptionKey(ipsecKeyAssociation->GetEncryptionKeyL());
	if(!encryptionKey)
		User::Leave(KErrArgument);
	
	sendMessage.Add( Int2Type<SADB_EXT_KEY_ENCRYPT>(), *encryptionKey, encryptionKey->Length() * 8);
	if (iAuthAlg) // Authentication is optional - use it only if algorithm has been set
		{
		const HBufC8 *authenticationKey(ipsecKeyAssociation->GetAuthenticationKeyL());
		if (!authenticationKey)
			User::Leave(KErrArgument);
		sendMessage.Add( Int2Type<SADB_EXT_KEY_AUTH>(), *authenticationKey, authenticationKey->Length() * 8);			
		}
	sendMessage.Add( Int2Type<SADB_EXT_ADDRESS_SRC>(), iSourceAddr);
	sendMessage.Add( Int2Type<SADB_EXT_ADDRESS_DST>(), iDestinationAddr);
	
	TRAPD(err, AddAssociationL(sendMessage, bigEndianSpi));
	// If something went wrong, try to remove the SA, to keep the SADB in consistent state.
	// We only do our best effort, since the error might be global to the device, or it may have occured before we've added the SA
	if (err != KErrNone)
		{
		TRAP_IGNORE(RemoveSaL(bigEndianSpi));
		TInt submittedSpiCount = iSubmittedSpiList.Count();
		if (submittedSpiCount > 0 && iSubmittedSpiList[submittedSpiCount - 1] == bigEndianSpi)
			iSubmittedSpiList.Remove(submittedSpiCount - 1);
				
		User::Leave(err);
		}
	DEBUG_PRINTF2(_L("IPSec key stream sink - processed new association with SPI %d."), ipsecKeyAssociation->GetSpiL());
	}
	
void CIpSecKeyStreamSink::RemoveSaL(TUint32 aSpi)
	{	
	TPfkeySendMsg sendMessage(SADB_DELETE, SADB_SATYPE_ESP, ++iSequenceNumber, RProcess().Id());	
	sendMessage.Add( Int2Type<SADB_EXT_SA>(), aSpi, iAuthAlg, iEncAlg); 
	sendMessage.Add( Int2Type<SADB_EXT_ADDRESS_DST>(), iDestinationAddr);
	SynchronousSendAndVerifyMessageL(sendMessage, SADB_DELETE, aSpi);
	}

void CIpSecKeyStreamSink::VerifyAssociationsNotSentL() const
	{
	if (iSubmittedSpiList.Count())
		{
		User::Leave(KErrNotSupported); // It is forbidden to change the algorithm once associations have been processed
		}	
	}

const TUint32 SADB_AALG_AESCBC = 12; // temporarily here until IPSec supports AES constants

void CIpSecKeyStreamSink::SetEncryptionAlgorithmL(const TEncryptionAlgorithm& aEncryptionAlgorithm)
	{
	VerifyAssociationsNotSentL();
	switch (aEncryptionAlgorithm)
		{
		case EAES_128_CBC: iEncAlg = SADB_AALG_AESCBC; break;
		case ENoEncryption: 
		case EAES_128_CTR:		
		default: User::Leave(KErrNotSupported);		
		};
	}

void CIpSecKeyStreamSink::SetAuthenticationAlgorithmL(const TAuthenticationAlgorithm& aAuthenticationAlgorithm)
	{
	VerifyAssociationsNotSentL();
	switch (aAuthenticationAlgorithm)
		{
		case EHMAC_SHA1: iAuthAlg = SADB_AALG_SHA1HMAC; break;
		case ENoAuthentication:	iAuthAlg = 0; break;
		default: User::Leave(KErrNotSupported);
		};
	}

const TUint KInet6AddrMaxBufferSize = 40;

void CIpSecKeyStreamSink::SetPolicyL()
	{
	DEBUG_PRINTF(_L("IPSec key stream sink - setting policy."));
	ASSERT(!iPolicySet);
	
	TBuf<KInet6AddrMaxBufferSize> destAddrStr;
	iDestinationAddr.Output(destAddrStr);
	
	// Convert 16-bit to 8-bit. For some strange reason, the string for IP address is returned in 16-bit.
	TBuf8<KInet6AddrMaxBufferSize> destAddrStr8bit;
	destAddrStr8bit.Copy(destAddrStr);
	
	HBufC8 *policyData = HBufC8::NewLC( KPolicyFormat().Length() + 256); // Allow size for port and IP spec.
	TPtr8 policyDataPtr(policyData->Des());
	policyDataPtr.AppendFormat(KPolicyFormat, iEncAlg, iAuthAlg, &destAddrStr8bit, iDestinationAddr.Port(), 
							   iSourceAddr.Port());	

	// Load the server-side policy to protect all packets to a specific port
	TRequestStatus status;
	iPolicyServer.LoadPolicy( *policyData, iPolicyHandle, status );
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	
	iPolicyServer.ActivatePolicy( iPolicyHandle(), status );
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());	
	
	CleanupStack::PopAndDestroy(policyData);	
	iPolicySet = ETrue;	
	DEBUG_PRINTF(_L("IPSec key stream sink - policy set."));
	}

static void ReadIpAddrFromStreamL(RReadStream& aStream, TInetAddr& addr)
	{
	TBuf<KInet6AddrMaxBufferSize> addrStr;
	TUint8 addrStrLen = aStream.ReadUint8L();
	aStream.ReadL(addrStr, addrStrLen);
	User::LeaveIfError(addr.Input(addrStr));
	TInt port = aStream.ReadInt32L();
	addr.SetPort(port);
	}

static void WriteIpAddrToStreamL(RWriteStream& aStream, const TInetAddr& addr)
	{
	TBuf<KInet6AddrMaxBufferSize> addrStr;
	addr.Output(addrStr);
	aStream.WriteUint8L(addrStr.Length());	
	aStream.WriteL(addrStr);
	aStream.WriteInt32L(addr.Port());	
	}

void CIpSecKeyStreamSink::DoExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint16L(CKeyStreamSink::EIpSecSinkId);
	WriteIpAddrToStreamL(aStream, iDestinationAddr);
	WriteIpAddrToStreamL(aStream, iSourceAddr);
	aStream.WriteUint16L(iEncAlg);
	aStream.WriteUint16L(iAuthAlg);
	}
	
CIpSecKeyStreamSink* CIpSecKeyStreamSink::NewLC(RReadStream& aReadStream)
	{
	TInetAddr destAddr;
	ReadIpAddrFromStreamL(aReadStream, destAddr);
	TInetAddr srcAddr;
	ReadIpAddrFromStreamL(aReadStream, srcAddr);	
	CIpSecKeyStreamSink* self = new (ELeave) CIpSecKeyStreamSink(srcAddr, destAddr);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iEncAlg = aReadStream.ReadUint16L();
	self->iAuthAlg = aReadStream.ReadUint16L();
	return self;
	}
