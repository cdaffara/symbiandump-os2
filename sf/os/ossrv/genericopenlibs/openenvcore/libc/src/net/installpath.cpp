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


#include <e32std.h>
#include <stdlib.h> 

void GetInstallationDataDir(TFileName& aPath)
	{
	Dll::FileName(aPath);
	aPath.Delete(2, aPath.Length());
	if (aPath[0] == L'z' || aPath[0] == L'Z')
		{
		aPath.Append(_L("\\resource\\pips\\"));
		}
	else
		{
		aPath.Append(_L("\\resource\\openc\\"));
		}
	}


extern "C" {

char* get_services_path(char* path)
	{
	TFileName pathbuf;
	GetInstallationDataDir(pathbuf);
	pathbuf.Append(_L("services"));
	wcstombs(path, (wchar_t*)(pathbuf.PtrZ()), pathbuf.Length()+1);
	return path;
	}
}	
