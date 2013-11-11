// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common client server message components
// 
//

#ifndef __D32SRV_H__
#define __D32SRV_H__

#ifndef __E32STD_H__
#include <e32std.h>
#endif

class Dbs
/**
@internalComponent
*/
	{
public:
	static TInt Start();
	IMPORT_C static TInt Run();
private:
	static TInt Init();
	};

#include "D32SRV.INL"
#endif
