/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "CCertStoreConduit.h"
#include "filecertstore.h"
#include "fsmarshaller.h"
#include "fsserver.h"
#include "fstokenutil.h"
#include "fsdatatypes.h"

CCertStoreConduit* CCertStoreConduit::NewL(CFSCertStoreServer& aServer)
	{
	return new (ELeave) CCertStoreConduit(aServer);
	}

CCertStoreConduit::CCertStoreConduit(CFSCertStoreServer& aServer) :
	iServer(aServer)
	{
	}

CCertStoreConduit::~CCertStoreConduit()
	{
	}

/**
 * Allocate a buffer for the externalised response - this is then copied into
 * the client's buffer.
 *
 * This also checks that the client's buffer is big enough (this is assumed to
 * be in message slot 3).  If the client buffer isn't big enough, the required
 * length is passed back in the first word of the buffer and the method leaves
 * with KErrOverflow.
 */
HBufC8* CCertStoreConduit::AllocResponseBufferLC(TInt aSize, const RMessage2& aMessage)
	{
	TInt writeBufLen = aMessage.GetDesLengthL(3);

	if (aSize > writeBufLen)
		{
		TPckg<TInt> theRequiredLength(aSize);
		aMessage.WriteL(3, theRequiredLength);
		User::Leave(KErrOverflow);
		}
	
	HBufC8* result = HBufC8::NewMaxLC(aSize);
	TPtr8 ptr = result->Des();
	ptr.FillZ();

	return result;
	}

void CCertStoreConduit::ServiceCertStoreRequestL(const RMessage2& aMessage)
	{
	TFSTokenMessages request = (TFSTokenMessages) aMessage.Function();
	TInt result = KErrNone;

	switch (request)
		{
		case EListCerts:
			ListCertsL(aMessage);		
			break;
			
		case EGetCert:
			GetCertL(aMessage);			
			break;
			
		case EApplications:
			ApplicationsL(aMessage);	
			break;
			
		case EIsApplicable:
			result = IsApplicableL(aMessage);	
			break;
			
		case ETrusted:
			result = TrustedL(aMessage);			
			break;
			
		case ERetrieve:
			RetrieveL(aMessage);		
			break;
			
		case EAddCert:
			AddCertL(aMessage);			
			break;
			
		case ERemoveCert:
			RemoveCertL(aMessage);		
			break;
			
		case ESetApplicability:
			SetApplicabilityL(aMessage);
			break;
			
		case ESetTrust:
			SetTrustL(aMessage);
			break;
			
		default:
			// Client made an illegal request
			PanicClient(aMessage, EPanicInvalidRequest);
			return;
		}
	
	aMessage.Complete(result);
	}

void CCertStoreConduit::ListCertsL(const RMessage2& aMessage)
	{
	// p[1] has filter as set by the client

	TInt bufLength = aMessage.GetDesLengthL(1);
	HBufC8* certFilterBuf = HBufC8::NewMaxLC(bufLength);
	TPtr8 ptr(certFilterBuf->Des());	
	aMessage.ReadL(1, ptr);
	CCertAttributeFilter* certFilter = NULL;
	TokenDataMarshaller::ReadL(*certFilterBuf, certFilter);
	CleanupStack::PopAndDestroy(certFilterBuf);
	CleanupStack::PushL(certFilter);
	
	RPointerArray<CCertInfo> certs;
	CleanupClosePushL(certs);
	iServer.ListL(*certFilter, certs);

	HBufC8* clientBuffer = AllocResponseBufferLC(TokenDataMarshaller::Size(certs), aMessage);	
	TPtr8 theData(clientBuffer->Des());
	TokenDataMarshaller::Write(certs, theData);
	aMessage.WriteL(3, theData);
	CleanupStack::PopAndDestroy(3, certFilter);
	}

void CCertStoreConduit::GetCertL(const RMessage2& aMessage)
	{
	// p[0] is the cert handle
	
	const CCertInfo& certInfo = iServer.GetCertL(aMessage.Int0());

	HBufC8* clientBuffer = AllocResponseBufferLC(TokenDataMarshaller::Size(certInfo), aMessage);	
	TPtr8 ptr(clientBuffer->Des());
	TokenDataMarshaller::Write(certInfo, ptr);
	aMessage.WriteL(3, ptr);
	CleanupStack::PopAndDestroy(clientBuffer);
	}

void CCertStoreConduit::ApplicationsL(const RMessage2& aMessage)
	{
	// p[0] is the cert handle
	
	const RArray<TUid>& apps = iServer.ApplicationsL(aMessage.Int0());
	
	HBufC8* clientBuffer = AllocResponseBufferLC(TokenDataMarshaller::Size(apps), aMessage);	
	TPtr8 outPtr(clientBuffer->Des());
	TokenDataMarshaller::Write(apps, outPtr);
	aMessage.WriteL(3, outPtr);
	CleanupStack::PopAndDestroy(clientBuffer);
	}

TInt CCertStoreConduit::IsApplicableL(const RMessage2& aMessage)
	{
	// p[0] is the cert handle
	// p[2] is the UID
	// p[3] to return the applicability bool
	
	TUid uid = { aMessage.Int2() };
	TBool result = iServer.IsApplicableL(aMessage.Int0(), uid);
	return result ? 1 : 0;  // Convert TBool to TInt
	}

TInt CCertStoreConduit::TrustedL(const RMessage2& aMessage)
	{
	//	p[1] has CCertInfo in question, p[2] to return the trusted bool
	
	TBool result = iServer.TrustedL(aMessage.Int0());	
	return result ? 1 : 0;  // Convert TBool to TInt
	}

void CCertStoreConduit::RetrieveL(const RMessage2& aMessage)
	{
	// p[0] is the cert handle
	// p[3] the buffer to return the retrieved certificate

	HBufC8* certData = iServer.RetrieveLC(aMessage.Int0(), aMessage);
	
	TInt maxSize = aMessage.GetDesLengthL(3);
	if (maxSize < certData->Size())
		{
		//	Client has passed in a descriptor which is too short
		User::Leave(KErrOverflow);
		}

	aMessage.WriteL(3, *certData);
	CleanupStack::PopAndDestroy(certData);
	}

void CCertStoreConduit::AddCertL(const RMessage2& aMessage)
	{
	//	p[1] has TAddCertDataStruct, p[3] has certificate data
	TPckgBuf<TAddCertDataStruct> data;
	aMessage.ReadL(1, data);

	TInt maxSize = aMessage.GetDesLengthL(3);
	HBufC8* certData = HBufC8::NewMaxLC(maxSize);
	TPtr8 ptr(certData->Des());
	ptr.FillZ();
	aMessage.ReadL(3, ptr);

	iServer.AddL(data(), *certData, aMessage);

	CleanupStack::PopAndDestroy(certData);
	}

void CCertStoreConduit::RemoveCertL(const RMessage2& aMessage)
	{
	// p[0] is the cert handle
	
	iServer.RemoveL(aMessage.Int0(), aMessage);
	}

void CCertStoreConduit::SetApplicabilityL(const RMessage2& aMessage)
	{
	// p[0] is the cert handle
	// p[2] the array of UIDs

	TInt bufLen = aMessage.GetDesLengthL(2);
	HBufC8* buf = HBufC8::NewLC(bufLen);
	TPtr8 bufPtr(buf->Des());
	aMessage.ReadL(2, bufPtr);

	RArray<TUid> apps;
	CleanupClosePushL(apps);
	TokenDataMarshaller::ReadL(*buf, apps);
		
	iServer.SetApplicabilityL(aMessage.Int0(), apps, aMessage);
	CleanupStack::PopAndDestroy(2, buf);	
	}

void CCertStoreConduit::SetTrustL(const RMessage2& aMessage)
	{
	// p[0] is the cert handle
	// p[2] the trust setting

	TBool trusted = !!aMessage.Int2();		
	iServer.SetTrustL(aMessage.Int0(), trusted, aMessage);
	}
