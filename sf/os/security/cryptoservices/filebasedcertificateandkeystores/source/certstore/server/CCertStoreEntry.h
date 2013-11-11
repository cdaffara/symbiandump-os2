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
* CCertStoreEntry class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CCERTSTOREENRY_H__
#define __CCERTSTOREENRY_H__

#include <e32base.h>
#include <s32std.h>

class CCertInfo;

/**
 * This class represents a cert in the store.  It contains the cert info, the id
 * of the stream containing the certificate itself and its applications and
 * trust settings.
 */
class CCertStoreEntry : public CBase
	{
public:
	/// Create a new CCertStoreEntry
	static CCertStoreEntry* NewL(const CCertInfo& aEntry,
								 RArray<TUid> aCertificateApps,
								 TBool aTrusted,
								 TStreamId aDataStreamId);

	/// Read a CCertStoreEntry from a stream.
	static CCertStoreEntry* NewLC(RReadStream& aStream);
	
	virtual ~CCertStoreEntry();

	/// Write a CCertStoreEntry to a stream.
	void ExternalizeL(RWriteStream& aDataStream) const;

	// Accessors
	const CCertInfo& CertInfo() const;
	TStreamId DataStreamId() const;
	const RArray<TUid>& CertificateApps() const;
	TBool IsApplicable(const TUid& aApplication) const;
	TBool Trusted() const;

private:
	CCertStoreEntry();
	void ConstructL(const CCertInfo& aEntry, RArray<TUid> aCertificateApps,
					TBool aTrusted, TStreamId aDataStreamId);
	void InternalizeL(RReadStream& aStream);

private:
	CCertInfo* iCertInfo;
	RArray<TUid> iCertificateApps;
	TBool iTrusted;
	TStreamId iDataStreamId;
	};

#endif
