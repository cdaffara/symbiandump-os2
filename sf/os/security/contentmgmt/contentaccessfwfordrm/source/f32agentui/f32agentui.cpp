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


#include <e32base.h>
#include <caf/caferr.h>
#include <caf/f32agentui.h>

using namespace ContentAccess;

EXPORT_C CF32AgentUi* TF32AgentUiFactory::CreateF32AgentUiL()
	{
	User::Leave(KErrCANotSupported);
	return NULL;
	}


// DLL entry point - only for EKA1

