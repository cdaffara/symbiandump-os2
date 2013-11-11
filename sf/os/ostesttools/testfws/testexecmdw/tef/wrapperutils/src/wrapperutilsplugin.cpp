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
#include <test/sysstartplugin.h>
#include <test/wrapperutilsplugin.h>

EXPORT_C CWrapperUtilsPlugin* CreateWrapperInstanceL()
	{
	return new (ELeave) CWrapperUtilsPlugin;
	}

void CWrapperUtilsPlugin::WaitForSystemStartL()
	{
	CSysStartPlugin* plugin = CSysStartPlugin::NewL();
	plugin->WaitForSystemStartL();
	delete plugin;
	}

TDriveNumber CWrapperUtilsPlugin::GetSystemDrive()
	{
	TDriveNumber driveNumber = EDriveC;
	CFileServPlugin* plugin = NULL;
	TRAPD(err, plugin = CFileServPlugin::NewL());
	if (KErrNone == err)
		{
		driveNumber = plugin->GetSystemDrive();
		delete plugin;
		}
	
	return driveNumber;
	}
	
