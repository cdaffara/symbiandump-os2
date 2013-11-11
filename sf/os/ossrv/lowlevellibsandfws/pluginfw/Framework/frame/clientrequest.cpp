// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
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

//system includes
#include <e32std.h>
#include <u32std.h>

//class signature
#include "clientrequest.h"

#ifndef __ECOMSERVER_TESTING__

//
// TClientRequest
//
TClientRequest::TClientRequest(const RMessage2& aMessage)
: iMessage(aMessage), iSecurityInfo(aMessage)
	{
	}
	
TBool TClientRequest::IsNull() const
	{
	return iMessage.IsNull();
	}

TInt TClientRequest::Function() const
	{
	return iMessage.Function();
	}

void TClientRequest::Complete(TInt aReason) const
	{
	iMessage.Complete(aReason);
	}

void TClientRequest::Panic(const TDesC& aCategory, TInt aReason) const
	{
	iMessage.Panic(aCategory, aReason);
	}

TInt TClientRequest::Int0() const
	{
	return iMessage.Int0();
	}

TInt TClientRequest::Int1() const
	{
	return iMessage.Int1();
	}

TInt TClientRequest::Int2() const
	{
	return iMessage.Int2();
	}

TInt TClientRequest::Int3() const
	{
	return iMessage.Int3();
	}

TInt TClientRequest::GetDesLength(TInt aParam) const
	{
	return iMessage.GetDesLength(aParam);
	}

TInt TClientRequest::GetDesMaxLength(TInt aParam) const
	{
	return iMessage.GetDesMaxLength(aParam);
	}

void TClientRequest::ReadL(TInt aParam, TDes8& aDes, TInt aOffset) const
	{
	iMessage.ReadL(aParam, aDes, aOffset);
	}

void TClientRequest::ReadL(TInt aParam, TDes& aDes, TInt aOffset) const
	{
	iMessage.ReadL(aParam, aDes, aOffset);
	}

TInt TClientRequest::Write(TInt aParam, const TDesC8& aDes, TInt aOffset) const
	{
	return iMessage.Write(aParam, aDes, aOffset);
	}

void TClientRequest::WriteL(TInt aParam, const TDesC8& aDes) const
	{
	iMessage.WriteL(aParam, aDes); 
	}

void TClientRequest::WriteL(TInt aParam, const TDesC& aDes, TInt aOffset) const
	{
	iMessage.WriteL(aParam, aDes, aOffset);
	}
	
//
// TCapabilitySet is the low level API used by all other capability APIs.
// It relies on the caller to observe the PlatSecEnforcement settings.
//
TBool TClientRequest::HasCapabilities(const TCapabilitySet& aCapabilities) const
	{
	return aCapabilities.HasCapabilities(iSecurityInfo.iCaps);
	}

#ifndef __REMOVE_PLATSEC_DIAGNOSTICS__
//
// This method relies on PlatSec::CapabilityCheckFail
// to observe the PlatSecDiagnostics flag.
//
void TClientRequest::EmitDiagnostic(const TCapabilitySet& aCapabilities, const CImplementationInformation& aImplInfo) const
	{
	TCapabilitySet capsMissing(iSecurityInfo.iCaps);
	capsMissing.Remove(aCapabilities);
	_LIT8(KErrorFormat, "ECOM: Client (%x) may not access plugin implementation (%x_v%d)");
	TBuf8<128> errorMsg;
	errorMsg.Format(KErrorFormat, iSecurityInfo.iSecureId.iId, aImplInfo.ImplementationUid(), aImplInfo.Version());
	errorMsg.ZeroTerminate();
	PlatSec::CapabilityCheckFail(
		iMessage,
		capsMissing,
		__PLATSEC_DIAGNOSTIC_STRING(reinterpret_cast<const char*>(errorMsg.Ptr())));
	}
#endif

//
// This method serves three purposes:
// 1. Perform capability check.
// 2. Emit diagnostic message (if capability check failed and __REMOVE_PLATSEC_DIAGNOSTICS__ is *not* defined.)
// 3. Observe the PlatSecEnforcement flag and return if capability check has failed.
//
// If enforcement is not enabled then the caller should proceed as though the original capability check in fact passed.
// If enforcement is enabled then the appropriate action for a failed capability check should happen.
//
// Two possible return values:
// 1. ETrue  - if capability check passed OR PlatSecEnforcement is Off.
// 2. EFalse - if capability check failed AND PlatSecEnforcement is On.
//
TBool TClientRequest::CheckCapability(const TCapabilitySet& aCapabilities, const CImplementationInformation&
#ifndef __REMOVE_PLATSEC_DIAGNOSTICS__
	aImplInfo
#endif
	) const
	{
	TBool hasCapabilities = HasCapabilities(aCapabilities);
#ifndef __REMOVE_PLATSEC_DIAGNOSTICS__
	if (!hasCapabilities) 
		EmitDiagnostic(aCapabilities, aImplInfo);
#endif
	return !hasCapabilities && PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) ? EFalse : ETrue;
	}
//
//
#else //IF _UNIT_TESTING_

TClientRequest::TClientRequest()
: iFunction(KMinTInt), iCapability(0U), iCompletion(KRequestPending),
iStatusActive(EFalse), iIdentity(KNullUid)
	{
	Mem::FillZ(&iParams, sizeof(TInt) * KMaxMessageArguments);
	}

TBool TClientRequest::IsNull() const
	{
	return iFunction == KMinTInt;
	}

TInt TClientRequest::Function() const
	{
	return iFunction;
	}

TUid TClientRequest::Identity() const
	{
	return iIdentity;
	}

void TClientRequest::Complete(TInt aReason) const
	{
	iCompletion = aReason;
	if(iStatusActive)
		{
		TRequestStatus* stat = iStatus;
		User::RequestComplete(stat, aReason);
		}
	}

void TClientRequest::Panic(const TDesC& aCategory, TInt aReason) const
	{
	User::Panic(aCategory, aReason);
	}

TInt TClientRequest::Int0() const
	{
	return iParams[0];
	}

TInt TClientRequest::Int1() const
	{
	return iParams[1];
	}

TInt TClientRequest::Int2() const
	{
	return iParams[2];
	}

TInt TClientRequest::Int3() const
	{
	return iParams[3];
	}

TInt TClientRequest::GetDesLength(TInt aParam) const
	{
	const TDesC8* desPtr = (const TDesC8*)iParams[aParam];
	return desPtr->Length();
	}

TInt TClientRequest::GetDesMaxLength(TInt aParam) const
	{
	const TDes8* desPtr = (const TDes8*)iParams[aParam];
	return desPtr->MaxLength();
	}

void TClientRequest::ReadL(TInt aParam, TDes8& aDes, TInt aOffset) const
	{
	const TDesC8* desPtr = (const TDesC8*)iParams[aParam];
	aDes.Copy(desPtr->Mid(aOffset));
	}

void TClientRequest::ReadL(TInt aParam, TDes& aDes, TInt aOffset) const
	{
	const TDesC* desPtr = (const TDesC*)iParams[aParam];
	aDes.Copy(desPtr->Mid(aOffset));
	}

TInt TClientRequest::Write(TInt aParam, const TDesC8& aDes, TInt aOffset) const
	{
	TDes8* desPtr = (TDes8*)iParams[aParam];
	desPtr->Copy(aDes.Mid(aOffset));
	return KErrNone;
	}

void TClientRequest::WriteL(TInt aParam, const TDesC8& aDes) const
	{
	User::LeaveIfError(Write(aParam, aDes));
	}

void TClientRequest::WriteL(TInt aParam, const TDesC& aDes, TInt aOffset) const
	{
	TDes* desPtr = (TDes*)iParams[aParam];
	desPtr->Copy(aDes.Mid(aOffset));
	}

TBool TClientRequest::HasCapabilities(const TCapabilitySet& aCapabilities) const
//dummy function here for testing purpose
	{
	TCapabilitySet dummycaps;
	dummycaps.SetEmpty();
	return aCapabilities.HasCapabilities(dummycaps);
	}

#ifndef __REMOVE_PLATSEC_DIAGNOSTICS__
void TClientRequest::EmitDiagnostic(const TCapabilitySet& /*aCapabilities*/, const CImplementationInformation& /*aImplInfo*/) const
// do nothing
	{
	}
#endif

TBool TClientRequest::CheckCapability(const TCapabilitySet& aCapabilities, const CImplementationInformation& /*aImplInfo*/) const
	{
	return HasCapabilities(aCapabilities);
	}
#endif //__ECOMSERVER_TESTING__
