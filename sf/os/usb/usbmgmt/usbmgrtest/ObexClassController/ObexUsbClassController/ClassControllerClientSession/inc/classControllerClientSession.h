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
*
*/


#ifndef CLASSCONTROLLERCLIENTSESSION_H
#define CLASSCONTROLLERCLIENTSESSION_H

#include <e32base.h>

/**
 * RClassControllerClient is a RSessionBase derived class.
 * It serves as the Client side of a Client and Server
 * configuration. Implementation of this class occurs within
 * the Obex Class Controller, it's member functions
 * cause messages to be sent to the server side code implemented in an 
 * executable process that start an Obex server.
 *
 */

class RClassControllerClient : public RSessionBase
	{
	public:
		IMPORT_C TInt Connect();
		IMPORT_C TInt StartService();
		IMPORT_C TInt StopService();
	};
#endif // CLASSCONTROLLERCLIENTSESSION_H

