// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Abstraction for "an environment" holding pairs of C strings (name, value)
// 
//

#include "ENVIRON.H"
#include "LPOSIX.H"
#include <string.h>

TPtrC16 TEnvVar::ValuePtr(const wchar_t* aValue)
//
// Turn a zero-terminated string into a descriptor which contains the terminator
//
	{
	TText16* value = (TText16*)aValue;
	return TPtrC16(value, User::StringLength(value)+1);
	}

void TEnvVar::ConstructL(const TDesC16& aName, const wchar_t* aValue)
	{
	TPtrC16 valueZ = ValuePtr(aValue);
	HBufC16* valueCopy = valueZ.AllocLC();
	iName = aName.AllocL();
	iValue = valueCopy;
	CleanupStack::Pop();
	}

void TEnvVar::Release()
	{
	delete iName;
	iName=0;
	delete iValue;
	iValue=0;
	}

TInt TEnvVar::SetValue(const wchar_t* aValue)
//
// Change the value - if this returns an error then the original value is still intact
//
	{
	TPtrC16 valueZ = ValuePtr(aValue);
	HBufC16* valueCopy=valueZ.Alloc();
	if (valueCopy==0)
		return KErrNoMemory;
	delete iValue;
	iValue = valueCopy;
	return KErrNone;
	}


HBufC16* TEnvVar::Match(const TDesC16& aName)
//
// return value if the name matches
//
	{
	if (iName!=0 && iName->Compare(aName)==0)
		return iValue;
	return 0;
	}

TUint TEnvVar::Length() const
//
// How much space do we need to externalize this pair?
// The format is name followed by '\0' followed by value followed by '\0'
//
	{
	if (iName==0) 
		return 0;
	return iName->Length()+iValue->Length()+2;
	}

TInt TEnvVar::Externalize(const wchar_t* aPair, TDes16& aBuffer)
//
// static function to externalize a "name=value" definition
//
	{
	const wchar_t* cp=aPair;
	for (cp=aPair; *cp; ++cp)
		{
		if (*cp==L'=')
			break;
		}
	if (*cp!=L'=')
		return 0;	// malformed

	TPtrC16 name((TText16*)aPair,cp-aPair);
	TPtrC16 value((TText16*)(cp+1));
	aBuffer.Append(name);
	aBuffer.Append(TChar(0));
	aBuffer.Append(value);
	aBuffer.Append(TChar(0));
	return 1;
	}

TInt TEnvVar::Externalize(TDes16& aBuffer)
	{
	if (iName==0) 
		return 0;
	aBuffer.Append(*iName);
	aBuffer.Append(TChar(0));
	aBuffer.Append(*iValue);
	aBuffer.Append(TChar(0));
	return 1;
	}

void TEnvVar::ConstructL(const TText16*& aPtr)
	{
	TPtrC16 name(aPtr);
	aPtr+=name.Length()+1;
	ConstructL(name,(const wchar_t*)aPtr);
	aPtr+=iValue->Length()+1;
	}

CEnvironment::~CEnvironment()
	{
	while (iCount>0)
		{
		--iCount;
		iVars[iCount].Release();
		}
	User::Free(iVars);
	}


void CEnvironment::ConstructL(TUint aCount, TDes16& aBuffer)
//
// Set up the environment from a descriptor. If this leaves then
// the CEnvironment destructor will be able to clean up properly.
//
	{
	// always allocate at least one slot - makes life easier elsewhere
	TInt bytes = (aCount+1)*sizeof(TEnvVar);
	iVars=(TEnvVar*) User::AllocL(bytes);
	Mem::FillZ(iVars,bytes);
	iCount=aCount+1;

	const TText16* data=aBuffer.Ptr();
	for (TUint i=0; i<aCount; ++i)
		iVars[i].ConstructL(data);
	}

HBufC16* CEnvironment::ExternalizeLC(TUint& aCount, wchar_t** envp)
	{
	if (envp==0)
		return ExternalizeLC(aCount);	// get current environment
	// Externalize supplied environment
	TInt length=0;
	wchar_t** ep=envp;
	for (ep=envp; *ep; ++ep)
		length+=wcslen(*ep)+1;
	HBufC16* retBuf = HBufC16::NewLC(length);
	TInt nvars=0;
	TPtr16 hbuf16=retBuf->Des();
	for (ep=envp; *ep; ++ep)
		nvars+=TEnvVar::Externalize(*ep, hbuf16);
	aCount=nvars;
	return retBuf;
	}

HBufC16* CEnvironment::ExternalizeLC(TUint& aCount)
	{
	TInt length=0;
	TUint i=0;
	for (i=0; i<iCount; ++i)
		length+=iVars[i].Length();
	HBufC16* retBuf = HBufC16::NewLC(length);
	TInt nvars=0;
	TPtr16 hbuf16=retBuf->Des();
	for (i=0; i<iCount; i++)
		nvars+=iVars[i].Externalize(hbuf16);
	aCount=nvars;
	return retBuf;
	}

// The interface used to support the STDLIB routines

wchar_t * CEnvironment::getenv(const wchar_t* aName) const
	{
	TPtrC16 name((TText16*)aName);
	for (TUint i=0; i<iCount; ++i)
		{
		HBufC16* value=iVars[i].Match(name);
		if (value)
			{
			const wchar_t* vptr = (const wchar_t*)(value->Ptr());
			return CONST_CAST(wchar_t*,vptr);	// I hope nobody modifies this data...
			}
		}
	return 0;
	}

int CEnvironment::setenv(const wchar_t* aName, const wchar_t* aValue, int aReplace, int& anErrno)
	{
	TPtrC16 name((TText16*)aName);
	TUint freeSlot=iCount;
	TEnvVar* ep=iVars;
	for (TUint i=0; i<iCount; ++i,++ep)
		{
		if (ep->NotEmpty())
			{
			const TDesC16* existing=ep->Match(name);
			if (existing)
				{
				TInt err=KErrNone;
				if (aReplace)
					ep->SetValue(aValue);
				return MapError(err,anErrno);
				}
			}
		else
			freeSlot=i;
		}
	if (freeSlot==iCount)	
		{
		// no free slots, time to grow the array
		ep=(TEnvVar*)User::ReAlloc(iVars,(iCount+1)*sizeof(TEnvVar));
		if (ep==0)
			return KErrNoMemory;
		iVars=ep;
		++iCount;
		}
	TRAPD(ret,iVars[freeSlot].ConstructL(name,aValue));
	return MapError(ret,anErrno);
	}

void CEnvironment::unsetenv(const wchar_t* aName)
	{
	TPtrC16 name((TText16*)aName);
	for (TUint i=0; i<iCount; ++i)
		{
		const TDesC16* value=iVars[i].Match(name);
		if (value)
			{
			iVars[i].Release();
			return;
			}
		}
	}

