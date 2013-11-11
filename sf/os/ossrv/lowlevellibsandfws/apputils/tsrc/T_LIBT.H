// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by Brendan, Dec 1996
// Library tester
// 
//

#ifndef __T_LIBT_H__
#define __T_LIBT_H__
#include <e32base.h>

class CLibTest : public CBase
	{
public:
	virtual TInt Test1() const=0;
	virtual TInt Test2() const=0;
	virtual TInt Test3()=0;
	};

#endif
