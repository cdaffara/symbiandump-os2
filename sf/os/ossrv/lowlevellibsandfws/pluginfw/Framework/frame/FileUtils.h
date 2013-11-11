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
// PathUtils.h
// Utility methods for manipulating file names and paths.
// 
//

/**
 @file
 @internalComponent
*/
#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_


/**
File name and path manipulation utilities.
@internalComponent
*/
class TEComFileUtils
	{
public:
	static void SetToDrive(TDes& aFileName, TInt aDrive);
	};



#endif /* _FILEUTILS_H_ */
