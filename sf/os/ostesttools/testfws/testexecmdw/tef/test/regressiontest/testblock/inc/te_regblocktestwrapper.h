/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This contains CTEFIntegrationTestWrapper
*
*/



/**
 @test
 @internalComponent
*/

#if (!defined __TE_REG_BLOCKTEST_WRAPPER_H__)
#define __TE_REG_BLOCKTEST_WRAPPER_H__


#include <test/datawrapper.h>

class CTe_RegBlockTestWrapper : public CDataWrapper
	{
public:
	CTe_RegBlockTestWrapper();
	~CTe_RegBlockTestWrapper();
	
	static	CTe_RegBlockTestWrapper*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	virtual TAny*	GetObject() { return iObject; }
	inline virtual void	SetObjectL(TAny* aObject)
		{
		DestroyData();
		iObject	= static_cast<TInt*> (aObject);
		}
		
	inline virtual void	DisownObjectL() 
		{
		iObject = NULL;
		}

	void DestroyData()
		{
		delete iObject;
		iObject=NULL;
		}
		
	inline virtual TCleanupOperation CleanupOperation()
		{
		return CleanupOperation;
		}

	void RunL(CActive* aActive, TInt aIndex);
protected:
	void ConstructL();
private:
	static void CleanupOperation(TAny* aAny)
		{
		TInt* number = static_cast<TInt*>(aAny);
		delete number;
		}
	
	// commands
	inline void DoCmdNewL(const TDesC& aEntry);
	inline void DoCmdModify(const TDesC& aEntry);
	inline void DoCmdModifyByName(const TDesC& aEntry);
	inline void DoCmdSetBlockResult(const TDesC& aSection);
	inline void DoCmdStartTimer();
	inline void DoCmdCheckTimer(const TDesC& aSection);
	inline void DoCmdSetError(const TDesC& aSection);
	inline void DoCmdPanickingFunction(const TDesC& aSection);
	inline void DoCmdGetDataWrapper(const TDesC& aSection);
	inline void DoCmdSetDataObject(const TTEFSectionName& aSection);
	inline void DoCmdGetDataObject(const TDesC& aSection);
	inline void DoCmdDummyFunction();
	inline void DoCmdIncreaseObjectValue();
	inline void DoCmdAddActiveObjectToScheduler();
	inline void DoCmdActivateActiveObject(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void DoCmdCheckActiveObjectWorkIsComplete(const TDesC& aSection);
	inline void DoCmdCancelActiveObject();
	inline void DoCmdCallMeFirstMkDir(const TDesC& aSection);
	inline void DoCmdCallMeSecondRmDir(const TDesC& aSection);
	inline void DoCmdWriteIntToConfig(const TTEFSectionName& aSection);
	inline void DoCmdWriteBoolToConfig(const TTEFSectionName& aSection);
	inline void DoCmdWriteHexToConfig(const TTEFSectionName& aSection);
	inline void DoCmdWriteStringToConfig(const TTEFSectionName& aSection);
	inline void DoCmdWriteSharedData();
	inline void DoCmdReadSharedData();
	inline void DoCmdTestNullController(const TTEFSectionName& aSection);
	inline void DoCmdLeaveFunction(const TDesC& aSection);
	// utilities
	TBool GetVerdictFromConfig(const TDesC& aSection, const TDesC& aParameterName, TVerdict& aVerdict);

protected:
	CActiveCallback*	iActiveCallback;
	TInt*				iObject;
	RTimer*				iActiveObject;
private:
	TTime				iTimerStart;
	};

#endif /* __TE_REG_BLOCKTEST_WRAPPER_H__ */

