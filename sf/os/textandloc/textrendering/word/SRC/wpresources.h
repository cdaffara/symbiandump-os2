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
* Miscellaneous resource stuff
*
*/


#if !defined(__WPRESOURCES_H__)
#define __WPRESOURCES_H__


#include <e32cmn.h>
#include <f32file.h>


/**
This class is used for retrieving default paths for files and folders 
used internally by Word.
@internalComponent
*/
class TWordFilePath
	{
// functions
public:
	static void GetWordDefaultFilePath(TFileName& aPath);
	static void GetNormalTemplateFolderName(TFileName& aPath);
	};
	



#endif
