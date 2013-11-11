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

#include <f32file.h>
#include <e32cons.h>

#include "ProfilerEshell.h"
#include <piprofiler/ProfilerSession.h>



_LIT(KProfilerEngineExe, "PIProfilerEngine.exe");

/*
 *
 * Static methods for controlling the profiler
 * through command line
 *
 */
// --------------------------------------------------------------------------------------------
static void PrintUsageInfo(const TDesC& aAdditionalInfo)
    {
    _LIT(KConsoleName, "Console");
    _LIT(KLegalNote, "PIProfiler Version 2.2.2 Copyright @ 2009 Nokia\n");
    _LIT(KUsageNote, "Usage: PIProfiler [start/end/timed] settingsfile [time to run]\n");
    _LIT(KExample, "Example: PIProfiler timed C:\\data\\piprofilersettings.txt 60\n");

    TRAP_IGNORE(CConsoleBase* console = Console::NewL(KConsoleName,TSize(KConsFullScreen,KConsFullScreen));
        console->Printf(KLegalNote);
        console->Printf(KUsageNote);
        console->Printf(KExample);
        console->Write(aAdditionalInfo);
    
        console->Printf(_L("\n[Press any key]"));
        console->Getch();
        delete console;);
    }

// --------------------------------------------------------------------------------------------
static TInt FindProcess()
    {
    TFindProcess procName;
    procName.Find(_L("PIProfilerEngine.exe*"));
    TFullName aResult;
    
    // find the first appearance, i.e. "myself"...
    TInt err(procName.Next(aResult));  // the first appearance
    if(err == KErrNotFound)
        {
        // now check if old Profiler is still running on
        procName.Find(_L("BappeaProf.exe*"));
        err = procName.Next(aResult);
        // check if old profiler process found 
        if(err == KErrNone)
            {
            // return error for error handling
            return KErrAlreadyExists;
            }
        // return KErrNotFound and a new profiler engine process can be started
        return KErrNotFound;
        }
    return err;
    }

/**
 * Function for starting profiler engine in the background,
 * call profiler to load settings
 * and start sampling process
 * 
 * @param configFile name and location of settings file
 * @param aRunTimeInSeconds run time in seconds
 * @param aBootTime run boot time sampling or not.
 */
// --------------------------------------------------------------------------------------------
static TInt StartProfilerProcess(const TDesC& configFile, TInt aRunTimeInSeconds, TBool aBootTime)
    {
    TInt err(KErrNone);
    RProcess proc;
    TRequestStatus status = KRequestPending;
    TBuf<256> conf;
    conf.Zero();
    conf.Append(configFile);
    if(aBootTime)
        {
        RDebug::Printf("boot time measurement");
        conf.Append(_L(" "));
        conf.Append(_L("boot"));
        }
    RDebug::RawPrint(conf);
    // check if process exists
    err = FindProcess();
    LOGSTRING2("PIProfiler: tried to find process, response %d", err); 

    // check if already exists and don't start a new eshell profiling
    if( err == KErrNotFound )
        {
        // try create new process
        err = proc.Create(KProfilerEngineExe, conf);

        LOGSTRING2("PIProfiler: created process, response %d", err); 

        // check if RProcess::Create() succeeded
        if( err == KErrNone )
            {
            // Trigger rendezvous on the supplied TRequestStatus object
            proc.Rendezvous(status); 

            // kick off the engine process
            proc.Resume();
            
            // wait for the constructor to complete 
            User::WaitForRequest(status); 
            
            // just lose the handle
            proc.Close();
            
            // start sampling, using settings found in settings file or if not found the default settings
            err = RProfiler::StartSampling();
            // check if command succesful
            if( err != KErrNone )
                {
                LOGSTRING2("PI Profiler start: err %d", err);
                _LIT(KNoteProfilerCannotStart, "PI Profiler: cannot start PI Profiler, check settings!");
                PrintUsageInfo(KNoteProfilerCannotStart);
                
                // check if process still alive
                if(err != KErrNotFound)
                    {
                    // exit the profiler process since process was started
                    RProfiler::ExitProfiler();
                    }
                return err;
                }
            
            if(aRunTimeInSeconds > 0)
                {
                RDebug::Print(_L("Profiler running for %d s... "), aRunTimeInSeconds);
                User::After(aRunTimeInSeconds*1000000);
                RDebug::Print(_L("************* Profiler process closing *********"));
        
                // stop sampling process
                err = RProfiler::StopSampling();
                // check if command succesfull
                if( err != KErrNone )
                    {
                    LOGTEXT(_L("Profiler: could not connect engine, stop failed"));
                    return err;
                    }
                
                // exit the profiler process
                err = RProfiler::ExitProfiler();
                // check if command succesfull
                if( err != KErrNone )
                    {
                    LOGTEXT(_L("Profiler: could not connect engine, exit failed"));
                    return err;
                    }
                }
            } 
        else
            {
            _LIT(KNoteCannotFindProfiler, "PI Profiler: could not find PIProfilerEngine.exe");
            PrintUsageInfo(KNoteCannotFindProfiler);
            }
        }
    // check if old Profiler is already running
    else if( err == KErrAlreadyExists )
        {
        _LIT(KNoteAlreadyRunning, "PI Profiler: old Profiler process already running, close it down before launching the new!");
        PrintUsageInfo(KNoteAlreadyRunning);
        }
    // otherwise show error note
    else
        {
        _LIT(KNoteAlreadyRunning, "PI Profiler: already running, not able to launch new one. NOTE: check if UI running!");
        PrintUsageInfo(KNoteAlreadyRunning);
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------
static TInt EndProfilerProcess()
    {
    LOGTEXT(_L("EndProfilerProcess() ..."));

    // call profiler to stop sampling
    TInt err = RProfiler::StopSampling();
    
    // check if command succesfull
    if( err != KErrNone )
        {
        LOGTEXT(_L("Profiler: could not connect engine, stop failed"));
        return err;
        }
    
    // exit the profiler process
    err = RProfiler::ExitProfiler();
    // check if command succesfull
    if( err != KErrNone )
        {
        LOGTEXT(_L("Profiler: could not connect engine, exit failed"));
        return err;
        }
    
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------
static TInt TestSettingsFile(const TDesC& configFile)
    {
    RFs fs;
    RFile file;
    TInt err(KErrNone);

    // check if file server can be connected
    if (fs.Connect() != KErrNone)
        {
        // file server couldn't be connected, return false
        return KErrNotFound;
        }

    // check if config file name length is > 0
    if (configFile.Length() > 0)
        {
        // open the file with the given path and name
        err = file.Open(fs,configFile,EFileRead);
        // check if file open was succesfull 
        if(err != KErrNone)
            {
            // return false if failed
            fs.Close();
            return err;
            }
        
        }
    else
        {
        // configFile length 0, return false
        fs.Close();
        return KErrNotFound;
        }
    // return true if tests passed
    file.Close();
    fs.Close();
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------
static TInt ParseCommandAndExecute()
    {
    // commands literals for finding the right keyword 
    _LIT(KAutomatedTestStart,"start*");
    _LIT(KAutomatedTestEnd,"end*");
    _LIT(KAutomatedTestTimed,"timed*");
    _LIT(KBootMeasurement,"boot*");
    TBuf<256> c;
    TInt match(KErrNotFound);
    TInt bootmatch(KErrNotFound);
    TBool myBoot=false;
    // copy the full command line with arguments into a buffer
    User::CommandLine(c);
    LOGSTRING2("command: %S", &c);

    // try to match with each of the literals defined above
    // (commands in atf format)
    
    // check if command is "start"
    match = c.Match(KAutomatedTestStart);
    if (match != KErrNotFound)
        {
        LOGTEXT(_L("Found keyword start"));

        TBuf<256> fileName;
        fileName.Append(c.Right(c.Length()-6));
        LOGSTRING2("Filename is %S", &fileName);
        if(TestSettingsFile(fileName) != KErrNone)
            {
            _LIT(KSettingsFileFailed, "False settings file");
            PrintUsageInfo(KSettingsFileFailed);
            return -2;
            }
        // execute Profile process 
        if( StartProfilerProcess(fileName, 0, myBoot) == KErrNone )
            {
            return -10;
            }
        return -2;
        }

    // check if command is "end"
    match = c.Match(KAutomatedTestEnd);
    if (match != KErrNotFound)
        {
        LOGTEXT(_L("Found keyword end"));

        // stop the profiling process
        EndProfilerProcess();
        return -10;
        }

    // check if command is "timed"
    match = c.Match(KAutomatedTestTimed);
    // check if command is "boot"
    bootmatch = c.Match(KBootMeasurement);
    if ((match!= KErrNotFound) || (bootmatch != KErrNotFound))
        {
        // command "timed" or " boot" found, need for finding settings file and run time next
        if(bootmatch != KErrNotFound)
            {
            LOGTEXT(_L("Found keyword boot"));
            myBoot = TRUE;
            }
        if(match != KErrNotFound)
            {
            LOGTEXT(_L("Found keyword timed"));
            }
        
        TBuf<256> temp;
        temp.Append(c);
        TLex lex(temp);
        
        TBuf<256> fileName;
        TInt seconds;

        // parse the first command line argument, the command itself
        lex.Mark();
        lex.SkipCharacters();
        if(lex.TokenLength() != 0)
            {
            #ifdef PIPROFILER_PRINTS
            TPtrC token = lex.MarkedToken();
            LOGSTRING2("Token 1 %S",&token);
            #endif
            }
        else
            {
            LOGTEXT(_L("Problem 1 in parsing command line"));
            _LIT(KSettingsFileFailed, "Failure: False argument");
            PrintUsageInfo(KSettingsFileFailed);
            return -2;
            }

        // parse the second command line argument, the settings file name
        lex.SkipSpace();
        lex.Mark();
        lex.SkipCharacters();
        if(lex.TokenLength() != 0)
            {
            TPtrC token2 = lex.MarkedToken();
            LOGSTRING2("Token 2 %S",&token2);
            fileName.Append(token2);
            LOGSTRING2("Value of fileName is %S",&fileName);
//            if(TestSettingsFile(fileName) != KErrNone)
//                {
//                _LIT(KSettingsFileFailed, "Failure: False settings file");
//                PrintUsageInfo(KSettingsFileFailed);
//                return -2;
//                }
            }
        else
            {
            LOGTEXT(_L("Problem 2 in parsing command line"));
            _LIT(KSettingsFileFailed, "Failure: No settings file specified");
            PrintUsageInfo(KSettingsFileFailed);
            return -2;
            }

        // parse the third command line argument, the run time in seconds
        lex.SkipSpace();
        lex.Mark();
        lex.SkipCharacters();
        if(lex.TokenLength() != 0)
            {
            // third token ok, try to convert into TInt value
            TPtrC token3 = lex.MarkedToken();
            LOGSTRING2("Token 3 %S",&token3);
            TLex num(token3);
            TInt err = num.Val(seconds);
            // check if given time value acceptable 
            if (err != KErrNone)
                {
                // value parsing failed, show info note to user
                _LIT(KSecondsFailed, "Failure: False time value");
                PrintUsageInfo(KSecondsFailed);
                return -2;
                }
            }
        else
            {
            LOGTEXT(_L("Problem 3 in parsing command line"));
            _LIT(KSecondsFailed, "Failure: False time value");
            PrintUsageInfo(KSecondsFailed);
            return -2;
            }           
        
        LOGSTRING3("Filename is %S, seconds is %d", &fileName, seconds);
        // execute Profile process 
        if( StartProfilerProcess(fileName, seconds, myBoot) == KErrNone )
            {
            return -10;
            }
        return -2;
        }
    
    // check if space character in the middle of command line string 
    if( c.LocateReverse(' ') != KErrNotFound)
        {
        _LIT(KWrongParameters, "Failure: Check command line parameters");
        PrintUsageInfo(KWrongParameters);
        return -2;
        }
    
    // return -1 if no command found
    LOGTEXT(_L("No keyword found"));
    return -1;
    }

// --------------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    // parse command line arguments
    ParseCommandAndExecute();

    return KErrNone;

    } 
