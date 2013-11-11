/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifndef __TESTAGENTCONTENT_H__
#define __TESTAGENTCONTENT_H__

#include <f32file.h>
#include <caf/agentinterface.h>
#include "TestAgentFile.h"
#include "TestAgentDrmContent.h"

/**
Test agent implementation of the CAgentContent class used to browse
DCF files

*/
class CTestAgentContent : public ContentAccess::CAgentContent
	{
public:
	// Two phase constructor used when the file is opened 
	// using a file name
	static CTestAgentContent* NewL(const TDesC& aURI, ContentAccess::TContentShareMode aShareMode);
	static CTestAgentContent* NewLC(const TDesC& aURI, ContentAccess::TContentShareMode aShareMode);
		
	// Two phase constructor used when the file is opened with 
	// a file handle
	static CTestAgentContent* NewL(RFile& aFile);
	static CTestAgentContent* NewLC(RFile& aFile);

	virtual ~CTestAgentContent();

public: 
	// From CAgentContent
	virtual TInt OpenContainer(const TDesC& aUniqueId);
	virtual TInt CloseContainer();
	virtual void GetEmbeddedObjectsL(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject>& aArray);
	virtual void GetEmbeddedObjectsL(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject>& aArray, ContentAccess::TEmbeddedType aType);
	virtual TInt Search(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive);
	virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId);
	virtual TInt GetAttributeSet(ContentAccess::RAttributeSet& aAttributeSet, const TDesC& aUniqueId);
	virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId);
	virtual TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId);
	virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);
	virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
	virtual void NotifyStatusChange(ContentAccess::TEventMask aMask, TRequestStatus& aStatus, const TDesC& aUniqueId);
	virtual TInt CancelNotifyStatusChange(TRequestStatus& aStatus, const TDesC& aUniqueId);
	virtual void RequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);
	virtual TInt CancelRequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);
	virtual void DisplayInfoL(ContentAccess::TDisplayInfo aInfo, const TDesC& aUniqueId);
	virtual TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);

private:
	CTestAgentContent();
	void ConstructL(const TDesC& aURI, ContentAccess::TContentShareMode aShareMode);
	void ConstructL(RFile& aFile);
	void SearchL(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive);
		
private:
	/** Handle to the filesystem */
	RFs iFs;
		
	/** File handle used to browse the DCF file*/
	RFile iFile;

	CTestAgentFile *iTestFile;
	CTestAgentDrmContent *iTestDrmContent;

	HBufC *iURI;
	ContentAccess::TContentShareMode iShareMode;
	TBool ConstructedWithFileHandle;
	TBool iDummyContainerOpen;
	};
#endif // __TestAgentContent_H__
