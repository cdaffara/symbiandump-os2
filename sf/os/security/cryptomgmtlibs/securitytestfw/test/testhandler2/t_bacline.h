/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_BACLINE_H
#define __T_BACLINE_H

#include <e32base.h>

// Not available until Platform 003

typedef TBuf<0x100> TBaCommand;

class CCommandLineArguments : public CBase
/** Parses command line arguments.

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

The main use of this class is in parsing the arguments of WINC command-line 
utilities.

This class is not intended for user derivation */
	{
public:
	// construct/destruct
	static CCommandLineArguments* NewLC();
	static CCommandLineArguments* NewL();
	~CCommandLineArguments();
	// extract
	TPtrC Arg(TInt aArg) const;
	TInt Count() const;
private:
	CCommandLineArguments();
	void ConstructL();
private:
	CArrayFixFlat<TPtrC>* iArgs;
	TBaCommand iCommandLine;
	TFileName iFileName;
	};

#endif
