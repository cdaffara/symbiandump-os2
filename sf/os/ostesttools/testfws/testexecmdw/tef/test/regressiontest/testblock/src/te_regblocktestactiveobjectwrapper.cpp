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
* This is the test class for TEF (PREQ1521).
*
*/



#include "te_regblocktestactiveobjectwrapper.h"

CTe_RegBlockTestActiveObjectWrapper::CTe_RegBlockTestActiveObjectWrapper()
	{
	}

CTe_RegBlockTestActiveObjectWrapper* CTe_RegBlockTestActiveObjectWrapper::NewL()
	{
	CTe_RegBlockTestActiveObjectWrapper*	ret = new (ELeave) CTe_RegBlockTestActiveObjectWrapper();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;	
	}

		
void CTe_RegBlockTestActiveObjectWrapper::ConstructL()
	{
	CTe_RegBlockTestWrapper::ConstructL();
	}
