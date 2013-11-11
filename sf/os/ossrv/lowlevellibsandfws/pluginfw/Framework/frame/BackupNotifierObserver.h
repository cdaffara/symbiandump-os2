// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides the interface to an observer that wishes to recieve suspend 
// and resume notifications.
// 
//

#ifndef __MBACKUPNOTIFIEROBSERVER_H__
#define __MBACKUPNOTIFIEROBSERVER_H__


class MBackupNotifierObserver
/**
@internalComponent
*/
	{
	public:
	/**
		@fn				Suspend()
		Intended Usage	: Provides suspension of behaviour in the class 
						implementing this interface.
		Error Condition	: Dependent upon the implementation class Suspend().
		@since			7.0
		@pre 			Implementation class has been fully constructed.
		@post			Some action has been taken to change the behaviour
						of the implementation class of this interface.
	 */
	
	virtual TInt Suspend() = 0;

	/**
		@fn				Resume()
		Intended Usage	: Provides resumption of suspended behaviour in the class 
						implementing this interface.
		Error Condition	: Dependent upon the implementation class Resume().
		@since			7.0
		@pre 			Implementation class has been fully constructed.
		@post			Some action has been taken to change the behaviour
						of the implementation class of this interface.
	 */
	
	virtual TInt Resume() = 0;
	};

#endif // __MBACKUPNOTIFIEROBSERVER_H__
