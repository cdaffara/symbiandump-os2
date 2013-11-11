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
* This contains CommandProcessor.h
*
*/



#ifndef __COMMAND_PROCESSOR_H__
#define __COMMAND_PROCESSOR_H__

#include <e32base.h>

class MCommandProcessorCallback
/**
 * @internalComponent
 * @test
 *
 * Test timer completion callback
 */
	{
public:
	/**
	* Method from which CCommandProcessor informs the user RunL call
	*
	* @leave	system wide error
	*/
	virtual void	NextCommandL() = 0;
	};

class CCommandProcessor : public CActive
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
	IMPORT_C virtual ~CCommandProcessor();
	IMPORT_C static CCommandProcessor*	NewL(MCommandProcessorCallback& aCallback, TInt aPriority=EPriorityStandard);
	IMPORT_C static CCommandProcessor*	NewLC(MCommandProcessorCallback& aCallback, TInt aPriority=EPriorityStandard);

	IMPORT_C void	Activate();
	IMPORT_C void	KickState();
	void			RunL();
	void			DoCancel();

protected:
	CCommandProcessor(MCommandProcessorCallback& aCallback, TInt aPriority);

private:
	void	ConstructL();

private:
	/**
	* This is internal and not intended for use.
	*/
	MCommandProcessorCallback&	iCallback;
	};

#endif /* __COMMAND_PROCESSOR_H__ */
