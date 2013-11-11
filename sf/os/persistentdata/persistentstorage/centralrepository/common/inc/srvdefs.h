// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SRVDEFS_H
#define SRVDEFS_H

#include <e32def.h>

const TUint32 KMetaBackup		= 0x01000000;
const TUint32 KMetaRfsValue		= 0x02000000; // Restore Factory Settings
const TUint32 KMetaDefaultValue = 0x80000000;
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	const TUint32 KMetaType			= 0x30000000;
	const TUint32 KMetaIndividual	= 0x40000000;
	const TUint32 KMetaInternal		= 0x70000000;	
#else
	const TUint32 KMetaType			= 0x70000000;
#endif	


const TUint8 KPersistFormatVersion = 1 ;
const TUint8 KPersistFormatSupportsDeletedSettings = 1 ;

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	const TUint8 KPersistFormatSupportsIndMetaIndicator = 2 ;
	//New internal error code to be mapped to the client panic code
	//these error codes will never reach the client so will not be published
	/**
	@internalComponent
	*/
	const TInt KErrMultiRofsOldCreUsed= 			-30001;
	const TInt KErrMultiRofsGlobalOverride=			-30002;
	const TInt KErrMultiRofsTypeOverride=			-30003;
	const TInt KErrMultiRofsIllegalRofs=			-30004;
#endif
namespace NCentralRepositoryResources
{
    enum  TCentRepLocation
        {
        EPersists,
        EInstall,
        ERom
        };
    
    enum  TCentRepFileType
        {
        ECre,
        EIni,
        ETmp,
        EUnknown
        };
}

#endif // SRVDEFS_H
