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
//

#ifndef __T_FAIL_H__
#define __T_FAIL_H__

GLREF_D RTest test;
GLREF_D RDbs TheDbs;
GLREF_D RDbNamedDatabase TheDatabase;

GLREF_C void Connect();
GLREF_C void Disconnect();
GLREF_C void PrepareDbFmtString();
GLREF_C void TestOpen2();
GLREF_C void Origins2();

class THeapFail
	{
public:
	virtual void Mark() const =0;
	virtual void Fail(TInt aCount) const =0;
	virtual void Reset() const =0;
	virtual void Check() const =0;
	};

class TContext
	{
public:
	virtual void OpenDbL() const =0;
	};

class TFail
	{
private:
	void Test(const THeapFail& aHeap,const TContext* aContext);
	virtual void RunL() 
		{
		}
	virtual void End() 
		{
		}

public:
	inline void Test(const THeapFail& aHeap,const TContext& aContext)
		{
		Test(aHeap,&aContext);
		}
	inline void Test(const THeapFail& aHeap)
		{
		Test(aHeap,0);
		}
	};

class TClientHeap : public THeapFail
	{
public:
	TClientHeap() 
		{
		}
		
private:
	void Mark() const
		{
		}
	void Fail(TInt aCount) const
		{
		// To avoid warnings
		aCount = aCount;
		}
	void Reset() const
		{
		}
	void Check() const
		{
		}
		
	};

class TServerHeap : public THeapFail
	{
public:
	TServerHeap() 
		{
		}
		
private:
	void Mark() const
		{
		}
	void Fail(TInt aCount) const
		{
		// To avoid warnings
		aCount = aCount;
		}
	void Reset() const
		{
		}
	void Check() const
		{
		}
		
	};

GLREF_D TClientHeap KClientHeap;
GLREF_D TServerHeap KServerHeap;

#endif//__T_FAIL_H__
