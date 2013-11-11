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


#ifndef __LANGUTIL_H__
#define __LANGUTIL_H__

#include <e32std.h>
#include <bautils.h>



class LangUtil
{
public:
     static void GetDowngradePathL(const RFs& aFs, const TLanguage aCurrentLanguage, RArray<TLanguage>& aLanguageArray );
     static void GetEquivalentLanguageList(TLanguage aLang, TLanguagePath& aEquivalents);
     static TLanguage IdealLanguage();
     static void NearestLanguageFile(const RFs& aFs, TFileName& aName);
     static void NearestLanguageFile(const RFs& aFs, TFileName& aName, TLanguage& aLanguage);
     static void NearestLanguageFileV2(const RFs& aFs, TFileName& aName, TLanguage& aLanguage);
     static void ReleaseIdealLanguage();
     static TInt SetIdealLanguage(TLanguage aLanguage);

};
#endif // end of __LANGUTIL_H
