/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that instantiates a process and starts executing
*                wlan engine in it.
*
*/


#include <e32base.h>
#include "wlmserver.h"

/*#if defined (__WINS__) 
EXPORT_C TInt WinsMain()
    {
    return reinterpret_cast<TInt>(&CWlmServer::StartServerThread);
    }
#else*/
GLDEF_C TInt E32Main()
    {
	RSemaphore globStartSignal;
    globStartSignal.CreateGlobal(KWLMServerSemaphore,0);
    return CWlmServer::StartServerThread();
    }
//#endif // __WINS__

// End of File
