// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <textbase.h>
#include "TextBasePanic.h"
//#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS	
//#include <graphics/gdi/gdistructs.h>
//#include <graphics/gdi/gdiconsts.h>
//#endif //SYMBIAN_ENABLE_SPLIT_HEADERS	


// Global panic function

_LIT(KTextBasePanicCategory,"TextBase");

void TextBasePanic(TTextBasePanic aError)
	{
	User::Panic(KTextBasePanicCategory,aError);
	}

_LIT(KTextBasePanicDesc1, "TextBase internal Panic %S, in file %S @ line %i");
_LIT(KTextBasePanicDesc2, "Assert condition = \"%S\"");
_LIT(KTextBasePanicDesc3, "TextBase internal %S, in file %S @ line %i");

void TextBasePanicWithCondAndInfo(TTextBasePanic aError, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KTextBasePanicDesc1, &aPanicName, &aFileName, aLine);
	RDebug::Print(buf);

	buf.Format(KTextBasePanicDesc2, &aCondition);
	RDebug::Print(buf);
	TextBasePanic(aError);
	}
	
void TextBasePanicLogWithInfo(const TDesC& aCommand, const TDesC& aCondition, const TDesC& aFileName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KTextBasePanicDesc3, &aCommand, &aFileName, aLine);
	RDebug::Print(buf);
	
	buf.Format(KTextBasePanicDesc2, &aCondition);
	RDebug::Print(buf);
	}
