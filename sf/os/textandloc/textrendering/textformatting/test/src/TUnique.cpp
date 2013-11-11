/*
* Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* TUnique.cpp test file for UniqueInstance classes
*
*/


#include "UniqueInstanceImpl.h"
#include <e32test.h>
#include "tunique.h"

using namespace UniqueInstance;

#define UNUSED_VAR(a) a = a

// we will manipulate TInts for simplicity
// some functions that will help us
namespace
{
TInt CompareVoids(void* pl, void* pr)
	{
	TInt l = *reinterpret_cast<TInt*>(pl);
	TInt r = *reinterpret_cast<TInt*>(pr);
	return r - l;
	}
TInt CompareTInts(const TInt* l, const TInt* r) { return *r - *l; }
void* CopyVoidL(void* a, TInt)
	{
	return new(ELeave) TInt(*reinterpret_cast<TInt*>(a));
	}
TInt* CopyTIntL(const TInt* a, TInt) { return new(ELeave) TInt(*a); }
void DeleteVoid(void* a)
	{
	delete reinterpret_cast<TInt*>(a);
	}
void DeleteTInt(TInt* a) { delete a; }

CTUniqueStep* TestStep;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)
}

/////////////////
//			   //
//	RSkipList  //
//			   //
/////////////////

namespace
{
TInt TestEmpty(RSkipList& a)
	{
	if (!a.IsEmpty())
		{
		TESTPRINT(_L("RSkipList : unexpectedly has elements"));
		return 1;
		}
	return 0;
	}
TInt TestAddL(RSkipList& a, void* aElt, TInt aExpectedRefCount)
	{
	SElement* e = a.AddExisting(aElt);
	if (!e)
		e = a.AddNewL(aElt);
	if (CompareVoids(e->iObject,aElt))
		{
        TESTPRINT(_L("RSkipList : added element does not compare equal to returned value"));
		return 1;
		}
	if (e->iRefCount != aExpectedRefCount)
		{
        TESTPRINT(_L("RSkipList : unexpected reference count"));
		return 1;
		}
	return 0;
	}
TInt TestRemove(RSkipList& a, void* aElt)
	{
	if (CompareVoids(a.Remove(aElt), aElt))
		{
        TESTPRINT(_L("RSkipList : removed element does not compare equal to returned value"));
		return 1;
		}
	return 0;
	}
void Test(RSkipList& a)
	{
	a.Test();
	}
void TestRSkipListL()
	{
	__UHEAP_MARK;

	RSkipList s;
	s.Open(CompareVoids, 16);
	Test(s);

	TInt* n500 = new(ELeave) TInt(-500);
	TInt* n400 = new(ELeave) TInt(-400);
	TInt* n300 = new(ELeave) TInt(-300);
	TInt* n200 = new(ELeave) TInt(-200);
	TInt* n190 = new(ELeave) TInt(-190);
	TInt* n180 = new(ELeave) TInt(-180);
	TInt* n170 = new(ELeave) TInt(-170);
	TInt* n160 = new(ELeave) TInt(-160);
	TInt* n150 = new(ELeave) TInt(-150);
	TInt* n140 = new(ELeave) TInt(-140);
	TInt* n130 = new(ELeave) TInt(-130);
	TInt* n120 = new(ELeave) TInt(-120);
	TInt* n110 = new(ELeave) TInt(-110);
	TInt* n100 = new(ELeave) TInt(-100);
	TInt* n87  = new(ELeave) TInt(-87);
	TInt* n45  = new(ELeave) TInt(-45);
	TInt* n3   = new(ELeave) TInt(-3);
	TInt* p999 = new(ELeave) TInt(999);
	TInt* p998 = new(ELeave) TInt(998);
	TInt* p980 = new(ELeave) TInt(980);
	TInt* p900 = new(ELeave) TInt(900);
	TInt* p800 = new(ELeave) TInt(800);
	TInt* p700 = new(ELeave) TInt(700);
	TInt* p600 = new(ELeave) TInt(600);
	TInt* p500 = new(ELeave) TInt(500);
	TInt* p490 = new(ELeave) TInt(490);
	TInt* p485 = new(ELeave) TInt(485);
	TInt* p480 = new(ELeave) TInt(480);
	TInt* p472 = new(ELeave) TInt(472);
	TInt* p471 = new(ELeave) TInt(471);
	TInt* p470 = new(ELeave) TInt(470);
	TInt* p469 = new(ELeave) TInt(469);
	TInt* p463 = new(ELeave) TInt(463);
	TInt* p460 = new(ELeave) TInt(460);
	TInt* p440 = new(ELeave) TInt(440);
	TInt* p400 = new(ELeave) TInt(400);
	TInt* p350 = new(ELeave) TInt(350);
	TInt* p300 = new(ELeave) TInt(300);
	TInt* p200 = new(ELeave) TInt(200);
	TInt* p101 = new(ELeave) TInt(101);
	TInt* p100 = new(ELeave) TInt(100);
	TInt* p99  = new(ELeave) TInt(99);
	TInt* p45  = new(ELeave) TInt(45);
	TInt* p2   = new(ELeave) TInt(2);

	// add everything a first time
	TestAddL(s, p999, 1);
	Test(s);
	TestAddL(s, n190, 1);
	Test(s);
	TestAddL(s, n180, 1);
	Test(s);
	TestAddL(s, n160, 1);
	Test(s);
	TestAddL(s, p485, 1);
	Test(s);
	TestAddL(s, n140, 1);
	Test(s);
	TestAddL(s, n200, 1);
	Test(s);
	TestAddL(s, p470, 1);
	Test(s);
	TestAddL(s, n500, 1);
	Test(s);
	TestAddL(s, p980, 1);
	Test(s);
	TestAddL(s, n150, 1);
	Test(s);
	TestAddL(s, n170, 1);
	Test(s);
	TestAddL(s, p490, 1);
	Test(s);
	TestAddL(s, p500, 1);
	Test(s);
	TestAddL(s, p600, 1);
	Test(s);
	TestAddL(s, p440, 1);
	Test(s);
	TestAddL(s, p998, 1);
	Test(s);
	TestAddL(s, p700, 1);
	Test(s);
	TestAddL(s, p463, 1);
	Test(s);
	TestAddL(s, p460, 1);
	Test(s);
	TestAddL(s, p400, 1);
	Test(s);
	TestAddL(s, p800, 1);
	Test(s);
	TestAddL(s, n130, 1);
	Test(s);
	TestAddL(s, n120, 1);
	Test(s);
	TestAddL(s, p471, 1);
	Test(s);
	TestAddL(s, p472, 1);
	Test(s);
	TestAddL(s, p480, 1);
	Test(s);
	TestAddL(s, n400, 1);
	Test(s);
	TestAddL(s, n300, 1);
	Test(s);
	TestAddL(s, p300, 1);
	Test(s);
	TestAddL(s, n110, 1);
	Test(s);
	TestAddL(s, p350, 1);
	Test(s);
	TestAddL(s, p101, 1);
	Test(s);
	TestAddL(s, p200, 1);
	Test(s);
	TestAddL(s, p469, 1);
	Test(s);
	TestAddL(s, p99, 1);
	Test(s);
	TestAddL(s, p100, 1);
	Test(s);
	TestAddL(s, p45, 1);
	Test(s);
	TestAddL(s, p900, 1);
	Test(s);
	TestAddL(s, n100, 1);
	Test(s);
	TestAddL(s, n87, 1);
	Test(s);
	TestAddL(s, n45, 1);
	Test(s);
	TestAddL(s, n3, 1);
	Test(s);
	TestAddL(s, p2, 1);
	Test(s);

	// remove a few things
	TestRemove(s, p500);
	Test(s);
	TestRemove(s, p900);
	Test(s);
	TestRemove(s, n100);
	Test(s);
	TestRemove(s, n200);
	Test(s);
	TestRemove(s, p800);
	Test(s);
	TestRemove(s, p100);
	Test(s);
	TestRemove(s, n400);
	TestRemove(s, p700);
	TestRemove(s, p200);
	TestRemove(s, p600);
	TestRemove(s, n300);
	TestRemove(s, p300);
	TestRemove(s, p400);
	TestRemove(s, n500);

	// add everything a second time
	TestAddL(s, p440, 2);
	TestAddL(s, p480, 2);
	TestAddL(s, n180, 2);
	TestAddL(s, n3, 2);
	TestAddL(s, n130, 2);
	TestAddL(s, p470, 2);
	TestAddL(s, n400, 1);
	TestAddL(s, n45, 2);
	TestAddL(s, n190, 2);
	TestAddL(s, p300, 1);
	TestAddL(s, n120, 2);
	TestAddL(s, n150, 2);
	TestAddL(s, p463, 2);
	TestAddL(s, n170, 2);
	TestAddL(s, p469, 2);
	TestAddL(s, p471, 2);
	TestAddL(s, p490, 2);
	TestAddL(s, p200, 1);
	TestAddL(s, p99, 2);
	TestAddL(s, p980, 2);
	TestAddL(s, p101, 2);
	TestAddL(s, p600, 1);
	TestAddL(s, n500, 1);
	TestAddL(s, n110, 2);
	TestAddL(s, p460, 2);
	TestAddL(s, p485, 2);
	TestAddL(s, n160, 2);
	TestAddL(s, n140, 2);
	TestAddL(s, p100, 1);
	TestAddL(s, p472, 2);
	TestAddL(s, p350, 2);
	TestAddL(s, p400, 1);
	TestAddL(s, p999, 2);
	TestAddL(s, n200, 1);
	TestAddL(s, p500, 1);
	TestAddL(s, p998, 2);
	TestAddL(s, n87, 2);
	TestAddL(s, n100, 1);
	TestAddL(s, p45, 2);
	TestAddL(s, p800, 1);
	TestAddL(s, p900, 1);
	TestAddL(s, p700, 1);
	TestAddL(s, n300, 1);
	TestAddL(s, p2, 2);

	Test(s);

	// remove everything
	TestRemove(s, p500);
	TestRemove(s, p900);
	TestRemove(s, p980);
	TestRemove(s, p471);
	TestRemove(s, p472);
	TestRemove(s, p480);
	TestRemove(s, p45);
	TestRemove(s, n160);
	TestRemove(s, n100);
	TestRemove(s, n87);
	TestRemove(s, p469);
	TestRemove(s, p490);
	TestRemove(s, n170);
	TestRemove(s, n200);
	TestRemove(s, p460);
	TestRemove(s, p800);
	TestRemove(s, n120);
	TestRemove(s, p485);
	TestRemove(s, p100);
	TestRemove(s, p99);
	TestRemove(s, p101);
	TestRemove(s, n110);
	TestRemove(s, n400);
	TestRemove(s, p463);
	TestRemove(s, n45);
	TestRemove(s, n3);
	TestRemove(s, n190);
	TestRemove(s, p700);
	TestRemove(s, p999);
	TestRemove(s, p200);
	TestRemove(s, p2);
	TestRemove(s, n180);
	TestRemove(s, n150);
	TestRemove(s, p600);
	TestRemove(s, n300);
	TestRemove(s, p300);
	TestRemove(s, p350);
	TestRemove(s, p400);
	TestRemove(s, p440);
	TestRemove(s, p470);
	TestRemove(s, p998);
	TestRemove(s, n140);
	TestRemove(s, n130);
	TestRemove(s, n500);

	Test(s);

	// add everything
	TestAddL(s, p999, 1);
	TestAddL(s, n200, 1);
	TestAddL(s, n3, 1);
	TestAddL(s, n110, 1);
	TestAddL(s, p460, 1);
	TestAddL(s, p980, 1);
	TestAddL(s, n500, 1);
	TestAddL(s, p700, 1);
	TestAddL(s, n300, 1);
	TestAddL(s, p2, 1);
	TestAddL(s, p463, 1);
	TestAddL(s, p472, 1);
	TestAddL(s, n100, 1);
	TestAddL(s, p45, 1);
	TestAddL(s, n45, 1);
	TestAddL(s, p469, 1);
	TestAddL(s, n120, 1);
	TestAddL(s, p470, 1);
	TestAddL(s, p400, 1);
	TestAddL(s, n190, 1);
	TestAddL(s, p998, 1);
	TestAddL(s, p490, 1);
	TestAddL(s, n180, 1);
	TestAddL(s, p600, 1);
	TestAddL(s, n160, 1);
	TestAddL(s, p480, 1);
	TestAddL(s, p350, 1);
	TestAddL(s, p200, 1);
	TestAddL(s, n130, 1);
	TestAddL(s, p485, 1);
	TestAddL(s, p500, 1);
	TestAddL(s, p471, 1);
	TestAddL(s, p440, 1);
	TestAddL(s, p800, 1);
	TestAddL(s, n87, 1);
	TestAddL(s, p101, 1);
	TestAddL(s, n140, 1);
	TestAddL(s, p100, 1);
	TestAddL(s, n400, 1);
	TestAddL(s, n170, 1);
	TestAddL(s, p99, 1);
	TestAddL(s, p300, 1);
	TestAddL(s, n150, 1);
	TestAddL(s, p900, 1);

	Test(s);

	// remove everything
	TestRemove(s, n120);
	TestRemove(s, p485);
	TestRemove(s, p100);
	TestRemove(s, n500);
	TestRemove(s, n400);
	TestRemove(s, p463);
	TestRemove(s, n150);
	TestRemove(s, p440);
	TestRemove(s, n170);
	TestRemove(s, n3);
	TestRemove(s, p500);
	TestRemove(s, p900);
	TestRemove(s, p980);
	TestRemove(s, p471);
	TestRemove(s, p600);
	TestRemove(s, n300);
	TestRemove(s, p300);
	TestRemove(s, p350);
	TestRemove(s, p700);
	TestRemove(s, n45);
	TestRemove(s, n200);
	TestRemove(s, p2);
	TestRemove(s, p400);
	TestRemove(s, n110);
	TestRemove(s, n190);
	TestRemove(s, p999);
	TestRemove(s, p200);
	TestRemove(s, p99);
	TestRemove(s, p470);
	TestRemove(s, p800);
	TestRemove(s, n180);
	TestRemove(s, p460);
	TestRemove(s, p998);
	TestRemove(s, n140);
	TestRemove(s, p472);
	TestRemove(s, p101);
	TestRemove(s, p480);
	TestRemove(s, n130);
	TestRemove(s, p45);
	TestRemove(s, n160);
	TestRemove(s, n100);
	TestRemove(s, n87);
	TestRemove(s, p469);
	TestRemove(s, p490);

	TestEmpty(s);

	delete n500;
	delete n400;
	delete n300;
	delete n200;
	delete n190;
	delete n180;
	delete n170;
	delete n160;
	delete n150;
	delete n140;
	delete n130;
	delete n120;
	delete n110;
	delete n100;
	delete n87;
	delete n45;
	delete n3;
	delete p2;
	delete p45;
	delete p99;
	delete p100;
	delete p101;
	delete p200;
	delete p300;
	delete p350;
	delete p400;
	delete p440;
	delete p460;
	delete p463;
	delete p469;
	delete p470;
	delete p471;
	delete p472;
	delete p480;
	delete p485;
	delete p490;
	delete p500;
	delete p600;
	delete p700;
	delete p800;
	delete p900;
	delete p980;
	delete p998;
	delete p999;

	s.Close();

	__UHEAP_MARKENDC(0);
	}
}

///////////////////////
//					 //
//  CRepositoryImpl  //
//					 //
///////////////////////
namespace
{
TInt TestObject(SElement* e, TInt val)
	{
	if (!e->iObject)
		{
        TESTPRINT(_L("CRepositoryImpl : object undefined"));
		return 1;
		}
	if (*reinterpret_cast<TInt*>(e->iObject) != val)
		{
        TESTPRINT(_L("CRepositoryImpl : object has wrong value"));
		return 1;
		}
	return 0;
	}
void TestCRepositoryImplL()
	{
	__UHEAP_MARK;

	CRepositoryImpl* rep = new(ELeave) CRepositoryImpl(CompareVoids, DeleteVoid,
		CopyVoidL, sizeof(TInt));
	rep->ConstructL(1);
	TInt* n47	= new(ELeave) TInt(-47);
	TInt* z		= new(ELeave) TInt(0);
	TInt* z2	= new(ELeave) TInt(0);
	TInt* p8765	= new(ELeave) TInt(8765);

	SElement* z0ui		= rep->IncOrCopyL(z);

	SElement* zui		= rep->InsertOrIncL(z);
	SElement* n47ui		= rep->InsertOrIncL(n47);
	SElement* p8765ui	= rep->InsertOrIncL(p8765);
	SElement* z2ui		= rep->InsertOrIncL(z2);

	z = new(ELeave) TInt(0);
	SElement* z3ui		= rep->IncOrCopyL(z);
	delete z;

	// test equal objects are nullified
	TESTPOINT(zui->iObject == z2ui->iObject);
	TESTPOINT(zui->iObject == z0ui->iObject);
	TESTPOINT(zui->iObject == z3ui->iObject);

	rep->DeleteOrDec(z0ui);
	rep->DeleteOrDec(z3ui);

	TestObject(zui, 0);
	TestObject(n47ui, -47);
	TestObject(p8765ui, 8765);

	rep->DeleteOrDec(n47ui);

	TestObject(zui, 0);
	TestObject(p8765ui, 8765);

	z = reinterpret_cast<TInt*>(rep->DetatchOrCopyL(zui));
	z2 = reinterpret_cast<TInt*>(rep->DetatchOrCopyL(z2ui));
	rep->DeleteOrDec(p8765ui);

	// test that copyable objects are not aliased
	TESTPOINT(z != z2);
	// test that a valid copy is returned
	TESTPOINT(*z == 0);
	TESTPOINT(*z2 == 0);

	delete z;
	delete z2;

	delete rep;

	__UHEAP_MARKENDC(0);
	}
}

///////////////////////////////////
//								 //
//  CUniqueInstanceRepository<>  //
//				and				 //
//		 RUniqueInstance<>		 //
//								 //
///////////////////////////////////
namespace
{
TInt TestNull(const RUniqueInstance<TInt>& a)
	{
	if (a.Peek())
		{
        TESTPRINT(_L("RUniqueInstance : null object has value"));
		return 1;
		}
	return 0;
	}
TInt TestValue(const RUniqueInstance<TInt>& a, TInt val)
	{
	if (!a.Peek())
		{
        TESTPRINT(_L("RUniqueInstance : unexpected null object"));
		return 1;
		}
	if (*a.Peek() != val)
		{
        TESTPRINT(_L("RUniqueInstance : object has wrong value"));
		return 1;
		}
	return 0;
	}
TInt TestRaw(TInt* ob, TInt val)
	{
	if (!ob)
		{
        TESTPRINT(_L("RUniqueInstance : object unexpectedly does not own"));
		return 1;
		}
	if (*ob != val)
		{
        TESTPRINT(_L("RUniqueInstance : object owns incorrect value"));
		return 1;
		}
	return 0;
	}
void TestRUniqueInstanceL()
	{
	__UHEAP_MARK;

	CUniqueInstanceRepository<TInt>* intRep =
		CUniqueInstanceRepository<TInt>::NewL(CompareTInts, DeleteTInt, CopyTIntL, 2);

	RUniqueInstance<TInt> a(*intRep);
	RUniqueInstance<TInt> b(*intRep);
	RUniqueInstance<TInt> c(*intRep);

	a.TakeL(new(ELeave) TInt(45));
	b.TakeL(new(ELeave) TInt(-6));
	c.TakeL(new(ELeave) TInt(45));

	// test that equal elements are unified
	TESTPOINT(a.Peek() == c.Peek());

	TestValue(a, 45);
	TestValue(b, -6);
	TestValue(c, 45);

	a.CopyTo(c);

	TestValue(a, 45);
	TestValue(b, -6);
	TestValue(c, 45);

	b.CopyTo(b);
	b.MoveTo(b);

	TestValue(a, 45);
	TestValue(b, -6);
	TestValue(c, 45);

	b.MoveTo(c);

	TestValue(a, 45);
	TestNull(b);
	TestValue(c, -6);

	b.MoveTo(c);

	TestValue(a, 45);
	TestNull(b);
	TestNull(c);

	c.TakeL(new(ELeave) TInt(87));
	c.CopyTo(b);
	TInt* r = c.DropL();

	TestRaw(r, 87);

	delete r;
	r = b.DropL();

	TestRaw(r, 87);

	delete r;

	a.Close();

	CUniqueInstanceRepository<TInt>* intRep2 =
		CUniqueInstanceRepository<TInt>::NewL(CompareTInts, DeleteTInt, CopyTIntL, 4);

	RUniqueInstance<TInt> d(*intRep2);
	RUniqueInstance<TInt> e(*intRep2);
	RUniqueInstance<TInt> f(*intRep2);

	a.TakeL(new(ELeave) TInt(123));
	b.TakeL(new(ELeave) TInt(-445));
	c.TakeL(new(ELeave) TInt(123));
	d.TakeL(new(ELeave) TInt(-7));
	e.TakeL(new(ELeave) TInt(18));
		{
		TInt n0 = -999;
		TInt n1 = 1;
		TInt n2 = 999;
		f.TakeCopyL(&n0);
		TestValue(f, -999);
		f.TakeCopyL(&n1);
		TestValue(f, 1);
		f.TakeCopyL(&n2);
		TestValue(f, 999);
		f.Close();
		}

	a.MoveTo(d);
	TestNull(a);
	TestValue(d, 123);
	c.CopyTo(d);
	TestValue(d, 123);
	e.MoveTo(a);
	e.TakeL(new(ELeave) TInt(18));

	// test that equal objects are unified
	TESTPOINT(a.Peek() == e.Peek());

	d.TakeL(new(ELeave) TInt(-445));

	// test that equal objects are unified
	TESTPOINT(b.Peek() == d.Peek());

	a.TakeL(new(ELeave) TInt(-445));

	// test that objects from different repositories are not unified
	TESTPOINT(a.Peek() != b.Peek());

	a.Close();

	// test that destroyed object peeks null
	TESTPOINT(a.Peek() == 0);

	b.MoveTo(c);
	b.Close();
	c.Close();
	d.Close();
	e.Close();

	delete intRep2;
	delete intRep;

	__UHEAP_MARKENDC(0);
	}
}


////////////
//		  //
//  Main  //
//		  //
////////////

TVerdict CTUniqueStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(_L("TUnique - Unique instance repository tests"));
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-UNIQUE-0001 Unique Instance Tests: "));

	TestRSkipListL();
	TestCRepositoryImplL();
	TestRUniqueInstanceL();

	return TestStepResult();
	}
