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

#include "UT_STD.H"
#include "D32COMP.H"
#include <d32dbmsconstants.h>

// Class RDbAccessPlan::TPlan

TInt RDbAccessPlan::TPlan::OrderByPlan(const TPlan& aLeft,const TPlan& aRight)
	{
	TUint lpos=aLeft.Type();
	TUint rpos=aRight.Type();
	__ASSERT(lpos!=0 && rpos!=0 ); // should be no table iterators
	return lpos-rpos;
	}

RDbAccessPlan::TPlan::TType RDbAccessPlan::TPlan::Type() const
//
// flag values: 0=A, 1=C, 2=B, 3=D, 8=E/F, 10=G/H, 16=M/N, 18=S/T,	20=I/J, 21=K/L, 22=O/P, 23=Q/R	
// This determines order of plans
//	
	{
	static const TUint KPosition[]={EPlanA,EPlanC,EPlanB,EPlanD,0,0,0,0,EPlanEF,0,EPlanGH,0,0,0,0,0,EPlanMN,
							 0,EPlanST,0,EPlanIJ,EPlanKL,EPlanOP,EPlanQR};
	__ASSERT_ALWAYS(((iFlags&EMask) < (sizeof(KPosition)/sizeof(KPosition[0]))), User::Invariant());						 
	return TType(KPosition[iFlags&EMask]);
	}

// Class RDbAccessPlan::TBounds

RDbAccessPlan::TBounds::TBounds(const TPlan& aPlan)
	: iLowerPred(aPlan.iLower),iUpperPred(aPlan.iUpper),iLower(0),iUpper(0),iInclusion(0)
	{
	SetLowerBounds();
	SetUpperBounds();
	if (aPlan.iIndex->Key()[0].iOrder==TDbKeyCol::EDesc)
		{
		TDbLookupKey* t=iLower;
		iLower=iUpper;
		iUpper=t;
		iInclusion=(iInclusion>>1)|(iInclusion<<1);
		}
	}

void RDbAccessPlan::TBounds::GetLookupKey(const CSqlCompPredicate& aCompPredicate,TDbLookupKey& aLookup)
	{
	const RSqlLiteral& value=aCompPredicate.Value();
	switch (aCompPredicate.ColType())
		{
	default:
		__ASSERT(0);
	case EDbColBit:
	case EDbColInt8:
	case EDbColUint8:
	case EDbColInt16:
	case EDbColUint16:
	case EDbColInt32:
	case EDbColUint32:
	case EDbColInt64:
		aLookup.Add(value.Int64());
		break;
	case EDbColReal32:
	case EDbColReal64:
		aLookup.Add(value.Real64());
		break;
	case EDbColDateTime:
		aLookup.Add(value.Time());
		break;
	case EDbColText8:
	case EDbColLongText8:
		aLookup.Add(value.Text8());
		break;
	case EDbColText16:
	case EDbColLongText16:
		aLookup.Add(value.Text16());
		break;
		}
	}

void RDbAccessPlan::TBounds::SetLowerBounds()
	{
	if (!iLowerPred) // iLower already set to 0
		return;
	GetLookupKey(*iLowerPred,iLowerKey);
	switch (iLowerPred->NodeType())
		{
	default:
		__ASSERT(0);
	case CSqlSearchCondition::ELessEqual:
	case CSqlSearchCondition::ELess:
		iLower=0;
		break;
	case CSqlSearchCondition::EEqual:
	case CSqlSearchCondition::EGreaterEqual:
		iInclusion|=CDbRecordIndex::EIncludeLower;
	case CSqlSearchCondition::EGreater:
		iLower=&iLowerKey;
		break;
		}
	}

void RDbAccessPlan::TBounds::SetUpperBounds()
	{	
	if (!iUpperPred)
		return;
	GetLookupKey(*iUpperPred,iUpperKey);
	switch (iUpperPred->NodeType())
		{
	default:
		__ASSERT(0);
	case CSqlSearchCondition::EGreaterEqual:
	case CSqlSearchCondition::EGreater:
		iUpper=0;
		break;
	case CSqlSearchCondition::EEqual:
	case CSqlSearchCondition::ELessEqual:
		iInclusion|=CDbRecordIndex::EIncludeUpper;
	case CSqlSearchCondition::ELess:
		iUpper=&iUpperKey;
		break;
		}
	}

// Class RDbAccessPlan::CDbCompPredicateList

RDbAccessPlan::CDbCompPredicateList* RDbAccessPlan::CDbCompPredicateList::NewLC(CSqlQuery& aQuery,TDbTextComparison aComparison,const CDbTableDef& aTableDef)
	{
	CDbCompPredicateList* self=new(ELeave) CDbCompPredicateList(aTableDef,aComparison);
	CleanupStack::PushL(self);
	CSqlSearchCondition& sc=aQuery.SearchCondition();
	self->ConstructL(sc);
	return self;
	}

void RDbAccessPlan::CDbCompPredicateList::ConstructL(CSqlSearchCondition& aSearchCondition)
//
// fill the list with valid comp pred's
//
	{
	TUint type=Type(aSearchCondition.NodeType());
	if (type&ECompPred) 
		{
		CSqlCompPredicate* cp=aSearchCondition.CompPredicate();
		const TDesC& colName=cp->ColumnName();
		if (IsIndexed(colName))
			AppendL(cp);
		else
			iRestriction=ETrue;
		}
	else if (type&EAnd)	
		{
		CSqlMultiNode* multiNode=aSearchCondition.MultiNode();	
		for (TInt ii=multiNode->Count();ii--;)				
			{
			CSqlSearchCondition* node=multiNode->SubNode(ii);
			ConstructL(*node);
			}
		}	
	else
		iRestriction=ETrue;
	}

TUint RDbAccessPlan::CDbCompPredicateList::Type(CSqlSearchCondition::TType aType) const
//
// converts CSqlSearchCondition::TType into flag
//
	{
	switch (aType)
		{
	case CSqlSearchCondition::EAnd:
		return EAnd;
	case CSqlSearchCondition::ELess:
		return ELess;
	case CSqlSearchCondition::ELessEqual:
		return ELessEqual;
	case CSqlSearchCondition::EEqual:
		return EEqual;
	case CSqlSearchCondition::EGreaterEqual:
		return EGreaterEqual;
	case CSqlSearchCondition::EGreater:
		return EGreater;
	default:
		return ENone;
		}
	}

TBool RDbAccessPlan::CDbCompPredicateList::IsIndexed(const TDesC& aColumnName)
//
// Checks if aColumnName is indexed. If its a text column the comparison method should be the same
//
	{
	const CDbTableIndexDef* key=iTableDef.Key(aColumnName);
	if (!key)
		return EFalse;
	const TDbColumnDef* colDef=NULL;
	TRAPD(errCode, colDef=iTableDef.Columns().ColumnL(aColumnName));
	if(errCode != KErrNone)
		return EFalse;
	if (colDef->Type()>EDbColDateTime && key->Key().Comparison()!=iComparison)
		return EFalse;
	return ETrue;
	}

CSqlCompPredicate* RDbAccessPlan::CDbCompPredicateList::CompPredicate(TDbColNo aColNo,TUint aType)
//
// Returns first CompPredicate found in the list with required type & col no, and removes from list
//
	{
	for (TInt ii=Count();ii--;)
		{
		CSqlCompPredicate* cp=At(ii);
		TUint type=Type(cp->NodeType());
		TDbColNo colNo=cp->ColNo();
		if (!type&aType || aColNo!=colNo)
			continue;
		Delete(ii);
		return cp;
		}
	return 0;
	}

// class Validate

void Validate::NameL(const TDesC& aName)
	{
	if (aName.Length()<=KDbMaxName)
		{
		TLex lex(aName);
		if (!lex.Eos() && lex.Get().IsAlpha())
			{
			TChar c;
			do
				{
				if (lex.Eos())
					return;
				c=lex.Get();
				} while (c.IsAlphaDigit()||c=='_');
			}
		}
	__LEAVE(KErrBadName);
	}

void Validate::UniqueNameL(TDesC const** aNames,TInt aCount,const TDesC& aName)
//
// Ensure that aName is not a duplicate of any of aNames, and add
// the new name to the collection. Binary search is used for speed
//
	{
	TInt left=0;
	TInt right=aCount;
	while (left<right)
		{
		TInt mid=(left+right)>>1;
		TInt c=aNames[mid]->CompareF(aName);
		if (c<0)
			left=mid+1;
		else if (c>0)
			right=mid;
		else
			__LEAVE(KErrArgument);
		}
	Mem::Move(aNames+left+1,aNames+left,(aCount-left)*sizeof(TDesC const*));
	aNames[left]=&aName;
	}

void Validate::ColSetL(const CDbColSet& aColSet)
	{
	TDbColSetIter iter(aColSet);
	if (!iter)
		__LEAVE(KErrArgument);
	TDesC const** names=(TDesC const**)User::AllocLC(aColSet.Count()*sizeof(TDesC const*));
	do
		{
		const TDbCol& col=*iter;
		NameL(col.iName);
		UniqueNameL(names,iter.Col()-1,col.iName);
		TInt type=TInt(col.iType);
		if (type<TInt(EDbColBit)||type>TInt(EDbColLongBinary))
			__LEAVE(KErrNotSupported);
		else if (col.iMaxLength<1&&col.iMaxLength!=KDbUndefinedLength)
			__LEAVE(KErrArgument);
		else if (col.iAttributes&~(TDbCol::ENotNull|TDbCol::EAutoIncrement))
			__LEAVE(KErrNotSupported);				// unknown attributes
		else if (type>EDbColUint32 && col.iAttributes&TDbCol::EAutoIncrement)
			__LEAVE(KErrArgument);					// auto increment on non-integral type
		} while (++iter);
	CleanupStack::PopAndDestroy();
	}

void Validate::KeyL(const CDbKey& aKey,const HDbColumnSet& aColumns)
//
// Check that the key is valid for the table
//
	{
	TInt max=aKey.Count()-1;
	if (max<0)
		__LEAVE(KErrArgument);
	for (TInt ii=0;ii<=max;++ii)
		{
		const TDbKeyCol& kCol=aKey[ii];
		HDbColumnSet::TIteratorC col=aColumns.ColumnL(kCol.iName);
		if (col==NULL)
			__LEAVE(KErrNotFound);
		TInt len=kCol.iLength;
		if (len!=KDbUndefinedLength)
			{
			if (col->iType<=EDbColDateTime)
				__LEAVE(KErrArgument);
			TInt cLen=col->iMaxLength;
			if (ii<max)
				{
				if (len!=cLen)
					__LEAVE(KErrNotSupported);
				continue;
				}
			if (len<=0)
				__LEAVE(KErrArgument);
			else if (cLen!=KDbUndefinedLength && cLen<len)
				__LEAVE(KErrArgument);
			}
		}
	}

// Class RDbAccessPlan

TUint RDbAccessPlan::FindMatchL(const CDbTableIndexDef* aIndex)
//
// Checks if index best matches the order specified
//
	{
	CDbKey& order=iQuery->SortSpecification();
	TInt count=order.Count();
    const TDbColumnDef* columnDef = iTable->Def().Columns().ColumnL(order[count-1].iName);
    __ASSERT(columnDef != NULL);
    TInt columnLength = columnDef->iMaxLength;
	const CDbKey& key=aIndex->Key();
	TUint ret=0;
	if (TextKeyL(order) && order.Comparison()!=key.Comparison())
		return ret;
	TInt kCount=key.Count();
	for (TInt ii=0,rev=0;;)
		{
		const TDbKeyCol& kcol=key[ii];
		const TDbKeyCol& ocol=order[ii];
		if (kcol.iName.CompareF(ocol.iName)!=0)
			break;
		TInt revcol=kcol.iOrder^ocol.iOrder;
		if (ii==0)
			rev=revcol;
		else if (rev!=revcol)
			break;
		if (++ii==count)	// end of order key	
			{
			ret|=EMatch;
			if (kcol.iLength!=columnLength)
				ret|=ETruncated;
			if (rev)
				ret|=EReverse;
			return ret;
			}
		if (ii==kCount)		// end of index key
			{
			ret|=EMatch;
			if (key.IsUnique())
				{			// will provide the right order by, use it
				if (rev)
					ret|=EReverse;
			return ret;
				}
			break;
			}
		}
	return ret;
	}

TBool RDbAccessPlan::TextKeyL(const CDbKey& anOrder)
//
// return whether any of the keys are text columns
//
	{
	const HDbColumnSet& cols=iTable->Def().Columns();
	TInt count=anOrder.Count();
	for (TInt ii=0;ii<count;++ii)
		{
        const TDbColumnDef* columnDef = cols.ColumnL(anOrder[ii].iName);
        __ASSERT(columnDef != NULL);

        switch (columnDef->Type())
			{
		case EDbColText8:
		case EDbColText16:
		case EDbColLongText8:
		case EDbColLongText16:
			return ETrue;
		default:
			break;
			}
		}
	return EFalse;
	}

CDbTableSource* RDbAccessPlan::TableLC(CDbTableDatabase& aDatabase,const TDesC& aTable)
	{
	__ASSERT(!iSource);
	CDbTableSource* source=aDatabase.TableSourceL(aTable);
	iSource=source;
	iTable=&source->Table();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	return source;
	}

void RDbAccessPlan::Insert(CDbDataStage* aStage)
	{
	__ASSERT(iSource);
	aStage->SetSource(iSource);
	iSource=aStage;
	}

void RDbAccessPlan::Cleanup(TAny* aPtr)
	{
	RDbAccessPlan& self=*STATIC_CAST(RDbAccessPlan*,aPtr);
	self.iPlans.Close();
	delete self.iSource;
	}

CDbRecordIter* RDbAccessPlan::IteratorL(const TPlan& aPlan)
//
// Returns the right iterator
//
	{
	if (aPlan.iFlags&TPlan::EIndex)
		return iTable->IteratorL(*aPlan.iIndex);
	if (aPlan.iFlags&TPlan::EBounded)
		return BoundedIteratorL(aPlan);
	return iTable->IteratorL();
	}

CDbRecordIter* RDbAccessPlan::BoundedIteratorL(const TPlan& aPlan)
	{
	TBounds bounds(aPlan);
	CDbRecordIter* iter=iTable->IteratorL(*aPlan.iIndex,bounds.iInclusion,bounds.iLower,bounds.iUpper);
	if (aPlan.iLower)
		iQuery->RemovePredicate(aPlan.iLower);
	if (aPlan.iUpper && aPlan.iLower!=aPlan.iUpper)
		iQuery->RemovePredicate(aPlan.iUpper);
	return iter;
	}

void RDbAccessPlan::RestrictionL()
	{
	CDbRestrictStage* restriction=new(ELeave) CDbRestrictStage(iComparison);
	Insert(restriction);
	CSqlSearchCondition* searchcondition=iQuery->AdoptSearchCondition();
	restriction->SetRestriction(searchcondition);
	}

void RDbAccessPlan::OrderByL(const RDbTableRow& aRowBuf)
	{
	CDbOrderByStage* ordering=new(ELeave) CDbOrderByStage(aRowBuf);
	Insert(ordering);
	ordering->ConstructL(iQuery->SortSpecification());
	}

void RDbAccessPlan::ProjectionL()
	{
	CDbProjectStage* projection=new(ELeave) CDbProjectStage;
	Insert(projection);
	projection->ConstructL(iQuery->ColumnList(),iTable->Def().Columns());
	}

void RDbAccessPlan::WindowL(const TPlan& aPlan,const TDbWindow& aWindow)
	{
	if (aPlan.iFlags&TPlan::EWindow)
		Insert(new(ELeave) CDbWindowStage(KDbUnlimitedWindow));
	else if (aWindow.Size()!=aWindow.ENone && aWindow.Size()!=aWindow.EUnlimited)
		Insert(new(ELeave) CDbWindowStage(aWindow));
	}

TBool RDbAccessPlan::IsIndexIteratorL(TPlan& aPlan,const CDbTableIndexDef* aIndex)
//
// If an index iterator can be used, sets aPlan, else returns EFalse
//
	{
	if (!iQuery->HasSortSpecification())
		return EFalse;
	TUint ret=FindMatchL(aIndex);
	if ((ret&EMatch)==0)
		return EFalse;
	// can use index iterator
	aPlan.iFlags&=~TPlan::EOrder;
	if (ret&EReverse)
		aPlan.iFlags|=TPlan::EReverse;
	aPlan.iFlags|=TPlan::EIndex;
	aPlan.iIndex=aIndex;
	return ETrue;
	}

TBool RDbAccessPlan::IsBoundedIteratorL(TPlan& aPlan,const CDbTableIndexDef* aIndex)
//
// If a bounded iterator can be used, sets aPlan, else returns EFalse
//
	{
	if (!iQuery->HasSearchCondition())
		return EFalse;
	TDbColNo keyColNo=iTable->Def().Columns().ColNoL(aIndex->Key()[0].iName);
	CDbCompPredicateList* list=CDbCompPredicateList::NewLC(*iQuery,iComparison,iTable->Def());
	CSqlCompPredicate* cp=list->CompPredicate(keyColNo,CDbCompPredicateList::EEqual); 
	if (cp==0 && (cp=list->CompPredicate(keyColNo))==0)								   
		{
		CleanupStack::PopAndDestroy(); // list
		return EFalse; 
		}
	// boundaries
	TUint type=list->Type(cp->NodeType());
	aPlan.iLower=type&(CDbCompPredicateList::ELess|CDbCompPredicateList::ELessEqual)?0:cp;
	aPlan.iUpper=type&(CDbCompPredicateList::EGreater|CDbCompPredicateList::EGreaterEqual)?0:cp;
	// find other boundary, if present
	if (list->Count()!=0 && cp->NodeType()!=CSqlSearchCondition::EEqual)	
		{
		TUint nextType=type&(CDbCompPredicateList::ELess|CDbCompPredicateList::ELessEqual) ? 
			CDbCompPredicateList::EGreater|CDbCompPredicateList::EGreaterEqual : 
			CDbCompPredicateList::ELess|CDbCompPredicateList::ELessEqual;
		CSqlCompPredicate* cp2=list->CompPredicate(keyColNo,nextType);
		if (cp2)
			{
			if (nextType&(CDbCompPredicateList::ELess|CDbCompPredicateList::ELessEqual))
				aPlan.iUpper=cp2;
			else
				aPlan.iLower=cp2;
			}
		}	
	// check which order the index is in and reverse if descending
	const TDbKeyCol& key=aIndex->Key()[0];
	if ((key.iOrder==TDbKeyCol::EDesc && !aPlan.iUpper) || (key.iOrder==TDbKeyCol::EAsc && !aPlan.iLower))
		aPlan.iFlags|=TPlan::EReverse;				// optimise the bounding for forwards iteration
	if (!list->IsRestriction() && list->Count()==0)
		aPlan.iFlags&=~TPlan::ERestrict;
	CleanupStack::PopAndDestroy();			// list
	aPlan.iFlags|=TPlan::EBounded;
	aPlan.iIndex=aIndex;
	return ETrue;
	}

void RDbAccessPlan::EvaluatePlansL()
//
// try to find a bounded or index iterator
//
	{
	TPlan plan;
	// initialise flags
	if (iQuery->HasSearchCondition())
		plan.iFlags|=TPlan::ERestrict;
	if (iQuery->HasSortSpecification())
		plan.iFlags|=TPlan::EOrder;
	// find the right type of iterator
	TSglQueIterC<CDbTableIndexDef> indexes(iTable->Def().Indexes().AsQue());
	for (const CDbTableIndexDef* index;(index=indexes++)!=0;)
		{ // only on first column
		TPlan newPlan=plan;
		TBool foundIter=IsBoundedIteratorL(newPlan,index);
		if (!foundIter)
			foundIter=IsIndexIteratorL(newPlan,index);
		else
			EvaluateReorderStage(newPlan,index);
		if (foundIter)
			{
			EvaluateWindowStage(newPlan);
			__LEAVE_IF_ERROR(iPlans.Append(newPlan));
			}
		}
	}

void RDbAccessPlan::ChoosePlanL(TPlan& aPlan)
	{
	ReducePlans();
	if (iPlans.Count()==0)
		{
		CreateTableIteratorPlan(aPlan);
		return;
		}
	TPlan::TType type=iPlans[0].Type();
	if (!iQuery->HasSearchCondition())						
		{													
		__ASSERT(type==TPlan::EPlanEF);
		GetSmallestKeySize(aPlan,TPlan::EPlanEF);
		}
	else if (!iQuery->HasSortSpecification())					
		{													
		if (type==TPlan::EPlanIJ)								
			{	
			GetSmallestKeySize(aPlan,TPlan::EPlanIJ);
			TInt r=IndexSpanL(aPlan);
			if (r!=CDbTable::EUnavailableSpan && !iWindow && r>60)
				CreateTableIteratorPlan(aPlan);
			}
		else if (type==TPlan::EPlanOP)					
			{
			TInt r=GetTightestRestrictionL(aPlan,TPlan::EPlanOP);
			if (r==CDbTable::EUnavailableSpan)	// no index stats available
				aPlan=iPlans[0];				// use first O/P as a guess
			else if ((!iWindow && r>55) || (iWindow && r>60))
				CreateTableIteratorPlan(aPlan);
			}
		else
			__ASSERT(0);
		}
	else if (type==TPlan::EPlanMN) 
		{
		GetSmallestKeySize(aPlan,TPlan::EPlanMN);
		if (iAccess==RDbRowSet::EUpdatable || iWindow)
			aPlan.iFlags|=TPlan::EWindow;
		}
	else if (type==TPlan::EPlanKL) 
		{	
		GetSmallestKeySize(aPlan,TPlan::EPlanKL);
		TInt r=IndexSpanL(aPlan);
		if (r!=CDbTable::EUnavailableSpan && r>60)
			{	// don't use K plan
			if (r<75 || GetSmallestKeySize(aPlan,TPlan::EPlanGH)==KErrNotFound)
				CreateTableIteratorPlan(aPlan);	
			}
		}
	else if (type==TPlan::EPlanQR) 
		{
		TInt r=GetTightestRestrictionL(aPlan,TPlan::EPlanQR);
		if (r==CDbTable::EUnavailableSpan)			// no index stats available
			aPlan=iPlans[0];						// use first Q/R as a guess
		else if (r>60)
			CreateTableIteratorPlan(aPlan);	
		}
	else 
		{
		__ASSERT(type==TPlan::EPlanGH);
		// don't use this plan without further data, resort to default
		CreateTableIteratorPlan(aPlan);	
		}
	}

void RDbAccessPlan::EvaluateWindowStage(TPlan& aPlan)
	{
	if (!iWindow)
		return;
	TUint f=aPlan.iFlags|TPlan::EWindow;
	if (f&TPlan::EReorder)
		f&=~TPlan::EWindow;
	if (f&TPlan::ERestrict)
		f|=TPlan::EWindow; 
	if (f&TPlan::EOrder)						// order-by stage includes window
		f&=~TPlan::EWindow;
	aPlan.iFlags=f;
	}

TInt RDbAccessPlan::GetTightestRestrictionL(TPlan& aPlan,TPlan::TType aType)
//
// aPlan is set to the smallest restricted plan with aType
//
	{
	TInt span=KMaxTInt;
	TPlan plan;
	for (TInt ii=iPlans.Count();ii--;)
		{
		if (iPlans[ii].Type()!=aType)
			continue;
		TInt t=IndexSpanL(iPlans[ii]);
		if (t==CDbTable::EUnavailableSpan)
			continue;
		__ASSERT(t == span ?  (iPlans[ii].iIndex != NULL && plan.iIndex != NULL) : ETrue);
		//coverity[uninit_use_in_call]
		if (t<span || (t==span && iPlans[ii].iIndex->Key().Count()<plan.iIndex->Key().Count()))
			{
			span=t;
			plan=iPlans[ii];
			}
		}
	aPlan=plan;
	return span!=KMaxTInt?span:CDbTable::EUnavailableSpan;
	}

TInt RDbAccessPlan::GetSmallestKeySize(TPlan& aPlan,TPlan::TType aType)
//
// aPlan is set to the plan with smallest index with aType
//
	{
	TInt cols=KMaxTInt;
	TPlan plan;
	for (TInt ii=iPlans.Count();ii--;)
		{
		if (iPlans[ii].Type()!=aType)
			continue;
		__ASSERT(iPlans[ii].iIndex);
		TInt count=iPlans[ii].iIndex->Key().Count();
		if (count<cols)
			{
			cols=count;
			plan=iPlans[ii];
			}
		}
	aPlan=plan;
	return cols!=KMaxTInt?cols:KErrNotFound;
	}

TInt RDbAccessPlan::IndexSpanL(const TPlan& aPlan)
	{
	__ASSERT(aPlan.iIndex);
	TBounds bounds(aPlan);
	return iTable->IndexSpanL(*aPlan.iIndex,bounds.iInclusion,bounds.iLower,bounds.iUpper);
	}

void RDbAccessPlan::ReducePlans()
	{
	for (TInt ii=iPlans.Count();--ii>=0;)
		{
		switch (iPlans[ii].Type())
			{
		default:
			continue;
		case TPlan::EPlanGH:									
			if (iWindow)
				iPlans.Remove(ii);								// remove Gw/Hw 
			break;
		case TPlan::EPlanST:
			iPlans[ii].iFlags|=(TPlan::EReorder|TPlan::EOrder);	// convert S/T to Q/R
			EvaluateWindowStage(iPlans[ii]);
			break;
			};
		}
// explicit conversion required as GCC can't find the TLinearOrder instantiation
	iPlans.Sort(TLinearOrder<TPlan>(TPlan::OrderByPlan));
	}

void RDbAccessPlan::CreateTableIteratorPlan(TPlan& aPlan)
	{
	aPlan.iFlags&=~(TPlan::EIndex|TPlan::EBounded);
	if (iQuery->HasSearchCondition())
		aPlan.iFlags=TPlan::ERestrict;
	if (iQuery->HasSortSpecification())
		aPlan.iFlags|=TPlan::EOrder;
	EvaluateWindowStage(aPlan);
	}

void RDbAccessPlan::EvaluateReorderStage(TPlan& aPlan,const CDbTableIndexDef* aIndex)
//
// for a bounded iter with an order by - can the index be used?
//
	{
	aPlan.iFlags|=TPlan::EReorder;
	if (!iQuery->HasSortSpecification())
		return;
	TUint ret=0;
	TRAPD(errCode, ret=FindMatchL(aIndex));
	if (errCode==KErrNone && ret&EMatch)
		{// do we really need a reorder stage?
		aPlan.iFlags&=~TPlan::EOrder;			// don't need to order it
		aPlan.iFlags&=~TPlan::EReorder;			// don't need a reorder stage 
		if (ret&EReverse)			
			aPlan.iFlags|=TPlan::EReverse;
		else
			aPlan.iFlags&=~TPlan::EReverse;
		}
	}

void RDbAccessPlan::PrepareQueryL(CDbTableSource* aSource)
	{
	if (iQuery->HasSearchCondition())
		{ 
		CSqlSearchCondition& sc=iQuery->SearchCondition();
		sc.BindL(aSource->Row());
		}
	if (iQuery->HasSortSpecification())
		{
		CDbKey& order=iQuery->SortSpecification();
		order.SetComparison(iComparison);
		Validate::KeyL(order,iTable->Def().Columns());
		}
	}
	

void RDbAccessPlan::BuildLC(CDbTableDatabase& aDatabase,RDbRowSet::TAccess aAccess,const TDbWindow& aWindow)
//
// Prepare the data pipeline
//
	{
	__ASSERT(iQuery);
	CDbTableSource* source=TableLC(aDatabase,iQuery->Table());
	//
	if (aAccess!=RDbRowSet::EInsertOnly)
		{	// insert only views do not use Iterators, Restrictions or Windows
		iWindow=aWindow.Size()==aWindow.EUnlimited;
		iAccess=aAccess;
		PrepareQueryL(source);
		EvaluatePlansL();
		TPlan plan;
		ChoosePlanL(plan);
		source->SetIterator(IteratorL(plan));
		if (plan.iFlags&TPlan::EReorder)
			Insert(new(ELeave) CDbReorderWindowStage); 
		if (plan.iFlags&TPlan::EReverse)
			source->ReverseIteratorL();
		if (plan.iFlags&TPlan::ERestrict)
			RestrictionL();
		if (plan.iFlags&TPlan::EOrder)	
			OrderByL(source->Row());
		WindowL(plan,aWindow);
		iPlans.Reset();
		}
	if (iQuery->HasColumnList())
		ProjectionL();
	}

void RDbAccessPlan::BuildLC(CDbTableDatabase& aDatabase,const TDesC& aTable,RDbRowSet::TAccess aAccess)
	{
	CDbTableSource* source=TableLC(aDatabase,aTable);
	if (aAccess!=RDbRowSet::EInsertOnly)
		source->SetIterator(iTable->IteratorL());
	}
