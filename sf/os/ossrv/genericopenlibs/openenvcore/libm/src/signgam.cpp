// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32std.h>

#ifdef __WINSCW__
#include <pls.h> // For emulator WSD API
const TUid KLibmUid3 = {0x102750d2}; 
#endif //__WINSCW__

#ifdef __cplusplus
extern "C" 
	{
#endif

#ifdef __WINSCW__

typedef struct
	{
	int _signgam;	
	}_libm_wsd;

TInt InitWSDVar(_libm_wsd* p)
	{
	p->_signgam = 0;
	return KErrNone;
	}
	
_libm_wsd* GetGlobals()
	{
	// Access the PLS of this process
	_libm_wsd* p = Pls<_libm_wsd>(KLibmUid3, &InitWSDVar);
	return p;
	}
	
#endif //__WINSCW__

EXPORT_C int* __signgam()
	{
#ifdef __WINSCW__
	return &(GetGlobals()->_signgam);
#else
	extern int _signgam;
	return &_signgam;
#endif //__WINSCW__
	}
	
#ifdef __cplusplus
}
#endif
