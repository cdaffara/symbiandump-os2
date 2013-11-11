/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology
*/

#ifndef _FSRUNPACKAGE_H
#define _FSRUNPACKAGE_H

#include <e32base.h>

// Disable WINS warning for symbol being too long (especially for template 
// instantiations)
#ifdef __WINS__
#pragma warning(disable : 4786)
#endif

// The CRunPackage class allows a CActive derived object to "callback"
// a member function with given parameters when the RunL function is called.
//
// It packages up the function call and the parameters into an object.
// This saves you on having to have all these different member variables
// to store parameters in an async call.
//
// The way this works is to create a member CRunPackage contained in
// your class. Your RunL would simply be
//
// if (iRunPackage)
//     {
//     iRunPackage->ExecuteL();
//     }
//
// And you set your iRunPackage by doing
// iRunPackage = CRunPackage2<....>(this, function, param1, param2);
//
// In addition, CRunPackage encapsulates transient variables allocated on
// the heap which must be destroyed when the CRunPackage object is destroyed.
//
// These objects can be added by calling the AddDeleteOnly, AddCloseDelete,
// and AddReleaseOnly methods. When CRunPackage is destroyed, these objects
// will be deleted.
class CRunPackage : public CBase
	{
private:
	// Private classes derived from CBase whose destructors release transient
	// resources when the RunPackage is destroyed
	template <class T>
	class CDeleteOnly : public CBase
		{
	public:
		CDeleteOnly(T* aPtr) : iPtr(aPtr) {}
		~CDeleteOnly() { delete iPtr; }

	private:
		T* iPtr;
		};

	// Close and delete an object - this is required for pointers to R classes
	template <class T>
	class CCloseDelete : public CBase
		{
	public:
		CCloseDelete(T* aPtr) : iPtr(aPtr) {}
		~CCloseDelete() 
			{
			iPtr->Close();
			delete iPtr; 
			}

	private:
		T* iPtr;
		};

	// Release an object
	template <class T>
	class CReleaseOnly : public CBase
		{
	public:
		CReleaseOnly(T* aPtr) : iPtr(aPtr) {}
		~CReleaseOnly() { iPtr->Release(); }

	private:
		T* iPtr;
		};

public:
	// when the destructor is invoked, if there is an iDeleteAction, its
	// destructor will be invoked 
	virtual ~CRunPackage();
	virtual void ExecuteL() = 0;

	// Operations to add delete operations
	template <class T>
	TInt AddDeleteOnly(T* aPtr)
		{
		return AddDeleteAction(new CDeleteOnly<T>(aPtr));
		}

	// adds a pointer which is to be closed and deleted
	template <class T>
	TInt AddCloseDelete(T* aPtr)
		{
		return AddDeleteAction(new CCloseDelete<T>(aPtr));
		}

	// adds a pointer which is to be released
	template <class T>
	TInt AddReleaseOnly(T* aPtr)
		{
		return AddDeleteAction(new CReleaseOnly<T>(aPtr));
		}

private:
	// Adds a deleted action
	TInt AddDeleteAction(CBase* aAction);

private:
	RPointerArray<CBase> iDeleteActions;
	};

// Implementation of RunPackage where callback takes no parameters
template <class T>
class CRunPackage0 : public CRunPackage
	{
public:
	typedef void (T::*TRunFn)();

	CRunPackage0(T& aParent, TRunFn aRunFn)
		: iParent(aParent), iRunFn(aRunFn) {}

	void ExecuteL() 
		{ 
		(iParent.*iRunFn)(); 
		}

private:
	T& iParent;
	TRunFn iRunFn;
	};


// implementation which takes one parameter
template <class T, class P1>
class CRunPackage1 : public CRunPackage
	{
public:
	typedef void (T::*TRunFn)(P1);

	CRunPackage1(T& aParent, TRunFn aRunFn, P1 aP1)
		: iParent(aParent), iRunFn(aRunFn), iP1(aP1) {}

	void ExecuteL() 
		{ 
		(iParent.*iRunFn)(iP1); 
		}

private:
	T& iParent;
	TRunFn iRunFn;
	P1 iP1;
	};

// implementation which takes two parameters
template <class T, class P1, class P2>
class CRunPackage2 : public CRunPackage
	{
public:
	typedef void (T::*TRunFn)(P1, P2);

	CRunPackage2(T& aParent, TRunFn aRunFn, P1 aP1, P2 aP2)
		: iParent(aParent), iRunFn(aRunFn), iP1(aP1), iP2(aP2) {}

	void ExecuteL() 
		{ 
		(iParent.*iRunFn)(iP1, iP2); 
		}

private:
	T& iParent;
	TRunFn iRunFn;
	P1 iP1;
	P2 iP2;
	};

// implementation which takes three parameters
template <class T, class P1, class P2, class P3>
class CRunPackage3 : public CRunPackage
	{
public:
	typedef void (T::*TRunFn)(P1, P2, P3);

	CRunPackage3(T& aParent, TRunFn aRunFn, P1 aP1, P2 aP2, P3 aP3)
		: iParent(aParent), iRunFn(aRunFn), iP1(aP1), iP2(aP2), iP3(aP3) {}

	void ExecuteL() 
		{ 
		(iParent.*iRunFn)(iP1, iP2, iP3); 
		}

private:
	T& iParent;
	TRunFn iRunFn;
	P1 iP1;
	P2 iP2;
	P3 iP3;
	};

// implementation which takes four parameters
template <class T, class P1, class P2, class P3, class P4>
class CRunPackage4 : public CRunPackage
	{
public:
	typedef void (T::*TRunFn)(P1, P2, P3, P4);

	CRunPackage4(T& aParent, TRunFn aRunFn, P1 aP1, P2 aP2, P3 aP3, P4 aP4)
		: iParent(aParent), iRunFn(aRunFn), iP1(aP1), iP2(aP2), iP3(aP3), iP4(aP4) {}

	void ExecuteL() 
		{ 
		(iParent.*iRunFn)(iP1, iP2, iP3, iP4);
		}

private:
	T& iParent;
	TRunFn iRunFn;
	P1 iP1;
	P2 iP2;
	P3 iP3;
	P4 iP4;
	};

// implementation which takes four parameters
template <class T, class P1, class P2, class P3, class P4, class P5>
class CRunPackage5 : public CRunPackage
	{
public:
	typedef void (T::*TRunFn)(P1, P2, P3, P4, P5);

	CRunPackage5(T& aParent, TRunFn aRunFn, P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5)
		: iParent(aParent), iRunFn(aRunFn), iP1(aP1), iP2(aP2), iP3(aP3), 
		  iP4(aP4), iP5(aP5) {}

	void ExecuteL() 
		{ 
		(iParent.*iRunFn)(iP1, iP2, iP3, iP4, iP5);
		}

private:
	T& iParent;
	TRunFn iRunFn;
	P1 iP1;
	P2 iP2;
	P3 iP3;
	P4 iP4;
	P5 iP5;
	};

// implementation which takes four parameters
template <class T, class P1, class P2, class P3, class P4, class P5, class P6>
class CRunPackage6 : public CRunPackage
	{
public:
	typedef void (T::*TRunFn)(P1, P2, P3, P4, P5, P6);

	CRunPackage6(T& aParent, TRunFn aRunFn, P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5, P6 aP6)
		: iParent(aParent), iRunFn(aRunFn), iP1(aP1), iP2(aP2), iP3(aP3), 
		  iP4(aP4), iP5(aP5), iP6(aP6) {}

	void ExecuteL() 
		{ 
		(iParent.*iRunFn)(iP1, iP2, iP3, iP4, iP5, iP6);
		}

private:
	T& iParent;
	TRunFn iRunFn;
	P1 iP1;
	P2 iP2;
	P3 iP3;
	P4 iP4;
	P5 iP5;
	P6 iP6;
	};


#endif
