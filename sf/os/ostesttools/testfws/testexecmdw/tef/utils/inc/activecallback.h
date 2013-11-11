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
* @file
* This contains ActiveCallback.h
*
*/



#ifndef __ACTIVE_CALLBACK_H__
#define __ACTIVE_CALLBACK_H__

#include <e32base.h>

class MActiveCallback
/**
 * @internalComponent
 * @test
 *
 * Asynchronus completion callback
 */
	{
public:
	/**
	 * Method from which CActiveCallback informs the user RunL call
	 *
	 * @param	aActive - the object that the RunL has completed on
	 * @param	aIndex - asynchronous command index
	 *
	 * @leave	system wide error
	 */
	virtual void	RunL(CActive* aActive, TInt aIndex) = 0;

	/**
	 * Method from which CActiveCallback informs the user DoCancel call
	 *
	 * @param	aActive - the object that the RunL has completed on
	 * @param	aIndex - asynchronous command index
	 *
	 * @leave	system wide error
	 */
	virtual void	DoCancel(CActive* aActive, TInt aIndex) = 0;
	};

class CActiveCallback : public CActive
/**
 * @internalComponent
 * @test
 *
 * @see		CActive
 *
 * Test Active Notification class
 *
 */
	{
public:
	IMPORT_C virtual ~CActiveCallback();
	IMPORT_C static CActiveCallback*	NewL(MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);
	IMPORT_C static CActiveCallback*	NewLC(MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);

	IMPORT_C void	Activate( TInt aIndex );
	IMPORT_C void	KickState();
	IMPORT_C void	RunL();
	IMPORT_C void	DoCancel();

protected:
	IMPORT_C CActiveCallback(MActiveCallback& aCallback, TInt aPriority);
	IMPORT_C void	ConstructL();

private:
	/**
	* This is internal and not intended for use.
	*/
	MActiveCallback&	iCallback;
	TInt				iIndex;
	};

#endif /* __ACTIVE_CALLBACK_H__ */
