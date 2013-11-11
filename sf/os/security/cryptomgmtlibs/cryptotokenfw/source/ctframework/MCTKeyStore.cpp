/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <ct.h>
#include "mctauthobject.h"
#include "mctkeystore.h"
#include "mctkeystoremanager.h"
#include <securityerr.h>
#include "ct/logger.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS

/** The type ID of CCTKeyInfo objects */
const TUid KKeyInfoUID = {0x101F5152};

#endif
////////////////////////////////////////////////////////////////////////////////
// CKeyInfoBase
////////////////////////////////////////////////////////////////////////////////

EXPORT_C CKeyInfoBase::CKeyInfoBase(TKeyIdentifier aID,
									TKeyUsagePKCS15 aUsage,
									TUint aSize, 
									HBufC* aLabel,
									TInt aHandle,
									const TSecurityPolicy& aUsePolicy,
									const TSecurityPolicy& aManagementPolicy,
									EKeyAlgorithm aAlgorithm,
									TInt aAccessType,
									TBool aNative,
									TTime aStartDate,
									TTime aEndDate,
									HBufC8* aPKCS8AttributeSet) :
	iID(aID),
	iUsage(aUsage),
	iSize(aSize), 
	iLabel(aLabel),
	iHandle(aHandle),
	iUsePolicy(aUsePolicy),
	iManagementPolicy(aManagementPolicy),
	iAlgorithm(aAlgorithm),
	iAccessType(aAccessType),
	iNative(aNative),
	iStartDate(aStartDate), 
	iEndDate(aEndDate),
	iPKCS8AttributeSet(aPKCS8AttributeSet)
	{
	}

EXPORT_C CKeyInfoBase::CKeyInfoBase()
	{
	}

EXPORT_C CKeyInfoBase::~CKeyInfoBase()
	{
	delete iLabel;
	delete iPKCS8AttributeSet;
	}

EXPORT_C void CKeyInfoBase::ConstructL()
	{
	}

EXPORT_C void CKeyInfoBase::ConstructL(RReadStream& aIn)
	{
	// Called from derived classes' NewL methods when interalizing a stream

	InternalizeL(aIn);
	}

EXPORT_C void CKeyInfoBase::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteL(iID);
	aStream.WriteInt32L(iUsage);
	aStream.WriteUint32L(iSize);
  	aStream.WriteInt32L(iLabel->Length());
  	TPtr16 theLabel(iLabel->Des());
  	aStream.WriteL(theLabel);
	aStream.WriteInt32L(iHandle);
	aStream.WriteL(TPckgC<TSecurityPolicy>(iUsePolicy));
	aStream.WriteL(TPckgC<TSecurityPolicy>(iManagementPolicy));		
	aStream.WriteInt32L(iAlgorithm);
	aStream.WriteInt32L(iAccessType);
	aStream.WriteInt32L(iNative);
	aStream.WriteL(TPckgC<TTime>(iStartDate));
	aStream.WriteL(TPckgC<TTime>(iEndDate));

	if (iPKCS8AttributeSet)
		{
		aStream.WriteInt32L(iPKCS8AttributeSet->Length());
		TPtr8 theAttributes(iPKCS8AttributeSet->Des());
		aStream.WriteL(theAttributes);
		}
	else
		aStream.WriteInt32L(0);
	}

void CKeyInfoBase::InternalizeL(RReadStream& aStream)
	{
	aStream.ReadL(iID);
	iUsage = static_cast<TKeyUsagePKCS15>(aStream.ReadInt32L());
	iSize = aStream.ReadUint32L();
	
	TInt labelLen = aStream.ReadInt32L();
	iLabel = HBufC::NewMaxL(labelLen);
	TPtr pLabel(iLabel->Des());
	pLabel.FillZ();
	// This will have made the length of pLabel equal to the length of
	// the alloc cell of iLabel, which may be longer than labelLen. So
	// we need to pass the length we want to read into ReadL
	aStream.ReadL(pLabel, labelLen);
	
	iHandle = aStream.ReadInt32L();

	TPckg<TSecurityPolicy> usePolicy(iUsePolicy);
	aStream.ReadL(usePolicy);
	TPckg<TSecurityPolicy> managementPolicy(iManagementPolicy);
	aStream.ReadL(managementPolicy);
		
	iAlgorithm = (EKeyAlgorithm)(aStream.ReadInt32L());
	iAccessType = (EKeyAccess)(aStream.ReadInt32L());
	iNative = (TBool)(aStream.ReadInt32L());
	TPckg<TTime> startDate(iStartDate);
	aStream.ReadL(startDate);
	TPckg<TTime> endDate(iEndDate);
	aStream.ReadL(endDate);

	TInt attributeLen = aStream.ReadInt32L();
	if (attributeLen > 0)
		{
		iPKCS8AttributeSet = HBufC8::NewMaxL(attributeLen);
		TPtr8 pAttributes(iPKCS8AttributeSet->Des());
		pAttributes.FillZ();
		//	This will have made the length of pAttributes equal to the length of
		//	the alloc cell of iPKCS8AttributeSet, which may be longer than attributeLen
		//	So we need to pass the length we want to read into ReadL
		aStream.ReadL(pAttributes, attributeLen);
		}
	}

////////////////////////////////////////////////////////////////////////////////
// CCTKeyInfo
////////////////////////////////////////////////////////////////////////////////

EXPORT_C CCTKeyInfo* CCTKeyInfo::NewL(TKeyIdentifier aID,
									  TKeyUsagePKCS15 aUsage, 
									  TUint aSize,
									  MCTAuthenticationObject* aProtector,
									  HBufC* aLabel,
									  MCTToken& aToken,
									  TInt aHandle,
									  const TSecurityPolicy& aUsePolicy,
									  const TSecurityPolicy& aManagementPolicy,
									  EKeyAlgorithm aAlgorithm,
									  TInt aAccessType,
									  TBool aNative,
									  TTime aStartDate,
									  TTime aEndDate,
									  HBufC8* aPKCS8AttributeSet /*= NULL*/)
	{
	CCTKeyInfo* me = NULL;
	me = new (ELeave) CCTKeyInfo(aID,
								 aUsage,
								 aSize,
								 aProtector,
								 aLabel,
								 aToken,
								 aHandle,
								 aUsePolicy,
								 aManagementPolicy,
								 aAlgorithm,
								 aAccessType,
								 aNative,
								 aStartDate,
								 aEndDate,
								 aPKCS8AttributeSet);

	CleanupReleasePushL(*me);
	me->ConstructL();
	CleanupStack::Pop();
	
	return me;
	}

CCTKeyInfo::CCTKeyInfo(TKeyIdentifier aID,
					   TKeyUsagePKCS15 aUsage,
					   TUint aSize, 
					   MCTAuthenticationObject* aProtector,
					   HBufC* aLabel,
					   MCTToken& aToken,
					   TInt aHandle,
					   const TSecurityPolicy& aUsePolicy,
					   const TSecurityPolicy& aManagementPolicy,
					   EKeyAlgorithm aAlgorithm,
					   TInt aAccessType,
					   TBool aNative, 
					   TTime aStartDate,
					   TTime aEndDate,
					   HBufC8* aPKCS8AttributeSet /*= NULL*/) :
	CKeyInfoBase(aID,
				 aUsage,
				 aSize,
				 aLabel,
				 aHandle,
				 aUsePolicy,
				 aManagementPolicy,
				 aAlgorithm,
				 aAccessType,
				 aNative,
				 aStartDate,
				 aEndDate,
				 aPKCS8AttributeSet),
	MCTTokenObject(aToken),
	iToken(aToken),
	iProtector(aProtector)
	{
	LOG1(_L("CCTKeyInfo::CCTKeyInfo() with iProtector %08x"), iProtector);
	}

EXPORT_C CCTKeyInfo* CCTKeyInfo::NewL(RReadStream& aStream, MCTToken& aToken)
	{
	CCTKeyInfo* me = new (ELeave) CCTKeyInfo(aToken);
	CleanupReleasePushL(*me);
	me->ConstructL(aStream);
	CleanupStack::Pop();
	return me;
	}

CCTKeyInfo::CCTKeyInfo(MCTToken& aToken) :
	CKeyInfoBase(),
	MCTTokenObject(aToken),
	iToken(aToken)
	{
	}

CCTKeyInfo::~CCTKeyInfo()
	{
	LOG(_L("CCTKeyInfo::~CCTKeyInfo"));
	if (iProtector)
		{
		LOG_INC_INDENT();
		iProtector->Release();
		LOG_DEC_INDENT();
		}
	}

const TDesC& CCTKeyInfo::Label() const
	{
	return CKeyInfoBase::Label();
	}

MCTToken& CCTKeyInfo::Token() const
	{
	return iToken;
	}

TUid CCTKeyInfo::Type() const
	{
	return KKeyInfoUID;
	}

TCTTokenObjectHandle CCTKeyInfo::Handle() const
	{
	return TCTTokenObjectHandle(Token().Handle(), HandleID());
	}

////////////////////////////////////////////////////////////////////////////////
// TCTKeyAttributeFilter
////////////////////////////////////////////////////////////////////////////////

EXPORT_C TCTKeyAttributeFilter::TCTKeyAttributeFilter()
	{
	iKeyId = KNullDesC8;
	iUsage = EPKCS15UsageAll;
	iPolicyFilter = EUsableKeys;
	iKeyAlgorithm = CCTKeyInfo::EInvalidAlgorithm;
	}
