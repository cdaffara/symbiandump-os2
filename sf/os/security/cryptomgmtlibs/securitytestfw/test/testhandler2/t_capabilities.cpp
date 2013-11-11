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
* Parsing of capability names
*
*/


/**
 @file
*/

#include "t_input.h"
#include "t_output.h"

// This file needs to be updated in three places whenever new capabilities are
// added - the list of capability names below and the methods
// Input::ParseCapabilityNameL and Output::writeCapabilityL.

// Capability names
_LIT8(KCapabilityTCB,  				"TCB");
_LIT8(KCapabilityCommDD,  			"CommDD");
_LIT8(KCapabilityPowerMgmt,  		"PowerMgmt");
_LIT8(KCapabilityMultimediaDD,  	"MultimediaDD");
_LIT8(KCapabilityReadDeviceData,  	"ReadDeviceData");
_LIT8(KCapabilityWriteDeviceData,  	"WriteDeviceData");
_LIT8(KCapabilityDRM,  				"DRM");
_LIT8(KCapabilityTrustedUI,  		"TrustedUI");
_LIT8(KCapabilityProtServ,  		"ProtServ");
_LIT8(KCapabilityDiskAdmin,  		"DiskAdmin");
_LIT8(KCapabilityNetworkControl,  	"NetworkControl");
_LIT8(KCapabilityAllFiles,  		"AllFiles");
_LIT8(KCapabilitySwEvent,  			"SwEvent");
_LIT8(KCapabilityNetworkServices,  	"NetworkServices");
_LIT8(KCapabilityLocalServices,  	"LocalServices");
_LIT8(KCapabilityReadUserData,  	"ReadUserData");
_LIT8(KCapabilityWriteUserData,  	"WriteUserData");
_LIT8(KCapabilityLocation,  		"Location");
_LIT8(KCapabilitySurroundingsDD,  	"SurroundingsDD");
_LIT8(KCapabilityUserEnvironment,  	"UserEnvironment");

// Script file tags
_LIT8(KCapabilityStart, "<capability>");
_LIT8(KCapabilityEnd, "</capability>");
_LIT8(KPolicyPass, "<pass/>");
_LIT8(KPolicyFail, "<fail/>");
_LIT8(KSecureIdStart, "<secureid>");
_LIT8(KSecureIdEnd, "</secureid>");
_LIT8(KVendorIdStart, "<vendorid>");
_LIT8(KVendorIdEnd, "</vendorid>");

_LIT(KCommaSpace, ", ");

EXPORT_C TCapability Input::ParseCapabilityNameL(const TDesC8& aName)
	{
	TCapability result = ECapability_None;

	if (aName == KCapabilityTCB) 					result = ECapabilityTCB;
	else if (aName == KCapabilityCommDD) 			result = ECapabilityCommDD;
	else if (aName == KCapabilityPowerMgmt) 		result = ECapabilityPowerMgmt;
	else if (aName == KCapabilityMultimediaDD) 		result = ECapabilityMultimediaDD;
	else if (aName == KCapabilityReadDeviceData) 	result = ECapabilityReadDeviceData;
	else if (aName == KCapabilityWriteDeviceData) 	result = ECapabilityWriteDeviceData;
	else if (aName == KCapabilityDRM) 				result = ECapabilityDRM;
	else if (aName == KCapabilityTrustedUI) 		result = ECapabilityTrustedUI;
	else if (aName == KCapabilityProtServ) 			result = ECapabilityProtServ;
	else if (aName == KCapabilityDiskAdmin) 		result = ECapabilityDiskAdmin;
	else if (aName == KCapabilityNetworkControl) 	result = ECapabilityNetworkControl;
	else if (aName == KCapabilityAllFiles) 			result = ECapabilityAllFiles;
	else if (aName == KCapabilitySwEvent) 			result = ECapabilitySwEvent;
	else if (aName == KCapabilityNetworkServices) 	result = ECapabilityNetworkServices;
	else if (aName == KCapabilityLocalServices) 	result = ECapabilityLocalServices;
	else if (aName == KCapabilityReadUserData) 		result = ECapabilityReadUserData;
	else if (aName == KCapabilityWriteUserData) 	result = ECapabilityWriteUserData;
	else if (aName == KCapabilityLocation) 			result = ECapabilityLocation;
	else if (aName == KCapabilitySurroundingsDD) 	result = ECapabilitySurroundingsDD;
	else if (aName == KCapabilityUserEnvironment) 	result = ECapabilityUserEnvironment;

	if (result == ECapability_None)
		{
		User::Leave(KErrArgument);
		}
	
	return result;
	}

EXPORT_C void Input::ParseCapabilitySetL(const TDesC8& aIn, TCapabilitySet& aOut)
	{
	aOut.SetEmpty();
	TInt pos = 0, err = KErrNone;
	for (;;)
		{
		const TDesC8& match = Input::ParseElement(aIn, KCapabilityStart, KCapabilityEnd, pos, err);
		if (err != KErrNone)
			{
			break;
			}
		aOut.AddCapability(ParseCapabilityNameL(match));
		}
	if (err != KErrNone && err != KErrNotFound)
		{
		User::Leave(err);
		}
	}

EXPORT_C void Input::ParseSecurityPolicyL(const TDesC8& aIn, TSecurityPolicy& aOut)
	{
	TInt options = 0;

	TBool pass = aIn.Find(KPolicyPass) != KErrNotFound; // why didn't we use a proper xml parser?
	if (pass) ++options;

	TBool fail = aIn.Find(KPolicyFail) != KErrNotFound;
	if (fail) ++options;

	TUint secureId = ParseIntElement(aIn, KSecureIdStart, KSecureIdEnd);
	if (secureId) ++options;

	TUint vendorId = ParseIntElement(aIn, KVendorIdStart, KVendorIdEnd);
	if (vendorId) ++options;

	// Can only sepcify one of pass, fail, secureid, vendorid
	if (options > 1)
		User::Leave(KErrArgument);

	// Parse the capabilities
	TCapabilitySet capSet;
	ParseCapabilitySetL(aIn, capSet);

	// Determine maximum number of capabilities allowed
	TInt maxCaps;
	if (pass || fail)
		{
		maxCaps = 0;
		}
	else if (secureId || vendorId)
		{
		maxCaps = 3;
		}
	else
		{
		maxCaps = 7;
		}
	
	TCapability caps[7];
	TInt count = 0;

	// Extract capabilities into array
	TInt i;
	for (i = 0 ; i < ECapability_Limit ; ++i)
		{
		TCapability c = static_cast<TCapability>(i);
		if (capSet.HasCapability(c))
			{
			// Check if more capabities are specified that allowed
			if (count == maxCaps)
				{
				User::Leave(KErrArgument);
				}
			caps[count++] = c;
			}
		}
	
	// Fill the rest of the array with ECapability_None
	for (i = count ; i < maxCaps ; ++i)
		{
		caps[i] = ECapability_None;
		}

	if (pass)
		{
		aOut = TSecurityPolicy(TSecurityPolicy::EAlwaysPass);
		}
	else if (fail)
		{
		aOut = TSecurityPolicy(TSecurityPolicy::EAlwaysFail);
		}
	else if (secureId)
		{
		aOut = TSecurityPolicy(TSecureId(secureId), caps[0], caps[1], caps[2]);
		}
	else if (vendorId)
		{
		aOut = TSecurityPolicy(TVendorId(vendorId), caps[0], caps[1], caps[2]);
		}			
	else
		{
		aOut = TSecurityPolicy(caps[0], caps[1], caps[2], caps[3],
							   caps[4], caps[5], caps[6]);
		}
	}

EXPORT_C void Output::writeCapabilityL(TCapability aCap)
	{
	switch (aCap)
		{
		case ECapabilityTCB:
			writeString(KCapabilityTCB);
			break;

		case ECapabilityCommDD:
			writeString(KCapabilityCommDD);
			break;

		case ECapabilityPowerMgmt:
			writeString(KCapabilityPowerMgmt);
			break;

		case ECapabilityMultimediaDD:
			writeString(KCapabilityMultimediaDD);
			break;

		case ECapabilityReadDeviceData:
			writeString(KCapabilityReadDeviceData);
			break;

		case ECapabilityWriteDeviceData:
			writeString(KCapabilityWriteDeviceData);
			break;

		case ECapabilityDRM:
			writeString(KCapabilityDRM);
			break;

		case ECapabilityTrustedUI:
			writeString(KCapabilityTrustedUI);
			break;

		case ECapabilityProtServ:
			writeString(KCapabilityProtServ);
			break;

		case ECapabilityDiskAdmin:
			writeString(KCapabilityDiskAdmin);
			break;

		case ECapabilityNetworkControl:
			writeString(KCapabilityNetworkControl);
			break;

		case ECapabilityAllFiles:
			writeString(KCapabilityAllFiles);
			break;

		case ECapabilitySwEvent:
			writeString(KCapabilitySwEvent);
			break;

		case ECapabilityNetworkServices:
			writeString(KCapabilityNetworkServices);
			break;

		case ECapabilityLocalServices:
			writeString(KCapabilityLocalServices);
			break;

		case ECapabilityReadUserData:
			writeString(KCapabilityReadUserData);
			break;

		case ECapabilityWriteUserData:
			writeString(KCapabilityWriteUserData);
			break;

		case ECapabilityLocation:
			writeString(KCapabilityLocation);
			break;

		case ECapabilitySurroundingsDD:
			writeString(KCapabilitySurroundingsDD);
			break;

		case ECapabilityUserEnvironment:
			writeString(KCapabilityUserEnvironment);
			break;

		default:
			User::Invariant();
		}
	}

EXPORT_C void Output::writeCapabilitySetL(const TCapabilitySet& aCaps)
	{
	TBool first = ETrue;
	for (TInt i = 0 ; i < ECapability_Limit ; ++i)
		{
		TCapability cap = static_cast<TCapability>(i);
		if (aCaps.HasCapability(cap))
			{
			if (!first)
				{
				writeString(KCommaSpace);
				}
			else
				{
				first = EFalse;
				}
			writeCapabilityL(cap);
			}
		}
	}

/**
 * The real TSecurityPolicy class has no accessors, so to extract information
 * from it we cast it to this class, which has exactly the same layout.
 */
class TPrintableSecurityPolicy
	{
public:
	void WriteL(Output& aOut) const;
	
public:
	enum TType
		{
		ETypeFail=0,	// Always fail
		ETypePass=1,	// Always pass
		ETypeC3=2,		// Up to 3 capabilities
		ETypeC7=3,		// Up to 7 capabilities
		ETypeS3=4,		// SID + up to 3 capabilities
		ETypeV3=5,		// VID + up to 3 capabilities

		ETypeLimit
		};

private:
	TPrintableSecurityPolicy();
	TBool WriteCapsL(Output& aOut, TBool aFirst) const;
	void WriteExtraCapsL(Output& aOut, TBool aFirst) const;
	
private:
	TUint8 iType;
	TUint8 iCaps[3];				// missing capabilities are set to 0xff
	union
		{
		TUint32 iSecureId;
		TUint32 iVendorId;
		TUint8 iExtraCaps[4];		// missing capabilities are set to 0xff
		};
	};

// Check noone added another type to the enumeration
__ASSERT_COMPILE(((TInt)TPrintableSecurityPolicy::ETypeLimit) == ((TInt)TSecurityPolicy::ETypeLimit));

EXPORT_C void Output::writeSecurityPolicyL(const TSecurityPolicy& aPolicy)
	{
	const TPrintableSecurityPolicy* p = reinterpret_cast<const TPrintableSecurityPolicy*>(&aPolicy);
	p->WriteL(*this);
	}

void TPrintableSecurityPolicy::WriteL(Output& aOut) const
	{
	switch (iType)
		{
		case ETypeFail:
			aOut.writeString(_L("AlwaysFail"));
			break;
		case ETypePass:
			aOut.writeString(_L("AlwaysPass"));
			break;
		case ETypeC3:
			WriteCapsL(aOut, ETrue);
			break;
		case ETypeC7:
			{
			TBool first = WriteCapsL(aOut, ETrue);
			WriteExtraCapsL(aOut, first);
			}
			break;
		case ETypeS3:
			aOut.writeString(_L("SID 0x"));
			aOut.writeHex(iSecureId);
			WriteCapsL(aOut, EFalse);
			break;
		case ETypeV3:
			aOut.writeString(_L("VID 0x"));
			aOut.writeHex(iVendorId);
			WriteCapsL(aOut, EFalse);
			break;
		default:
			User::Invariant();
		}
	}

TBool TPrintableSecurityPolicy::WriteCapsL(Output& aOut, TBool aFirst) const
	{
	for (TInt i = 0 ; i < 3 ; ++i)
		{
		if (iCaps[i] != 0xff)
			{
			if (!aFirst)
				{
				aOut.writeString(KCommaSpace);
				}
			aFirst = EFalse;
			aOut.writeCapabilityL((TCapability) iCaps[i]);
			}
		}
	return aFirst;
	}

void TPrintableSecurityPolicy::WriteExtraCapsL(Output& aOut, TBool aFirst) const
	{
	for (TInt i = 0 ; i < 3 ; ++i)
		{
		if (iExtraCaps[i] != 0xff)
			{
			if (!aFirst)
				{
				aOut.writeString(KCommaSpace);
				}
			aFirst = EFalse;
			aOut.writeCapabilityL((TCapability) iExtraCaps[i]);
			}
		}
	}
