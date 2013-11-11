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
// ULogger sys config
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/


#ifndef SYSCONFIGIMPL_H
#define SYSCONFIGIMPL_H

#include <e32cmn.h>		 
#include <f32file.h>	 
#include "uloggersysconfig.h"
#include <bsul/inifile.h>
#include "uloggershared.h"



namespace Ulogger
{
	
NONSHARABLE_CLASS(CConfigSettingsImpl) : public CBase
{
public:
	/**Standard Symbian OS construction method.*/
	static CConfigSettingsImpl* NewL();

	TBool Next(TPtrC8& aSetting,TPtrC8& aSettingValue);
	
	void Reset();
	/**Destructor.*/
	~CConfigSettingsImpl();
private:
	friend class CConfigImpl;
	CConfigSettingsImpl();
	BSUL::CIniSecIter8* iIniSecIter;
};


NONSHARABLE_CLASS(CConfigImpl) : public CBase
	{
public:
	static CConfigImpl* NewL(RHeap* aHeap,TFileName& aFilename);
	~CConfigImpl();
	
	// Allow Objects to register for configuration change notifications
	
	//Get operation									 
	TInt GetKeyValue(const TDesC8& aSectionName,const TDesC8& aKey,TPtrC8& aValue);
	TInt GetSection(const TDesC8& aSectionName,CConfigSettingsIter& aSection);	
	TInt GetKeyCount(const TDesC8& aSectionName,TPtrC8& aLastKey);
	
	//modify operation	
	TInt SetKeyValue(const TDesC8& aSectionName,const TDesC8& aKey, const TDesC8& aValue);
	TInt RemoveKey(const TDesC8& aSectionName,const TDesC8& aKeyName);
	TInt RemoveSection(const TDesC8& aSectionName);

	//utils
	TInt GetPointerToKeyName(const TDesC8& aSectionName,const TDesC8& aKeyName,TPtrC8& aKeyPointer);
	TInt CheckValueExist(const TDesC8& aSectionName,const TDesC8& aValue,TInt& aKeyCount);
	
	TInt PersistIniFile();
	TInt GenerateInternalKey(const TDesC8& aSection,TBuf8<15>& aKeyName);	
	void GetSystemDrive(TDriveName& aSystemDrive);
	
private:
	CConfigImpl(RHeap* aHeap,TFileName& aFileName) : 
		iHeap(aHeap),iFileName(aFileName) {}
	void ConstructL();	
	void LoadConfigFileL();
	void GetConfigFileName(TFileName& aFileName);
	
private:	
	/** Mutex for controlling write access */
	RMutex iWriteMutex;
	/** Pointer to private heap */
	RHeap* iHeap;
	/** Handle to file server */
	RFs iFs;
	/** Ini-file parser */
	BSUL::CIniDocument8* iIniFileDocument;
	
	TFileName iFileName;
	};

	
}
#endif // SYSCONFIGIMPL_H

