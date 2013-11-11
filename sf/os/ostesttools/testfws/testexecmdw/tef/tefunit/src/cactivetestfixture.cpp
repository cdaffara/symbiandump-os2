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
 @file CActiveTestFixture.cpp
*/

#include "tefunit.h"

CActiveTestFixture::~CActiveTestFixture()
/**
 * Destructor
 */
	{
	}

void CActiveTestFixture::SetupL()
/**
 * SetupL
 */
	{
	}

void CActiveTestFixture::TearDownL()
/**
 * TearDownL
 */
	{
	}

void CActiveTestFixture::ConstructEnvL()
/**
 * ConstructEnvL
 */
	{
	// Construct a new ActiveScheduler and install it
	iSched = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iSched);
	}

void CActiveTestFixture::DestroyEnvL()
/**
 * DestroyEnvL
 */
	{
	delete iSched;
	iSched = NULL;
	}

void CActiveTestFixture::StartEnvL()
/**
 * StartEnvL
 */
	{
	// Start the ActiveScheduler
	CActiveScheduler::Start();
	}

void CActiveTestFixture::StopEnvL()
/**
 * StopEnvL
 */
	{
	// Stop the ActiveScheduler
	CActiveScheduler::Stop();
	}
