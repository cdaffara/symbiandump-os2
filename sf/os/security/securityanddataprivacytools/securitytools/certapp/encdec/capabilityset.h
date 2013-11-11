#ifndef __CAPABILITYSET_H__
#define __CAPABILITYSET_H__/*
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


#include <e32base.h>
#include <e32capability.h>
#include "encdec.h"

/**
 * @file
 * @internalComponent
 */

const TInt KCapabilitySetMaxSize = (((TInt)ECapability_HardLimit + 7)>>3);

class CapabilitySet
	{
public:
	CapabilitySet();
	CapabilitySet(const CapabilitySet &aRef);
	CapabilitySet &operator=(const CapabilitySet &aRhs);
	void AddCapability(TCapability aCapability);
	TBool HasCapability(TCapability aCapability) const;
	void ExternalizeL(RWriteStream &aStream) const;
	void InternalizeL(RReadStream &aStream);

private:
	TUint32 iCaps[KCapabilitySetMaxSize / sizeof(TUint32)];
	};

void EncodeHuman(REncodeWriteStream& aStream,const CapabilitySet &aCapSet);
void DecodeHuman(RDecodeReadStream& aStream, CapabilitySet &aCapSet);



#endif
