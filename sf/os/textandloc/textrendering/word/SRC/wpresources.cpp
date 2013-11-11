/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* WPFILE.CPP
*
*/


#include <f32file.h>

#include "wpresources.h"


/**
Get default path for word documents
@param aPath this will hold the resulting path
@internalComponent
*/
void TWordFilePath::GetWordDefaultFilePath(TFileName& aPath)
	{
	_LIT(KDriveMask, "_:\\");
	aPath.Copy(KDriveMask);
	aPath[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	}
	

/**
Get path of the folder containing document templates
@param aPath this will hold the resulting path
@internalComponent
*/
void TWordFilePath::GetNormalTemplateFolderName(TFileName& aPath)
	{
	// grab process's private path
	RFs fs;
	fs.Connect();
	fs.PrivatePath(aPath);
	fs.Close();
	// prepend ROM drive name at front
	TDriveUnit romDrive(static_cast<TInt>(EDriveZ));
	aPath.Insert(0, romDrive.Name());
	}
