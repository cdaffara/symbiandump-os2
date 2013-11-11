/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef USBCONTROLAPPSHARED_H
#define USBCONTROLAPPSHARED_H

// P&S Shutdown property
static const TInt32 KUidUsbControlAppCategoryValue=0x10285EE9;
static const TUid	KUidUsbControlAppCategory={KUidUsbControlAppCategoryValue};
static const TUint	KUsbControlAppShutdownKey=0x10285EE6;

// Standard H4 TextShell screen size
const TInt KScreenWidth = 40;
const TInt KScreenDepth = 24;

// Message Queue between Control App and Viewer
_LIT(KControlAppViewerMsgQName, "ControlAppViewerMsgQ");
const TInt KControlAppViewerMsgQSlots	= 5;
const TInt KViewerNumCharactersOnLine	= 32;
const TInt KEventLineNumCharacters		= KViewerNumCharactersOnLine-1; // Chars that can go into an Event for display 
_LIT(KPrefix, "!:"); // All user messages shown with prefix "!:"
const TInt KEventLineMsgNumCharacters	= KEventLineNumCharacters - 2; // -2 for "!:" prefix

#endif // USBCONTROLAPPSHARED_H
