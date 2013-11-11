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
// TCmdLinePrm structure
// 
//

#ifndef __CN_MAIN_H__
#define __CN_MAIN_H__

#include <e32std.h>

/**
TCmdLinePrm structure represents parsed command line arguments - 
requested action (input, output), text file path, binary file path.
@internalComponent
*/
struct TCmdLinePrm
	{
	void Zero()
		{
		iAction = ENone;
		iTxtFile.Zero();
		iBinFile.Zero();
		}
	typedef enum {ENone, ETxt2Bin, EBin2Txt} TAction;//Requested action type
	TAction		iAction;//ETxt2Bin: TXT->BIN, or EBin2Txt: BIN->TXT (not implemented)
	TFileName	iTxtFile;//Text policy file path
	TFileName	iBinFile;//Binary policy file path
	};

#endif//__CN_MAIN_H__
