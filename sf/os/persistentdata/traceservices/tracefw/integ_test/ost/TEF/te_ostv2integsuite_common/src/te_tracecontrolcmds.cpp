// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Ost Performance Tests ULogger commands.
//



/**
 @file te_tracecontrolcmds.cpp
 @internalTechnology
 @prototype
*/

#include "te_tracecontrolcmds.h"
#include "te_dataparameters.h"
#include "te_ostv2integsuite_defs.h"

using namespace Ulogger; //CA:added so can use old ulogger api

CUptULogger::CUptULogger()
	{
	/*Constructor*/
	}

CUptULogger::~CUptULogger()
	{
	/*Destructor*/
	}

_LIT(KTextvalue, "C:\\logs\\ULoggerStartFileTestLog.utf");
/** This user-side method configures the passed ULogger session to log to a default file and with filter settings as specified by the user
    and starts the session in order to output trace points.
@param aLogger is ULogger session being passed to the routine to be configured and started
@param aTestUTraceULoggerMatch is the user-defined boolean specifying whether the Ulogger filter settings should match the (test-specific) trace points or not
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUptULogger::StartFileL(RULogger& aLogger, const TBool& aGroupIdFiltering, const TBool& aComponentIdFiltering, const TBool& aGroupIdFilterMatch, const TBool& aComponentIdFilterMatch)
	{
	TInt err=0;
	//set plugin name as default file
	_LIT8(KTextmedia,"uloggerfileplugin");
	TPtrC8 pluginPtr(KTextmedia);
	//set plugin configuration as default file settings
	_LIT(KTextsetting, "output_path");
	//_LIT(KTextvalue, "C:\\logs\\UPTApiCallTest.utf");
	TPluginConfiguration pluginConfig;
	pluginConfig.SetKey(KTextsetting);
	pluginConfig.SetValue(KTextvalue);
	
	//now configure and start ulogger
	err = StartGeneralL(aLogger, pluginPtr, pluginConfig, aGroupIdFiltering, aComponentIdFiltering, aGroupIdFilterMatch, aComponentIdFilterMatch);
	return err;
	}

/** This user-side method configures the passed ULogger session to output to the passed plugin (with configurations) and with filter settings as specified by the user
    and starts the session in order to output trace points.
@param aLogger is ULogger session being passed to the routine to be configured and started
@param aPluginName is the TPtrC8 passed, identifying which ULogger plugin to output to
@param aPluginConfiguration is the TPluginConfiguration passed, indicating the hardware configurations for the specified plugin
@param aTestUTraceULoggerMatch is the user-defined boolean specifying whether the Ulogger filter settings should match the (test-specific) trace points or not
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUptULogger::StartGeneralL(RULogger& aLogger, const TPtrC8& aPluginName, const TPluginConfiguration& aPluginConfiguration, const TBool& aGroupIdFiltering, const TBool& aComponentIdFiltering, const TBool& aGroupIdFilterMatch, const TBool& aComponentIdFilterMatch)
	{
	TInt err=0;
	
	//connect to the ULogger session
	err=aLogger.Connect();
	
    if (err!=KErrNone)
        {
        for (TInt i=2; i<12;i++)
            {
            User::After(1000);
            err = aLogger.Connect();
            if (err==KErrNone)
                {
                break;
                }
            }
        }
	
	//clear any configurations stored in the ULogger configuration file
	CClearConfig configIni;
	configIni.ClearL(aLogger);
	
	//prepare arrays for storing the filter settings to configure ULogger (and push any heap objects on the cleanup stack)
	CArrayFixFlat<TUint8> *setPrimFilter = new (ELeave)CArrayFixFlat<TUint8>(256);
	CleanupStack::PushL(setPrimFilter);
	RArray<TUint32> setSecondFilter;
	CleanupClosePushL(setSecondFilter);
	//now configure the filtering and filter match options according to the passed parameters:
	//aGroupIdFiltering aComponentIdFiltering aGroupIdFilterMatch aComponentIdFilterMatch	

	if(aGroupIdFiltering==1)
	    {
	    //We might have previously "disabled" primary filtering by enabling all filter
	    //values, we'll disable them all here initially and then enable what we want below
        TInt i;
        for (i=0;i<=255;i++)
            {
            setPrimFilter->AppendL((TUint8)i);
            }
        TInt err=aLogger.SetPrimaryFiltersEnabled(*setPrimFilter,EFalse);
        setPrimFilter->Delete(0,setPrimFilter->Count());
	    }
		//C.A. previously this but done by default:err=aLogger.EnableGroupIdFiltering();
	else
		//C.A. previously:err=aLogger.DisableClassificationFiltering();
		//C.A.: just enable all classifications so all is allowed through
		{
		TInt i;
		for (i=0;i<=255;i++)
			{
			setPrimFilter->AppendL((TUint8)i);
			}
		TInt err=aLogger.SetPrimaryFiltersEnabled(*setPrimFilter,ETrue);
		setPrimFilter->Delete(0,setPrimFilter->Count());
		}

	err=aLogger.SetSecondaryFilteringEnabled(aComponentIdFiltering);
//	C.A. previously	
/*	if(aComponentIdFiltering==1)
		err=aLogger.EnableModuleUidFiltering();
	else
		err=aLogger.DisableModuleUidFiltering();*/
	
	if(aGroupIdFilterMatch==1)
		{
		//if the user specifies that ULogger should have classifiaction filter settings which match the test trace points, then fill the filter arrays with the test trace point identifiers accordingly
		SetFilterL(setPrimFilter);		
		}
	else
		{
		//if the user specifies that ULogger should have classifciation filter settings which dont match the test trace points, then fill the filter arrays with the trace point identifiers which differ from the test trace point identifiers
		setPrimFilter->AppendL((TGroupId)(KGroupId+1));
		}
	
	if(aComponentIdFilterMatch==1)
			{
			//if the user specifies that ULogger should have module id filter settings which match the test trace points, then fill the filter arrays with the test trace point identifiers accordingly
			setSecondFilter.Append((TComponentId) KComponentId);
			}
		else
			{
			//if the user specifies that ULogger should have module id settings which dont match the test trace points, then fill the filter arrays with the trace point identifiers which differ from the test trace point identifiers
			setSecondFilter.Append((TComponentId) (KComponentId+1));
			}

	//set the filter settings in ULogger according to the above defined filter arrays
	err=aLogger.SetPrimaryFiltersEnabled(*setPrimFilter,ETrue);//C.A. previously:err=aLogger.EnableClassifications(*setPrimFilter);
	if(err==0)
		err=aLogger.SetSecondaryFiltersEnabled(setSecondFilter,ETrue);//C.A. previously:err=aLogger.EnableModuleUids(setSecondFilter);
	
	
	if(err==0)	
		{
		//now set the ULogger session to output to the specified plugin and configuration
		err=aLogger.ActivateOutputPlugin(aPluginName);//C.A. previously:err=aLogger.SetActiveOutputPlugin(aPluginName);
	
		if((err == KErrNone)/*Added*/||(err == KErrAlreadyExists))//C.A. have to add cuz old api returns -11 if plugin already activated
			{
			err=aLogger.SetPluginConfigurations(aPluginName, aPluginConfiguration);
			if(err==0)
				{
				//given the ULogger session has successfully been configured to output to the default file, start the session
				err=aLogger.Start();//C.A. previously:err=aLogger.StartOutputting();
				}
			}
		}
	
	//cleanup any heap objects

	CleanupStack::PopAndDestroy(&setSecondFilter);	
	CleanupStack::PopAndDestroy(setPrimFilter);
	
	return err;
	}
	
/** This user-side method sets the filter for OSTv2
 */
void CUptULogger::SetFilterL(CArrayFixFlat<TUint8>* aSetPrimFilter)
    {
    aSetPrimFilter->AppendL(KGroupId);
    }

/** This user-side method stops the passed ULogger session 
@param aLogger is ULogger session being passed to the routine to be configured and started
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUptULogger::Stop(RULogger& aLogger)
	{
	//stop the passed ULogger session
	TInt err=aLogger.Stop();//C.A. previously:TInt err=aLogger.StopOutputting();
	return err;
	}

/** This user-side method stops the passed ULogger file session and deletes the log file to ensure the disk 
 * space is not maxed out for intensive performance testing.
@param aLogger is ULogger session being passed to the routine to be configured and started
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUptULogger::StopAndDeleteFile(RULogger& aLogger)
	{
	//stop the passed ULogger session
	TInt err=aLogger.Stop();//C.A. previously:TInt err=aLogger.StopOutputting();
	
	//create handle to the output file and connect to the session
	RFs outputFileSession;
	RFile outputFile;
	err=outputFileSession.Connect();
	
	//delete the output file (i.e. replace with a zero length file) and close the sessions
	err=outputFile.Replace(outputFileSession, KTextvalue ,EFileWrite);
	outputFile.Close();
	outputFileSession.Close();	
	
	return err;
	}

TInt CUptULogger::DeleteFile()
	{
	RFs outputFileSession;
	RFile outputFile;
	TInt err = outputFileSession.Connect();	
	//delete the output file (i.e. replace with a zero length file) and close the sessions
	err=outputFile.Replace(outputFileSession, KTextvalue ,EFileWrite);
	outputFile.Close();
	outputFileSession.Close();	
	
	return err;
	}

TInt CUptULogger::GetDataL(TUint8*& aData, TUint& aSize)
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile file;
	TInt error = file.Open(fsSession, KTextvalue, EFileRead);
	if(error == KErrNone)
		{
		TInt size = 0;
		file.Size(size);
		aSize = size;
		aData = new TUint8[aSize];
		if(!aData)
			error = KErrNoMemory;
		if(!error)
			{
			TPtr8 ptr(aData, aSize);
			error = file.Read(ptr);
			}
		file.Close();//is really called at fsSession.Close(); 
		fsSession.Close();
		}
	CleanupStack::PopAndDestroy();//fsSession
	return error;
	}


/** This sanity test method is executed at the start of the test run to verify that UPT methods in
this class are stable before any of the performance tests are carried out 
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */
TInt CUptULogger::Test()
	{
	return KErrNone;
	}


