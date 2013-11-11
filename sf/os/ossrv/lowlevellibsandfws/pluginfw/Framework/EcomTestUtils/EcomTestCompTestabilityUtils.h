// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Helper methods for manipulating EComTestBehaviour.ini file for 
// Ecom component testing purposes.
// 
//


#ifndef __ECOMTESTCOMPTESTABILITYUTILS_H__
#define __ECOMTESTCOMPTESTABILITYUTILS_H__

#include <e32test.h>
#include <bautils.h>

void EnableEcomTestBehaviour(RTest& aTest, RFs& /*aFs*/);
void DisableEcomTestBehaviour(RTest& aTest, RFs& /*aFs*/);

#endif //__ECOMTESTCOMPTESTABILITYUTILS_H__

