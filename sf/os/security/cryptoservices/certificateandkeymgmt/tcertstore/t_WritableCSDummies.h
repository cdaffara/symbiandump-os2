/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/
 
#ifndef __WRITABLECSDUMMIES_H__
#define __WRITABLECSDUMMIES_H__


#include <e32std.h>
#include <ct.h>
#include <mctwritablecertstore.h>


// this is a helper dummy class just not to 
// implement all the interfaces all the time
// later we derive two child classes
// TNewCSClasswDeletable and TOldCSClasswoDeletable
class TWritableCertStoreDummy : public MCTWritableCertStore
	{
public:	
	TWritableCertStoreDummy() : iCertStore(NULL) {};

// from MCTTokenInterface
	virtual MCTToken& Token() { return iCertStore->Token(); };

// from MCTCertStore
	virtual void List(  RMPointerArray<CCTCertInfo>& /*aCerts*/,
						const CCertAttributeFilter& /*aFilter*/,
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelList() {};
	virtual void GetCert(CCTCertInfo*& /*aCertInfo*/, 
						const TCTTokenObjectHandle& /*aHandle*/, 
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelGetCert() {};
	virtual void Applications(const CCTCertInfo& /*aCertInfo*/, 
						RArray<TUid>& /*aApplications*/,
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelApplications() {};
	virtual void IsApplicable(const CCTCertInfo& /*aCertInfo*/, 
						TUid /*aApplication*/, 
						TBool& /*aIsApplicable*/,
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelIsApplicable() {};
	virtual void Trusted(const CCTCertInfo& /*aCertInfo*/,
						TBool& /*aTrusted*/, 
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelTrusted() {};
	virtual void Retrieve(const CCTCertInfo& /*aCertInfo*/, 
						TDes8& /*aEncodedCert*/, 
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelRetrieve() {};

// from MCTWritableCertStore
	// old Add() w/o aDeletable
	virtual void Add( const TDesC& /*aLabel*/,
						TCertificateFormat /*aFormat*/,
						TCertificateOwnerType /*aCertificateOwnerType*/, 
						const TKeyIdentifier* /*aSubjectKeyId*/,
						const TKeyIdentifier* /*aIssuerKeyId*/,
						const TDesC8& /*aCert*/, 
						TRequestStatus& /*aStatus*/) {};

//	here goes new Add(.., const TBool aDeletable, ..) with a deletable param
// 	this one DOES HAVE a default implementation in MCTWritableCertStore
// 	but not every dummy CS class has its own implementation of this
	virtual void Add(const TDesC& aLabel,
					 TCertificateFormat aFormat,
					 TCertificateOwnerType aCertificateOwnerType, 
					 const TKeyIdentifier* aSubjectKeyId,
					 const TKeyIdentifier* aIssuerKeyId,
					 const TDesC8& aCert, 
					 const TBool aDeletable,
					 TRequestStatus& aStatus ) 
		{ 
		MCTWritableCertStore::Add( aLabel, aFormat, aCertificateOwnerType, 
						aSubjectKeyId, aIssuerKeyId, aCert, aDeletable, aStatus );
		}


	virtual void CancelAdd() {};

	virtual void Remove(const CCTCertInfo& /*aCertInfo*/, 
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelRemove() {};


/*  
	SETAPPLICABILITY()  - MCTWritableCertStore v.1

1)	virtual void SetApplicability(const CCTCertInfo& aCertInfo, 
							   RArray<TUid>* aApplications, TRequestStatus &aStatus) = 0;

2)	IMPORT_C void SetApplicability(const CCTCertInfo& aCertInfo, 
						  const RArray<TUid>& aApplications, TRequestStatus &aStatus);


	SETAPPLICABILITY  - MCTWritableCertStore v.2

1)	virtual void SetApplicability(const CCTCertInfo& aCertInfo, 
						 const RArray<TUid>& aApplications, TRequestStatus &aStatus) = 0;


	Notice what's done migrating v.1 -> v.2:
	1) first method (RArray<TUid>*) is removed, and
	2) the 2nd method (const RArray<TUid>&) is made purevirtual
*/

	virtual void SetApplicability(const CCTCertInfo& /*aCertInfo*/, 
								  RArray<TUid>* /*aApplications*/, 
								  TRequestStatus& /*aStatus*/) {};

	virtual void SetApplicability(const CCTCertInfo& /*aCertInfo*/, 
						 		  const RArray<TUid>& /*aApplications*/, 
						 		  TRequestStatus& /*aStatus*/) {};
		
		
	virtual void CancelSetApplicability() {};
	virtual void SetTrust(const CCTCertInfo& /*aCertInfo*/,
						TBool /*aTrusted*/, 
						TRequestStatus& /*aStatus*/) {};
	virtual void CancelSetTrust() {};


private:
	MCTCertStore *iCertStore;

	};		// class TWritableCertStoreDummy
	


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . TOldCSClasswoDeletable

class TOldCSClasswoDeletable : public TWritableCertStoreDummy
	{

public:
	TOldCSClasswoDeletable()	{};

//	doesn't know about new Add(.., const TBool aDeletable, ..) being available
// 	thus, uses grandparent's default implementation (returns KErrNotSupported)

	};		// class TOldCSClasswoDeletable



// . . . . . . . . . . . . . . . . . . . . . . . . . . . . TNewCSClasswDeletable

class TNewCSClasswDeletable : public TWritableCertStoreDummy
	{

public:
	TNewCSClasswDeletable()	{};
	
	// old Add(..) w/o aDeletable
	virtual void Add(const TDesC& aLabel,
					 TCertificateFormat aFormat,
					 TCertificateOwnerType aCertificateOwnerType, 
					 const TKeyIdentifier* aSubjectKeyId,
					 const TKeyIdentifier* aIssuerKeyId,
					 const TDesC8& aCert, 
					 TRequestStatus& aStatus ) 
		{ 
		TWritableCertStoreDummy::Add( aLabel, aFormat, aCertificateOwnerType, 
						aSubjectKeyId, aIssuerKeyId, aCert, aStatus );
		}

	// new Add(.., TBool aDeletable, ..) method
	virtual void Add( const TDesC& aLabel, 
						TCertificateFormat aFormat,
						TCertificateOwnerType aCertificateOwnerType, 
						const TKeyIdentifier* aSubjectKeyId,
						const TKeyIdentifier* aIssuerKeyId,
						const TDesC8& aCert, 
						const TBool aDeletable,
						TRequestStatus& aStatus);

	};		// class TNewCSClasswDeletable


// new Add(.., TBool aDeletable, ..) implementation
void TNewCSClasswDeletable::Add( const TDesC& /*aLabel*/, 
							 TCertificateFormat /*aFormat*/,
                             TCertificateOwnerType /*aCertificateOwnerType*/, 
                             const TKeyIdentifier* /*aSubjectKeyId*/,
                             const TKeyIdentifier* /*aIssuerKeyId*/,
                             const TDesC8& /*aCert*/, 
                             const TBool /*aDeletable*/,
                             TRequestStatus& aStatus
                             )
    {
	TRequestStatus* status = &aStatus;	
	User::RequestComplete( status, KErrNone );
    }

#endif	//	__WRITABLECSDUMMIES_H__
