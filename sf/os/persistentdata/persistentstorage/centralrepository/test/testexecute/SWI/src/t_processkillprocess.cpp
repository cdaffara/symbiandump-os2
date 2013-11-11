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

#include "../../../t_cenrep_helper.h"
#include <f32file.h>
#include <e32debug.h>
	
GLDEF_C TInt E32Main()
	{
	TFileName name;
	User::CommandLine(name);
	name.TrimAll();
	return KillProcess(name);
	}

