// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CCommandLineArguments class
// 
//

#ifndef __CN_CMDPARSE_H__
#define __CN_CMDPARSE_H__

#include <e32base.h>

/* Parses command line arguments.

The class provides functions to access the arguments that are supplied when 
a program is launched as a new process. 

The program name is returned as argument 0. Other arguments are returned as 
arguments 1, 2 etc. 

The Count() function indicates how many arguments there are, including the 
program name. Arguments may be quoted to contain blanks and quotes.

The command line arguments and process name occupy 256 characters each. In 
order to minimise the space used throughout the lifetime of a program, it 
is recommended that the program parse the arguments shortly after initialisation, 
save the argument values appropriately, and then destroy the CCommandLineArguments 
object.

The main use of this class is in parsing the arguments of command-line 
utilities. 
@internalComponent
*/
class CCommandLineArguments : public CBase
	{
public:
	static CCommandLineArguments* NewLC();
	static CCommandLineArguments* NewL();
	virtual ~CCommandLineArguments();
	TPtrC Arg(TInt aArg) const;
	TInt Count() const;

private:
	inline CCommandLineArguments();
	void ConstructL();

private:
	CArrayFixFlat<TPtrC>* iArgs;
	HBufC* iCommandLine;
	TFileName iFileName;

	};

#endif//__CN_CMDPARSE_H__
