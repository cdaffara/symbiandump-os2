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

#ifndef T_DBFAIL_H
#define T_DBFAIL_H

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
		User::__DbgMarkStart(RHeap::EUser);
		}
	void Fail(TInt aCount) const
		{
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext,aCount);
		}
	void Reset() const
		{
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
		}
	void Check() const
		{
		User::__DbgMarkEnd(RHeap::EUser,0);
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
		TheDbs.SetHeapFailure(RHeap::EFailNext,aCount);
		}
	void Reset() const
		{
		TheDbs.SetHeapFailure(RHeap::ENone,0);
		}
	void Check() const
		{
		}
		
	};

GLREF_D TClientHeap KClientHeap;
GLREF_D TServerHeap KServerHeap;

#endif//T_DBFAIL_H
