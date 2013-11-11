// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

const TText* S_Resource[SYS_PAGE_IS+1] =
	{
	_S(""),
	_S("+-*/<,>.\x1FNSWE"),		// explicit 0x1F character in the RSS file
	_S("%s"),
	_S("This item is not available"),
	_S("This item cannot be changed"),
	_S("Busy"),
	_S("Scanning"),
	_S("Printing to %s"),
	_S("(page %u)")
	};

_LIT(KRomResourceFile, "z:\\system\\data\\TRSC.RSC");
_LIT(KRomResourceFile2, "z:\\system\\data\\NewRscFormat.RSC");
_LIT(KRomResourceFileHeader, "z:\\system\\data\\RscHeader.bin");

// end of T_RSC.H
