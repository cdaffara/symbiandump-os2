// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// T-class for internalALL usage 	
// These classes comprise part of the interface (the rest is defined in RScheduler)
// 
//

#if !defined (__SCHINFOINTERNAL_H__)
#define __SCHINFOINTERNAL_H__



/** 
Defines a condition which a Publish and Subscribe Uid must satisfy.

A condition encapsulates three pieces of information:

the UID identifying the P&S variable against which a test is to be made.

the value against which that P&S variable is to be tested.

the type of test to be made.

@see RScheduler::CreatePersistentSchedule
@see RScheduler::EditSchedule
@see RScheduler::ScheduleTask
@see RScheduler::GetScheduleL
@see RProperty

@internalAll
*/
class TTaskSchedulerCondition
	{
public:
	/** 
	An enumeration defining the type of test to be made against 
	a Publish and Subscribe property. 
	*/
	enum TConditionType
		{
		/** Tests that a value is equal to a state variable value. */
		EEquals,		
		/** Tests that a value is unequal to a state variable value. */
		ENotEquals,		
		/** Tests that a value is greater than a state variable value. */
		EGreaterThan,	
		/** Tests that a value is less than a state variable value. */
		ELessThan		
		};
	inline TTaskSchedulerCondition();
	inline TTaskSchedulerCondition(TUid aCategory,
								TUint aKey, 
								TInt aState, 
								TConditionType aType);
public:
	/** P&S category.*/
	TUid iCategory;
	/** P&S key.*/
	TUint iKey;
	/** Integer state of corresponding P&S variable to be tested against*/
	TInt iState;
	/** type of test to be performed */
	TConditionType iType;
	};


/** 
Constructs the object with default values.

The UID identifying the P&S category against which a test is to be made 
is set to KNullUid.  The sub key is set to zero.

The value against which that P&S variable is to be tested is set to zero.

The type of test to be made is set to EEquals. 
*/
inline TTaskSchedulerCondition::TTaskSchedulerCondition()
:	iCategory(KNullUid), 
	iKey(0),
	iState(0), 
	iType(EEquals)
	{
	}

/** 
Constructs the object with the specified values.

Note that the RProperty variable identified by the aCategory/aKey pair 
must be of integer type for this to be a valid task scheduler condition.

@param aCategory The publish and subscribe category to be tested.
@param aKey The publish and suscribe sub-key to be tested.
@param aState The value against which the P&S variable identified by the 
specified UID is to be tested. 
@param aType The type of test to be made. 

@see RProperty
*/
inline TTaskSchedulerCondition::TTaskSchedulerCondition(TUid aCategory,
										TUint aKey, 
										TInt aState, 
										TConditionType aType)
:	iCategory(aCategory),
	iKey(aKey), 
	iState(aState), 
	iType(aType)
	{
	}
	


#endif
