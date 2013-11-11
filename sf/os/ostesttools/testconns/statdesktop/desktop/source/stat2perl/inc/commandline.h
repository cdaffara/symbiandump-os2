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
* Description:     include file for Commandline access							*
*/





// Provides access to the command line
//

#ifndef COMMANDLINE_H
#define COMMANDLINE_H

class CommandLine
{
public:
	CommandLine();
	virtual ~CommandLine(){}

	char * GetProgramArgument(const unsigned int iIndex);
	unsigned int GetNumberOfArgs() {return iCount;}
	bool FindArgument(const char *pArg);
	char * GetArgumentBySwitch(const char *pSwitch, const bool bIncludeSwitch = false);
	char * GetArgumentString();

private:
	unsigned int iCount;
	char *pArguments[256];
	char *pCmdLine;
};

#endif
