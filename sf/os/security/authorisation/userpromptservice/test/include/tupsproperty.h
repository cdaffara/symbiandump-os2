/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
 @test
*/

#ifndef UPSPROPERTY_H
#define UPSPROPERTY_H

// interval between instances of either a PE or a DC. This value is added to the indexes below
static const TInt KInstanceInterval = 100;

// policy evaluator indexes
static const TInt KPe_Start = 0;
static const TInt KPe_Status = 1;
static const TInt KPe_RunningInstance = 2;
static const TInt KPe_ClientSid = 3;
static const TInt KPe_ServerSid = 4;
static const TInt KPe_Leave = 5;
static const TInt KPe_Error = 6;
static const TInt KPe_ServiceId = 7;
static const TInt KPe_ForcePrompt = 8;
static const TInt KPe_EvaluatorInfo = 9;
static const TInt KPe_HoldEvaluatorOpen = 10;

// dialog creator indexes
static const TInt KDc_Start = 50;
static const TInt KDc_Status = 1;
static const TInt KDc_RunningInstance = 2;
static const TInt KDc_ClientSid = 3;
static const TInt KDc_ServerSid = 4;
static const TInt KDc_Leave = 5;
static const TInt KDc_Error = 6;
static const TInt KDc_ServiceId = 7;
static const TInt KDc_UpsRequestedButtons = 20;
static const TInt KDc_WhichButtonToPress = 21;
static const TInt KDc_HoldPrepareDialogOpen = 22;
static const TInt KDc_HoldDisplayDialogOpen = 23;
static const TInt KDc_SelectFingerprint = 24;

static const TInt32 KPropertyCreator=0x101F7784; 
static const TUid KPropertyCreatorUid={KPropertyCreator};

class CUpsProperty : public CBase
	{
public:
	enum TType
	{
		EPolicyEvaluator,
		EDialogCreator
	};

	static CUpsProperty* NewL(const TUid& aCategory);
	virtual ~CUpsProperty() {};

	void GetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TInt& aValue);
	void GetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TDes& aValue);
	void SetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TInt aValue);
	void SetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TDes& aValue);

	TInt GetPropertyKey(TInt aPropertyKey, TInt aStart, TInt aInterval, TInt aInstanceNumber);

protected:
	TUid iCategory;

private:
	CUpsProperty(const TUid& aCategory);
	};


#endif
