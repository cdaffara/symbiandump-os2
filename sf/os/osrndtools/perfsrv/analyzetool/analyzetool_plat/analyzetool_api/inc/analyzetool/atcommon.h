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
* Description:  Common declarations/definitions for Analyze Tool.
*
*/


#ifndef ATCOMMON_H_
#define ATCOMMON_H_

// CONSTANTS
const TInt KATMaxCallstackLength = 256;
const TInt KATMaxFreeCallstackLength = 256;
const TInt KATMaxSubtestIdLength = 256;

// The following constants only limit the configuration UI.
// The bigger the number is, the more space will be allocated run-time
// by the client. So, change with care.
const TInt KATMaxProcesses = 20;
const TInt KATMaxDlls = 30;

// Constants defining call stack address range in multiple memory model.
const TInt32 KATMultipleMemoryModelLowLimit = 0x70000000;
const TInt32 KATMultipleMemoryModelHighLimit = 0x90000000;


// CLASS DECLARATIONS

/**
*  A class for storing process information
*/
class TATProcessInfo
    {
    public:
    
        /** The ID of the process. */
        TUint iProcessId;
        
        /** The name of the process. */
        TBuf8<KMaxProcessName> iProcessName;
        
        /** The starting time of the process. */
        TInt64 iStartTime;
    };


// ENUMERATIONS

/**
*  Enumeration for different logging modes of Analyze Tool 
*/
enum TATLogOption
    {
    /** Using the default. */
    EATUseDefault = 0,
    
    /** Logging to a file in S60. */
    EATLogToFile,
    
    /** Logging to debug channel. */
    EATLogToTrace,
    
    /** Logging to debug channel bypassing storage server. */
    EATLogToTraceFast,
    
    /** Logging switched off. */
    EATLoggingOff
    };

/**
*  Class which supports interfacing with AnalyzeTool exported
*  functions. Mainly meant for STIF integration.
*/
class AnalyzeToolInterface
    {
    public:

        /**
        * This function starts subtest with a given name.
        * @param aSubtestId The name identifying this particular sub test. The length
        *   of this descriptor must not be greater than KATMaxSubtestIdLength, or
        *   otherwise the method raises a STSEClient: 2 panic.
        */
        IMPORT_C static void StartSubTest( const TDesC8& aSubtestId );

        /**
        * This function stops a subtest with a given name.
        * @param aSubtestId The name identifying this particular sub test. The length
        *   of this descriptor must not be greater than KATMaxSubtestIdLength, or
        *   otherwise the method raises a STSEClient: 2 panic. 
        */   
        IMPORT_C static void StopSubTest( const TDesC8& aSubtestId );

    };
    

#endif /*ATCOMMON_H_*/
