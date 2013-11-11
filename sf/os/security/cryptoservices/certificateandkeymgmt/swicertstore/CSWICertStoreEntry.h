/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CSWICertStoreEntry class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSWICERTSTOREENTRY_H__
#define __CSWICERTSTOREENTRY_H__

#include <e32base.h>
#include <s32std.h>
#include <swicertstore.h>

class MCTToken;
class CCTCertInfo;

/**
 * This class is used to associate an entry with the TStreamId of the stream
 * where the data of the certificate is stored.
 */
NONSHARABLE_CLASS(CSWICertStoreEntry) : public CBase
	{
public:
	static CSWICertStoreEntry* NewL(const CCTCertInfo& aCertInfo,const RArray<TUid>& aCertificateApps,TBool aTrusted,TStreamId aDataStreamId,const TCertMetaInfo& aCertMetaInfo,TInt aStoreIndex);

	/// Read a CSWICertStoreEntry from a stream.
	static CSWICertStoreEntry* NewLC(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex, TInt aStoreIndex = 0);

	virtual ~CSWICertStoreEntry();

	// Accessors
	const CCTCertInfo& CertInfo() const;
	TStreamId DataStreamId() const;
	const RArray<TUid>& CertificateApps() const;
	TBool IsApplicable(const TUid& aApplication) const;
	TBool Trusted() const;
	const TCertMetaInfo& CertMetaInfo() const;
	//returns the store index to which the entry belongs
	TInt StoreIndex() const;
	
private:
	CSWICertStoreEntry(TInt aStoreIndex = 0);
	void ConstructL(const CCTCertInfo& aCertInfo,const RArray<TUid>& aCertificateApps,TBool aTrusted,TStreamId aDataStreamId,const TCertMetaInfo& aCertMetaInfo);
	void InternalizeL(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex);

private:
	CCTCertInfo* iCertInfo;
	RArray<TUid> iCertificateApps;
	TBool iTrusted;
	TStreamId iDataStreamId;
	TCertMetaInfo iCertMetaInfo;
	//represents the store in which the certificate entry resides
	TInt iStoreIndex;
	};

#endif
