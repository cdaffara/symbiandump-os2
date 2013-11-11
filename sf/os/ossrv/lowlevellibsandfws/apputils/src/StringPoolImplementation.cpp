// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "StringPoolImplementation.h"

const TInt KMapGranularity=20;

CStringPoolNode::~CStringPoolNode()
	{
	delete iDes;
	}

CStringPoolImplementation::CStringPoolImplementation() : iStringMapList(KMapGranularity, _FOFF(TStringIdMap, iSourceTableVal)), iStringMapListReverse(KMapGranularity, _FOFF(TStringIdMap, iTargetTableVal))
	{
	}

CStringPoolImplementation::~CStringPoolImplementation()
	{
	// Look for non-expirable strings
	TInt i;
	for (i = 0; i < KHashModulo; i++ )
		{
		DeleteUndeletableStrings(iCIHashTable, i);
		DeleteUndeletableStrings(iCSHashTable, i);		
		}
#ifdef _DEBUG

	__LOG(_L8(":Closing String Pool.\n"))
		TBool leaksFound = EFalse;
	// Check that the string pool is empty, or more accurately that
	// everything in it is a pre-loaded string
	for (i = 0; i < KHashModulo; i++ )
		{
		if (iCIHashTable[i])
			{
			for (TInt j = 0; j < iCIHashTable[i]->Count(); j++)
				{
				if (!StringUtils::IsTableEntry(iCIHashTable[i]->At(j).iVal))
					{
					if (!leaksFound)
						{
						__LOG(_L8("The following strings were leaked through not being Closed:\n"))
						leaksFound = ETrue;
						}

					// Get the problem string
					CStringPoolNode* theProblem = 
						reinterpret_cast<CStringPoolNode*>(
							iCIHashTable[i]->At(j).iVal & KTokenToNode);
					if(theProblem->iDes)
					    __LOG(theProblem->iDes->Des());
					}
				}
			}
		if (iCSHashTable[i])
			{
			for (TInt j = 0; j < iCSHashTable[i]->Count(); j++)
				{
				if (!StringUtils::IsTableEntry(iCSHashTable[i]->At(j).iVal))
					{
					if (!leaksFound)
						{
						__LOG(_L8("The following strings were leaked through not being Closed:\n"))
						leaksFound = ETrue;
						}

					// Get the problem string
					CStringPoolNode* theProblem = 
						reinterpret_cast<CStringPoolNode*>(
							iCSHashTable[i]->At(j).iVal & KTokenToNode);
					if(theProblem->iDes)
					    __LOG(theProblem->iDes->Des());
					}
				}
			}
		if (leaksFound)
			__DEBUGGER();
		}

	if (!leaksFound)
		__LOG(_L8("No leakages were detected\n"));

#endif //_DEBUG
	for (TInt ii = 0; ii < KHashModulo; ii++)
		{
		delete iCIHashTable[ii];
		delete iCSHashTable[ii];
		}
		
	iTablePtrs.Close();
	iStringMapList.Close();
	iStringMapListReverse.Close();
	iRollbackMapList.Close();	
	iRollbackHashListCS.Close();
	iRollbackHashListCI.Close();
	// Notify the external users of the StringPool that the object is getting closed
	TInt cBCounter = iCallBacks.Count();
	if(cBCounter>0)
		{
		while (--cBCounter>=0)
			{
			iCallBacks[cBCounter]->StringPoolClosing();
			}
		}
	iCallBacks.Close();
	}

// Check for any undeletable string and delete them now
void CStringPoolImplementation::DeleteUndeletableStrings(CArrayFixSeg<RStringTokenEither>* aArray[KHashModulo], TInt i)
	{
	if (aArray[i])
		{
		for (TInt j = 0; j < aArray[i]->Count(); ++j)
			{
			if (!StringUtils::IsTableEntry(aArray[i]->At(j).iVal))
				{
				CStringPoolNode* node= reinterpret_cast<CStringPoolNode*>(aArray[i]->At(j).iVal & KTokenToNode);
				if (KMarkedForNoDeleted==node->iRefcount)
					{
					delete node;										
					aArray[i]->Delete(j);
					j--;
					}
				}
			}
		}	
	}

CStringPoolImplementation* CStringPoolImplementation::NewL()
	{
	CStringPoolImplementation* table = new (ELeave) CStringPoolImplementation();	
	return table;
	}

void CStringPoolImplementation::CleanupHashCS(TAny* aImplementation)
	{
	CStringPoolImplementation* imp=reinterpret_cast<CStringPoolImplementation*>(aImplementation);
	CleanUpHash(&imp->iRollbackHashListCS, imp->iCSHashTable);
	}

void CStringPoolImplementation::CleanupHashCI(TAny* aImplementation)
	{
	CStringPoolImplementation* imp=reinterpret_cast<CStringPoolImplementation*>(aImplementation);
	CleanUpHash(&imp->iRollbackHashListCI, imp->iCIHashTable);
	}

void CStringPoolImplementation::CleanUpHash(RPointerArray <RStringTokenEither>* aHashCleanup, CArrayFixSeg<RStringTokenEither>* aHash[KHashModulo])
	{
	if (aHashCleanup->Count()>0)
		{
		RStringTokenEither* token=(*aHashCleanup)[0];	// Get first entry
		for (TInt i = 0; i < KHashModulo; i++ )
			{
			if (aHash[i])
				{
				for (TInt j = 0; j < aHash[i]->Count(); j++)
					{
					if (!StringUtils::IsTableEntry(aHash[i]->At(j).iVal))
						{						
						if (aHash[i]->At(j).iVal==token->iVal)
							{		
							CStringPoolNode* node= reinterpret_cast<CStringPoolNode*>(aHash[i]->At(j).iVal & KTokenToNode);
							delete node;
							aHash[i]->Delete(j);
							aHashCleanup->Remove(0);
							break;
							}
						}
					}
				}
			
			}
		}
	}

void CStringPoolImplementation::CleanupIdMap(TAny* aImplementation)
	{
	CStringPoolImplementation* imp=reinterpret_cast<CStringPoolImplementation*>(aImplementation);
	if (imp->iRollbackMapList.Count()>0)
		{
		TStringIdMap* map=imp->iRollbackMapList[0];			
		TInt index=imp->iStringMapList.FindInUnsignedKeyOrder(*map);
		imp->iRollbackMapList.Remove(0);
		if (index!=KErrNotFound)
			{
			imp->iStringMapList.Remove(index);			
			}		
		index=imp->iStringMapListReverse.FindInUnsignedKeyOrder(*map);
		if (index!=KErrNotFound)
			{
			for (TInt count=index;count<imp->iStringMapListReverse.Count();++count)
				{
				if (imp->iStringMapListReverse[count].iTargetTableVal==map->iTargetTableVal && imp->iStringMapListReverse[count].iSourceTableVal==map->iSourceTableVal)
					{				
					imp->iStringMapListReverse.Remove(index);
					}
				}
			}
		}
	}

void CStringPoolImplementation::AddTableL(const TStringTable& aTable)
	{
	for (TInt count=0;count<iTablePtrs.Count();++count)	// check for adding the same table twice
		{
		if (iTablePtrs[count]==&aTable)
			{
			return;
			}
		}
	User::LeaveIfError(iTablePtrs.Append(&aTable));	// Add the pointer to this table so we can keep track of the Table IDs. The table ID is the index in this array
	//Is the table Case Sensitive or not?
	TBool (*genericValFromIndex)(TInt, TUint16);
	if (aTable.iCaseSensitive==1)
		genericValFromIndex = StringUtils::ValFromIndex;
	else
		genericValFromIndex = StringUtils::ValFromIndexF;

	CArrayFixSeg<RStringTokenEither>** hashTableToUse =
	aTable.iCaseSensitive ? iCSHashTable :iCIHashTable ;	
	TInt cleanupCounter=0;
	for (TUint i = 0; i < aTable.iCount; ++i)
		{
		const TStLitC8<1>* string=reinterpret_cast<const TStLitC8<1>* >(aTable.iTable[i]);		
		
		// Try to find the string in memory, maybe as a dynamic string or as a member of an another table  
		RStringTokenEither token=FindDes(*string, !aTable.iCaseSensitive);
		if (!token.IsNull())
			{
			TStringIdMap map;			
			map.iSourceTableVal=StringUtils::ValFromIndex(i, (TInt16)(iTablePtrs.Count()-1),aTable.iCaseSensitive);
			map.iTargetTableVal=token.iVal;

			// Put on cleanup stack
			User::LeaveIfError(iRollbackMapList.Append(&map));
			TCleanupItem cleanup(CleanupIdMap, this);
			CleanupStack::PushL(cleanup);
			++cleanupCounter;
			
			User::LeaveIfError(iStringMapList.InsertInUnsignedKeyOrder(map));

			// Check if this is a link to a dynamic string
			if (!StringUtils::IsTableEntry(token.iVal))
				{
				CStringPoolNode* node = StringUtils::NodePtr(token.iVal);
				node->iRefcount=KMarkedForNoDeleted; // Make sure this string never gets deleted
				}

			// Now store the reverse array						
			User::LeaveIfError(iStringMapListReverse.InsertInUnsignedKeyOrderAllowRepeats(map));
			}
		else
			{
			TUint8 hash = static_cast<TUint8>(Hash(*string));
			CArrayFixSeg<RStringTokenEither>* collisionList = hashTableToUse[hash];
			if ( !collisionList )
				//HashTableToUse now is used as list of all entry with the same hash
				collisionList = hashTableToUse[hash] = 
				new (ELeave) CArrayFixSeg<RStringTokenEither>( 2 );
			RStringTokenEither s;
			s.iVal = genericValFromIndex(i, (TInt16)(iTablePtrs.Count()-1));
			
			__LOG2(_L8("Table entry being added with hash %d, val %d"), hash, s.iVal);
			__LOG(*reinterpret_cast<const TStLitC8<1>* >(aTable.iTable[i]));
			// Put on cleanup stack
			if (aTable.iCaseSensitive==1)
				{
				User::LeaveIfError(iRollbackHashListCS.Append(&s));
				TCleanupItem cleanup(CleanupHashCS, this);
				CleanupStack::PushL(cleanup);
				}
			else
				{
				User::LeaveIfError(iRollbackHashListCI.Append(&s));
				TCleanupItem cleanup(CleanupHashCI, this);
				CleanupStack::PushL(cleanup);
				}
			
			++cleanupCounter;
			collisionList->AppendL(s);
			}
		}	
	CleanupStack::Pop(cleanupCounter);
	iRollbackMapList.Reset();
	iRollbackHashListCS.Reset();
	iRollbackHashListCI.Reset();
	}

// Find FirstVal given duplicate val
TInt32 CStringPoolImplementation::FindFirstValFromDuplicate(TInt32 aDuplicateVal) const 
	{
	TStringIdMap map;
	map.iSourceTableVal=aDuplicateVal;	
	TInt index=iStringMapList.FindInUnsignedKeyOrder(map);
	if (index!=KErrNotFound)		
		return iStringMapList[index].iTargetTableVal;
	else
		return KErrNotFound;
	}



// Find table index Val given first val & table UID
TInt CStringPoolImplementation::FindTableIndexFromFirstVal(TInt32 aFirstVal, TInt aTableUid) const
	{
	TStringIdMap map;
	map.iTargetTableVal=aFirstVal;		
	TInt index=iStringMapListReverse.FindInUnsignedKeyOrder(map);
	if (KErrNotFound==index)
		return KErrNotFound;

	for (TInt count=index;count<iStringMapListReverse.Count();++count)
		{
		if (iStringMapListReverse[count].iTargetTableVal==aFirstVal && StringUtils::TableUid(iStringMapListReverse[count].iSourceTableVal)==aTableUid)
			{
			return StringUtils::TableIndex(iStringMapListReverse[count].iSourceTableVal);
			}
		}	
		return KErrNotFound;
	}


// Find the UId for a given table
TInt16 CStringPoolImplementation::TableUid(const TStringTable& aTable) const
	{
	for (TInt count=0; count<iTablePtrs.Count(); ++count)
		{
		if (iTablePtrs[count]==&aTable)
			return (TInt16)count;
		}
	return KErrNotFound;
	}

// Find a reference to the table that first added the  string represented by aVal to the pool 
const TStringTable& CStringPoolImplementation::TableRef(TInt32 aVal) const
	{
	__ASSERT_DEBUG(aVal!=0, StringPoolPanic::Panic(StringPoolPanic::EIllegalUseOfNullString));
	TInt16 tableUid=(TInt16)(aVal>>20);
	const TStringTable* theTableRef=(iTablePtrs[tableUid]);
	return  *theTableRef;
	}

// Find the descriptor for a given table and index
const TDesC8& CStringPoolImplementation::TableLookup(TInt aIndex, TInt aTableUid) const
	{	
	return *reinterpret_cast<const TStLitC8<1>*>(iTablePtrs[aTableUid]->iTable[aIndex]);
	}


// Lookup with allocating
//
RStringTokenEither 
CStringPoolImplementation::OpenL( const TDesC8& aAttributeName,
								  TBool aCaseInsensitive)
	{
	// lookup the attribute
	RStringTokenEither s(FindDes( aAttributeName , aCaseInsensitive));
	if (!s.IsNull())
		{
		if (!StringUtils::IsTableEntry(s.iVal))
			{

			CStringPoolNode* node = StringUtils::NodePtr(s.iVal);
			if (KMarkedForNoDeleted!=node->iRefcount)
				node->iRefcount++;
			__LOG1(_L8("String copied (during open). Count is now %d"), node->iRefcount);
			__LOG(*node->iDes);
			}
		return s;
		}
	
	// create a new node at the end of the appropriate array
	CStringPoolNode* newnode = new (ELeave) CStringPoolNode();
	CleanupStack::PushL( newnode );
	newnode->iDes = aAttributeName.AllocL();
	newnode->iRefcount = 1;

	TInt hash = Hash( aAttributeName );
	CArrayFixSeg<RStringTokenEither>** hashTableToUse = 
		aCaseInsensitive ? iCIHashTable : iCSHashTable;
	__LOG2(_L8("Newly added with hash value %d, node val 0x%x\n"), hash, newnode)
	__LOG(aAttributeName);

	newnode->iHash = static_cast<TUint8>(hash);
	CArrayFixSeg<RStringTokenEither>* collisionList = hashTableToUse[hash];
	if ( !collisionList )
		collisionList = hashTableToUse[hash] = new (ELeave) CArrayFixSeg<RStringTokenEither>( 2 );

	s.iVal = reinterpret_cast<TUint32>(newnode);
	if (aCaseInsensitive)
		s.iVal += 2;
	collisionList->AppendL(s);

	CleanupStack::Pop(); // newnode

	return s;
	}

void CStringPoolImplementation::Close(RStringTokenEither aString)
	{
	if (StringUtils::IsTableEntry(aString.iVal))
		return;

	CStringPoolNode* node = StringUtils::NodePtr(aString.iVal);
	if (KMarkedForNoDeleted == node->iRefcount)	// -1 means a non-expirable string
		return;
	if (--node->iRefcount == 0)
		{
		//this is  the last reference of this string
		CArrayFixSeg<RStringTokenEither>** hashTableToUse = 
		aString.iVal & 2 ? iCIHashTable : iCSHashTable;

		// Delete the node and delete the entry in the relevant collision list 
		CArrayFixSeg<RStringTokenEither>* collisionList = hashTableToUse[node->iHash];
		TInt count = collisionList->Count();		
		for (TInt i = 0; i < count; i++)
			{
			if (collisionList->At(i) == aString)
				{
				// Log the fact that a string reference is about to die...
				__LOG1(_L8("Removing string with hash value %d\n"), node->iHash)
				__LOG(node->iDes->Des());
				collisionList->Delete(i);
				break;
				}
			}
		delete node;
		}
	else
		{
		__LOG1(_L8("String closed. Count is now %d"), 
			   node->iRefcount);
		__LOG(node->iDes->Des());
		}
	}

void CStringPoolImplementation::IncrementCount(RStringTokenEither aString)
	{
	if (StringUtils::IsTableEntry(aString.iVal))
		return;
	CStringPoolNode* node = StringUtils::NodePtr(aString.iVal);
	if (KMarkedForNoDeleted!=node->iRefcount)
		node->iRefcount++;
	__LOG1(_L8("String copied. Count is now %d"), node->iRefcount);
	__LOG(*node->iDes);
	}

// Very simple case-sensitive comparison. We can assume that the
// strings are the same length, and we only care if the strings are
// the same. (Unlike normal comparison functions that also tell you
// which one is 'smaller')
TBool CStringPoolImplementation::CompareCS(const TDesC8& s1, const TDesC8& s2)
	{
	const TUint8* ptr1 = s1.Ptr();
	const TUint8* ptr2 = s2.Ptr();
	const TUint8* stop = &ptr1[s1.Length()];
	for (; ptr1 < stop; ptr1++,ptr2++)
		{
		if (*ptr1 != *ptr2)
			return EFalse;
		}
	return ETrue;
	}

// Note that the hash function must generate the same hash values for
// strings that differ by case. If changing the algorithm here make
// sure this is still true.
TBool CStringPoolImplementation::CompareCI(const TDesC8& s1, const TDesC8& s2)
	{
	const TUint8* ptr1 = s1.Ptr();
	const TUint8* ptr2 = s2.Ptr();
	const TUint8* stop = &ptr1[s1.Length()];
	for (; ptr1 < stop; ptr1++,ptr2++)
		{
		if (*ptr1 != *ptr2)
			{
			// They're not exactly the same; see if they differ only
			// by case. If one character is a letter, we can do a
			// comparison ignoring bit 5 in both cases. If that
			// matches, they are the same.
			if (!((*ptr1 & KCaseInsensitive) == (*ptr2 & KCaseInsensitive) &&
				  (*ptr1 >= 'A' && *ptr1 <= 'Z' || 
				   *ptr1 >= 'a' && *ptr1 <= 'z')))
				return EFalse;
			}
		}
	return ETrue;
	}

// Find the given descriptor in the hash table
//
RStringTokenEither 
CStringPoolImplementation::FindDes( const TDesC8& aAttributeName, TBool aCaseInsensitive) 
	{
	CArrayFixSeg<RStringTokenEither>** hashTableToUse = 
		aCaseInsensitive ? iCIHashTable : iCSHashTable;
	CArrayFixSeg<RStringTokenEither>* collisionList =hashTableToUse[Hash(aAttributeName)];
	RStringPool pool;
	TBool (*compareFunction)(const TDesC8&, const TDesC8&);
	if (aCaseInsensitive)
		compareFunction = CompareCI;
	else
		compareFunction = CompareCS;
	pool.iImplementation = this;
	if ( collisionList )
		{
		TInt length=aAttributeName.Length();
		TInt count = collisionList->Count();
		for ( TInt i = 0; i < count; i++ )
			{
			RStringTokenEither token = collisionList->At(i);
			RStringEither s(this, token);
			const TDesC8& string = s.DesC();
			if ( string.Length()==length && 
				 (*compareFunction)(aAttributeName, string))
				return token;
			}
		}
	return RStringTokenEither();
	}


// Generate a hash value
//
TUint CStringPoolImplementation::Hash( const TDesC8& aDes ) const
	{
	// We ignore bit 5, which is a crude way of making the hash case
	// insensitive. This means that things that might differ only by
	// case end up in the same bucket, and we can then worry about
	// whether they're really the same later.
	TInt len=aDes.Length();
	TUint hash = 0;
	const TUint8* ptr=aDes.Ptr();
	for ( TInt i = 0; i < len; i++ )
		hash = 131*hash + (*ptr++ & KCaseInsensitive);
	return hash % KHashModulo;
	}

TInt StringUtils::ValFromIndex(TInt aIndex, TUint16 aTableId)
	{
	return (aTableId << 20) + (aIndex << 2) + 1;
	}

TInt StringUtils::ValFromIndexF(TInt aIndex, TUint16 aTableId)
	{
	return (aTableId << 20) + (aIndex << 2) + 3;
	}

TInt StringUtils::ValFromIndex(TInt aIndex, TUint16 aTableId, TBool aCaseSensitive)
	{
	if (aCaseSensitive)
		return ValFromIndex(aIndex, aTableId);
	else
		return ValFromIndexF(aIndex, aTableId);
	}
void  CStringPoolImplementation::AddCallBackL( MStringPoolCloseCallBack& aCallBack)
	{
	User::LeaveIfError(iCallBacks.Append(&aCallBack));
	}
