// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

inline RSettingsArray::RSettingsArray() :
	RArray<TServerSetting>(KGranularity, _FOFF(TServerSetting, iKey))
	{iIsDefault=EFalse;}

inline RSettingsArray::~RSettingsArray()
	{ Reset(); }

inline TServerSetting* RSettingsArray::Find(TInt aKey) const
	{
	TInt i = FindInUnsignedKeyOrder(TServerSetting(aKey));
	return i>=0 ? const_cast<TServerSetting*>(&operator[](i)) : (TServerSetting*)0;
	}

inline TInt RSettingsArray::FindIndex(const TServerSetting& aSetting) const
	{
	return FindInUnsignedKeyOrder(aSetting);
	}

inline void RSettingsArray::OrderedInsertL(const TServerSetting& aEntry)
	{ InsertInUnsignedKeyOrderL(aEntry); }

inline TInt RSettingsArray::OrderedInsert(const TServerSetting& aEntry)
	{ return InsertInUnsignedKeyOrder(aEntry); }

inline TInt RSettingsArray::Count() const
	{ return RArray<TServerSetting>::Count(); }

inline const TServerSetting& RSettingsArray::operator[](TInt aIndex) const
	{
	return RArray<TServerSetting>::operator[](aIndex);
	}

inline TServerSetting& RSettingsArray::operator[](TInt aIndex)
	{
	return RArray<TServerSetting>::operator[](aIndex);
	}

inline void RSettingsArray::Remove(TInt aId)
	{
	TInt i = FindInUnsignedKeyOrder(TServerSetting(aId));
	if(i>=0)
		DeleteElement(i);
	}

inline void RSettingsArray::Reset()
	{
	for(TInt i=Count()-1; i>=0; i--)
		{
		operator[](i).Reset();
		}
	RArray<TServerSetting>::Reset();
	}

inline void RSettingsArray::Close()
	{
	Reset();
	}

inline void RSettingsArray::DeleteElement(TInt aIndex)
	{
	operator[](aIndex).Reset();
	RArray<TServerSetting>::Remove(aIndex);
	}

inline void RSettingsArray::RemoveElement(TInt aIndex)
	{
	RArray<TServerSetting>::Remove(aIndex);
	}

inline void RSettingsArray::AdoptL(RSettingsArray& aSrc)
	{
	Reset();

	for(TInt i=aSrc.Count()-1; i>=0; i--)
		{
		AppendL(aSrc[0]);
		aSrc.RemoveElement(0);
		}
	}

inline void RSettingsArray::ExternalizeL(RWriteStream& aStream) const
	{
	
	TInt32 numElements = Count() ;	
	
	aStream << numElements ;
	for (TInt32 count = 0; count < numElements; count++)
		{
			aStream << (*this)[count] ;
		} 
	}


inline void RSettingsArray::WriteBackupStream(RWriteStream& aStream) const
	{
	
	// Note: Unlike the usual implementation of Externalize()
	// only selected data (settings with the "backup" bit set
	// in metadata) are streamed out!!!
	TInt32 numElementsToExternalize = 0;
	TInt32 numElements = Count() ;	
	TInt32 count ;
		
	for (count = 0; count < numElements; count++)
		{
		if ((*this)[count].Meta() & KMetaBackup) 
			numElementsToExternalize++ ;
		} 
	
	aStream << numElementsToExternalize ;
	for (count = 0; count < numElements; count++)
		{
		if ((*this)[count].Meta() & KMetaBackup) 
			aStream << (*this)[count] ;
		} 
	}

inline void RSettingsArray::InternalizeL(RReadStream& aStream)
	{
 	TInt32 numElements;
	
	aStream >> numElements ;	
	for (TInt32 count = 0; count < numElements; count++)
		{
		TServerSetting serverSetting ;
		aStream >> serverSetting ;
		serverSetting.PushL();
		if(IsDefault())
			{
			serverSetting.SetClean();			
			}
		AppendL(serverSetting) ;
		serverSetting.Pop();
		} 
	}

inline TBool RSettingsArray::IsDefault() const
	{
	return iIsDefault;
	}

inline void RSettingsArray::SetIsDefault(TBool aIsDefault)
	{
	iIsDefault=aIsDefault;
	}

inline RRangePolicyArray::RRangePolicyArray() :
	RArray<TSettingsAccessPolicy>()
	{}

inline RRangePolicyArray::~RRangePolicyArray()
	{ Reset(); }

inline TSettingsAccessPolicy* RRangePolicyArray::Find(TInt aKey) const
	{
	for(TInt ii = Count()-1; ii>=0;ii--)
		{
		if(operator[](ii).IsInRange(aKey))
			return const_cast<TSettingsAccessPolicy*>(&operator[](ii));
		}
	return NULL;
	}

inline void RRangePolicyArray::ExternalizeL(RWriteStream& aStream) const
	{
	TInt32 numElements = Count() ;
	
	aStream << numElements ;
	
	for (TInt32 count = 0; count < numElements; count++)
		{
		aStream << (*this)[count] ;
		} 
	}

inline void RRangePolicyArray::InternalizeL(RReadStream& aStream)
	{
	TInt32 numElements = Count() ;	
	aStream >> numElements ;
	for (TInt32 count = 0; count < numElements; count++)
		{
		TSettingsAccessPolicy newElement ;
		aStream >> newElement ;
		AppendL(newElement) ;
		} 
	}

inline RDefaultMetaArray::RDefaultMetaArray() :
	RArray<TSettingsDefaultMeta>()
	{}

inline RDefaultMetaArray::~RDefaultMetaArray()
	{ Reset(); }

inline TSettingsDefaultMeta* RDefaultMetaArray::Find(TInt aKey) const
	{
	for(TInt ii = Count()-1; ii>=0;ii--)
		{
		if(operator[](ii).IsInRange(aKey))
			return const_cast<TSettingsDefaultMeta*>(&operator[](ii));
		}
	return NULL;
	}

inline void RDefaultMetaArray::ExternalizeL(RWriteStream& aStream) const
	{
	TInt32 numElements = Count() ;
	
	aStream << numElements ;
	
	for (TInt32 count = 0; count < numElements; count++)
		{
		aStream << (*this)[count] ;
		} 
	}

inline void RDefaultMetaArray::InternalizeL(RReadStream& aStream)
	{
	TInt32 numElements = Count() ;	
	aStream >> numElements ;
	for (TInt32 count = 0; count < numElements; count++)
		{
		TSettingsDefaultMeta newElement ;
		aStream >> newElement ;
		AppendL(newElement) ;
		} 
	}

inline RSingleMetaArray::RSingleMetaArray() : RArray<TSettingSingleMeta>()
	{
	}

inline TInt RSingleMetaArray::Find(TUint32 aKey, TUint32& aMeta) const
	{
	TInt num = Count();
	for(TInt i = 0; i < num; i++)
		{
		TSettingSingleMeta singleMeta = static_cast<TSettingSingleMeta> (operator[](i));
		if(singleMeta.GetKey() == aKey)
			{
			aMeta = singleMeta.GetMeta();
			return KErrNone;
			}
		}
	return KErrNotFound;
	}
