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
// ULogger config
//



/**
 @file
 @internalTechnology
 @prototype
*/
#ifndef ULOGGERCONFIGMANAGER_H_
#define ULOGGERCONFIGMANAGER_H_
#include "uloggersysconfig.h"

namespace Ulogger
	{
	
	class CConfigFileManager : public CBase
	{
	public:
		IMPORT_C static CConfigFileManager* NewL();
		IMPORT_C TInt RefreshConfigFiles();
		IMPORT_C TInt GetSectionValues(const TDesC8& aSectionName, CConfigSettingsIter& aIter);
		IMPORT_C TInt GetOutputPlugins(CConfigSettingsIter& aIter);
		IMPORT_C TInt RemovePluginSettings(const TDesC8& aOutputChanId);
		IMPORT_C TInt GetActiveFilters(CConfigSettingsIter& aIter,TInt aFilter);
		IMPORT_C TInt RemoveActiveFilter(const RArray<TUint32>&  aFilter, const TInt &aFilterValue);
		//Get direct setting's value API
		IMPORT_C TInt SetActiveFilter(const RArray<TUint32>& aFilter, const TDesC8 &aSectionName);
		IMPORT_C TInt SetTraceSettings(const TDesC8&  aValue, const TDesC8& aSetting);
		IMPORT_C TInt SetPluginSetting(const TDesC8& aOutputChanId,
										const TDesC8& aSetting,
										const TDesC8& aValue);
		IMPORT_C TInt SetActiveOutputPlugin(const TDesC8& aMediaName);
		IMPORT_C TInt SetActiveInputPlugin(const TDesC8& aMediaName);
		IMPORT_C TInt GetActiveInputPlugins(CConfigSettingsIter& aIter);
		IMPORT_C TInt GetPluginSettings(CConfigSettingsIter& aIter);
		IMPORT_C TInt DeActivateOutputPlugin(const TDesC8& aMediaName);
		IMPORT_C TInt DeActivateInputPlugin(const TDesC8& aMediaName);
	private:
		TInt ConstructL();
		TInt InitializeFilesL();
		TInt CheckIfFileExistsInPathL(const TDesC& aFilename, const TDesC& aPath, TFileName& aFullFilePath);
		TInt CopyFileToSystemDriveL(TFileName &aFilePath);
	private:
		CConfig*	iConfig;
		TFileName iFilename;
	};

}//namespace
#endif /*ULOGGERCONFIGMANAGER_H_*/
