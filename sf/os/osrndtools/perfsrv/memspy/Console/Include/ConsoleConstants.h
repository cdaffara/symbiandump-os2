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

#ifndef CONSOLE_CONSTANTS_H
#define CONSOLE_CONSTANTS_H

// System includes
#include <e32std.h>

// Literal constants
_LIT( KMemSpyConsoleNewLine, "\r\n" );
_LIT( KMemSpyConsoleName, "MemSpy Console" );
_LIT( KMemSpyConsoleInputPrompt, " > %S" );
_LIT( KMemSpyConsoleWildcardCharacter, "*" );

// Numerical constants
const TInt KMemSpyMaxDisplayLengthThreadName = 48;
const TInt KMemSpyMaxDisplayLengthSizeText = 14;
const TInt KMemSpyMaxInputBufferLength = 200;

// Key constants (S60)
const TInt KMemSpyUiS60KeyCodeButtonOk = 2000;
const TInt KMemSpyUiS60KeyCodeButtonCancel = 2001;
const TInt KMemSpyUiS60KeyCodeRockerEnter = 63557;

#endif
