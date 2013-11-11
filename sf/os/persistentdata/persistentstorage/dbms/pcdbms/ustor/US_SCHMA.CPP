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

#include "US_STD.H"
#include "U32STD_DBMS.H"
#include <d32dbmsconstants.h>

// class TRecordSize

TUint8 const TRecordSize::FieldSizes[]=
	{
	0,					// EDbColBit
	sizeof(TInt8),		// EDbColInt8
	sizeof(TUint8),		// EDbColUint8
	sizeof(TInt16),		// EDbColInt16
	sizeof(TUint16),	// EDbColUint16
	sizeof(TInt32),		// EDbColInt32
	sizeof(TUint32),	// EDbColUint32
	sizeof(TInt64),		// EDbColInt64
	sizeof(TReal32),	// EDbColReal32
	sizeof(TReal64),	// EDbColReal64
	sizeof(TTime)		// EDbColDateTime
	};

TBool TRecordSize::Set(const HDbColumnSet& aColumns)
//
// Calculate stats for the record size and shape from the definition
//
	{
	TInt fix=0,null=0,var=0,blob=0;
	HDbColumnSet::TIteratorC const end=aColumns.End();
	HDbColumnSet::TIteratorC col=aColumns.Begin();
	do
		{
		TBool notnull=col->iAttributes&TDbCol::ENotNull;
		if (notnull==0)
			++fix;
		TDbColType type=col->Type();
		__ASSERT(type>=EDbColBit&&type<=EDbColLongBinary);
		if (type==EDbColBit)
			++fix;
		else if (type<=EDbColDateTime)
			{
			TInt bits=FixedFieldSize(type)<<3;
			if (notnull)
				fix+=bits;
			else
				null+=bits;
			}
		else if (type<=EDbColBinary)
			{
			TInt size=col->iMaxLength;
			if (type==EDbColText16)
				size<<=1;
			var+=8+(size<<3);
			}
		else
			++blob;
		} while(++col<end);
//
// assuming Blobs take at least 16 bytes + 1 bit
	TInt max=(fix+null+var+blob*(1+(KMinInlineLimit<<3))+7)>>3;
	if (max>KDbStoreMaxRecordLength)
		return ETrue;
//
// Assuming a Maximally full record, how much excess space is available for Blobs?
//
	iInlineLimit=KDbMaxInlineBlobSize;
	if (blob)
		{	// use the spare space for extra inlining
		TInt spare=(KDbStoreMaxRecordLength-max);
		TInt inl=spare/blob+KMinInlineLimit-1;
		if (inl<KDbMaxInlineBlobSize)
			iInlineLimit=inl;
		}
//
// Calculate the average cluster size for a column set
// This assumes that the nullable columns are present 50%, Variable average 1/16
// Blobs achieve 16 bytes, or inline limit (if smaller)
//
	TInt average=(fix+(null>>1)+(var>>4)+blob*(1+(16<<3))+7)>>3;
	TInt clustering=KClusterLimit/average;
	if (clustering==0)
		clustering=1;
	else if (clustering>KMaxClustering)
		clustering=KMaxClustering;
	iClustering=clustering;
	return EFalse;
	}

void TRecordSize::CheckSizeL(const HDbColumnSet& aColumns)
//
// Check that the columns definition is a valid size
//
	{
	TRecordSize size;
	if (size.Set(aColumns))
		__LEAVE(KErrTooBig);
	}

TInt TRecordSize::InlineLimit(const HDbColumnSet& aColumns)
//
// Evaluate the inline limit for the column set. It is assumed to be valid
//
	{
	TRecordSize size;
	__DEBUG(TBool chk =)size.Set(aColumns);
	__ASSERT(!chk);
	return size.InlineLimit();
	}

// Streaming column definitions

LOCAL_C void ExternalizeL(const TDbColumnDef& aCol,RWriteStream& aStream)
	{
	aStream<<*aCol.iName;
	aStream.WriteUint8L(aCol.iType);
	aStream.WriteUint8L(aCol.iAttributes);
	switch (aCol.iType)
		{
	case EDbColBinary:
	case EDbColText8:
	case EDbColText16:
		aStream.WriteUint8L(aCol.iMaxLength);
		break;
	default:
		break;
		}
	}

LOCAL_C void InternalizeL(TDbColumnDef& aCol,RReadStream& aStream)
	{
	aCol.iName=HBufC::NewL(aStream,KDbMaxColName);
	TDbColType type=TDbColType(aStream.ReadUint8L());
	aCol.iType=TUint8(type);
	aCol.iAttributes=aStream.ReadUint8L();
	if (type>EDbColLongBinary || (aCol.iAttributes&~(TDbCol::ENotNull|TDbCol::EAutoIncrement))!=0)
		__LEAVE(KErrCorrupt);
	if (type>=EDbColText8 && type<=EDbColBinary)
		{
		aCol.iMaxLength=aStream.ReadUint8L();
		if (aCol.iMaxLength==0)
			__LEAVE(KErrCorrupt);
		}
	else
		aCol.iMaxLength=KDbUndefinedLength;
	}

inline RWriteStream& operator<<(RWriteStream& aStream,const TDbColumnDef& aCol)
	{ExternalizeL(aCol,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TDbColumnDef& aCol)
	{InternalizeL(aCol,aStream);return aStream;}

// Streaming key columns

LOCAL_C void ExternalizeL(const TDbKeyCol& aKeyCol,RWriteStream& aStream)
	{
	aStream<<aKeyCol.iName;
	aStream.WriteUint8L(aKeyCol.iLength!=KDbUndefinedLength ? aKeyCol.iLength : 0);
	aStream.WriteUint8L(aKeyCol.iOrder);
	}

LOCAL_C void InternalizeL(TDbKeyCol& aKeyCol,RReadStream& aStream)
	{
	TPtr des=aKeyCol.iName.Des();
	aStream>>des;
	TUint len=aStream.ReadUint8L();
	aKeyCol.iLength=len!=0 ? TInt(len) : KDbUndefinedLength;
	aKeyCol.iOrder=TDbKeyCol::TOrder(aStream.ReadUint8L());
	if (aKeyCol.iOrder>TDbKeyCol::EDesc)
		__LEAVE(KErrCorrupt);
	}

inline RWriteStream& operator<<(RWriteStream& aStream,const TDbKeyCol& aCol)
	{ExternalizeL(aCol,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TDbKeyCol& aCol)
	{InternalizeL(aCol,aStream);return aStream;}


// Class CDbStoreIndexDef

CDbStoreIndexDef::CDbStoreIndexDef()
	{}

CDbStoreIndexDef* CDbStoreIndexDef::NewLC(const TDesC& aName)
	{
	CDbStoreIndexDef* self=new(ELeave) CDbStoreIndexDef;
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}

CDbStoreIndexDef* CDbStoreIndexDef::NewLC(const TDesC& aName,const CDbKey& aKey,const HDbColumnSet& aColumns)
	{
	CDbStoreIndexDef* self=NewLC(aName);
	CDbKey& key=self->Key();
	TInt max=aKey.Count();
	for (TInt ii=0;ii<max;++ii)
		{
		TDbKeyCol kCol=aKey[ii];
		const TDbColumnDef& col=*aColumns.ColumnL(kCol.iName);
		switch (col.iType)
			{
		default:
			break;
		case EDbColText8:
		case EDbColText16:
			if (kCol.iLength==KDbUndefinedLength)
				kCol.iLength=col.iMaxLength;
			break;
		case EDbColLongText8:
		case EDbColLongText16:
			if (kCol.iLength==KDbUndefinedLength)
				__LEAVE(KErrArgument);
			break;
		case EDbColBinary:
		case EDbColLongBinary:
			__LEAVE(KErrArgument);
			break;
			}
		key.AddL(kCol);
		}
	if (aKey.IsUnique())
		key.MakeUnique();
	if (aKey.IsPrimary())
		key.MakePrimary();
	key.SetComparison(aKey.Comparison());
	CheckSizeL(key,aColumns);
	return self;
	}

CDbStoreIndexDef* CDbStoreIndexDef::NewL(RReadStream& aStream)
//
// Construct an index definition from persistent storage
//
	{
	TDbName name;
	aStream>>name;
	CDbStoreIndexDef* self=NewLC(name);
	CDbKey& key=self->Key();
	TDbTextComparison comp=TDbTextComparison(aStream.ReadUint8L());
	if (comp>EDbCompareCollated)
		__LEAVE(KErrCorrupt);
	key.SetComparison(comp);
	if (aStream.ReadUint8L())
		key.MakeUnique();
	TCardinality count;
	aStream>>count;
	for (TInt ii=count;ii>0;--ii)
		{
		TDbKeyCol keycol;
		aStream>>keycol;
		key.AddL(keycol);
		}
	aStream>>self->iTokenId;
	CleanupStack::Pop();
	return self;
	}

void CDbStoreIndexDef::ExternalizeL(RWriteStream& aStream) const
	{
	aStream<<Name();
	const CDbKey& key=Key();
	aStream.WriteUint8L(TUint8(key.Comparison()));
	aStream.WriteUint8L(key.IsUnique());
	TInt max=key.Count();
	aStream<<TCardinality(max);
	for (TInt ii=0;ii<max;++ii)
		aStream<<key[ii];
	aStream<<iTokenId;
	}

TInt CDbStoreIndexDef::KeySize(const TDbKeyCol& aKeyCol,const TDbColumnDef& aColumn)
	{
	LOCAL_D const TUint8 KFixedSize[]=
		{
		sizeof(TUint32),
		sizeof(TInt32),
		sizeof(TUint32),
		sizeof(TInt32),
		sizeof(TUint32),
		sizeof(TInt32),
		sizeof(TUint32),
		sizeof(TInt64),
		sizeof(TReal32),
		sizeof(TReal64),
		sizeof(TTime)
		};
	TDbColType t=aColumn.Type();
	if (TUint(t)<sizeof(KFixedSize)/sizeof(KFixedSize[0]))
		return KFixedSize[t];
	switch (t)
		{
	default:
		__ASSERT(0);
	case EDbColText8:
	case EDbColLongText8:
		return aKeyCol.iLength;
	case EDbColText16:
	case EDbColLongText16:
		return aKeyCol.iLength<<1;
		}
	}

void CDbStoreIndexDef::CheckSizeL(const CDbKey& aKey,const HDbColumnSet& aColSet)
//
// Check the size of the key for the index definition
//
	{
	TInt len=aKey.IsUnique()?0:sizeof(TDbRecordId);
	for (TInt ii=aKey.Count();--ii>=0;)
		{
		const TDbKeyCol& keyCol=aKey[ii];
		len+=Align4(KeySize(keyCol,*aColSet.ColumnL(keyCol.iName)));
		}
	if (len>KMaxIndexKeySize)
		__LEAVE(KErrTooBig);
	}


// Class CDbStoreDef

LOCAL_C void SetColumnL(TDbColumnDef& aDef,const TDbCol& aCol,TUint aFlag=0)
	{
	aDef.SetL(aCol);
	if (aDef.iAttributes&TDbCol::EAutoIncrement)
		aDef.iAttributes|=TDbCol::ENotNull;	// auto-increment => not-null
	aDef.iFlags=TUint8(aFlag);
	TDbColType type=aCol.iType;
	if (type>=EDbColText8 && type<=EDbColBinary)
		{
		if (aCol.iMaxLength==KDbUndefinedLength)
			aDef.iMaxLength=KDbStoreMaxColumnLength;
		else if (aCol.iMaxLength>KDbStoreMaxColumnLength)
			__LEAVE(KErrNotSupported);
		}
	else
		aDef.iMaxLength=KDbUndefinedLength;
	}

LOCAL_C HDbColumnSet::TIterator CheckColumnsL(HDbColumnSet::TIterator anIter,const CDbColSet& aColSet,TInt aNotNull,TUint aFlag=0)
//
// Check the columns from aColset into anIter, according to ENotNull attribute
// Validate as we go
//
	{
	for (TDbColSetIter iter(aColSet);iter;++iter)
		{
		TInt att=iter->iAttributes;
		if (att&TDbCol::EAutoIncrement)
			att|=TDbCol::ENotNull;		// auto-increment => not-null
		if ((att&TDbCol::ENotNull)==aNotNull)
			{
			SetColumnL(*anIter,*iter,aFlag);
			++anIter;
			}
		}
	return anIter;
	}

CDbStoreDef::CDbStoreDef()
	{}

CDbStoreDef* CDbStoreDef::NewLC(const TDesC& aName,TInt aColumnCount)
	{
	CDbStoreDef* self=new(ELeave) CDbStoreDef;
	CleanupStack::PushL(self);
	self->ConstructL(aName,aColumnCount);
	return self;
	}

CDbStoreDef* CDbStoreDef::NewLC(const TDesC& aName,const CDbColSet& aColSet)
//
// Construct a table definition from the column set supplied
//
	{
	CDbStoreDef* self=NewLC(aName,aColSet.Count());
	HDbColumnSet& columns=self->Columns();
	HDbColumnSet::TIterator def=CheckColumnsL(columns.Begin(),aColSet,TDbCol::ENotNull);
	def=CheckColumnsL(def,aColSet,0);
	__ASSERT(def==columns.End());
	TRecordSize::CheckSizeL(columns);
	self->Changed();
	return self;
	}

CDbStoreDef* CDbStoreDef::NewL(RReadStream& aStream)
//
// Construct a table definition from persistent storage
//
	{
	TDbName name;
	aStream>>name;
	TCardinality count;
	aStream>>count;
	CDbStoreDef* self=NewLC(name,count);
	HDbColumnSet& columns=self->Columns();
	HDbColumnSet::TIterator iter=columns.Begin();
	const HDbColumnSet::TIteratorC end=columns.End();
	do
		{
		aStream>>*iter;
		} while (++iter<end);
	aStream>>count;
	TInt cluster=count;
	if (cluster==0 || cluster>KMaxClustering)
		__LEAVE(KErrCorrupt);
	aStream>>self->iTokenId;
	RDbIndexes& indexes=self->Indexes();
	aStream>>count;
	for (TInt ii=count;ii>0;--ii)
		indexes.Add(CDbStoreIndexDef::NewL(aStream));
	self->Changed();
	CleanupStack::Pop();
	return self;
	}

void CDbStoreDef::Changed()
//
// The definition has changed, following creation or alteration of the table
// Recalculate cached data for the definition.
//
	{
	CDbTableDef::Changed();
	__DEBUG(TBool dbg =)iInfo.Set(Columns());
	__ASSERT(!dbg);
	}

void CDbStoreDef::ExternalizeL(RWriteStream& aStream) const
	{
	aStream<<Name();
	const HDbColumnSet& columns=Columns();
	aStream<<TCardinality(columns.Count());
	HDbColumnSet::TIteratorC iter=columns.Begin();
	const HDbColumnSet::TIteratorC end=columns.End();
	do
		{
		aStream<<*iter;
		} while (++iter<end);
	aStream<<TCardinality(Clustering());	// old stuff, not needed
	aStream<<iTokenId;
	aStream<<TCardinality(Indexes().Count());
	TSglQueIterC<CDbStoreIndexDef> ixIter(Indexes().AsQue());
	for (const CDbStoreIndexDef* def;(def=ixIter++)!=0;)
		aStream<<*def;
	}

void CDbStoreDef::AlteredColumnSetL(HDbColumnSet& aSet,const CDbColSet& aChange,const CDbColSet& aAdd)
//
// Generate an altered column set
// We can hijack the non-user attribs of the column sets for marking changes
//
	{
	// add not-null columns to the front
	HDbColumnSet::TIterator newCol=CheckColumnsL(aSet.Begin(),aAdd,TDbCol::ENotNull,TDbColumnDef::EAdded);
	// copy current set, minus deleted ones, apply text column length changes
	TDbColSetIter change(aChange);
	HDbColumnSet::TIterator col=Columns().Begin();
	HDbColumnSet::TIteratorC const end=Columns().End();
	do
		{
		TUint flag=col->iFlags;
		if (flag&TDbColumnDef::EDropped)
			continue;
		if (flag&(TDbColumnDef::EChangedType|TDbColumnDef::EChangedLen))
			{
			// check allowed changes
			SetColumnL(*newCol,*change);
			++change;
			if (flag&TDbColumnDef::EChangedType)
				{	// validate type changes (only text->longtext etc)
				if (!TDbCol::IsLong(newCol->Type()) || newCol->iType-col->iType!=3)
					__LEAVE(KErrNotSupported);
				}
			else
				{
				col->iFlags=TUint8(flag&~TDbColumnDef::EChangedLen);	// no real changes req'd
				if (newCol->iMaxLength<col->iMaxLength)
					__LEAVE(KErrNotSupported);					// can only extend columns
				}
			}
		else
			newCol->SetL(*col);
		++newCol;
		} while (++col<end);
	// add nullable columns to the end
	newCol=CheckColumnsL(newCol,aAdd,0,TDbColumnDef::EAdded);
	__ASSERT(newCol==aSet.End());
	TRecordSize::CheckSizeL(aSet);
	}

