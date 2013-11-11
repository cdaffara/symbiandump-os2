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
* This is the test class for TEF (PREQ10437).
*
*/



#include "tefloggertestactiveobjectwrapper.h"

CTEFLoggerTestActiveObjectWrapper::CTEFLoggerTestActiveObjectWrapper()
	{
	}

CTEFLoggerTestActiveObjectWrapper* CTEFLoggerTestActiveObjectWrapper::NewL()
	{
	CTEFLoggerTestActiveObjectWrapper*	ret = new (ELeave) CTEFLoggerTestActiveObjectWrapper();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;	
	}

		
void CTEFLoggerTestActiveObjectWrapper::ConstructL()
	{
	CTEFLoggerTestWrapper::ConstructL();
	}
