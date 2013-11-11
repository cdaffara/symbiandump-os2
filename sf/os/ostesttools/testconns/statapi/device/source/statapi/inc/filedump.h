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



#ifndef FILEDUMP_H
#define FILEDUMP_H

#include <e32base.h>
#include <f32file.h>
#include <e32cons.h>

#include "stat_interfaces.h"

#define ST_MAXMESSAGELENGTH		510

class FileDump : public MNotifyLogMessage
{
public:
	FileDump():bInitialised(EFalse), pConsole(NULL){}

	// from MNotifyLogMessage
	GLDEF_C TInt Init(RFs &fsSession, TPtrC16 filename, CConsoleBase* console);
	GLDEF_C TBool IsInitialised() const;
	GLDEF_C void Msg(TPtrC16 text, ...);
	GLDEF_C void CloseFile();

private:
	TBool bInitialised;
	RFile file;
	CConsoleBase* pConsole;
	TBuf16<ST_MAXMESSAGELENGTH + 2> buf;	// temp buffer

	GLDEF_C TBool WriteBuffer16ToFile(TDesC16 &buffer, RFile &handle);
};

#endif
