/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contains the WSD solution
 *
*/


// INCLUDE FILES
#define EMULATOR ((defined(__WINS__) || defined(__WINSCW__)))

#if EMULATOR

#include <e32std.h>

#include "wsd_solution.h"
#include "pls.h" // For emulator WSD API 

const TUid KLibdlUid3 = {0x10281F2D};

LOCAL_C TInt InitializeWsd(TLibcryptWsd *aData)
{
	unsigned int bits32Initializer[32] = 
	{
		0x80000000, 0x40000000, 0x20000000, 0x10000000,
		0x08000000, 0x04000000, 0x02000000, 0x01000000,
		0x00800000, 0x00400000, 0x00200000, 0x00100000,
		0x00080000, 0x00040000, 0x00020000, 0x00010000,
		0x00008000, 0x00004000, 0x00002000, 0x00001000,
		0x00000800, 0x00000400, 0x00000200, 0x00000100,
		0x00000080, 0x00000040, 0x00000020, 0x00000010,
		0x00000008, 0x00000004, 0x00000002, 0x00000001
	};
	aData->des_initialised = 0;
	
	aData->m_sbox = NULL;
	aData->psbox = NULL;
	
	aData->ip_maskl = NULL;
	aData->ip_maskr = NULL;
	
	aData->fp_maskl = NULL;
	aData->fp_maskr = NULL;
	
	aData->key_perm_maskl = NULL;
	aData->key_perm_maskr = NULL;
	
	aData->comp_maskl = NULL;
	aData->comp_maskr = NULL;
	
	aData->old_rawkey0 = 0;
	aData->old_rawkey1 = 0;
	
	for(int i=0 ; i<32 ; ++i)
	{
		(aData->bits32)[i] = bits32Initializer[i];
	}
	
	// Initialize desKey array
	for(int j=0 ; j<64 ; ++j)
	{
		(aData->desKey)[j] = 0;
	}
	aData->bSetkeyInvoked = 0;
	
	return KErrNone;
}

extern "C" TLibcryptWsd* GetGlobals()
{
	// Access the PLS of this process
	TLibcryptWsd *p = Pls<TLibcryptWsd>(KLibdlUid3,InitializeWsd);
	return p;
}

#endif
