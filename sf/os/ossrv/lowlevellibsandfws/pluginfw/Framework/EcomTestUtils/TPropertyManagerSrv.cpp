// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32debug.h>
#include "TPropertyManager.h"

_LIT(KSeparator, "|"); // Char used to separate arguments

static TInt DefineProperty(TUid aCategory, TUint aKey, TInt aAttr,TInt aPreallocated)
	{
	_LIT_SECURE_ID(mySid,0X10204FC5);
	
	TSecurityPolicy readPolicy = TSecurityPolicy::EAlwaysPass;
	TSecurityPolicy writePolicy = TSecurityPolicy(mySid);

	TInt err = RProperty::Define(aCategory, aKey, aAttr, readPolicy, writePolicy, aPreallocated);
	
	//If the variable is already defined then return KErrNone as this is fine,
	if(err == KErrAlreadyExists)
		{
		err = KErrNone;	
		}
	
	RDebug::Print(_L("Property Manager - Define Property Cat = %d Key = %d err = %d\n"), aCategory,aKey, err);
		
	return err;
	}

static TInt DeleteProperty(TUid aCategory, TUint aKey)
	{
	TInt err = RProperty::Delete(aCategory, aKey);
	
	//If the variable is not found then return KErrNone as this is fine
	if(err == KErrNotFound)
		{
		err = KErrNone;	
		}
	
	RDebug::Print(_L("Property Manager - Delete Property Cat = %d Key = %d err = %d\n"), aCategory,aKey, err);
			
	return err;
	}

static TInt SetProperty(TUid aCategory, TUint aKey, TInt aValue)
	{
	TInt err = RProperty::Set(aCategory, aKey, aValue);
	RDebug::Print(_L("Property Manager - Set Property Cat = %d Key = %d Value = %d  err = %d\n"), aCategory,aKey,aValue,err);
	return err;
	}

static TInt SetProperty(TUid aCategory, TUint aKey, const TDesC8 &aValue)
	{
	TInt err = RProperty::Set(aCategory, aKey, aValue);
	RDebug::Print(_L("Property Manager - Set Property Cat = %d Key = %d Value = %S  err = %d\n"), aCategory,aKey,&aValue,err);
	return err;
	}

static TInt SetProperty(TUid aCategory, TUint aKey, const TDesC16 &aValue)
	{
	TInt err = RProperty::Set(aCategory, aKey, aValue);
	RDebug::Print(_L("Property Manager - Set Property Cat = %d Key = %d Value = %S  err = %d\n"), aCategory,aKey,&aValue,err);
	return err;
	}

TInt GetNumber(const TPtrC& aText, TInt& aLength)
	{
	TInt total = 0;
	aLength = 0;
	TInt textLength = aText.Length();
	
	//Loop through each character and verify that it is a valid digit
	//before converting to a decimal representation and adding to the total
	while (aLength < textLength)
		{
		TChar currentChar = aText[aLength];
		if (currentChar < '0' || currentChar > '9')
			{
			return total;
			}
		
		TInt digit = 0;
		TUint charValue = currentChar;
		
		//Convert from character value to decimal
		digit = charValue - 0x30; //(offset for western digit characters
		total = (total * 10) + digit;
		aLength++;
		}
	return total;
	}

static TInt ParseArgs(TDesC& aArgs, TInt& aStartIndex)
	{
	TInt length;
	TInt value;
	
	//Get a substrng of aArgs begining at aStartIndex
	TPtrC string = aArgs.Mid(aStartIndex,aArgs.Length() - aStartIndex);
	
	//Find the position of the first separator
	TInt pos = string.Find(KSeparator);
	
	if(pos > 0)
		{
		//Get the number that exists in the substring before the separator
		value = GetNumber(string.Mid(0,pos),length);	
		}
	//If a separator is not found, then this must be the last number in aArgs
	else
		{
		value = GetNumber(string,length);
		}

	//Update the start index to the character following the separator
	aStartIndex += (pos +1);
	
	return value;
	}

static TInt ProcessCommandLine(TDesC& aArgs)
	{
	TInt error;
	TInt pos = 0;
	TInt length = aArgs.Length();
		
	//Pull out the common elements of all commands - Op, Cat & Key
	PropertyManager::TOperation operation = PropertyManager::TOperation(ParseArgs(aArgs,pos));
	TInt category = ParseArgs(aArgs,pos);
	TUid categoryUid = TUid::Uid(category);
	TUint key =	ParseArgs(aArgs,pos);
	
	TInt attr;
	TInt preallocated;
	TInt intVal;
	TBuf8<64> value;
	
	//Handle each operation separately to pull out the remaining arguments
	//and process the request.
	switch(operation)
		{		
		case PropertyManager::EDefineProperty:
		attr = ParseArgs(aArgs,pos);
		preallocated = ParseArgs(aArgs,pos);
		error = DefineProperty(categoryUid,key,attr,preallocated);
		break;
		
		case PropertyManager::EDeleteProperty:
		error = DeleteProperty(categoryUid,key);
		break;
		
		case PropertyManager::ESetPropertyInt:
		intVal = ParseArgs(aArgs,pos);
		error = SetProperty(categoryUid,key,intVal);
		break;
		
		case PropertyManager::ESetPropertyDes8:
		value.Copy(aArgs.Mid(pos,(aArgs.Length()-pos)));
		error = SetProperty(categoryUid,key,value);
		break;
		
		case PropertyManager::ESetPropertyDes16:
		error = SetProperty(categoryUid,key,aArgs.Mid(pos,(aArgs.Length()-pos)));
		break;
		
		default:
		error = KErrArgument;
		break;
		}
	
	return error;
	}

GLDEF_C TInt E32Main()
    {
        	
    CTrapCleanup* cleanup = CTrapCleanup::New(); 

	TBuf<64> args;
	User::CommandLine(args);

	TInt error = ProcessCommandLine(args);
	
	delete cleanup;
	return error;	
    	
	}

