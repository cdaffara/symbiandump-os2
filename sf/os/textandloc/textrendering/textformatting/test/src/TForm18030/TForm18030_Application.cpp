/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
////////////////////////////////////////////////////////////////////////
//
// Source file for the implementation of the
// application class - TForm18030_Application
//
////////////////////////////////////////////////////////////////////////

#include "TForm18030.h"

const TUid KUidTForm18030 = { 0xE800005A };

//             The function is called by the UI framework to ask for the
//             application's UID. The returned value is defined by the
//             constant KUidTForm18030e and must match the second value
//             defined in the project definition file.
//
TUid TForm18030_Application::AppDllUid() const
	{
	return KUidTForm18030;
	}

//             This function is called by the UI framework at
//             application start-up. It creates an instance of the
//             document class.
//
CApaDocument* TForm18030_Application::CreateDocumentL()
	{
	return new (ELeave) TForm18030_Document(*this);
	}
