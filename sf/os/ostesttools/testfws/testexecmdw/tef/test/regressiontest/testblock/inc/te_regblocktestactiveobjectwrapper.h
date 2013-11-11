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
* This contains CTEFIntegrationTestActiveObjectWrapper
*
*/



/**
 @test
 @internalComponent
*/

#if (!defined __TE_REG_BLOCKTEST_ACTIVE_OBJECT_WRAPPER_H__)
#define __TE_REG_BLOCKTEST_ACTIVE_OBJECT_WRAPPER_H__



#include "te_regblocktestwrapper.h"

class CTe_RegBlockTestActiveObjectWrapper : public CTe_RegBlockTestWrapper
	{
public:
	CTe_RegBlockTestActiveObjectWrapper();
	
	static	CTe_RegBlockTestActiveObjectWrapper*	NewL();

	virtual TAny*	GetObject() { return iActiveObject; }
	inline virtual void	SetObjectL(TAny* aObject)
		{
		DestroyData();
		iActiveObject	= static_cast<RTimer*> (aObject);
		}
		
	inline virtual void	DisownObjectL() 
		{
		iActiveObject = NULL;
		}

	void DestroyData()
		{
		delete iActiveObject;
		iActiveObject=NULL;
		}

	inline virtual TCleanupOperation CleanupOperation()
		{
		return CleanupOperation;
		}

private:
	void ConstructL();

	static void CleanupOperation(TAny* aAny)
		{
		RTimer* timer = static_cast<RTimer*>(aAny);
		timer->Cancel();
		timer->Close();
		delete timer;
		}
	};

#endif /* __TE_REG_BLOCKTEST_ACTIVE_OBJECT_WRAPPER_H__ */

