/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "swicertstore.h"
static const EnumEntry enumDetailsForTBool[] =
{
    { "false", false},
    { "true", true},
    { "EFalse", false},
    { "ETrue", true},
	{ 0,0 }
};

EncDecContainerItem *SwiCertStoreEntry::Factory()
{
	return new SwiCertStoreEntry;
}

SwiCertStoreEntry::SwiCertStoreEntry()
	: CertStoreEntry(true),
	  iCapabilitySet("CapabilitySet"), 
	  iTmpFlags("Mandatory/SystemUpgrade"),
	  iMandatory("Mandatory", enumDetailsForTBool),
	  iSystemUpgrade("SystemUpgrade", enumDetailsForTBool)
{
	// We only need to initialise EncDecObject members which wrap non-class types
	iTmpFlags.Value() = 0;
}

SwiCertStoreEntry::~SwiCertStoreEntry()
{
}

const int KMandatoryMask = 0x01;
const int KSystemUpgradeMask = 0x02;

void SwiCertStoreEntry::Encode(REncodeWriteStream &aWriteStream)
{
	CertStoreEntry::Encode(aWriteStream);
	aWriteStream << iCapabilitySet;
	if(aWriteStream.HumanReadable())
		{
		// Write the Mandatory and SystemUpgrade fields
		aWriteStream << iMandatory;
		aWriteStream << iSystemUpgrade;
		}
	else
		{
		// Encode the Mandatory and SystemUpgrade values and write the binary field
		TUint8 tmp = 0;
		if(iMandatory.Value())
			{
			tmp |= KMandatoryMask;
			}
		
		if(iSystemUpgrade.Value())
			{
			tmp |= KSystemUpgradeMask;
			}
		
		iTmpFlags.Value() = tmp;
		aWriteStream << iTmpFlags;
		}

}

void SwiCertStoreEntry::Decode(RDecodeReadStream &aReadStream)
{
	CertStoreEntry::Decode(aReadStream);
	aReadStream >> iCapabilitySet;

	if(aReadStream.HumanReadable())
		{
		// Read the Mandatory and SystemUpgrade fields
		if(aReadStream.PeakToken() == iMandatory.Name())
			{
			aReadStream >> iMandatory;
			}
		else
			{
			iMandatory.SetValue("false");
			}
			if(aReadStream.PeakToken() == iSystemUpgrade.Name())
			{
			aReadStream >> iSystemUpgrade;
			}
		else
			{
			iSystemUpgrade.SetValue("false");
			}
		}
	else
		{
		// Read the binary field and decode the Mandatory and SystemUpgrade values
		aReadStream >> iTmpFlags;
		iMandatory.SetValue((iTmpFlags.Value() & KMandatoryMask) != 0);
		iSystemUpgrade.SetValue((iTmpFlags.Value() & KSystemUpgradeMask) != 0);
		}
}

SwiCertStoreEntry& SwiCertStoreEntry::operator= (const SwiCertStoreEntry& aRhs)
{
	if(this == &aRhs) return *this; // handle self assignment

	// Copy base class members
	CertStoreEntry::operator=(*static_cast<const CertStoreEntry *>(&aRhs));

	// Copy our additional members
	iCapabilitySet = aRhs.iCapabilitySet;
	// iTmpFlags does not need copying
	iMandatory = aRhs.iMandatory;
	iSystemUpgrade = aRhs.iSystemUpgrade;

	return *this;
}


// End of file
