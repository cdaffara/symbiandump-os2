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


#ifndef __KEYTOOL_ENGINE_H_
#define __KEYTOOL_ENGINE_H_

#include <e32base.h>
#include <unifiedkeystore.h>

#include "keytool_commands.h"

class CKeyToolController;

class CKeyToolEngine : public CActive
	{
public:
	static CKeyToolEngine* NewLC(CKeyToolController* aController);
	static CKeyToolEngine* NewL(CKeyToolController* aController);
	~CKeyToolEngine();
	void ConstructL();	
		
public: // Actual commands
	void ListL(CKeyToolParameters* aParam);
	void ImportL(CKeyToolParameters* aParam);
	void RemoveL(CKeyToolParameters* aParam);
	void DisplayUsageL(CKeyToolParameters* aParam);
	void ListStoresL(CKeyToolParameters* aParam);	
	void SetPolicyL(CKeyToolParameters* aParam);
#ifdef KEYTOOL
	void MigrateStoreL(CKeyToolParameters* aParam);
#endif // KEYTOOL
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

protected:
	void InitializeL();

private:
	CKeyToolEngine(CKeyToolController* aController);
		
private:
	enum 
		{		
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
		ESetPolicy,
#ifdef KEYTOOL
		EMigrateStore,
#endif // KEYTOOL
		} iCurrentAction;
		
	TBool iInitialized;
	TBool iActiveStarted;	
	CKeyToolCommand* iHandler;		
	CKeyToolParameters* iParam;
	CKeyToolController* iController;
	
	RFs iFs;
	CUnifiedKeyStore* iKeyStore;
	CActiveScheduler* iScheduler;	
#ifdef KEYTOOL
	CKeytoolMigrateStore* iMigrateStoreHandler;
#endif // KEYTOOL
	};
	
#endif 

