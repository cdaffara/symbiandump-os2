// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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

#include <e32base.h>

/**
This 8-bit variable is used in agent resolution. It is used to override CAF's 
default resolution behaviour of only loading Stream Access Agents (SAAs)
from the ROM drive. Compile time value is 0 - default resolution behaviour. 
When non-zero CAF will load all SAA visible to the ECOM service regardless of 
the drive. In the emulator platform the default resolution behaviour is the 
same as when this variable is non-zero, therefore loading all SAAs.

This variable is changed at ROM build time using the patchdata obey keyword.
See Symbian OS Developer Library documentation on how to patch DLL data exports.
@file

@publishedPartner
@released
*/
IMPORT_C extern const TUint8 KCafLoadPostProductionAgents;

