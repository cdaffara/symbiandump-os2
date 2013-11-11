/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TestUtil - filewatcher class
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef __TESTUTILFILEWATCHER_H__
#define __TESTUTILFILEWATCHER_H__

#include <e32base.h>
#include <f32file.h>

/**
 * This active object waits for an indication that the media has changed,
 * and then notifies its observer.
 */
class CFileWatcher : public CActive
	{
public:
	static CFileWatcher* NewL(RFs& aFs, const RMessagePtr2& aMessage);
	static CFileWatcher* NewLC(RFs& aFs, const RMessagePtr2& aMessage);
	
	~CFileWatcher();

	TInt Drive() const;
private:
	CFileWatcher(RFs& aFs, const RMessagePtr2& aMessage);

	void ConstructL();

// from CActive
	void DoCancel();
	void RunL();

private:
	/// Not owned by this class
	RFs& iFs;

	// Message which originated the notify request
	const RMessagePtr2 iMessage;
};

	
#endif // #ifndef __DRIVEWATCHER_H__

