/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#ifndef OMXXMLTESTWRAPPER_H
#define OMXXMLTESTWRAPPER_H

#include <test/datawrapper.h>

class COmxXmlTestRunner;

class COmxXmlTestWrapper : public CDataWrapper
	{
public:
	static COmxXmlTestWrapper* NewL();
	~COmxXmlTestWrapper();
	
	TBool DoCommandL(const TTEFFunction& aCommand,
			         const TTEFSectionName& aSection,
			         const TInt aAsyncErrorIndex);

	TAny* GetObject();

	void RunL(CActive* aActive, TInt aIndex);
	
private:
	COmxXmlTestWrapper();
	void ConstructL();
	
	void DoNewCmdL(const TDesC& aFilename);
	void DoRunTestCmdL(const TDesC& aScriptSection);
	
	CActiveCallback* iActiveCallback;
	COmxXmlTestRunner* iTestRunner;
	};

#endif //OMXXMLTESTWRAPPER_H
