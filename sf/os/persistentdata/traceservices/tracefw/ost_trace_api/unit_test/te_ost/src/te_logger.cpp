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
//



/**
 @file te_logger.cpp
 @internalTechnology
*/
#include <f32file.h>
#include "te_logger.h"

static const int KPrimaryFilterMaxSize = 256;
_LIT(KLogFilePath, "c:\\logs\\testexecute\\utrace\\");
char KBtraceClassifications[KPrimaryFilterMaxSize] = {0};
unsigned KBtraceMode = 3;
unsigned KBtraceBufferSize = 1024;

void TLogger::SetLogFilename(const TDesC& aLogFilename)
	{
	iLogFilename = KLogFilePath;
	iLogFilename.Append(aLogFilename);
	}

TInt TLogger::DeleteLogfileL()
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	//don't leave even if there are errors
	fsSession.Connect(); 
	TInt error = fsSession.Delete(iLogFilename); 
	CleanupStack::PopAndDestroy();//fsSession
	return error;
	}

TInt TLogger::StartLoggingL(const TTraceConfigs aTraceData)
	{
	//BTrace attempt
	TInt error = KErrNone;
	if(iLogFilename.Compare(KLogFilePath)==0)
		return KErrNotFound; //no filename found
	
/*	error= iLogger.Open();
	if(error!=KErrNone)
		return error;//Couldn't open BTrace driver.

	iLogger.Empty(); // discard old data

	//prepare buffer and mode
	iLogger.ResizeBuffer(KBtraceBufferSize*1024);
	iLogger.SetMode(KBtraceMode);
	
	iLogger.SetFilter(aTraceData.iClassification,1);

	//Set secondary filters
	error = iLogger.SetFilter2(aTraceData.iComponentId,1); //enable all filter2
*/
	
	//Would like to re-write this to use btrace so we dont have a dependency
	error = iLogger.Connect();
	if(!error)
		{
		//this is rather annoying... just want to set one filter
		//I'd also actually rather have one enable and one disable method
		//same for filtering
		//TPluginConfigurations cant return it's values = quite annoying
		//Shouldn't have the restart method
		//StartServer should be RunServer
		//Start / Stop is clearly confusing, use StartOutputting - FinishOutputting or FlushBuffer?
		//Why can't I do?:
		//CArrayFixFlat<TUint8> getClassifications(8);
		//error = logger.GetEnabledClassifications(getClassifications);
		
		//Also want to change names of MOuptutPlugin and MInputPlugin to ConfigureUTF and RetrieveUTFData.
		//Can we rename ULogger? I'd like to...

		CArrayFixFlat<TUint8>* array = new (ELeave) CArrayFixFlat<TUint8>(4);
		CleanupStack::PushL(array);

		array->AppendL(aTraceData.iGroupId);
		error = iLogger.SetPrimaryFiltersEnabled(*array, ETrue);
		
		array->Reset();
		array->AppendL(3);
		array->AppendL(15);
		array->AppendL(13);
		
		iLogger.SetPrimaryFiltersEnabled(*array, EFalse);
		array->Reset();
		CleanupStack::PopAndDestroy(array);

		if(error == KErrNone)
			{
			RArray<TComponentId> compUids;
			compUids.Append(aTraceData.iComponentId);
			error = iLogger.SetSecondaryFiltersEnabled(compUids, ETrue);
			compUids.Close();
			if(error == KErrNone)
				{
				error = iLogger.SetSecondaryFilteringEnabled(ETrue);
				if(error == KErrNone)
					{
					error = iLogger.ActivateOutputPlugin(_L8("uloggerfileplugin"));
					if(error == KErrNone || error == KErrAlreadyExists)
						{
						TPluginConfiguration pluginConfig;
						pluginConfig.SetKey(_L("output_path"));
						pluginConfig.SetValue(iLogFilename);
						error = iLogger.SetPluginConfigurations(_L8("uloggerfileplugin"), pluginConfig);
						if(error == KErrNone)
							{
							error = iLogger.Start();
							}
						}
					}
				}
			}
		}

	return error;
	}
	
TInt TLogger::StopLogging()
	{
/*	BTrace attempt
 * 	for(TInt i = KPrimaryFilterMaxSize-1; i >= 0; i--)
		iLogger.SetFilter(i,0); // turn off all classifications
	TUint32* uids = NULL;
	TInt filtering = EFalse;
	TInt numberOfUids = iLogger.Filter2(uids, filtering);
	while(numberOfUids > 0)
		{
		iLogger.SetFilter2(uids[(numberOfUids-1)], 0);
		numberOfUids--;
		}
	delete uids;
	return DumpBTracesToFile();
	*/
	return iLogger.Stop();  
	}
/*
TInt TLogger::DumpBTracesToFile()
	{
	TUint8* data = NULL;
	TInt size = 0;
	TInt error = KErrNone;
	TUint oldMode = iLogger.Mode();
	iLogger.SetMode(0); // turn off trace capture while we dump

	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile file;
	error = file.Open(fsSession, iLogFilename, EFileWrite);
	if(error == KErrNotFound)
		error = file.Replace(fsSession,iLogFilename,EFileWrite);
	if(error == KErrNone)
		{
		while((size=iLogger.GetData(data))!=0)
			{
			error=file.Write(0xffff, TPtrC8(data,size));//append, don't overwrite
			if(error!=KErrNone)
				break; //Error writing to file.
			iLogger.DataUsed();
			}
		file.Flush(); 
		file.Close();
		}
	iLogger.SetMode(oldMode);
	return error;
	}
*/

TInt TLogger::GetDataL(TUint8*& aData, TUint& aSize)
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile file;
	TInt error = file.Open(fsSession, iLogFilename, EFileRead);
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
	
