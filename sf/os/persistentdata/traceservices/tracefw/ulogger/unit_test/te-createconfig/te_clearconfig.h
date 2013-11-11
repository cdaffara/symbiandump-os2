/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values thoughout your test project
* 
*
*/



/**
 @file te_clearconfig.h
 @internalTechnology
*/
#if (!defined __TE_CLEARCONFIG_H__)
#define __TE_CLEARCONFIG_H__

#include <uloggerclient.h>
using namespace Ulogger;

class CClearConfig : public CBase
{
public:
	~CClearConfig();
	CClearConfig();
	void ClearL(RULogger& loggerSession);
//private:
	
};

#endif
