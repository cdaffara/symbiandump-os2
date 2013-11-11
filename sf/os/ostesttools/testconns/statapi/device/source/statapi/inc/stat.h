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


#ifndef __STAT_H__
#define __STAT_H__

const TUint KVersionMajor = 1; 
const TUint KVersionMinor = 1;
const TUint KVersionPatch = 1002;

const char  OPT_DELIMITER	= '|';
const char	RESYNC_ID		= '?';
const char	FAILED_ID		= '*';
const char  REFRESH_ID		= 'Q';

_LIT(KFileSrvDll, "efsrv.dll");
_LIT(KFileSeparator, "\\");
_LIT(KStatLogFile, "statoutput.log");
_LIT(KReDrive, "$:");

#endif //__STAT_H__
