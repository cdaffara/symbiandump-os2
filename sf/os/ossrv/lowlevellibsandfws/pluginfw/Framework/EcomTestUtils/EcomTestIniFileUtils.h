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
// Helper methods for manipulating EcomSrvr.ini file for 
// testing purposes.
// 
//


#ifndef __ECOMTESTINIFILEUTILS_H__
#define __ECOMTESTINIFILEUTILS_H__

#include <e32test.h>
#include <bautils.h>

void DisableSsa(RTest& aTest, RFs& aFs);
void EnableSsa(RTest& aTest, RFs& aFs);
void ResetSsa(RTest& aTest, RFs& aFs);
void TestEnableDisableSsaL(RTest& aTest, RFs& aFs);

#endif //__ECOMTESTINIFILEUTILS_H__
