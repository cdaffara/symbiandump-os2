/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/




#include <_ansi.h>
#include "glibbackend.h"

extern "C" EXPORT_C char* applicationpath()
{
	//allocate memory in private heap as we will not be able to free it
	char *buf = (char *)pAlloc(32);
	return getcwd(buf,32);
}
