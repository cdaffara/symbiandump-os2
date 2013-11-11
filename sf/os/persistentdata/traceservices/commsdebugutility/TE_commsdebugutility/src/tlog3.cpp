// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "tlog3.h"
#include "TestMessage.h"




EXPORT_C void TestLog3::Test()
	{
	__FLOG_DECLARATION_VARIABLE;
    __FLOG_OPEN(KStdSubsysTag8,KStdCompTag8);
    __FLOG(KTestMessage8);
    __FLOG_CLOSE;
    }
    
