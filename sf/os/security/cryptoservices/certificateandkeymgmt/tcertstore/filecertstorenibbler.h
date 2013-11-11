/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef __FILECERTSTORENIBBLER_H__
#define __FILECERTSTORENIBBLER_H__

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>

class CFileCertstoreNibbler : public CTimer
{
public:
	static CFileCertstoreNibbler* NewL(TTimeIntervalMicroSeconds32 aInterval);
	~CFileCertstoreNibbler();
public:
	void StartTimer();
protected:
	virtual void ConstructL();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
private:
	CFileCertstoreNibbler(TTimeIntervalMicroSeconds32 aInterval);
private:
	void NibbleFileL();	//	writes to filecertstore file to test concurrency
private:
	RFs iFs;
	TTimeIntervalMicroSeconds32 iNibbleInterval;
	TStreamId iNibbleStreamId;
};

#endif	//	__FILECERTSTORENIBBLER_H__

