/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Data types used by the filetokens server
*
*/


inline CKeyInfo::CKeyInfo() :
	CKeyInfoBase()
	{
	}

inline CKeyInfo::~CKeyInfo()
	{
#ifdef SYMBIAN_AUTH_SERVER
	delete iAuthExpression;
#endif // SYMBIAN_AUTH_SERVER
	}

inline void CKeyInfo::CleanupPushL()
	{
	CleanupStack::PushL(this);
	}

inline TInt CKeyInfo::Handle() const
	{
	return iHandle;
	}

inline void CKeyInfo::SetAccessType(TInt aAccessType)
	{
	iAccessType = aAccessType;
	}

#ifdef SYMBIAN_AUTH_SERVER
inline AuthServer::TIdentityId CKeyInfo::Identity() const
	{
	return iIdentityId;
	}

inline const TDesC& CKeyInfo::AuthExpression() const
	{
	return *iAuthExpression;
	}

inline TInt CKeyInfo::Freshness() const
	{
	return iFreshness;
	}

inline void CKeyInfo::SetIdentity(AuthServer::TIdentityId aIdentityId)
	{
	iIdentityId = aIdentityId;
	}

inline void CKeyInfo::SetAuthExpressionL(const TDesC& aAuthExpression)
	{
	delete iAuthExpression;
	iAuthExpression = NULL;
	iAuthExpression = aAuthExpression.AllocL();
	iAuthExpression->Des().Copy(aAuthExpression);
	}

inline void CKeyInfo::SetFreshness(TInt aFreshness)
	{
	iFreshness = aFreshness;
	}

inline void CKeyInfo::ResetAuthExpression()
	{
	delete iAuthExpression;
	iAuthExpression = NULL;
	}

#endif // SYMBIAN_AUTH_SERVER


inline const TInteger& CDHParams::N() const
	{
	return iN;
	}

inline const TInteger& CDHParams::G() const
	{
	return iG;
	}

inline const TDesC& CCertInfo::Label() const
	{
	return iLabel;
	}

inline TInt CCertInfo::Reference() const
	{
	return iCertificateId;
	}

inline const TKeyIdentifier& CCertInfo::SubjectKeyId() const
	{
	return iSubjectKeyId;
	}

inline const TKeyIdentifier& CCertInfo::IssuerKeyId() const
	{
	return iIssuerKeyId;
	}

inline TCertificateFormat CCertInfo::CertificateFormat() const
	{
	return iFormat;
	}

inline TCertificateOwnerType CCertInfo::CertificateOwnerType() const
	{
	return iCertificateOwnerType;
	}

inline TInt CCertInfo::Size() const
	{
	return iSize;
	}

inline TInt CCertInfo::CertificateId() const
	{
	return iCertificateId;
	}

inline TBool CCertInfo::IsDeletable() const
	{
	return iDeletable;
	}
