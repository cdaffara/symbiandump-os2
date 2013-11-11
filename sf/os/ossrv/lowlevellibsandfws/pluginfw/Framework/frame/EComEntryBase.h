// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This class contains all the entry class which is used to register the 
// plugin.  The additional data and functionality is to take account of plugins
// that are discovered by their dll entry, rsc entry, and spi entry
// 
//

#ifndef _EComEntryBase_H
#define _EComEntryBase_H

#include <f32file.h>
#include <barsc2.h>
#include "RegistryData.h"

/*
CPluginBase is the base class for ECOM plugins discovered through different 
mechanism e.g through Dll, Rsc file or from Spi file.
*/
class CPluginBase:public CBase
{
public:
	CPluginBase();
	CResourceFile* RscFile() const{return iRscFile;}
	virtual ~CPluginBase();
	virtual HBufC* RscFileExt() const{	return NULL;}
public:
/**
iDllSecondUid: Identifies type of the Interface Implementation Collection. (collection or collection3)
			   It will be used by CLoadManager to decide how to initialise a plugin.	
*/
	TUid iDllSecondUid;
/**
iDllThirdUid: Identifies a component uniquely. 
*/
	TUid iDllThirdUid;
	
	TTime iDllModifiedTime;
	HBufC* iDllName;		
protected:
	CResourceFile* iRscFile;
};

//

//

class RResourceArchive;
/*
CSecurePlugin refers to the new secure ecom plugin that is discovered through 
resource file as a result of platform security. These resource files are stored
in <drive>\Resource\Plugins
*/
class CSecurePlugin :public CPluginBase
{
public:
	static CSecurePlugin* NewL(RFs& aFs,const TEntry& aEntry,const TDriveName& aDriveName, TBool aIsRO);
	virtual ~CSecurePlugin();
	HBufC* RscFileExt() const{	return iRscFileExt;}
private:
	CSecurePlugin(const TEntry& aEntry);
	void ConstructL(RFs& aFs,const TEntry& aEntry,const TDriveName& aDriveName, TBool aIsRO);
	HBufC* iRscFileExt;
};

//
/*
CSpiPlugin refers to ecom plugin that is discovered through reading the resource
archive(a spi file). The resource archive only applies to read only drive.
*/
class CSpiPlugin :public CPluginBase
{
public:
	static CSpiPlugin* NewL(RResourceArchive& aRscArchive);
private:
	CSpiPlugin();
	void ConstructL(RResourceArchive& aRscArchive);
	
};

#endif  //_EComEntryBase_H
