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
//

#include "UD_STD.H"
#include <f32file.h>
#include <e32uid.h>

/**
Security UID will be extracted from the format string and stored
in the place referenced by aUid parameter.
If uid name is presented in the format string, aUidName will be set to reference it.
*/
void ExtractUidAndName(const TDesC& aDbFormatStr, TUid& aPolicyUid, TPtrC& aUidName)
	{
	aPolicyUid = KNullUid;
	aUidName.Set(aDbFormatStr);
	TInt len = aDbFormatStr.Length();
	if(len >= KMaxUidName)
		{
		TLex lex(aDbFormatStr.Mid(len - KMaxUidName));
		if(TUint(lex.Get()) == '[' && 
		   lex.Val(*(TUint32*)&aPolicyUid, EHex) == KErrNone && 
		   TUint(lex.Get()) == ']' && 
		   lex.Eos())
			aUidName.Set(aDbFormatStr.Left(len - KMaxUidName));
		}
	}
