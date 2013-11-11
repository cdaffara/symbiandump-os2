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
* This contains CTEFLoggerTestWrapper
*
*/



/**
 @test
 @internalComponent
*/

#if (!defined __TEF_INTEGRATION_TEST_WRAPPER_H__)
#define __TEF_INTEGRATION_TEST_WRAPPER_H__


#include <test/datawrapper.h>

class CTEFLoggerTestWrapper : public CDataWrapper
	{
public:
	CTEFLoggerTestWrapper();
	~CTEFLoggerTestWrapper();
	
	static	CTEFLoggerTestWrapper*	NewL();

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
	inline void DoCmdModifyByName(const TDesC& aEntry);
	inline void DoCmdGetDataObject(const TDesC& aSection);
	inline void DoCmdLogSeverity(const TDesC& aEntry);
	inline void DoCmdPrint(const TDesC& aEntry);
	inline void DoCmdPrintNoVALIST(const TDesC& aEntry);
	void DoCmdKUnCoveredFuntions(const TDesC& aSection);
	void DoCmdKGetCPPModuleName();
	void GetLongSectionName(const TDesC& aSection);
	void GetLongKeyName(const TDesC& aSection);
	// utilities
	TBool GetVerdictFromConfig(const TDesC& aSection, const TDesC& aParameterName, TVerdict& aVerdict);

protected:
	CActiveCallback*	iActiveCallback;
	TInt*				iObject;
	RTimer*				iActiveObject;
private:
	TTime				iTimerStart;
	};

#endif /* __TEF_INTEGRATION_TEST_WRAPPER_H__ */

