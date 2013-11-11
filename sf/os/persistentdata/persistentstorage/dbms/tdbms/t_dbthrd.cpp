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

#include "t_dbstress.h"

const TInt KCashLimit=500;
const TInt KOverdraftLimit=100;

const TInt KMinHeap=0x2000;
const TInt KMaxHeap=0x20000;
#ifdef _DEBUG
const TInt KAllocFailRate=1000;
#endif

LOCAL_D TBuf<256> Sql;
LOCAL_D	TBuf8<256> LogBuf;

class CThread : public CBase
	{
public:
	static TInt Entry(TAny*);
protected:
	CThread() {}
	~CThread();
private:
	static void EntryL();
	void GoL();
	void ConstructL();
	void OpenViewsL();
	void RecoverL();
	void CompactL();
	void Rollback();
	void Reset();
	void WorkL();
	void TransactionL();
	void AccountL(TInt anAccount);
	void StatementTimeL();
	void VerifyL();
//
	void LogSize();
private:
	RFs iFs;
	RFile iLog;
	CFileStore* iStore;
	RDbStoreDatabase iData;
	TBool iOpen;
	RDbView iAccs;
	RDbView iTrans;
	TInt iError;
	TInt iPoint;
	};

GLDEF_C TInt StartThread(RThread& aThread,TRequestStatus& aStat)
	{
	TInt r=aThread.Create(_L("Thread"),CThread::Entry,KDefaultStackSize,KMinHeap,KMaxHeap,NULL);
	if (r==KErrNone)
		{
		aThread.SetPriority(EPriorityLess);
		aThread.Logon(aStat);
		aThread.Resume();
		RunTimer.Start();
		}
	return r;
	}

TInt CThread::Entry(TAny*)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		return KErrNoMemory;
	TRAPD(r,EntryL());
	delete cleanup;
	return r;
	}

void CThread::EntryL()
	{
	CThread* self=new(ELeave) CThread;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->GoL();		// never returns
	}

CThread::~CThread()
	{//The destructor is never ever executed! See CThread::EntryL() for details!
	iAccs.Close();
	iTrans.Close();
	iData.Close();
	delete iStore;
	iLog.Close();
	TInt err = iFs.Delete(KLogFile);
	if(err != KErrNone)
		{
		RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &KLogFile);
		}
	iFs.Close();
	}

void CThread::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.SetSessionPath(KTestDir));
	User::LeaveIfError(iLog.Replace(iFs,KLogFile,EFileWrite|EFileStreamText));
	iStore=CFileStore::OpenL(iFs,KTestDatabase,EFileRead|EFileWrite|EFileWriteDirectIO);
	LogSize();
	iData.OpenL(iStore,iStore->Root());
	}

//
// Never exits
//
void CThread::GoL()
	{
	__UHEAP_SETFAIL(RHeap::ETrueRandom,KAllocFailRate);
	for (;;)
		{
		TRAPD(r,WorkL());
		NewCount=OldCount;
		LogBuf.Format(_L8(" *** Point %d with code %d"),iPoint,r);
		iLog.Write(LogBuf);
		LogSize();
		iError=r;
		Rollback();
		LogSize();
		if (r==KErrDiskFull)
			User::Leave(r);
		}
	}

//
// Report the file size
//
void CThread::LogSize()
	{
	TInt size;
	if (iStore->File().Size(size)==KErrNone)
		{
		LogBuf.Format(_L8("\nFile size=%d"),size);
		iLog.Write(LogBuf);
		}
	}

void CThread::Rollback()
	{
	if (iOpen)
		{
		iLog.Write(_L8("\nCancel"));
		iAccs.Cancel();
		iTrans.Cancel();
		}
	if (iData.InTransaction())
		{
		iLog.Write(_L8("\nRollback"));
		iData.Rollback();
		}
	}

void CThread::Reset()
	{
	iLog.Write(_L8("\nReset"));
	TEST(iOpen);
	iAccs.Reset();
	iTrans.Reset();
	}

void CThread::RecoverL()
	{
	iPoint=70;
	User::LeaveIfError(iLog.Write(_L8("\nRecovering")));
	if (iOpen)
		{
		iAccs.Close();
		iTrans.Close();
		iOpen=EFalse;
		}
	User::LeaveIfError(iData.Recover());
	}

void CThread::CompactL()
	{
	iPoint=90;
	User::LeaveIfError(iLog.Write(_L8("\nCompacting")));
	TInt b=iStore->ReclaimL();
	b-=iStore->CompactL();
	iStore->CommitL();
	LogBuf.Format(_L8(": %d bytes reclaimed"),b);
	User::LeaveIfError(iLog.Write(LogBuf));
	}

void CThread::OpenViewsL()
	{
	iPoint=80;
	User::LeaveIfError(iLog.Write(_L8("\nOpening")));
	User::LeaveIfError(iAccs.Prepare(iData,_L("select id,balance,statement_balance from accounts")));
	TInt r=iTrans.Prepare(iData,_L("select t_date,from_id,to_id,amount from transactions"));
	if (r!=KErrNone)
		{
		iAccs.Close();
		User::Leave(r);
		}
	iOpen=ETrue;
	}

void CThread::WorkL()
	{
	iPoint=0;
	switch (iError)
		{
	case KErrDied:
		Rollback();
		break;
	case KErrNotReady:
		Reset();
		break;
	case KErrCorrupt:
		RecoverL();
		break;
		}
	iPoint=1;
	for (;;)
		{
		LogSize();
		if (!iOpen)
			OpenViewsL();
		switch (Random(100))
			{
		case 0:	case 1:
			StatementTimeL();
			break;
		case 2: case 3:
			RecoverL();
			break;
		case 4: // case 5: case 6: case 7: case 8: case 9:
			CompactL();
			break;
		default:
			TransactionL();
			break;
			}
		}
	}

void CThread::AccountL(TInt anAccount)
	{
	Sql.Format(_L("id=%d"),anAccount);
	iAccs.FirstL();
	TEST(iAccs.FindL(iAccs.EForwards,Sql)>=0);
	}

//
// generate and add a single transaction
//
void CThread::TransactionL()
	{
	iPoint=2;
	User::LeaveIfError(iLog.Write(_L8("\nTransaction")));
	TInt from;
	if (Random(3)==0)
		from=ECash;
	else
		from=Random(KAccountIDs);
	TInt to=(Random(KAccountIDs-1)+from+1)%KAccountIDs;
	AccountL(from);
	iAccs.GetL();
	TEST(iAccs.ColInt(1)==from);
	TInt avail=iAccs.ColInt(2)+KOverdraftLimit;
	TInt amount;
	if (to==ECash)
		{
		if (avail<10)
			return;
		amount=10*(1+Random(Min(avail,KCashLimit)/10));
		}
	else
		{
		amount=1+Random(100);
		if (Random(100)<5)
			amount*=10;
		if (Random(100)<5)
			amount*=10;
		while (amount>avail)
			amount/=4;
		if (amount==0)
			return;
		}
	iPoint=3;
	LogBuf.Format(_L8(" %08d: %d -> %d, %5d"),++TransId,from,to,amount);
	User::LeaveIfError(iLog.Write(LogBuf));
	iPoint=4;
	iData.Begin();
	iTrans.InsertL();
	iTrans.SetColL(1,TransId);
	iTrans.SetColL(2,from);
	iTrans.SetColL(3,to);
	iTrans.SetColL(4,amount);
	iPoint=5;
	iTrans.PutL();
	iPoint=6;
	iAccs.UpdateL();	// from
	TInt frombalance=iAccs.ColInt(2)-amount;
	iAccs.SetColL(2,frombalance);
	iPoint=7;
	iAccs.PutL();
	iPoint=8;
	AccountL(to);
	iPoint=9;
	iAccs.UpdateL();	// to
	TInt tobalance=iAccs.ColInt(2)+amount;
	iAccs.SetColL(2,tobalance);
	iPoint=10;
	iAccs.PutL();
	iPoint=11;
// this will invoke commit, so update counts now
	NewCount=OldCount+1;
	TInt r=iData.Commit();
	if (r!=KErrNone)
		{
		NewCount=OldCount;
		User::Leave(r);
		}
// succeeded
	OldCount=NewCount;
	LogBuf.Format(_L8("; [%d,%d]"),frombalance,tobalance);
	iLog.Write(LogBuf);
	}

//
// deliver statements
//
void CThread::StatementTimeL()
	{
	iPoint=50;
	User::LeaveIfError(iLog.Write(_L8("\nStatement")));
	iPoint=51;
	VerifyL();
	// discard transactions
	iPoint=52;
	iData.Begin();
	for (iTrans.BeginningL();iTrans.NextL();)
		iTrans.DeleteL();
	for (iAccs.BeginningL();iAccs.NextL();)
		{
		iAccs.UpdateL();
		iAccs.SetColL(3,iAccs.ColInt(2));	// set statement balance
		iAccs.PutL();
		}
	iPoint=53;
	NewCount=0;
	TInt r=iData.Commit();
	if (r!=KErrNone)
		{
		NewCount=OldCount;
		User::Leave(r);
		}
	OldCount=NewCount;
	}

void CThread::VerifyL()
	{
	TInt balance[KAccountIDs];
	TEST(iAccs.CountL()==KAccountIDs);
	for (iAccs.BeginningL();iAccs.NextL();)
		{
		iAccs.GetL();
		TInt id=iAccs.ColInt(1);
		balance[id]=iAccs.ColInt(3);
		}
	TInt transact=0;
	for (iTrans.BeginningL();iTrans.NextL();)
		{
		++transact;
		iTrans.GetL();
		TInt from=iTrans.ColInt(2);
		TInt to=iTrans.ColInt(3);
		TInt amount=iTrans.ColInt(4);
		balance[from]-=amount;
		balance[to]+=amount;
		}
	TEST(transact==iTrans.CountL());
	for (iAccs.BeginningL();iAccs.NextL();)
		{
		iAccs.GetL();
		TInt id=iAccs.ColInt(1);
		if (balance[id]!=iAccs.ColInt(2))
			User::Panic(_L("Oh-oh"),4321);
		}
	}
