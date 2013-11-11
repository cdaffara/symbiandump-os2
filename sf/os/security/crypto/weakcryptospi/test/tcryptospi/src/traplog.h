/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology
*/
#if (!defined __TRAPLOG_H__)
#define __TRAPLOG_H__

#include <e32cmn.h>
													
#define TRAP_LOG(_err, _call)										\
	{																\
	TInt& __eref = _err;											\
	__eref = 0;														\
	TRAP(__eref, _call);											\
	if(__eref != KErrNone)											\
		{															\
		INFO_PRINTF2(_L("*** ERROR: TRAP_LOG Leave - %d"), __eref);	\
		User::Leave(__eref);										\
		}															\
	}																\

#define TRAPD_LOG(_err, _call)										\
	TInt _err;														\
	{																\
	TRAP(_err, _call);												\
	if(_err != KErrNone)											\
		{															\
		INFO_PRINTF2(_L("*** ERROR: TRAPD_LOG Leave - %d"), _err);	\
		User::Leave(_err);											\
		}															\
	}																\

#endif	//__TRAPLOG_H__
