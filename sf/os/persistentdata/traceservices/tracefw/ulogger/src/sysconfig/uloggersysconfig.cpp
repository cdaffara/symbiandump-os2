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


#include "uloggersysconfig.h"
#include "sysconfigimpl.h"
#include "uloggershared.h"
#include <e32base.h>		 


namespace Ulogger
{
	
/** Creates a sysconfig settings iterator object
@return a pointer to the created object
@leave KErrNoMemory if not enough memory available
*/
EXPORT_C CConfigSettingsIter* CConfigSettingsIter::NewL()
	{
	CConfigSettingsIter* self=new (ELeave)CConfigSettingsIter();
	CleanupStack::PushL(self);
	self->iImpl=CConfigSettingsImpl::NewL();
	CleanupStack::Pop();
	return self;
	}

/**
A function to loop through the settings in the system config
and return the setting and its value through the passed in pointer
@param aSetting a pointer to the buffered setting name
@param aSettingValue a pointer to the buffered setting value
@return ETrue if there are more setting
        EFalse if the iterator is at end of settings
		Other system wide errors
@post the iterator now points to the next setting
*/	
EXPORT_C TBool CConfigSettingsIter::Next(TPtrC8& aSetting,TPtrC8& aSettingValue)
	{
	return iImpl->Next(aSetting,aSettingValue);
	}

/**
Reset the iterator to point to the first setting
@post the iterator now points to the first setting
*/	
EXPORT_C void CConfigSettingsIter::Reset()
	{
	iImpl->Reset();
	}

/**
Public Destructor
*/	
EXPORT_C CConfigSettingsIter::~CConfigSettingsIter()
	{
	delete iImpl;
	}

CConfigSettingsIter::CConfigSettingsIter(){}

////////////////////////////////////////////////////////////////////////////

/**
Create a pointer to a CConfig object initialised with the current
configuration settings for the system wide framework.

Therefore it allocates and manages memory to store these settings.
If the client has a requirement to use a private heap then a reference to it
must be supplied should the notification funtionality be required 
as this will result in the settings being reloaded and hence memory
will need to be reallocated.
Clients with this requirement will need to ensure heaps are switch to ensure
the CConfig object is itself allocated on the correct heap.

@param aHeap a pointer to a private heap location
@param aFilename name of the configuration file with full path
@return a pointer to the new CConfig object
*/
EXPORT_C CConfig* CConfig::NewL(RHeap* aHeap,TFileName& aFilename)
	{
	CConfig* self= new(ELeave) CConfig();
	CleanupStack::PushL(self);
	self->iImpl=CConfigImpl::NewL(aHeap,aFilename);
	CleanupStack::Pop();
	return self;
	}

/**
Create a pointer to a CConfig object initialised with the current
configuration settings for the system wide framework. Places object
on the Cleanup stack.

Therefore it allocates and manages memory to store these settings.
If the client has a requirement to use a private heap then a reference to it
must be supplied should the notification funtionality be required 
as this will result in the settings being reloaded and hence memory
will need to be reallocated.
Clients with this requirement will need to ensure heaps are switch to ensure
the CConfig object is itself allocated on the correct heap.

@param aHeap a pointer to a private heap location
@param aFilename name of the configuration file with full path
@return a pointer to the new CConfig object
*/
EXPORT_C CConfig* CConfig::NewLC(RHeap* aHeap,TFileName& aFilename)
	{
	CConfig* self=CConfig::NewL(aHeap,aFilename);
	CleanupStack::PushL(self);
	return self;
	}


/**
Create and initialize a setting iterator to the list of output
channels
@param aIter reference to the setting iterator object
@return KErrNone if no error
        Other system wide errors
*/
EXPORT_C TInt CConfig::GetOutputPlugins(CConfigSettingsIter& aIter)
	{
	return iImpl->GetSection(KActiveSection,aIter);
	}	


EXPORT_C TInt CConfig::GetActivePlugins(CConfigSettingsIter& aIter)
	{
	return iImpl->GetSection(KActiveControlSection,aIter);
	}	


/**
Create and initialize a setting iterator to the list filters
@param aIter reference to the setting iterator object
@return KErrNone if no error
        Other system wide errors
*/
EXPORT_C TInt CConfig::GetActiveFilters(CConfigSettingsIter& aIter,TInt aFilter)
	{
	if(aFilter == 1)
		return iImpl->GetSection(KPrimaryFilterSection,aIter);
	else if(aFilter == 2)
		return iImpl->GetSection(KSecondaryFilterSection,aIter);
	else
		return KErrNone;
	}
	
EXPORT_C TInt CConfig::GetSectionValues(const TDesC8& aSectionName,CConfigSettingsIter& aIter)
	{
		return iImpl->GetSection(aSectionName,aIter);
	}	
	
	
/**
Removes output channel setting section from the configuration file
@param aOutputChanId the plugin name which has to be removed
@return KErrNone if no error, 
        KErrNotFound if output channel setting section does not exist
		Other System wide errors
*/
EXPORT_C TInt CConfig::RemovePluginSettings(const TDesC8& aChanId)
	{
	//
	// NEW CODE:
	//

	CConfigSettingsIter* iter = CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);

	TInt ret = iImpl->GetSection(aChanId, *iter);

	if (KErrNone == ret)
		{
		TPtrC8 key;
		TPtrC8 dummy;

		//remove all keys from section aChanId
		while(iter->Next(key, dummy))
			{
			iImpl->RemoveKey(aChanId, key);
			iter->Reset(); // otherwise we'd be skipping every 2nd element!
			}

		//remove section
		iImpl->RemoveSection(aChanId);
			//finally write into ini file
			ret = iImpl->PersistIniFile();
		}

	CleanupStack::PopAndDestroy(iter);

	return ret;
	}

/**
Set a particular channel id settings, create one if settings does not exist yet
@param aOutputChanId the channel id
@param aSetting the channel setting to modify or add
@param aValue the value to assign to this setting
@return KErrNone if no error, 
        KErrNotFound if no matching output id
		Other System wide errors
*/
EXPORT_C TInt CConfig::SetPluginSetting(const TDesC8& aChanId,const TDesC8& aSetting,const TDesC8& aValue)
	{
	TInt ret = iImpl->SetKeyValue(aChanId,aSetting,aValue);

	if (KErrNone == ret)
		{
		//finally write into ini file
		ret = iImpl->PersistIniFile();
		}

	return ret;
	}

/**
Register Active Plugin
An internal active name that maps to this plugin is generated internally
@param aPluginName the plugin to be added to the system.
@param aMediaName pointer to the internal media name
@return KErrNone if no error
        KErrAlreadyExists if the plugin name already exists
		Other system wide errors
*/
EXPORT_C TInt CConfig::SetActiveOutputPlugin(const TDesC8& aMediaName)
	{
	//first check whether plugin already registered
	TPtrC8 key;
	TPtrC8 val;
	TPtrC8 aMedia;
	
	TInt keyCount=0;
	TInt ret=iImpl->CheckValueExist(KActiveSection,aMediaName,keyCount);
	if(ret == KErrAlreadyExists)
		return ret;
	if ((ret==KErrNone && keyCount))
	{			
		CConfigSettingsIter* iter = CConfigSettingsIter::NewL();
		CleanupStack::PushL(iter);
		GetOutputPlugins(*iter);
		while(iter->Next(val,key))
		{
			ret=iImpl->RemoveKey(KActiveSection,val);	
		}
		CleanupStack::PopAndDestroy();		
	}
	
	//get the internally generated name
	TBuf8<15> internalName;
	ret=iImpl->GenerateInternalKey(KActiveSection,internalName);
	if (ret!=KErrNone)
		return ret;		
	
	ret=iImpl->SetKeyValue(KActiveSection,internalName,aMediaName);
	if (ret!=KErrNone)
		return ret;
	
	ret= iImpl->GetPointerToKeyName(KActiveSection,internalName,aMedia);
	if (ret!=KErrNone)
		return ret;
	
	//finally write into ini file
	return iImpl->PersistIniFile();
	}


EXPORT_C TInt CConfig::SetActiveInputPlugin(const TDesC8& aMediaName)
	{
	//first check whether plugin already registered
	TPtrC8 key;
	TPtrC8 val;
	TPtrC8 aMedia;
	
	TInt keyCount=0;
	TInt ret=iImpl->CheckValueExist(KActiveControlSection,aMediaName,keyCount);
	if(ret == KErrAlreadyExists)
		return ret;
	if ((ret==KErrNone && keyCount))
	{			
		CConfigSettingsIter* iter = CConfigSettingsIter::NewL();
		CleanupStack::PushL(iter);
		GetActivePlugins(*iter);
		while(iter->Next(val,key))
		{
			ret=iImpl->RemoveKey(KActiveControlSection,val);	
		}
		CleanupStack::PopAndDestroy();		
	}
	
	//get the internally generated name
	TBuf8<15> internalName;
	ret=iImpl->GenerateInternalKey(KActiveControlSection,internalName);
	if (ret!=KErrNone)
		return ret;		
	
	ret=iImpl->SetKeyValue(KActiveControlSection,internalName,aMediaName);
	if (ret!=KErrNone)
		return ret;
	
	ret= iImpl->GetPointerToKeyName(KActiveControlSection,internalName,aMedia);
	if (ret!=KErrNone)
		return ret;
	
	//finally write into ini file
	return iImpl->PersistIniFile();
	}


EXPORT_C TInt CConfig::DeActivateOutputPlugin(const TDesC8& aMediaName)
	{
	TPtrC8 key;
	TPtrC8 val;
	TPtrC8 aMedia;
	
	TInt keyCount=0;
	TInt ret=iImpl->CheckValueExist(KActiveSection,aMediaName,keyCount);
	if(ret==KErrNone)
		return KErrNotFound;
	else if (ret==KErrAlreadyExists)
		{			
		CConfigSettingsIter* iter = CConfigSettingsIter::NewL();
		CleanupStack::PushL(iter);
		User::LeaveIfError(GetOutputPlugins(*iter));
		while(iter->Next(val,key))
			{
			ret=iImpl->RemoveKey(KActiveSection,val);	
			}
		CleanupStack::PopAndDestroy();		
		}
	if(ret!=KErrNone)
		return ret;
	return iImpl->PersistIniFile();
	}


EXPORT_C TInt CConfig::DeActivateInputPlugin(const TDesC8& aMediaName)
	{
	TPtrC8 key;
	TPtrC8 val;
	TPtrC8 aMedia;
	
	TInt keyCount=0;
	TInt ret=iImpl->CheckValueExist(KActiveControlSection,aMediaName,keyCount);
	if(ret==KErrNone)
		return KErrNotFound;
	else if (ret==KErrAlreadyExists)
		{			
		CConfigSettingsIter* iter = CConfigSettingsIter::NewL();
		CleanupStack::PushL(iter);
		TInt a = GetActivePlugins(*iter);
		if(a != KErrNone)
			return a;
		while(iter->Next(val,key))
			{
			ret=iImpl->RemoveKey(KActiveControlSection,val);	
			}
		CleanupStack::PopAndDestroy();		
		}
	if(ret!=KErrNone)
		return ret;
	return iImpl->PersistIniFile();
	}


EXPORT_C TInt CConfig::SetActiveFilter(const RArray<TUint32>&  aFilter, const TDesC8 &aSectionName)
{
	TInt error = KErrNone;  
	
	for(TInt i = 0; i < aFilter.Count(); i++)
		{
		//copy the filter as a string
		HBufC8* filter = HBufC8::NewLC(32);
		filter->Des().Num(aFilter[i]);
		TPtr8 filterPtr(filter->Des());
		TInt keycount = 0;
		//check if the section exists
		error = iImpl->CheckValueExist(aSectionName, filterPtr, keycount);
		if (!error)//either the section or the value didn't exist
			{
			TBuf8<15> internalName;
			error = iImpl->GenerateInternalKey(aSectionName, internalName);
			if(!error)
				error = iImpl->SetKeyValue(aSectionName,internalName,filterPtr);
			}
		if(error == KErrAlreadyExists) //ignore these
			error = KErrNone;
		CleanupStack::PopAndDestroy();//filter			
		if(error)
			break;
		}
	//finally write into ini file
	iImpl->PersistIniFile();
	return error; 
}

EXPORT_C TInt CConfig:: SetTraceSettings(const TDesC8&  aValue, const TDesC8&  aSetting)
{	
	TPtrC8 aMedia;
	TPtrC8 value;
	//get the internally generated name
	
	TInt ret;
	if(aSetting.Compare(KBuffer) == 0)	
	{
		ret=iImpl->SetKeyValue(KTrace,KBuffer,aValue);
		if (ret!=KErrNone)
			return ret;
		
		ret= iImpl->GetPointerToKeyName(KTrace,KBuffer,aMedia);
		if (ret!=KErrNone)
			return ret;	
	}
	else if(aSetting.Compare(KSecondaryGlobalFilter) == 0)		
	{
		iImpl->GetKeyValue(KTrace,KSecondaryGlobalFilter,value);
		if(value.Compare(aValue)==0)
			return KErrNone;
		else
			{
			ret=iImpl->SetKeyValue(KTrace,KSecondaryGlobalFilter,aValue);
			if (ret!=KErrNone)
				return ret;
		
			ret= iImpl->GetPointerToKeyName(KTrace,KSecondaryGlobalFilter,aMedia);
			if (ret!=KErrNone)
				return ret;	
			}
	}
	else if(aSetting.Compare(KDataNotification) == 0)		
	{
		ret=iImpl->SetKeyValue(KTrace,KDataNotification,aValue);
		if (ret!=KErrNone)
			return ret;
		
		ret= iImpl->GetPointerToKeyName(KTrace,KDataNotification,aMedia);
		if (ret!=KErrNone)
			return ret;	
	}
	else if(aSetting.Compare(KBufferMode) == 0)		
	{
		ret=iImpl->SetKeyValue(KTrace,KBufferMode,aValue);
		if (ret!=KErrNone)
			return ret;
		
		ret= iImpl->GetPointerToKeyName(KTrace,KBufferMode,aMedia);
		if (ret!=KErrNone)
			return ret;	
	}
	else 
		return KErrNotFound;
	
	
	//finally write into ini file
	return iImpl->PersistIniFile();
		
}



EXPORT_C TInt CConfig:: GetTraceSettings(CConfigSettingsIter& aIter)
	{
	return iImpl->GetSection(KTrace,aIter);
	}	

/**
Remove a Filter based on its internal media name. It will fail if
any of this media is still referenced by one of the output channel
@param aMediaName the internal media name of the plugin to be removed
@return KErrNone if no error
        KErrNotFound if no matching media name
        KErrAccessDenied if still referenced by one of the output channel
*/

EXPORT_C TInt CConfig::RemoveActiveFilter(const RArray<TUint32>&  aFilter, const TInt &aFilterValue)
{
	TPtrC8 sectionName;
	TPtrC8 val;
	TPtrC8 key;
	TInt ret = KErrNotFound;
	
	if(aFilterValue == 1)
		sectionName.Set(KPrimaryFilterSection());
	else if(aFilterValue == 2)
		sectionName.Set(KSecondaryFilterSection());
	
	CConfigSettingsIter* iter = CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);
	GetActiveFilters(*iter, aFilterValue);
	
	for(TInt i=0; i<aFilter.Count();i++)
		{
		//ret = iImpl->GetSection(aSectionName,iter);
		HBufC8* filter = HBufC8::NewLC(32);
		filter->Des().Num(aFilter[i]);
		TPtr8 ptr(filter->Des());
		CleanupStack::Pop(1);
		
		while(iter->Next(val,key))
		{
			if (key.Compare(ptr)==0)
				{
				TInt tmpRet = iImpl->RemoveKey(sectionName,val);
				if(ret != KErrNone)
					ret = tmpRet;
				break;
				}				
		}
		iter->Reset();	
				
		}
	CleanupStack::PopAndDestroy();	
	//finally write into ini file
	iImpl->PersistIniFile();
	return ret;
		
}

/**
Public Destructor
*/
EXPORT_C CConfig::~CConfig()
	{
	delete iImpl;	
	}

//default constructor
CConfig::CConfig(){}

}

