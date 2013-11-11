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

#include <e32std.h>
#include <e32base.h>
#include <stddef.h> //for wchar_t

class CEnvironment;
class TEnvVar
/**
@internalComponent
*/
	{
	friend class CEnvironment;	// who manages my storage
public:
	void Release();
	void ConstructL(const TDesC16& aName, const wchar_t* aValue);
	TInt SetValue(const wchar_t* aValue);
	HBufC16* Match(const TDesC16& aName);
	TUint Length() const;
	const TInt NotEmpty() const { return (TInt)iValue; };
	static TInt Externalize(const wchar_t* aPair, TDes16& aBuffer);
	TInt Externalize(TDes16& aBuffer);
	void ConstructL(const TText16*& aPtr);
private:
	static TPtrC16 ValuePtr(const wchar_t* aValue);
	HBufC16* iName;
	HBufC16* iValue;		// data is zero terminated
	};

NONSHARABLE_CLASS(CEnvironment) : public CBase
/**
@internalComponent
*/
	{
public:
	CEnvironment() {};
	~CEnvironment();

	wchar_t* getenv(const wchar_t* aName) const;
	int setenv(const wchar_t* aName, const wchar_t* aValue, int aReplace, int& anErrno);
	void unsetenv(const wchar_t* aName);

	void ConstructL(TUint aCount, TDes16& aBuffer);
	HBufC16* ExternalizeLC(TUint& aCount);
	HBufC16* ExternalizeLC(TUint& aCount, wchar_t** envp);
private:
	TUint iCount;
	TEnvVar* iVars;
	};
