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
// SQL DML statements for Table framework
// 
//

#include "UT_STD.H"

// Class CSqlValues

inline CSqlValues::TEntry::TEntry(const RSqlLiteral& aValue)
	:iValue(aValue)
	{}

CSqlValues::CSqlValues()
	:iValues(EGranularity)
	{}

CSqlValues::~CSqlValues()
//
// close all the literal objects
//
	{
	for (TInt ii=iValues.Count();--ii>=0;)
		iValues[ii].iValue.Close();
	iValues.Close();
	}

void CSqlValues::AddL(const RSqlLiteral& aLiteral)
	{
	TEntry e(aLiteral);
	__DEBUG(e.iType=TDbColType(-1);)
	__LEAVE_IF_ERROR(iValues.Append(e));
	}

void CSqlValues::BindL(const CDbDataSource& aSource)
//
// Bind the values to a column set and optional column name list
//
	{
	iValues.Compress();				// we have finished adding values
	TInt columns=iValues.Count();
	__ASSERT(columns>0);
	if (aSource.ColumnCount()<columns)
		__LEAVE(KErrArgument);		// insert-statement is bad
	for (TInt ii=0;ii<columns;++ii)
		{
		TEntry& e=iValues[ii];
		__ASSERT(e.iType==-1);
		e.iType=aSource.ColumnDef(ii+1).Type();
		if (e.iValue.IsNull())
			continue;
		switch (e.iType)
			{
		default:	// cannot set this kind of column from SQL
			__LEAVE(KErrGeneral);
			break;
		case EDbColBit:
		case EDbColUint8:
		case EDbColUint16:
		case EDbColUint32:
			e.iValue.ToUint32L();
			break;
		case EDbColInt8:
		case EDbColInt16:
		case EDbColInt32:
			e.iValue.ToInt32L();
			break;
		case EDbColInt64:
			e.iValue.ToInt64L();
			break;
		case EDbColReal32:
			e.iValue.ToReal32L();
			break;
		case EDbColReal64:
			e.iValue.ToReal64L();
			break;
		case EDbColDateTime:
			e.iValue.ToTimeL();
			break;
		case EDbColText8:
		case EDbColLongText8:
			e.iValue.ToText8L();
			break;
		case EDbColText16:
		case EDbColLongText16:
			e.iValue.ToText16L();
			break;
			}
		}
	}

void CSqlValues::WriteL(CDbDataSource& aSource,CDbTable& aTable) const
//
// Set the row buffer with the values
//
	{
	__ASSERT(iValues.Count()>0);
	CDbBlobSpace* blobstore=0;
	TInt columns=iValues.Count();
	for (TInt ii=0;ii<columns;++ii)
		{
		TDbColumn col(aSource.Column(ii+1));
		const TEntry& e=iValues[ii];
		if (TDbCol::IsLong(e.iType))
			{	// check if we need to delete a blob
			if (!blobstore)
				blobstore=aTable.BlobsL();
			__ASSERT(blobstore);
			const TDbBlob& blob=TDbColumnC(col).Blob();
			if (!blob.IsInline())
				blobstore->DeleteL(blob.Id());
			}
		if (e.iValue.IsNull())
			{	// null value
			col.SetNull();
			continue;
			}
		switch (e.iType)
			{
		default:
			__ASSERT(0);
		case EDbColBit:
		case EDbColUint8:
		case EDbColUint16:
		case EDbColUint32:
			col.SetL(e.iValue.Uint32());
			break;
		case EDbColInt8:
		case EDbColInt16:
		case EDbColInt32:
			col.SetL(e.iValue.Int32());
			break;
		case EDbColInt64:
			col.SetL(e.iValue.Int64());
			break;
		case EDbColReal32:
			col.SetL(e.iValue.Real32());
			break;
		case EDbColReal64:
			col.SetL(e.iValue.Real64());
			break;
		case EDbColDateTime:
			col.SetL(e.iValue.Time());
			break;
		case EDbColText8:
			col.SetL(e.iValue.Text8());
			break;
		case EDbColText16:
			col.SetL(e.iValue.Text16());
			break;
		case EDbColLongText8:
			{
			const TDesC8& val=e.iValue.Text8();
			const TUint8* ptr=val.Ptr();
			TInt size=val.Length();
			if (size>blobstore->InlineLimit())
				col.SetBlobL(blobstore->CreateL(EDbColLongText8,ptr,size),size);
			else
				col.SetBlobL(ptr,size);
			}
			break;
		case EDbColLongText16:
			{
			const TDesC16& val=e.iValue.Text16();
			const TUint8* ptr=REINTERPRET_CAST(const TUint8*,val.Ptr());
			TInt size=val.Length()<<1;
			if (size>blobstore->InlineLimit())
				col.SetBlobL(blobstore->CreateL(EDbColLongText16,ptr,size),size);
			else
				col.SetBlobL(ptr,size);
			}
			break;
			}
		}
	}

// Class CSqlDMLStatement

CSqlDMLStatement::~CSqlDMLStatement()
	{
	delete iValues;
	}

CSqlValues& CSqlDMLStatement::ValuesL()
	{
	CSqlValues* v=iValues;
	if (!v)
		iValues=v=new(ELeave) CSqlValues;
	return *v;
	}

// Class CSqlInsertStatement

CSqlInsertStatement* CSqlInsertStatement::NewLC()
	{
	CSqlInsertStatement* self=new(ELeave) CSqlInsertStatement;
	CleanupStack::PushL(self);
	return self;
	}

CDbIncremental* CSqlInsertStatement::ExecuteL(CDbTableDatabase& aDatabase,TDbTextComparison aComparison,TInt& aRows)
//
// Execute an insert-statement. This does not requre incremental work, so return 0
//
	{
	aRows=1;	// 1 row changed after insertion
	CSqlQuery* query=&Query();
	RDbAccessPlan plan(query,aComparison);
	plan.BuildLC(aDatabase,RDbRowSet::EInsertOnly,TDbWindow());
	CDbDataSource& src=plan.Source();
	Values().BindL(src);
	src.NewRowL(KDbNullRecordId);
	RDbTransaction& t=plan.Table().Database().Transaction();
	t.DMLBeginLC();
	Values().WriteL(src,plan.Table());
	src.PrepareToWriteRowL(src.EAppend);
	src.WriteRowL(src.EAppend,src.ENoSynch);
	t.DMLCommitL();
	CleanupStack::Pop();				// transaction complete
	CleanupStack::PopAndDestroy();		// plan
	return 0;	// no incremental work to be done
	}

// Class CDbIncrementalDML

CDbIncrementalDML* CDbIncrementalDML::NewL(CSqlModifyStatement& aStatement,CDbTableDatabase& aDatabase,TDbTextComparison aComparison)
	{
	CSqlQuery* query=&aStatement.Query();
	RDbAccessPlan plan(query,aComparison);
	plan.BuildLC(aDatabase,RDbRowSet::EUpdatable,TDbWindow());
	CDbIncrementalDML* self=new(ELeave) CDbIncrementalDML(plan);
	CleanupStack::PopAndDestroy();	// plan
	if (aStatement.IsUpdate())
		{
		CleanupStack::PushL(self);
		self->iValues=aStatement.AdoptValues();
		self->iValues->BindL(*self->iSource);
		CleanupStack::Pop();	// self
		}
	self->iSource->Reset();
	self->Transaction().DMLBegin();
	self->SetState(EEvaluating);
	return self;
	}

CDbIncrementalDML::~CDbIncrementalDML()
	{
	if (iState!=ECommitted && iState!=EInitialising)
		Transaction().DMLRollback();
	delete iSource;
	delete iValues;
	}

TBool CDbIncrementalDML::NextL(TInt& aRows)
	{
	__ASSERT(iState!=ECommitted);
	if (iState==EFailed)
		__LEAVE(KErrDied);
	TState s=iState;
	iState=EFailed;
	TInt work=256;
	TDbPosition next=EDbNext;
	if (s==EEvaluating)
		{	// evaluate the data source
		TBool atrow=EFalse;
		if (iSource->EvaluateL(work,iRecord,atrow))
			{
			iState=EEvaluating;
			return ETrue;	// more to do
			}
		iRecord=KDbNullRecordId;
		next=EDbFirst;
		s=EUpdating;
		}
	// iterate across the data source
	for (;;)
		{
		if (s==EDeleting)
			{
			Transaction().DMLBeginLC();
			CDbDataSource::TDelete del=iSource->DeleteRowL(iRecord,CDbDataSource::ESynch);
			Transaction().DMLCommitL();
			CleanupStack::Pop();				// transaction complete
			++aRows;
			work-=4;
			switch (del)
				{
			case CDbDataSource::EDeletedAtEnd:
				Transaction().DMLTouch();
				Transaction().DMLCommitL();
				iState=ECommitted;
				return EFalse;
			case CDbDataSource::EDeletedInLimbo:
				s=EUpdating;
				// coverity[fallthrough]
			case CDbDataSource::EDeletedAtNext:
				if (work<0)
					{	// exhausted
					iState=s;
					return ETrue;
					}
				}
			}
		else
			{
			switch (iSource->GotoL(work,next,iRecord))
				{
			default:
				__ASSERT(0);
			case CDbDataSource::ESuccess:
				next=EDbNext;
				if (!IsUpdate())
					{
					s=EDeleting;
					break;
					}
				iSource->ReadRowL(iRecord);
				iValues->WriteL(*iSource,iTable);
				iSource->PrepareToWriteRowL(CDbDataSource::EReplace);
				Transaction().DMLBeginLC();
				iSource->WriteRowL(CDbDataSource::EReplace,CDbDataSource::ESynch);
				Transaction().DMLCommitL();
				CleanupStack::Pop();				// transaction complete
				++aRows;
				work-=8;
				break;
			case CDbDataSource::ESynchFailure:
				// dead
				__LEAVE(KErrDied);
			case CDbDataSource::EExhausted:
				// more to do
				iState=EUpdating;
				return ETrue;
			case CDbDataSource::ENoRow:
				// completed the operation!
				Transaction().DMLTouch();
				Transaction().DMLCommitL();
				iState=ECommitted;
				return EFalse;
				}
			}
		}
	}


// Class CSqlModifyStatement

CSqlModifyStatement* CSqlModifyStatement::NewLC()
	{
	CSqlModifyStatement* self=new(ELeave) CSqlModifyStatement;
	CleanupStack::PushL(self);
	return self;
	}

CDbIncremental* CSqlModifyStatement::ExecuteL(CDbTableDatabase& aDatabase,TDbTextComparison aComparison,TInt& aRows)
//
// Execute an update/delete-statement, returning the incremental object that does the work
//
	{
	aRows=0;		// no rows affected initially
	return CDbIncrementalDML::NewL(*this,aDatabase,aComparison);
	}
