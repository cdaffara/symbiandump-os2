/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TBATTERYCHARGINGDEFINITIONS_H
#define TBATTERYCHARGINGDEFINITIONS_H

#include <e32base.h>


const TUint32 KBatteryChargingTUint = 0x102834EB;
_LIT(KBattChargingImg, "tbatterychargingrepositorywriter.exe");

const TUid KBatteryChargingTUid = {KBatteryChargingTUint};


const TUint KBattChargWriteRepositoryUid = 0x101fe1db;
const TUint KBattChargReadPropertyCurrentUid = 0x101fe1db;


typedef struct TDataFromPropBattChargToTBatteryCharging
	{
	TInt iCurrent;
	TInt iError;
	}TDataFromPropBattChargToTBatteryCharging;

const TUint KPropBattChargNumOfData	=2;

const TUint KBattChargWriteRepositoryKey	= 0x102834EE;
const TUint KBattChargWriteRepositoryAckKey	= 0x102834EF;

const TUint KBattChargReadCurrentChargingKey	= 0x102834F0;
const TUint KBattChargReadCurrentChargingAckKey	= 0x102834F1;


#endif //TBATTERYCHARGINGDEFINITIONS_H