/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <apgcli.h>
#include <f32file.h>
#include <charconv.h> 
#include <e32cmn.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include <caf/virtualpath.h>
#include <caf/caferr.h>
#include "testagentattributes.h"


using namespace ContentAccess;

_LIT(KTestAgentPrivateDir, "c:\\private\\12345678\\");
_LIT(KDummyPrivateDir, "c:\\tcaf\\testdata\\TestAgentPrivateDir\\");

void TTestAgentAttributes::GetDummyFileName(const TDesC& aFileName, TDes& aDummyFileName)
	{
	if(aFileName.Left(KTestAgentPrivateDir().Length()) == KTestAgentPrivateDir)
		{
		aDummyFileName.Copy(KDummyPrivateDir());
		aDummyFileName.Append(aFileName.Right(aFileName.Length() - KTestAgentPrivateDir().Length()));
		}
	else
		{
		aDummyFileName.Copy(aFileName);		
		}
	}

TInt TTestAgentAttributes::GetAttributeSet(CTestAgentFileInterface& aFile, RAttributeSet& aAttributeSet) 
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt value = 0;
	TInt err = KErrNone;
	TInt numAttributes = aAttributeSet.Count();
	
	// loop through all the attriutes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		attribute = aAttributeSet[i];
		err = aFile.GetAttribute(attribute, value);
		aAttributeSet.SetValue(attribute, value, err);
		}	
	return KErrNone;
	}


TInt TTestAgentAttributes::GetStringAttributeSet(CTestAgentFileInterface& aFile, RStringAttributeSet& aStringAttributeSet)
	{
	TInt i = 0;
	TInt attribute = 0;
	TInt err = KErrNone;
	
	// maybe this should be a CAF defined length
	TBuf <KMaxDataTypeLength> buf;

	TInt numAttributes = aStringAttributeSet.Count();

	// loop through all the attriutes in the set and find their values
	for(i = 0; i < numAttributes; i++)
		{
		attribute = aStringAttributeSet[i];
		err = aFile.GetStringAttribute(attribute, buf);
		err = aStringAttributeSet.SetValue(attribute, buf, err);
		if(err != KErrNone)
			{
			break;
			}
		}	
	return err;
	}

TInt TTestAgentAttributes::CheckUniqueId(const TDesC& aUniqueId)
	{
	// Allow the followig
	// a zero length descriptor which is the entire DCF file
	// or KDefaultContentObject which is the content object (OMA 1.0)
	// of the CID?
	if(aUniqueId.Length() == 0 || aUniqueId == KDefaultContentObject())
		{
		return KErrNone;	
		}
	else 
		{
		return KErrNotFound;	
		}
	}
	
TInt TTestAgentAttributes::CheckVirtualPath(const TVirtualPathPtr& aVirtualPath)
	{
	// check the Unique Id
	TInt err = CheckUniqueId(aVirtualPath.UniqueId());
	
	// check if the file exists
	
	
	return err;
	}	
