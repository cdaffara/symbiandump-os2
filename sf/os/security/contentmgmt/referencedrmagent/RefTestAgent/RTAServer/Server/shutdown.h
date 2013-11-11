/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* sisregistry - server shutdown classes
*
*/


/**
 @file
 @test
 @internalComponent
*/
#ifndef __SHUTDOWN_H__
#define __SHUTDOWN_H__

#include <e32base.h>

namespace ReferenceTestAgent
	{

	class CShutdown : public CTimer
		{
		enum {KSisRegistryShutdownDelay=0x200000};	// approx 2s
	public:
		CShutdown();
		void ConstructL();
		void Start();
	private:
		void RunL();
		};

//////////////////////////////////
// Timer specific inlines
//////////////////////////////////
	inline CShutdown::CShutdown() :CTimer(-1)
		{
		CActiveScheduler::Add(this);
		}

	inline void CShutdown::ConstructL()
		{
		CTimer::ConstructL();
		}

	inline void CShutdown::Start()
		{
		After(KSisRegistryShutdownDelay);
		}
	} //namespace

#endif // 
