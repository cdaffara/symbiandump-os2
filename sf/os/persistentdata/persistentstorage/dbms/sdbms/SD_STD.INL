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


inline TInt DbsMessage(TInt aFunction,TInt aHandle)
	{
	return (aFunction<<KDbsHandleBits)|aHandle;
	}

inline TDbsFunction DbsFunction(TInt aMessage)
	{
	return TDbsFunction(aMessage>>KDbsHandleBits);
	}

inline TInt DbsHandle(TInt aMessage)
	{
	return aMessage&KDbsHandleMask;
	}

inline TDbsType DbsType(TInt aHandle)
	{
	return TDbsType(aHandle&KDbsTypeMask);
	}

inline TInt DbsMagic(TInt aHandle)
	{
	return (aHandle>>KDbsTypeBits)&KDbsMagicMask;
	}

inline TInt DbsObjectIndex(TInt aHandle)
	{
	return aHandle>>(KDbsTypeBits+KDbsMagicBits);
	}

inline TInt DbsMakeHandle(TInt aIndex,TInt aMagic,TDbsType aType)
	{
	return (((aIndex<<KDbsMagicBits)|aMagic)<<KDbsTypeBits)|aType;
	}

inline TInt DbsSessionHandle()
	{
	return DbsMakeHandle(1<<(KDbsIndexBits-1),0,EDbsSession);
	}

// streaming functions
inline RWriteStream& operator<<(RWriteStream& aStream,const TDbCol& aCol)
	{
	ExternalizeL(aCol,aStream);
	return aStream;
	}

inline RWriteStream& operator<<(RWriteStream& aStream,const CDbColSet& aColSet)
	{
	ExternalizeL(aColSet,aStream);
	return aStream;
	}

inline RReadStream& operator>>(RReadStream& aStream,CDbColSet& aColSet)
	{
	InternalizeL(aColSet,aStream);
	return aStream;
	}

inline RWriteStream& operator<<(RWriteStream& aStream,const CDbKey& aKey)
	{
	ExternalizeL(aKey,aStream);
	return aStream;
	}

inline RReadStream& operator>>(RReadStream& aStream,CDbKey& aKey)
	{
	InternalizeL(aKey,aStream);
	return aStream;
	}

inline RWriteStream& operator<<(RWriteStream& aStream,const CDbNames& aNames)
	{
	ExternalizeL(aNames,aStream);
	return aStream;
	}

inline RReadStream& operator>>(RReadStream& aStream,CDbNames& aNames)
	{
	InternalizeL(aNames,aStream);
	return aStream;
	}

/**
This operator is used for externalizing CDbStrings array to a stream.
@internalComponent
*/	
inline RWriteStream& operator<<(RWriteStream& aStream,const CDbStrings& aStrings)
	{
	ExternalizeL(aStrings,aStream);
	return aStream;
	}

/**
This operator is used for internalizing CDbStrings array from a stream.
@internalComponent
*/	
inline RReadStream& operator>>(RReadStream& aStream,CDbStrings& aStrings)
	{
	InternalizeL(aStrings,aStream);
	return aStream;
	}

template <class T>
inline TExternalizeFunction Externalizer(const T*)
	{
	return TExternalizer<T>::Function();
	}

// Class TDbsParam
inline void TDbsParam::operator=(const TDesC8& aDes)
	{
	iPtr=&aDes;
	}

inline void TDbsParam::operator=(const TDesC16& aDes)
	{
	iPtr=&aDes;
	}

inline void TDbsParam::operator=(TInt aVal)
	{
	iInt=aVal;
	}

// Class RDbsObject
inline RDbsObject::RDbsObject():
	iHandle(0)
	{
	}

inline RDbsObject::RDbsObject(const RDbs& aDbs):
	RDbs(aDbs),
	iHandle(KDbsSessionHandle)
	{
	}

inline TInt RDbsObject::Handle() const
	{
	return iHandle;
	}

inline void RDbsObject::OpenL(const RDbsObject& aDbs,TDbsFunction aFunction)
	{
	OpenL(aDbs,aFunction,0);
	}

inline void RDbsObject::OpenL(const RDbsObject& aDbs,TDbsFunction aFunction,const TIpcArgs& aArgs)
	{
	OpenL(aDbs,aFunction,&aArgs);
	}

inline void RDbsObject::OpenL(const RDbs& aDbs,TDbsFunction aFunction,const TIpcArgs& aArgs)
	{
	OpenL(RDbsObject(aDbs),aFunction,&aArgs);
	}

inline void RDbsObject::SendReceive(TDbsFunction aFunction,TRequestStatus& aStatus) const
	{
	SendReceive(aFunction,0,aStatus);
	}

inline TInt RDbsObject::SendReceive(TDbsFunction aFunction,const TIpcArgs& aArgs) const
	{
	return SendReceive(aFunction,&aArgs);
	}

inline TInt RDbsObject::SendReceiveL(TDbsFunction aFunction,const TIpcArgs& aArgs) const
	{
	return SendReceiveL(aFunction,&aArgs);
	}

inline void RDbsObject::SendReceive(TDbsFunction aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus) const
	{
	SendReceive(aFunction,&aArgs,aStatus);
	}

// Class CDbsDatabase
inline CDbsDatabase::CDbsDatabase()
	{
	}

// Class CDbsNotifier
inline CDbsNotifier::CDbsNotifier()
	{
	}

// Class CDbsIncrmental
inline CDbsIncremental::CDbsIncremental()
	{
	}

// Class CDbsConstraint
inline CDbsConstraint::CDbsConstraint()
	{
	}

// Class HDbsBuf
inline HDbsBuf::HDbsBuf()
	{
	iBuf.iExt=-1;
	}

inline HDbsBuf::~HDbsBuf()
	{
	iIpc.Close();
	}

inline void HDbsBuf::SetPos(TRead,TInt aPos)
	{
	iRPos=aPos;
	}

inline void HDbsBuf::SetPos(TWrite,TInt aPos)
	{
	iWPos=aPos;
	}

inline TInt HDbsBuf::Pos(TRead) const
	{
	return iRPos;
	}

inline TInt HDbsBuf::Pos(TWrite) const
	{
	return iWPos;
	}

inline TInt HDbsBuf::MovePos(TRead,TInt anOffset)
	{
	return iRPos+=anOffset;
	}

inline TInt HDbsBuf::MovePos(TWrite,TInt anOffset)
	{
	return iWPos+=anOffset;
	}

inline TInt HDbsBuf::Lag(TRead) const
	{
	return Ptr(ERead)-End(ERead);
	}

inline TInt HDbsBuf::Lag(TWrite) const
	{
	return Ptr(EWrite)-iBuf.iData;
	}

inline TInt HDbsBuf::Mark(TRead) const
	{
	return Pos(ERead)+Lag(ERead);
	}

inline TInt HDbsBuf::Mark(TWrite) const
	{
	return Pos(EWrite)+Lag(EWrite);
	}

// Class CDbsSource
inline TInt CDbsSource::LinkOffset()
	{
	return _FOFF(CDbsSource,iLink);
	}

inline CDbsSource::CDbsSource(const TDbFormat& aFormat):
	iFormat(aFormat)
	{
	}

inline void CDbsSource::Open()
	{
	++iConnections;
	}

inline CDbSource& CDbsSource::Source()
	{
	__ASSERT(iSource);
	return *iSource;
	}

// Class CDbsDatabaseStub
inline CDbsDatabaseStub::CDbsDatabaseStub()
	{
	}

// Class CDbsConnection
inline void CDbsConnection::Set(CDbsSource& aSource)
	{
	__ASSERT(!iSource);
	iSource=&aSource;
	aSource.Open();
	}

inline CDbsSource& CDbsConnection::Source() const
	{
	__ASSERT(iSource);
	return *iSource;
	}

inline const CDbsConnection& CDbsConnection::Connection(const CDbObject& aObject)
	{
	__ASSERT(aObject.Context());
	return *STATIC_CAST(const CDbsConnection*,aObject.Context());
	}

inline CDbsSource& CDbsConnection::Source(const CDbObject& aObject)
	{
	return Connection(aObject).Source();
	}

// Class RDbsSources
inline RDbsSources::RDbsSources(RDbCache& aCache):
  	iSources(CDbsSource::LinkOffset()),
  	iDrivers(aCache)
  	{
  	}
  
inline void RDbsSources::Close()
  	{
  	__ASSERT(iSources.IsEmpty());
  	iDrivers.Close();
  	}
  
// Class CDbsServer
inline TDes& CDbsServer::Name0()
	{
	return iName0;
	}

inline TDes& CDbsServer::Name1()
	{
	return iName1;
	}

inline TDes& CDbsServer::FileName()
	{
	return iFileName;
	}

inline RDbsSources& CDbsServer::Sources()
	{
	return iSources;
	}

inline RFs& CDbsServer::Fs()
	{
	return iFs;
	}

inline RDriveSpaceCol& CDbsServer::DriveSpaceCol()
	{
	return iDriveSpaceCol;
	}

inline void CDbsServer::Panic(const TDesC& aCategory,TInt aPanic)
	{
	Message().Panic(aCategory,aPanic);
	}

inline CPolicyProxy& CDbsServer::PolicyProxy() const
	{
	__ASSERT(iPolicyProxy);
	return *iPolicyProxy;
	}

inline RDbPropsFactory& CDbsServer::DbPropsFactory()
	{
	return iDbPropsFactory;
	}

// Class CDbsSession::TEntry
inline TDbsType CDbsSession::TEntry::Type() const
	{
	return TDbsType(iType);
	}

inline CDbsDatabaseStub& CDbsSession::TEntry::DatabaseStub()
	{
	__ASSERT(Type()==EDbsDatabase);
	return *STATIC_CAST(CDbsDatabaseStub*,iObject);
	}

inline CDbDatabase& CDbsSession::TEntry::Database()
	{
	__ASSERT(Type()==EDbsDatabase);
	return *STATIC_CAST(CDbDatabase*,iObject);
	}

inline CDbIncremental& CDbsSession::TEntry::Incremental()
	{
	__ASSERT(Type()==EDbsIncremental);
	return *STATIC_CAST(CDbIncremental*,iObject);
	}

inline CDbCursor& CDbsSession::TEntry::Cursor()
	{
	__ASSERT(Type()==EDbsCursor);
	return *STATIC_CAST(CDbCursor*,iObject);
	}

inline CDbRowConstraint& CDbsSession::TEntry::Constraint()
	{
	__ASSERT(Type()==EDbsConstraint);
	return *STATIC_CAST(CDbRowConstraint*,iObject);
	}

inline HDbsStream& CDbsSession::TEntry::Stream()
	{
	__ASSERT(Type()==EDbsStream);
	return *STATIC_CAST(HDbsStream*,iObject);
	}

inline CDbsObserver::HObserver& CDbsSession::TEntry::Observer()
	{
	__ASSERT(Type()==EDbsObserver);
	return *STATIC_CAST(CDbsObserver::HObserver*,iObject);
	}

// Class CDbsSession
inline TInt CDbsSession::Add(CDbIncremental* aIncremental, const MPolicy* aPolicy)
	{
	return DoAdd(aIncremental,EDbsIncremental, aPolicy);
	}

inline TInt CDbsSession::Add(CDbCursor* aCursor, const MPolicy* aPolicy)
	{
	return DoAdd(aCursor,EDbsCursor, aPolicy);
	}

inline TInt CDbsSession::Add(CDbRowConstraint* aConstraint, const MPolicy* aPolicy)
	{
	return DoAdd(aConstraint,EDbsConstraint, aPolicy);
	}

inline TInt CDbsSession::Add(HDbsStream* aStream, const MPolicy* aPolicy)
	{
	return DoAdd(aStream,EDbsStream, aPolicy);
	}

inline TInt CDbsSession::Add(CDbsObserver::HObserver* aObserver, const MPolicy* aPolicy)
	{
	return DoAdd(aObserver,EDbsObserver, aPolicy);
	}

inline CDbsServer& CDbsSession::Server() const
	{
	return *static_cast<CDbsServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

inline RDbsSources& CDbsSession::Sources()
	{
	return Server().Sources();
	}

// Class HDbsStream
inline HDbsStream::HDbsStream(MStreamBuf* aHost,TInt aReadPos):
	iHost(*aHost),
	iRPos(aReadPos),
	iWPos(0)
	{
	__ASSERT(aHost);
	}

inline HDbsStream::~HDbsStream()
	{
	iHost.Release();
	}

inline TInt HDbsStream::SizeL()
	{
	return iHost.SizeL();
	}

inline void HDbsStream::SynchL()
	{
	iHost.SynchL();
	}

// Class HBufBuf
inline HBufBuf::HBufBuf():
	iBuf(0)
	{
	}

inline HBufBuf::~HBufBuf()
	{
	delete iBuf;
	}

// Class CDbsSecureDatabase
inline CDbsSecureDatabase::CDbsSecureDatabase()
	{
	}

