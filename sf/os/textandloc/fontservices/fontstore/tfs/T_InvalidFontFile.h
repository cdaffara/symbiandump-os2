/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __INVALIDFONTFILE_H__
#define __INVALIDFONTFILE_H__

#include <e32base.h>

//The contents of the invalid font file.
_LIT(KInvalidFontFileText, "This file should not load...");

//The target folder for the invalid file.
//Required to ensure it exists.
_LIT(KEclipsingFolder, "c:\\resource\\fonts\\");

//The full path of the invalid file to be generated.
_LIT(KEclipsingFile, "c:\\resource\\fonts\\DejaVuSansCondensed.ttf");

GLDEF_C TInt E32Main();

#endif  // __INVALIDFONTFILE_H__

