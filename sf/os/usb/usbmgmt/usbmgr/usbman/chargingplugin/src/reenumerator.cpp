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
 @file
 @internalComponent
*/

#include "reenumerator.h"

CUsbChargingReEnumerator* CUsbChargingReEnumerator::NewL(RDevUsbcClient& aLdd)
	{
	CUsbChargingReEnumerator* self = new (ELeave) CUsbChargingReEnumerator(aLdd);
	return self;
	}

CUsbChargingReEnumerator::~CUsbChargingReEnumerator()
	{
	Cancel();
	}

void CUsbChargingReEnumerator::ReEnumerate()
	{
	if (IsActive())
		{
		Cancel();
		}
	iLdd.ReEnumerate(iStatus);
	SetActive();
	}

CUsbChargingReEnumerator::CUsbChargingReEnumerator(RDevUsbcClient& aLdd)
: CActive(EPriorityStandard), iLdd(aLdd)
	{
	CActiveScheduler::Add(this);
	}

void CUsbChargingReEnumerator::RunL()
	{
	}

void CUsbChargingReEnumerator::DoCancel()
	{
	iLdd.ReEnumerateCancel();
	}
