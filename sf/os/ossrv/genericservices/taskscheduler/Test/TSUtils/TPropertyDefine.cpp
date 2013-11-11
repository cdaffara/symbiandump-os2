// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32property.h>
#include <numberconversion.h>
#include <swi/swispubsubdefs.h>

using namespace Swi;

_LIT(KSeparator, "|"); // Invalid filepath char used to separate filenames

	
//This function defines a P&S variable as requested by the parameters
static TInt CreateVariableL(TUid aCategory, TInt aKey, TInt aAttr)
	{
	TSecurityPolicy readPolicy = TSecurityPolicy::EAlwaysPass;
	TSecurityPolicy writePolicy = TSecurityPolicy::EAlwaysPass;
	
	TInt err = RProperty::Define(aCategory, aKey, aAttr, readPolicy, writePolicy);
	
	if(err != KErrAlreadyExists)
		{
		//This will leave for any value other than KErrNone
		User::LeaveIfError(err);	
		}
		
	//If the variable is already defined then return KErrNone as this is fine,
	return KErrNone;
	}

GLDEF_C TInt E32Main()
    {
    	
    CTrapCleanup* cleanup = CTrapCleanup::New(); 
	
	TDigitType digitType;
	TInt length;
	TBuf<32> args;
	User::CommandLine(args);
	TInt error;
	
	TInt pos = args.Find(KSeparator);
	
	if(pos > 0)
		{
		TInt32 category = NumberConversion::ConvertFirstNumber(args.Mid(0,pos), length, digitType);
		TUid categoryUid = TUid::Uid(category);
		TPtrC remainder = args.Mid(pos+1, args.Length()-(pos+1));
		pos = remainder.Find(KSeparator);
		TUint key = NumberConversion::ConvertFirstNumber(remainder.Mid(0,pos), length, digitType);
		TInt attr =	NumberConversion::ConvertFirstNumber(
			remainder.Mid(pos+1, remainder.Length()-(pos+1)), length, digitType
			);
		
		//create the appropriate variable
		error = CreateVariableL(categoryUid, key, attr);			
		}
		
	else error = pos;

	delete cleanup;
	return error;	
    	
	}
