/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <test/fileservplugin.h>

CFileServPlugin* CFileServPlugin::NewL()
	{
	CFileServPlugin* self = new(ELeave) CFileServPlugin();
	return self;
	}

CFileServPlugin::CFileServPlugin()
	{
	}

CFileServPlugin::~CFileServPlugin()
	{
	
	}

TDriveNumber CFileServPlugin::GetSystemDrive()
	{
	TDriveNumber defaultSysDrive(EDriveC);
	RFs fileServer;
	TVersionName version(fileServer.Version().Name());
	if (fileServer.Version().iMajor >= 2 &&
		fileServer.Version().iBuild >= 1100)
		{
		_LIT(KFileSrvDll, "efsrv.dll");

		RLibrary pluginLibrary;
		TInt pluginErr = pluginLibrary.Load(KFileSrvDll);

		if (pluginErr == KErrNone)
			{
			typedef TDriveNumber(*fun1)();
			fun1 sysdrive;

	#ifdef __EABI__
			sysdrive = (fun1)pluginLibrary.Lookup(336);
	#else
			sysdrive = (fun1)pluginLibrary.Lookup(304);
	#endif
			defaultSysDrive = sysdrive();
			}
		pluginLibrary.Close();
		}
	
	return defaultSysDrive;
	}
