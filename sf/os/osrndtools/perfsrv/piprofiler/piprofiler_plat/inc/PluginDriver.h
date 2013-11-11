/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <piprofiler/ProfilerGenericClassesKrn.h>

const TInt KMinRate = 10;
const TInt KMaxRate = 1000;

enum TState{ERunning, EStopping, EStopped};

const TUid KGppPropertyCat={0x20201F70};
enum TGppPropertyKeys
	{
	EGppPropertySyncSampleNumber
	};


/*
 *
 *
 *	Class DPluginDriver definition, inherited by sampler implementations except GPP
 *
 *
 */

class DPluginDriver : public DLogicalChannel
{

public:
	DPluginDriver();
	~DPluginDriver();

public:	
	TInt					ProcessStreamReadCancel();

	TInt					StopSampling();

private:
	TInt					ProcessPrintStreamRequest(TDesC8* aDes/*,TRequestStatus* aStatus*/);
	
	void 					FastPrintf(TDesC8* aDes);
 
public:
	DThread*				iClient;

	// property and value; 
	// GPP sampler start time, needed to set other samplers in sync
	RPropertyRef 			iSampleStartTimeProp;
	TInt					iSampleStartTime;
	TUint32 				iSyncOffset;		// offset from the start
	
	
	TUint32					sampleRunning;
	const TUint*			iInterruptStack;
	TState					iState;
	TInt					doingDfc;

	// request status objects for requests
	TRequestStatus*			iEndRequestStatus;
	TRequestStatus*			iStreamReadCancelStatus;

	// sample stream object used in stream mode
	DProfilerSampleStream		iSampleStream;

	// just for testing
	TUint32*				stackTop;

};

#include <piprofiler/PluginDriver.inl>
