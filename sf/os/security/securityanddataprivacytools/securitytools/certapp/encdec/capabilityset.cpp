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


#define __INCLUDE_CAPABILITY_NAMES__
#include "capabilityset.h"

CapabilitySet::CapabilitySet()
{
	memset(iCaps,0,sizeof(iCaps));
}

#if 0
// Not currently used or tested
CapabilitySet::CapabilitySet(const CapabilitySet &aRef)
{
	memcpy(iCaps, aRef.iCaps, sizeof(iCaps));
}
#endif

CapabilitySet &CapabilitySet::operator=(const CapabilitySet &aRhs)
{
BULLSEYE_OFF
	if(this == &aRhs) return *this; // handle self assignment
BULLSEYE_RESTORE
	memcpy(iCaps, aRhs.iCaps, sizeof(iCaps));
	return *this;
}


void CapabilitySet::AddCapability(TCapability aCapability)
{
	BULLSEYE_OFF
	if((TUint32)aCapability>=(TUint32)ECapability_HardLimit)
		{
		dbg << Log::Indent() << "Illegal capabaility value " << aCapability << Log::Endl();
		FatalError();
		}
	BULLSEYE_RESTORE
	
	TInt index = aCapability>>3;
	TUint8 mask = (TUint8)(1<<(aCapability&7));
	//mask &= ((TUint8*)&AllSupportedCapabilities)[index];
	((TUint8*)iCaps)[index] |= mask;
}

TBool CapabilitySet::HasCapability(TCapability aCapability) const
{
	BULLSEYE_OFF
	if((TUint32)aCapability>=(TUint32)ECapability_HardLimit)
		{
		FatalError();
		}
	BULLSEYE_RESTORE

	return (((TUint8*)iCaps)[aCapability>>3]>>(aCapability&7))&1;
}


void EncodeHuman(REncodeWriteStream& aStream,const CapabilitySet &aCapSet)
{
	aStream.WriteCStr(" { ");
	for(TUint32 cap=0; cap < ECapability_Limit; ++cap)
		{
		if(aCapSet.HasCapability(TCapability(cap)))
			{
			aStream.WriteCStr(CapabilityNames[cap]);
			aStream.WriteSpace();
			}
		}

	for(TUint32 cap=ECapability_Limit; cap < ECapability_HardLimit; ++cap)
		{
		if(aCapSet.HasCapability(TCapability(cap)))
			{
			EncodeHuman(aStream, TUint32(cap));		
			aStream.WriteSpace();
			}
		}

	aStream.WriteByte('}');
}

void DecodeHuman(RDecodeReadStream& aStream, CapabilitySet &aCapSet)
{
	aStream.CheckName("{");
	while(aStream.PeakToken() != "}")
		{
		AutoIndent ai(prog);
		// We process PeakToken so if it is not valid we can call
		// DecodeHuman TUint32 to read the next token and process it
		// as a number.
		prog << Log::Indent() << "Parsing capability '" << aStream.PeakToken() << "'" << Log::Endl();
		TUint32 cap;
		for(cap=0; cap < ECapability_Limit; ++cap)
			{
			if(aStream.PeakToken() == CapabilityNames[cap])
				{
				break;
				}
			}
		if(cap < ECapability_Limit)
			{
			// The token was ok, so read/discard it.
			aStream.ReadNextToken();
			}
		else
			{
			// Fallback to decoding as a number
			prog<< Log::Indent() << "WARNING: Unknown capability '" << aStream.PeakToken() << "' attempting to decode as a bit number..." << Log::Endl();
			DecodeHuman(aStream, cap);
			prog << Log::Indent() << "Decoded as " << cap << Log::Endl();
			}
		aCapSet.AddCapability(TCapability(cap));
		}
	aStream.CheckName("}");
}


void CapabilitySet::ExternalizeL(RWriteStream &aStream) const
{
	TPckg<typeof(iCaps)> capsPckg(iCaps);
	aStream << capsPckg;
}

void CapabilitySet::InternalizeL(RReadStream &aStream)
{
	TPckg<typeof(iCaps)> capsPckg(iCaps);
	aStream >> capsPckg;
}



// End of file
