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
* MCTKeyStore.inl
* CKeyInfoBase
*
*/

/**
 @file 
 @publishedPartner
 @released
*/

inline TKeyIdentifier CKeyInfoBase::ID() const
	{
	return iID;
	}

inline TKeyUsagePKCS15 CKeyInfoBase::Usage() const
	{
	return iUsage;
	}

inline TUint CKeyInfoBase::Size() const
	{
	return iSize;
	}

inline const TDesC& CKeyInfoBase::Label() const
	{	
	if (iLabel)
		return *iLabel;
	else
		return KNullDesC;
	}

inline const TSecurityPolicy& CKeyInfoBase::UsePolicy() const
	{
	return iUsePolicy;
	}

inline const TSecurityPolicy& CKeyInfoBase::ManagementPolicy() const
	{
	return iManagementPolicy;
	}

inline CKeyInfoBase::EKeyAlgorithm CKeyInfoBase::Algorithm() const
	{
	return iAlgorithm;
	}

inline TInt CKeyInfoBase::AccessType() const
	{
	return iAccessType;
	}

inline TBool CKeyInfoBase::Native() const
	{
	return iNative;
	}

inline TTime CKeyInfoBase::StartDate() const
	{
	return iStartDate;
	}

inline TTime CKeyInfoBase::EndDate() const
	{
	return iEndDate;
	}

inline const TDesC8& CKeyInfoBase::PKCS8AttributeSet() const
	{
	if (iPKCS8AttributeSet)
		return *iPKCS8AttributeSet;
	else
		return KNullDesC8;
	}

inline TInt CKeyInfoBase::HandleID() const
	{
	return iHandle;
	}

inline void CKeyInfoBase::SetHandle(TInt aHandle)
	{
	iHandle = aHandle;
	}

inline void CKeyInfoBase::SetIdentifier(TKeyIdentifier aId)
	{
	iID = aId;
	}

inline void CKeyInfoBase::SetSize(TUint aSize)
	{
	iSize = aSize;
	}

inline void CKeyInfoBase::SetAlgorithm(EKeyAlgorithm aAlg)
	{
	iAlgorithm = aAlg;
	}

// CCTKeyInfo

inline CCTKeyInfo::operator TCTTokenObjectHandle() const
	{
	return Handle();
	}

inline MCTAuthenticationObject* CCTKeyInfo::Protector() const
	{
	return iProtector;
	}

inline void CCTKeyInfo::SetProtector(MCTAuthenticationObject& aProtector)
	{
	iProtector = &aProtector;
	}
