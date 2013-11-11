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
* FILECERTSTOREMAPPING.H
* CFileCertStoreMapping class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __FILECERTSTOREMAPPING_H__
#define __FILECERTSTOREMAPPING_H__

#include <e32base.h>
#include <s32std.h>
#include <ct.h>
#include <mctcertstore.h>

/**
 * This class is used to associate an entry with the TStreamId of the stream
 * where the data of the certificate is stored.
 */
class CFileCertStoreMapping : public CBase
	{
public:
	static CFileCertStoreMapping* NewL();
	static CFileCertStoreMapping* NewLC();
	void ConstructL();
	CFileCertStoreMapping();
	~CFileCertStoreMapping();
	/**
	 * Sets the iEntry member. If there is already an entry, it is deleted.
	 */
	void SetEntry(CCTCertInfo* aCertInfo);
	void SetId(TStreamId aId);
	CCTCertInfo* Entry() const;
	TStreamId Id() const;
	void ExternalizeL(RWriteStream& aStream) const;
	const RArray<TUid>& CertificateApps() const;
	void SetCertificateApps(RArray<TUid>* aCertificateApps);
	TBool IsApplicable(const TUid& aApplication) const;
	TBool Trusted() const;
	void SetTrusted(TBool aTrusted);
	void SetTempRemoved(TBool aFlag);
	TBool IsTempRemoved();

private:
	CCTCertInfo* iEntry;
	RArray<TUid>* iCertificateApps;
	TBool iTrusted;
	/**
	 * The id of the stream where the certificate data is stored.
	 */
	TStreamId iId;
	TBool iTempRemoved;
	};

#endif
