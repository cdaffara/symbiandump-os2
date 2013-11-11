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


#include "uloggerconfigmanager.h"
#include "uloggershared.h"
#include <e32base.h>		 
#include <f32file.h>

namespace Ulogger
{

EXPORT_C CConfigFileManager* CConfigFileManager::NewL()
	{
	CConfigFileManager* self = new (ELeave) CConfigFileManager;
	CleanupStack::PushL(self);
	self->iConfig = NULL;
	TInt error = self->ConstructL();
	CleanupStack::Pop();
	if(error)
		{
		delete self;
		self = NULL;
		}
	return self;
	}

TInt CConfigFileManager::ConstructL()
	{
	return InitializeFilesL();
	}

EXPORT_C TInt CConfigFileManager::RefreshConfigFiles()
	{
	return InitializeFilesL();
	}

TInt CConfigFileManager::InitializeFilesL()
	{
	//Load the respective configuration file
	TInt error = CheckIfFileExistsInPathL(KConfigFilename, KPublicConfigFilePath, iFilename);
	if (error == KErrNotFound || error == KErrPathNotFound)
		{
		error = CheckIfFileExistsInPathL(KConfigFilename, KPrivateConfigFilePath, iFilename);
		if((error != KErrNotFound) && (iFilename == KDefaultConfigFilePath))
			{
			error = CopyFileToSystemDriveL(iFilename);
			}
		}
	if(!error)
		{
		if(iConfig)
			delete iConfig;
		iConfig = CConfig::NewL(NULL, iFilename);
		}
	if(iConfig == NULL)
		error = KErrNotFound;
	return error;
	}

TInt CConfigFileManager::CheckIfFileExistsInPathL(const TDesC& aFilename, const TDesC& aPath, TFileName& aFullFilePath)
	{
	RFs fs;
	TParse fileParse;
	User::LeaveIfError(fs.Connect());
	TFindFile findfile(fs);
	TInt error = findfile.FindByDir(aFilename,aPath);
	if(error == KErrNone) //file is found, now set the aFullFilePath to the full path including drive
		{
		fileParse.Set(findfile.File(),NULL,NULL);
		aFullFilePath.Zero();
		aFullFilePath.Append(fileParse.FullName());
		}
	fs.Close();
	return error;
	}

/* Copy file to the System drives private path 
 * if the drive exists otherwise
 * create the drive and copy the file*/
TInt CConfigFileManager::CopyFileToSystemDriveL(TFileName &aFilePath)
	{
	TFileName fileName;
	TDriveName aSystemDrive;
	TDriveUnit driveunit(RFs::GetSystemDrive());
	aSystemDrive.Zero();
	aSystemDrive=driveunit.Name();
	fileName.Zero();
	fileName.Append(aSystemDrive);
	fileName.Append(KPrivateConfigFilePath);

	RFs fs;
	User::LeaveIfError(fs.Connect());

	TInt error= fs.MkDir(fileName);
	if(error==KErrNone || error== KErrAlreadyExists)
		{
		CFileMan* fMan = CFileMan::NewL(fs);
		CleanupStack::PushL(fMan);
		fileName.Append(KConfigFilename);
		User::LeaveIfError(fMan->Copy(KDefaultConfigFilePath, fileName, CFileMan::EOverWrite) );
		CleanupStack::PopAndDestroy(fMan);
		User::LeaveIfError(fs.SetAtt(fileName,0, KEntryAttReadOnly));
		}
	fs.Close();
	aFilePath.Zero();
	aFilePath.Append(fileName);
	return error;
	}


EXPORT_C TInt CConfigFileManager::GetSectionValues(const TDesC8& aSectionName,CConfigSettingsIter& aIter)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->GetSectionValues(aSectionName, aIter);
		}
	return error;
	}

EXPORT_C TInt CConfigFileManager::GetOutputPlugins(CConfigSettingsIter& aIter)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->GetOutputPlugins(aIter);
		}
	return error;
	}

EXPORT_C TInt CConfigFileManager::RemovePluginSettings(const TDesC8& aOutputChanId)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->RemovePluginSettings(aOutputChanId);
		}
	return error;	
	}
EXPORT_C TInt CConfigFileManager::GetActiveFilters(CConfigSettingsIter& aIter,TInt aFilter)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->GetActiveFilters(aIter, aFilter);
		}
	return error;	
	}

EXPORT_C TInt CConfigFileManager::RemoveActiveFilter(const RArray<TUint32>&  aFilter, const TInt &aFilterValue)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->RemoveActiveFilter(aFilter, aFilterValue);
		}
	return error;	
	}
//Get direct setting's value API													
EXPORT_C TInt CConfigFileManager::SetActiveFilter(const RArray<TUint32>& aFilter, const TDesC8 &aSectionName)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->SetActiveFilter(aFilter, aSectionName);
		}
	return error;	
	}
EXPORT_C TInt CConfigFileManager::SetTraceSettings(const TDesC8&  aValue, const TDesC8& aSetting)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->SetTraceSettings(aValue, aSetting);
		}
	return error;		
	}
EXPORT_C TInt CConfigFileManager::SetPluginSetting(const TDesC8& aOutputChanId,
								const TDesC8& aSetting,
								const TDesC8& aValue)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->SetPluginSetting(aOutputChanId, aSetting, aValue);
		}
	return error;		
	}
EXPORT_C TInt CConfigFileManager::SetActiveOutputPlugin(const TDesC8& aMediaName)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->SetActiveOutputPlugin(aMediaName);
		}
	return error;		
	}
EXPORT_C TInt CConfigFileManager::SetActiveInputPlugin(const TDesC8& aMediaName)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->SetActiveInputPlugin(aMediaName);
		}
	return error;			
	}
EXPORT_C TInt CConfigFileManager::GetActiveInputPlugins(CConfigSettingsIter& aIter)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->GetActivePlugins(aIter);
		}
	return error;			
	}
EXPORT_C TInt CConfigFileManager::GetPluginSettings(CConfigSettingsIter& aIter)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->GetTraceSettings(aIter);
		}
	return error;			
	}
EXPORT_C TInt CConfigFileManager::DeActivateOutputPlugin(const TDesC8& aMediaName)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->DeActivateOutputPlugin(aMediaName);
		}
	return error;			
	}
EXPORT_C TInt CConfigFileManager::DeActivateInputPlugin(const TDesC8& aMediaName)
	{
	TInt error = RefreshConfigFiles();
	if(!error)
		{
		error = iConfig->DeActivateInputPlugin(aMediaName);
		}
	return error;		
	}
}//namespace
