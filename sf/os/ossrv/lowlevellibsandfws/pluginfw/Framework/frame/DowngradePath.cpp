// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include <bautils.h>
#include "DowngradePath.h"
#include "FileUtils.h"

Languages RDowngradePath::iCurrentLanguage;

//overwrite the old array with values in the new array
void CopyLanguageArrayL(Languages& old_array,const Languages& new_array)
	{
	old_array.Reset();
	CleanupClosePushL(old_array);
	for (TInt i=0;i<new_array.Count();i++)
		{
		old_array.AppendL(new_array[i]);
		}
	CleanupStack::Pop();	
	}

void RDowngradePath::Reset()
	{
	iCurrentLanguage.Reset();
	}

/**
Get whether the current downgrade path has changed
@return ETrue if so, EFalse if not.
*/
TBool RDowngradePath::HasChangedL(RFs& aFs)
	{
	TBool hasChanged = EFalse;
	TInt currentLanguageCount=iCurrentLanguage.Count();
	
	//check current language settings
	if (currentLanguageCount==0)
		{
		CleanupClosePushL(iCurrentLanguage);
		//this is during the server boot time,store this settings
		BaflUtils::GetDowngradePathL(aFs,User::Language(),iCurrentLanguage);
		CleanupStack::Pop();
		return ETrue; 
		}
	else
		{
		//we need to check the new settings against current stored settings
		//this is likely the case during language change notification
		Languages newLanguage;
		CleanupClosePushL(newLanguage);
			
		// get current system downgrade path			
		BaflUtils::GetDowngradePathL(aFs, User::Language(), newLanguage);
	
		// path lengths match
		if (currentLanguageCount == newLanguage.Count())
			{
			TInt langs = newLanguage.Count();
			// compare each entry in path
			for (TInt i = 0; i < langs; ++i)
				{
				if (iCurrentLanguage[i] != newLanguage[i])
					{
					hasChanged = ETrue;
					CopyLanguageArrayL(iCurrentLanguage,newLanguage);
					break;
					}
				}
			}
		// path lengths don't match
		else
			{
			CopyLanguageArrayL(iCurrentLanguage,newLanguage);			
			hasChanged = ETrue;
			}

		CleanupStack::PopAndDestroy(&newLanguage);	// cleanup: newLanguage		
		}
	return hasChanged;
	}

