/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Class declarations for User Prompt Service test/reference notifier implementation.
*
*/


/**
 @file
 @internalComponent
 @test
*/

#ifndef TUPSNOTIFIER_H
#define TUPSNOTIFIER_H

#include <e32base.h>
#include <techview/eikdialg.h>		
#include <bautils.h>
#include <eikenv.h>
#include <eiknotapi.h>
#include <ecom/implementationproxy.h>

#include <tups_notifierapp.rsg>
#include "tups_notifierapp.hrh"
#include <ups/upsnotifierutil.h>

const TInt32 KUpsTestNotifierImplementation(0xE1111F11);
const TInt KDisplayCharWidth = 50;

IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);

/**
Test/reference implementation of a User Prompt Service notifier /
*/
NONSHARABLE_CLASS(CUpsTestNotifier) : public CBase, public MEikSrvNotifierBase2
	{
public:
	static CUpsTestNotifier* NewL();

protected:
	// From MEikSrvNotifierBase2	
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);

private:
	CUpsTestNotifier();
	void ConstructL();
	void CreateAndDisplayDialogL(const RMessagePtr2 &aMessage, TInt aReplySlot,
		const UserPromptService::CPromptData& aPromptData);
	void DoStartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	TBool CheckScriptedResponseL(UserPromptService::TPromptResult& aResult);
	
	TNotifierInfo iInfo;
	CEikonEnv* iEikEnv;
	TInt iResourceFileOffset;
	};

/**
Dialog for the user security prompt
 */
NONSHARABLE_CLASS(CUpsTestNotifierDialog) : public CEikDialog
	{
public:
	CUpsTestNotifierDialog(const RMessagePtr2 &aMessage, TInt aReplySlot,
						  const UserPromptService::CPromptData& aPromptData);

protected:
	// from CEikDialog
	void PreLayoutDynInitL();
	void AppendTextWithEllipsisL(const TDesC& aTag, CEikLabel* aLabel, const TDesC& aText);
	HBufC* GetServiceNameLC();
	void SetActiveButtonsL();

	TBool OkToExitL(TInt aButtonId);
	
private:
	RMessage2 iMessage;
	TInt iReplySlot;
	const UserPromptService::CPromptData& iPromptData;
	};

#endif // TUPSNOTIFIER_H
