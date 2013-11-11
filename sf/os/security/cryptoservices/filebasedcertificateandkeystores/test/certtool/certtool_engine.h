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


#ifndef __CERTTOOL_ENGINE_H_
#define __CERTTOOL_ENGINE_H_

#include <e32base.h>
#include <unifiedcertstore.h>

#include "certtool_commands.h"
#include "keytool_commands.h"

class CCertToolController;

class CCertToolEngine : public CActive
	{
public:
	static CCertToolEngine* NewLC(CCertToolController* aController);
	static CCertToolEngine* NewL(CCertToolController* aController);
	~CCertToolEngine();
	void ConstructL();	
		
public: // Actual commands
	void ListL(CKeyToolParameters* aParam);
	void ImportPrivateL(CKeyToolParameters* aParam);
	void RemovePrivateL(CKeyToolParameters* aParam);
	void ImportL(CKeyToolParameters* aParam);
	void RemoveL(CKeyToolParameters* aParam);
	void SetAppsL(CKeyToolParameters* aParam);
	void ListStoresL(CKeyToolParameters* aParam);	
	void AddAppsL(CKeyToolParameters* aParam);
	void RemoveAppsL(CKeyToolParameters* aParam);
	void DisplayUsageL(CKeyToolParameters* aParam);
	void SetManagerPolicyL(CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CCertToolEngine(CCertToolController* aController);
	void SetupUidCommandL(CKeyToolParameters* aParam);
		
private:
	enum 
		{		
		EInitialiseKeyStore,
		EInitialise,
		EExecuting,
		EDone
		} iState;
	enum 
		{
		EIdle,
		EList,
		EImport,
		ERemove,
		EAddApps,
		ESetApps,
		ERemoveApps,
		ESetPolicy
		} iCurrentAction;
		
	CCertToolCommand* iHandler;		
	CKeyToolParameters* iParam;
	CCertToolController* iController;
	
	RFs iFs;
	CUnifiedCertStore* iCertStore;

	RFs iFsKeyStore;
	CUnifiedKeyStore* iKeyStore;
	CKeyToolCommand* iKeyHandler;		
	CKeyToolController* iKeyController;
	
	CActiveScheduler* iScheduler;	
	};
	
#endif 

