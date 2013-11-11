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
//

#include <e32uid.h>
#include <bautils.h>
#include <barsread2.h>
#include <baspi.h>
#include <ecom/ecomerrorcodes.h>

#include "Discoverer.h"
#include "EComEntryBase.h"
#include "EComUidCodes.h"
#include "DriveInfo.h"

const TInt KEcomResourceIndex=1;
const TInt KEComDllExtensionLength=4;

CPluginBase::CPluginBase():CBase()
{
}

CPluginBase::~CPluginBase()
{
	delete iDllName;
	delete iRscFile;
}

//
CSecurePlugin::CSecurePlugin(const TEntry& aEntry):CPluginBase()
{
	//only needs to copy the modified time here and construct the dll name
	iDllModifiedTime=aEntry.iModified;
}

CSecurePlugin* CSecurePlugin::NewL(RFs& aFs,const TEntry& aEntry,const TDriveName& aDriveName, TBool aIsRO)
{
	CSecurePlugin* self=new (ELeave) CSecurePlugin(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aEntry,aDriveName, aIsRO);
	CleanupStack::Pop();
	return self;
}

void CSecurePlugin::ConstructL(RFs& aFs,const TEntry& aEntry,const TDriveName& aDriveName, TBool aIsRO)
{
	TInt resourceNameOnlyLength=aEntry.iName.Length()-KExtensionLength;
	TPtrC resourceNameOnly(aEntry.iName.Left(resourceNameOnlyLength));

	//constructing the dll file name	
	iDllName=HBufC::NewL(KEComDllExtensionLength+resourceNameOnlyLength);
	TPtr namePtr=iDllName->Des();
	namePtr.Append(resourceNameOnly);
	namePtr.Append(KDllExtension);	

	//Constructing the rsc file name
	TFileName rscFileName;
	rscFileName.Append(aDriveName);
	rscFileName.Append(KResourcePlugins);
	rscFileName.Append(resourceNameOnly);
	rscFileName.Append(KRscExtension);

	//Construct the CResourceFile
	BaflUtils::NearestLanguageFile(aFs,rscFileName);
	iRscFile=CResourceFile::NewL(aFs,rscFileName,0,0);

	//Now get the secure id from resource and store in iDllThirdUid
	//In addition, store the correct Interface Implementation Collection Uid in iDllSecondUid
	RResourceReader theReader;
	theReader.OpenLC(iRscFile,KEcomResourceIndex);
	const TUid uid={theReader.ReadInt32L()};
	if (uid == KUidEComResourceFormatV3)		
		{
		// If the resource format is version 3, then the dll type must
		// be of type "plugin3".
		iDllSecondUid = KUidInterfaceImplementationCollection3;
		}
	else
		{
		iDllSecondUid = KUidInterfaceImplementationCollection;	
		}
			
	if (uid==KUidEComResourceFormatV2 || uid==KUidEComResourceFormatV3)
		{
		iDllThirdUid.iUid=theReader.ReadInt32L();
		}
	else
		iDllThirdUid=uid;

	//Construct resource file extension
	if(!aIsRO)
		{
		// rscFileName is sane so we can use TParsePtrC instead of TParse
		TParsePtrC fileNameParser(rscFileName);
		TPtrC fileNameParserPtr = fileNameParser.Ext();
		iRscFileExt = fileNameParserPtr.AllocL();
		}
	
	//Perform cleanup now
	CleanupStack::PopAndDestroy(&theReader);
}

CSecurePlugin::~CSecurePlugin()
{
	delete iRscFileExt;
}

//	
CSpiPlugin::CSpiPlugin():CPluginBase()
{
}

CSpiPlugin* CSpiPlugin::NewL(RResourceArchive& aRscArchive)
{
	CSpiPlugin* self=new (ELeave) CSpiPlugin();
	CleanupStack::PushL(self);
	self->ConstructL(aRscArchive);
	CleanupStack::Pop();
	return self;
}

void CSpiPlugin::ConstructL(RResourceArchive& aRscArchive)
{
	HBufC* resourceName = NULL;
	iRscFile=aRscArchive.NextL(resourceName);
	CleanupStack::PushL(resourceName);
	
	//Now set the iDllName
	iDllName=HBufC::NewL(KEComDllExtensionLength+resourceName->Length());
	TPtr namePtr(iDllName->Des());	
	namePtr.Append(*resourceName);
	namePtr.Append(KDllExtension);
		
	//Now get the secure id from resource and store the uid in iDllThirdUid
	RResourceReader theReader;
	theReader.OpenLC(iRscFile,KEcomResourceIndex);
	const TUid uid={theReader.ReadInt32L()};
	if (uid == KUidEComResourceFormatV3)		
		{
		// If the resource format is version 3, then the dll type must
		// be of type "plugin3".
		iDllSecondUid = KUidInterfaceImplementationCollection3;
		}
	else
		{
		iDllSecondUid = KUidInterfaceImplementationCollection;		
		}
	
	TUid dllUid;	
	if (uid==KUidEComResourceFormatV2 || uid==KUidEComResourceFormatV3)
		{
		dllUid.iUid=theReader.ReadInt32L();
		}
	else
		dllUid=uid;
	iDllThirdUid=dllUid;
	CleanupStack::PopAndDestroy(&theReader);
		
	//Now perform the necessary cleanup
	CleanupStack::PopAndDestroy(resourceName);
}



