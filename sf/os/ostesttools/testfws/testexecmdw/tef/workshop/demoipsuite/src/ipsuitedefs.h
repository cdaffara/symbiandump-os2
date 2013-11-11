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



/**
 @file IPSuiteDefs.h
*/

// Keys in the configuration ini file
_LIT(KDemoSuiteIPAddress,"Ip_Addr");
_LIT(KDemoSuiteTestData,"Test_Data");
_LIT(KDemoSuiteIPPort,"Port");

// For test step panics
_LIT(KDemoIPSuitePanic,"DemoIPSuite");
enum TDemoSuitePanicCodes{ETCPDataCorrupt = 1,EUDPDataCorrupt};

