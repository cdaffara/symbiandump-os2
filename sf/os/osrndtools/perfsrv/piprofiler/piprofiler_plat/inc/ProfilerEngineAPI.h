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


#ifndef PROFILERCONTROLLER_H
#define PROFILERCONTROLLER_H

#include <e32std.h>
#include <s32stor.h>

class CProfilerSettings;

class CProfilerEngineAPI : public CBase
{
public:
    enum TProfilerEngineTraceMode 
        { 
        EProfilerEngineDebugOutputMode, 
        EProfilerEngineFileSystemMode 
        };
    
	~CProfilerEngineAPI();

	IMPORT_C static CProfilerEngineAPI* NewL();
	void ConstructL();

	IMPORT_C void SaveSettings();
	IMPORT_C void LoadSettings();

	// controlling all sampler plugins
	IMPORT_C TInt StartProfiling();
	IMPORT_C TBool StopProfiling();
	IMPORT_C TInt LaunchProfiler();

	// controlling separate plugins
	IMPORT_C TInt StartSampler(TUint32 aUid);
	IMPORT_C TInt StopSampler(TUint32 aUid);
	IMPORT_C TInt EnableSampler(TUint32 aUid);
	IMPORT_C TInt DisableSampler(TUint32 aUid);
	IMPORT_C TInt EnableWriter(TUint32 aUid);
	IMPORT_C TInt DisableWriter(TUint32 aUid);

	// for meeting the HTI requirements
	IMPORT_C TInt SetSettings(TUint aSamplerId, TDes& aSettings);
    IMPORT_C TInt SetTraceMode(TProfilerEngineTraceMode aMode, TDes& aTraceDataStruct);
    IMPORT_C TInt GetSamplerInfo(TUint aSamplerId, TDes& aCompleteSamplerInfo);
	
	// old implementations
	IMPORT_C TBool GetSamplerVersion(TDes& version);

	IMPORT_C void LeaveProfilerBG(TBool aVal);
	
	
public:
	TBool DriveIsValidL(const TDesC& drive);
	IMPORT_C CProfilerSettings* Settings() const;

private:

	CProfilerEngineAPI();

	TInt				FindProcessL();
private:
	CProfilerSettings* 	iSettings;
	
	RProcess* 			iProfilerProcess;

	TBool 				iProfilerLaunched;
	
	TBool 				iLeaveProfilerBG;

	};

#endif
