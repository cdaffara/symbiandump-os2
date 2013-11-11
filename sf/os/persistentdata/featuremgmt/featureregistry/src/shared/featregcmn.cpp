// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Methods, data structures and constants common to Feature Registry Setup EXE
// and Public API DLL, including private panic method.
// 
//

#include <e32std.h>
#include "featregcmn.h"

GLDEF_C void Panic(TFeatRegPanic aReason)
	{
	_LIT(KCategory, "FeatReg");
	User::Panic(KCategory, aReason);
	}
