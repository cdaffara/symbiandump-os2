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
//

/**
 @file
 @internalTechnology
 @prototype
*/


#ifndef ULOGGERSYSCONFIG_H
#define ULOGGERSYSCONFIG_H

#include <e32base.h> //CBase


namespace Ulogger
{

class CConfigSettingsImpl;
/**
ULogger Configuration settings iterator class
This class is used to iterate all the settings within a
section in the configuration store.
*/
class CConfigSettingsIter : public CBase
{
	friend class CConfigImpl;
public:
	IMPORT_C static CConfigSettingsIter* NewL();
	
	IMPORT_C ~CConfigSettingsIter();
		
	IMPORT_C TBool Next(TPtrC8& aSetting,TPtrC8& aSettingValue);
	
	IMPORT_C void Reset();
	
private:
	CConfigSettingsIter();
	CConfigSettingsImpl* iImpl;
};

class CConfigImpl;
/**
ULogger Configuration class
This class is provided to allow read and write access to
the configuration store. It also allows the client to be 
notified when there is a configuration change.
*/
class CConfig : public CBase
	{
public:
	IMPORT_C static CConfig* NewL(RHeap* aHeap,TFileName& aFilename);
	IMPORT_C static CConfig* NewLC(RHeap* aHeap,TFileName& aFilename);
	IMPORT_C ~CConfig();

	IMPORT_C TInt GetSectionValues(const TDesC8& aSectionName,CConfigSettingsIter& aIter);
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
	
	IMPORT_C TInt GetActivePlugins(CConfigSettingsIter& aIter);
		
	IMPORT_C TInt GetTraceSettings(CConfigSettingsIter& aIter);
	
	IMPORT_C TInt DeActivateOutputPlugin(const TDesC8& aMediaName);

	IMPORT_C TInt DeActivateInputPlugin(const TDesC8& aMediaName);
	
private:

	CConfig();
	CConfigImpl* iImpl;
	};
}
#endif //ULOGGERSYSCONFIG_H

