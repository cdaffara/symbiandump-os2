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
// Name        : dll1.cpp
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include <e32cons.h>
#include <e32uid.h>
#include <dlfcn.h>

#ifndef EKA2 
GLDEF_C TInt E32Dll(TDllReason /*aReason*/) 

// DLL entry point
	{
	return(KErrNone);
	}
#endif 

extern "C" EXPORT_C int strcmp(const char *string1, const char *string2)
{
	while (*string1 == *string2++)
		if (*string1++ == 0)
			return (0);
	return (*(const unsigned char *)string1 - *(const unsigned char *)(string2 - 1));
}

extern "C" EXPORT_C float abs(float i)
{
	return (i < 0) ? -i : i;
}


