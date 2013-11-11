// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test CentrepPatchData.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __T_CENTREPPATCHDATA_H__
#define __T_CENTREPPATCHDATA_H__

#include <e32def.h>


#ifdef __EXE__
	//to detect the number of ROFS configuration currently enabled
	extern const TUint32 KCentrepTestNumberROFS;
#endif

#ifdef __EXE__
	//to detect the number of ROFS configuration currently enabled
	//0 means TXT only
	//1 means CRE only
	//2 means both TXT and CRE
	extern const TUint32 KCentrepPerfTestRunMode;
#endif

#endif
