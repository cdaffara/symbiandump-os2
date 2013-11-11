// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// simple accessors/querying functions
// 
//


inline TInt CSchedule::Id() const
	{return iId;}

inline const TTsTime& CSchedule::DueTime() const
	{return iDueTime;}

inline TBool CSchedule::Persists() const
	{return iPersists;}

inline TBool CSchedule::Enabled() const
	{return iEnabled;}

inline void CSchedule::SetEnabled(TBool aEnabled)
	{iEnabled = aEnabled;}

inline TSglQue<TScheduledTask>* CSchedule::Tasks()
	{return &iTaskList;}

inline TBool CSchedule::HasTasks() const
	{return !(iTaskList.IsEmpty());}

inline const TDesC& CSchedule::Name() const
	{return *iName;}
	
inline TScheduleType CSchedule::Type() const
	{
	if(iConditions.Count()==0)
		return ETimeSchedule;
	else
		return EConditionSchedule;	
	}
	
inline void CSchedule::CheckAccessAllowedL(const RMessagePtr2& aMessage) const
	{
	if(!IsAccessAllowed(aMessage))
		User::Leave(KErrPermissionDenied);
	}
