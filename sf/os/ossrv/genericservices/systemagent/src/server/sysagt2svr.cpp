// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hal.h>
#include <e32base.h>
#include "SaPrivate.h"
#include "SaCls.h"
#include <saclsdefines.h>
#include <saclscommon.h>

/**
Count of the properties registered at startup.
50 Uids issued from 0x100052C3 to 0x100052F4 
0x100052C3 is not used, 0x100052C4 is used for KUidSystemAgentExe
leaving 48 for property uids
@internalComponent
*/
const TInt  KPropertyCount = 48;

//Security policy definitions for the KPropertyCount predefined properties.
_LIT_SECURITY_POLICY_C1(KSecurityPolicyNone, ECapability_None);
_LIT_SECURITY_POLICY_C1(KSecurityPolicyWriteDeviceData, ECapabilityWriteDeviceData);
_LIT_SECURITY_POLICY_S1(KSecurityPolicySwiSIDTrustedUi, 0x101F7295, ECapabilityTrustedUI);
_LIT_SECURITY_POLICY_S0(KSecurityPolicyJavaSID, 0x1020329F);

// Security policy for secure backup engine
const TInt KSBESID = 0x10202D56;
_LIT_SECURITY_POLICY_S0(KSBEWritePolicy, KSBESID);
_LIT_SECURITY_POLICY_PASS(KSBEReadPolicy);

// Security policy definitions for LBS
_LIT_SECURITY_POLICY_C1(KLBSLastKnownLocationReadPolicy, ECapabilityReadDeviceData);
_LIT_SECURITY_POLICY_S0(KLBSLastKnownLocationWritePolicy, 0x101f97b2);
_LIT_SECURITY_POLICY_PASS(KLBSGpsHwStatusReadPolicy);
_LIT_SECURITY_POLICY_C1(KLBSGpsHwStatusWritePolicy, ECapabilityWriteDeviceData);

//The function initializes aWritePolicy parameter with the appropriate 
//value, depending on the value of aPropertyUid parameter.
static void GetPropertyPolicies(const TUid& aPropertyUid, const TSecurityPolicy*& aWritePolicy)
	{
	switch(aPropertyUid.iUid)
		{
		case KUidPhonePwrValue:
		case KUidSIMStatusValue:
		case KUidNetworkStatusValue:
		case KUidNetworkStrengthValue:
		case KUidChargerStatusValue:
		case KUidBatteryStrengthValue:
		case KUidCurrentCallValue:
			aWritePolicy = &KSecurityPolicyWriteDeviceData;
			break;
		default:
			aWritePolicy = &KSecurityPolicyNone;
			break;
		};
	}

/**
This function tries to define a property, and if it is not already defined, it sets its value to a default value.
@internalComponent
@leave Some of the system-wide error codes.
@param aCategory The UID that identifies the property category. 
@param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
@param aAttr This describes the property type, a TType value; persistence, as defined by the KPersistent bit, may be ORed in. 
@param aReadPolicy A security policy defining the security attributes a process must have in order to read this value. 
@param aWritePolicy A security policy defining the security attributes a process must have in order to write this value. 
@param aDefaultValue The default value to assign to the property, if the property does not already exist and we define it. If the default value is zero (0) the property will not be set, since this is the default.
*/
static void DefinePSPropertyL(TUid aCategory, TInt aKey, TInt aAttr, const TSecurityPolicy &aReadPolicy, const TSecurityPolicy &aWritePolicy, TInt aDefaultValue)
	{
	TInt err = RProperty::Define(aCategory, aKey, aAttr, aReadPolicy, aWritePolicy);
	
	if (err != KErrAlreadyExists)
		{
		// Leave if the error is not one of KErrNone or KErrAlreadyExists
		User::LeaveIfError(err);
		}
		
	if (err == KErrNone && aDefaultValue != 0)
		{
		// Initialise the value if it was not already defined, and the value is not already set
		User::LeaveIfError(RProperty::Set(aCategory, aKey, aDefaultValue));
		}
		
	}


/** 
The function creates P&S properties with uid's from 0x100052C5 to 0x100052C5 + KPropertyCount.
They were (and are) used by the implementation of SystemAgent server.
If a property is registered for the first time, its initial value will be set to KErrUnknown.
@internalComponent
@leave Some of the system-wide error codes.
*/
static void CreatePSPropertiesL()
	{
	TUid saUid = TUid::Uid(KUidPhonePwrValue);
	for(TInt i=0;i<KPropertyCount;++i)
		{
		const TSecurityPolicy* writePolicy = NULL;
		::GetPropertyPolicies(saUid, writePolicy);
		
		DefinePSPropertyL(KUidSystemCategory, saUid.iUid, RProperty::EInt, KSecurityPolicyNone, *writePolicy, KErrUnknown);
		++saUid.iUid;
		}
	}
	
/** 
The function creates P&S properties with uids KUidJavaInstallKey and Swi::KUidSoftwareInstallKey.
If a property is registered for the first time, its initial value will be set to 0, indicating
no operations are currently being performed.
@internalComponent
@leave Some of the system-wide error codes.
*/
static void CreateSwiPropertiesL()
	{
		
	// Java properties are policed on the JavaHelperServer SID
		
	DefinePSPropertyL(KUidSystemCategory, KSAUidJavaInstallKeyValue, RProperty::EInt, KSecurityPolicyNone, KSecurityPolicyJavaSID, 0);
	DefinePSPropertyL(KUidSystemCategory, KUidJmiLatestInstallation, RProperty::EInt, KSecurityPolicyNone, KSecurityPolicyJavaSID, 0);
		
	// Native properties are policed on the TrustedUI capability
		
	DefinePSPropertyL(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, RProperty::EInt, KSecurityPolicyNone, KSecurityPolicySwiSIDTrustedUi, 0);
	DefinePSPropertyL(KUidSystemCategory, KUidSwiLatestInstallation, RProperty::EInt, KSecurityPolicyNone, KSecurityPolicySwiSIDTrustedUi, 0);

	DefinePSPropertyL(KUidSystemCategory, KSWIUidsCurrentlyBeingProcessed, RProperty::EByteArray, KSecurityPolicyNone, KSecurityPolicySwiSIDTrustedUi, 0);
		
	}

/** 
The function creates the P&S property with uid KUidUnifiedCertstoreFlag
If a property is registered for the first time, its initial value will be set to 0, indicating
no operations are currently being performed.
@internalComponent
@leave Some of the system-wide error codes.
*/
static void CreateUnifiedCertstorePropertiesL()
	{
	// Allow any process to read or write this value
	// initial value of zero
	DefinePSPropertyL(KUidSystemCategory, KUidUnifiedCertstoreFlag, RProperty::EInt, KSecurityPolicyNone, KSecurityPolicyNone, 0);
	}


/** 
The function creates P&S properties with uids KUidBackupRestoreKey.
If a property is registered for the first time, its initial value will be set to 0, indicating
no operations are currently being performed.
@internalComponent
@leave Some of the system-wide error codes.
*/
static void CreateSBEPropertiesL()
	{
	// We define two properties currently, one for Java MIDlet install and one for native
	// Software Install
	TInt properties[]=
 		{
		KUidBackupRestoreKey // backup and restore key
		};
	
	for (TInt i=0; i < sizeof(properties)/sizeof(properties[0]); ++i)
		{
		DefinePSPropertyL(KUidSystemCategory, properties[i], RProperty::EInt, KSBEReadPolicy, KSBEWritePolicy, 0);
		}
	}


/** 
The function creates the P&S property with uids KSAPosLastKnownLocation & KSAPosIntGpsHwStatus
If a property is registered for the first time, its initial value will be set to 0, indicating
no operations are currently being performed.
@internalComponent
@leave Some of the system-wide error codes.
*/
static void CreateLBSPropertiesL()
	{
	DefinePSPropertyL(KSAPosLastKnownLocationCategory, KSAPosLastKnownLocation, RProperty::EText, KLBSLastKnownLocationReadPolicy, KLBSLastKnownLocationWritePolicy, 0);
	DefinePSPropertyL(KSAPosIndicatorCategory, KSAPosIntGpsHwStatus, RProperty::EInt, KLBSGpsHwStatusReadPolicy, KLBSGpsHwStatusWritePolicy, 0);
	}


static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
    User::LeaveIfError(User::RenameThread(KSystemAgentServerName));

	::CreatePSPropertiesL();
	::CreateSwiPropertiesL();
	::CreateUnifiedCertstorePropertiesL();
	::CreateSBEPropertiesL();
	::CreateLBSPropertiesL();
	
	RProcess::Rendezvous(KErrNone);
	}

// Server process entry-point
// Recover the startup parameters and run the server
TInt E32Main()
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}


