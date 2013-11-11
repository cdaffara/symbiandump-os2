// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name		: uloggerconfig.h
// Author	  : 
// Version	 : 1.0
// CClearConfig declaration
//




#ifndef ULOGGERCONFIG_
#define ULOGGERCONFIG_

#include <e32cons.h>
#include <e32def.h>
#include <uloggerclient.h>

using namespace Ulogger;

class CClearConfig : public CBase
{
public:
	~CClearConfig();
	CClearConfig();
	void ClearL(RULogger& loggerSession);
	void DefaultL(RULogger& loggerSession);
//private:
	
};

#endif /*ULOGGERCONFIG_*/


