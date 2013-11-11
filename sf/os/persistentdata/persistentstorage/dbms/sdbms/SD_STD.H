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
// DBMS Client/Server architecture
// 
//


#include "U32STD.H"
#include "D32SRV.H"
#include "D32DRVR.H"
#include <f32file.h>
#include <s32mem.h>
#include "D32VER.H"
#include "D32Map.h"
#include "Sd_DbProps.h"
#include "Sd_PolicyProxy.h"
#include "Sd_DriveSpace.h"

// other classes referenced
class HDbsColumns;
struct TDbProps;

// classes defined
class TDbsParam;
class RDbsObject;
class CDbsDatabase;
class CDbsIncremental;
class CDbsCursor;
class CDbsConstraint;
class CDbsNotifier;
class CDbsSource;
class RDbsSources;
class CDbsServer;
class CDbsSession;
class HDbsStream;

#if defined(_DEBUG)
/**
@internalComponent
*/
#define _SMALL_BUFFER
#endif

using namespace DBSC;

/**
@internalComponent
*/
enum TDbsPanic
	{
	EDbsUnimplemented,              //0
	EDbsInvalidColumn,
	EDbsWrongType,
	EDbsNoRowData,
	EDbsNotInUpdate,
	EDbsBadDescriptor,
	EDbsBadHandle,
	EDbsStreamMarkInvalid,
	EDbsStreamLocationInvalid,
	EDbsObserverRequestPending,
    EDbsInvalidDrive                //10
	};

/**
used by proxy classes
@internalComponent
*/
GLREF_C void Panic(TDbsPanic aPanic);	

/**
@internalComponent
*/
GLREF_C void ExternalizeL(const TDbCol& aCol,RWriteStream& aStream);
GLREF_C void ExternalizeL(const CDbColSet& aColSet,RWriteStream& aStream);
GLREF_C void InternalizeL(CDbColSet& aColSet,RReadStream& aStream);
GLREF_C void ExternalizeL(const CDbKey& aKey,RWriteStream& aStream);
GLREF_C void InternalizeL(CDbKey& aKey,RReadStream& aStream);
GLREF_C void ExternalizeL(const CDbNames& aNames,RWriteStream& aStream);
GLREF_C void InternalizeL(CDbNames& aNames,RReadStream& aStream);
GLREF_C void ExternalizeL(const CDbStrings& aNames,RWriteStream& aStream);
GLREF_C void InternalizeL(CDbStrings& aNames,RReadStream& aStream);

/**
@internalComponent
*/
_LIT(KDbsServerName,"!DBMS server");
_LIT(KDbsServerImg,"EDBSRV");		// DLL/EXE name
const TUid KDbsServerUid3={0x100012a5};


/**
@internalComponent
*/
const TInt KDbsExitDelay=4*0x100000;	// ~4 seconds
const TInt KDbsLeavePanic=~(1<<20);
const TInt KTablePolicyMaskBit = 1024;	//Used in RDbs::Get<Obj>Policy() methods

/**
shared classes
@internalComponent
*/
enum TDbsType {EDbsFree,EDbsDatabase,EDbsIncremental,EDbsCursor,EDbsConstraint,EDbsStream,EDbsObserver,EDbsMaxType=EDbsObserver,EDbsSession=EDbsFree};

/**
Make sure that KDbsFunc2SecurityPolicyMask array in SD_PolicyProxy.cpp gets updated if 
new enum items are going to be added to TDbsFunction.
@internalComponent
*/
enum TDbsFunction
	{
// Session functions
	EDbsResourceMark,					//0
	EDbsResourceCheck,
	EDbsResourceCount,
	EDbsSetHeapFailure,
	EDbsOpenDatabase,
// common functions
	EDbsClose,
// Database functions
	EDbsDatabaseAuthenticate,           //Not used in SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version
	EDbsDatabaseDestroy,
	EDbsDatabaseBegin,
	EDbsDatabaseCommit,
	EDbsDatabaseRollback,				//10
	EDbsDatabaseProperty,
	EDbsDatabaseCreateTable,
	EDbsDatabaseTables,
	EDbsDatabaseColumns,
	EDbsDatabaseIndexes,
	EDbsDatabaseKeys,
	EDbsDatabaseOpenObserver,
	EDbsDatabaseOpenUtility,
	EDbsDatabaseOpenDropTable,
	EDbsDatabaseOpenAlterTable,			//20
	EDbsDatabaseOpenCreateIndex,
	EDbsDatabaseOpenDropIndex,
	EDbsDatabaseExecute,
	EDbsDatabasePrepareView,
	EDbsDatabaseOpenTable,
// Observer functions
	EDbsObserverNotify,
	EDbsObserverCancel,
// Incremental functions
	EDbsIncrementalNext,
// Cursor functions
	EDbsCursorColumnTypes,
	EDbsCursorReset,					//30
	EDbsCursorEvaluate,
	EDbsCursorUnevaluated,
	EDbsCursorSetIndex,
	EDbsCursorSeek,
	EDbsCursorAtBeginning,
	EDbsCursorAtEnd,
	EDbsCursorAtRow,
	EDbsCursorCount,
	EDbsCursorGotoPos,
	EDbsCursorBookmark,					//40
	EDbsCursorGotoBookmark,
	EDbsCursorGet,
	EDbsCursorInsert,
	EDbsCursorUpdate,
	EDbsCursorRetrieveRow,
	EDbsCursorCancel,
	EDbsCursorPut,
	EDbsCursorDelete,
	EDbsCursorColumns,
	EDbsCursorColumnDef,				//50
	EDbsCursorSetNull,
	EDbsCursorColumnSize,
	EDbsCursorColumnSource,
	EDbsCursorColumnSink,
	EDbsCursorOpenConstraint,
	EDbsCursorMatch,
	EDbsCursorFind,
// stream functions
	EDbsStreamRead,
	EDbsStreamWrite,
	EDbsStreamSize,						//60
	EDbsStreamSynch,
// Session functions
	EDbsCreateDatabase,
	EDbsDatabaseList,
	EDbsCopyDatabase,
	EDbsDeleteDatabase,
	EDbsGetSecurityPolicy,
// reserved disk space handling functions
	EDbsReserveDriveSpace,
	EDbsFreeReservedSpace,
	EDbsReserveGetAccess,
	EDbsReserveReleaseAccess,			//70
//
	EDbsGetBackupPath,	
	EDbsGetBackupPaths,	
	//Insert new functuions here
	EDbsLast
	};

/**
@internalComponent
*/
const TInt KDbsTypeBits=4;
const TInt KDbsMagicBits=4;
const TInt KDbsIndexBits=14;
const TInt KDbsFunctionBits=9;
const TInt KDbsHandleBits=KDbsTypeBits+KDbsMagicBits+KDbsIndexBits;
const TInt KDbsTypeMask=(1<<KDbsTypeBits)-1;
const TInt KDbsMagicMask=(1<<KDbsMagicBits)-1;
const TInt KDbsHandleMask=(1<<KDbsHandleBits)-1;
const TInt KDbsObjectReturn=1<<(KDbsFunctionBits-1);
const TInt KDbsIndexLimit=1<<KDbsIndexBits;

/**
@internalComponent
*/
#define KDbsSessionHandle DbsSessionHandle()

/**
@internalComponent
*/
inline TInt DbsMessage(TInt aFunction,TInt aHandle);
inline TDbsFunction DbsFunction(TInt aMessage);
inline TInt DbsHandle(TInt aMessage);
inline TDbsType DbsType(TInt aHandle);
inline TInt DbsMagic(TInt aHandle);
inline TInt DbsObjectIndex(TInt aHandle);
inline TInt DbsMakeHandle(TInt aIndex,TInt aMagic,TDbsType aType);
inline TInt DbsSessionHandle();

#if !defined(_SMALL_BUFFER)
/**
@internalComponent
*/
const TInt KDbsStreamBufSize=0x600;		// 1.5K
const TInt KDbsStreamIoSize=0xc00;		// 3K
const TInt KDbsColumnTypesSize=0x200;	// .5K
#else
const TInt KDbsStreamBufSize=2;
const TInt KDbsStreamIoSize=32;
const TInt KDbsColumnTypesSize=2;
#endif

/**
@internalComponent
*/
class TDbsStreamBuf
	{
public:
	enum {ESize=KDbsStreamBufSize};
public:
	TInt iExt;
	TUint8 iData[ESize];
	};

/**
@internalComponent
*/
class TDbsColumns
	{
public:
	enum {EMax=KDbsColumnTypesSize};
public:
	TInt iCount;
	TUint8 iData[EMax];
	};

/**
client side classes
@internalComponent
*/
class TDbsParam
	{
public:
	inline void operator=(const TDesC8& aDes);
	inline void operator=(const TDesC16& aDes);
	inline void operator=(TInt aVal);
//
	static TPtrC8 PrepareLC(const CDbColSet& aColSet);
	static TPtrC8 PrepareLC(const CDbKey& aKey);
	static TPtrC8 PrepareLC(const TDbLookupKey& aKey);
	
private:
	union
		{
		const TAny* iPtr;
		TInt iInt;
		};
	};

/**
@internalComponent
*/
typedef TDbsParam TDbsMessage[4];

/**
@internalComponent
*/
class RDbsObject : private RDbs
	{
public:
	inline RDbsObject();
	inline void OpenL(const RDbsObject& aDbs,TDbsFunction aFunction);
	inline void OpenL(const RDbsObject& aDbs,TDbsFunction aFunction,const TIpcArgs& aArgs);
	inline void OpenL(const RDbs& aDbs,TDbsFunction aFunction,const TIpcArgs& aArgs);
	void Close();
//
	inline TInt Handle() const;
//
	TInt SendReceive(TDbsFunction aFunction) const;
	inline TInt SendReceive(TDbsFunction aFunction,const TIpcArgs& aArgs) const;
	TInt SendReceiveL(TDbsFunction aFunction) const;
	inline TInt SendReceiveL(TDbsFunction aFunction,const TIpcArgs& aArgs) const;
	inline void SendReceive(TDbsFunction aFunction,TRequestStatus& aStatus) const;
	inline void SendReceive(TDbsFunction aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus) const;
private:
	inline RDbsObject(const RDbs& aDbs);
	void SendReceive(TDbsFunction aFunction,const TIpcArgs* aArgs,TRequestStatus& aStatus) const;
	TInt SendReceive(TDbsFunction aFunction,const TIpcArgs* aArgs) const;
	TInt SendReceiveL(TDbsFunction aFunction,const TIpcArgs* aArgs) const;
	void OpenL(const RDbsObject& aDbs,TDbsFunction aFunction,const TIpcArgs* aArgs);
private:
	TInt iHandle;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsDatabase) : public CDbDatabase
	{
	friend class CDbsSecureDatabase;
public:
	static CDbsDatabase* NewL(const RDbs& aDbs,const TDesC& aDatabase,const TDesC& aFormat);
	~CDbsDatabase();
//
private:
	inline CDbsDatabase();
	void ConstructL(const RDbs& aDbs,TIpcArgs& aArgs);
// from CDbDatabase
	TInt Destroy();
	TInt Begin();
	TInt Commit();
	void Rollback();
	TInt Property(TProperty aProperty);
	void CreateTableL(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey);
	void TablesL(CDbTableNames& aNames);
	void ColumnsL(CDbColSet& aColSet,const TDesC& aName);
	void IndexesL(CDbIndexNames& aNames,const TDesC& aTable);
	void KeysL(CDbKey& aKey,const TDesC& aName,const TDesC& aTable);
	CDbNotifier* OpenNotifierL();
	CDbIncremental* OpenUtilityL(TUtility aType,TInt& aStep);
	CDbIncremental* OpenCompactL(TInt& aStep);
	CDbIncremental* OpenRecoverL(TInt& aStep);
	CDbIncremental* OpenDropTableL(const TDesC& aTable,TInt& aStep);
	CDbIncremental* OpenAlterTableL(const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep);
	CDbIncremental* OpenCreateIndexL(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep);
	CDbIncremental* OpenDropIndexL(const TDesC& aName,const TDesC& aTable,TInt& aStep);
	CDbIncremental* OpenExecuteL(const TDesC& aSql,TDbTextComparison aComparison,TInt& aInit);
	CDbCursor* PrepareViewL(const TDbQuery& aQuery,const TDbWindow& aWindow,RDbRowSet::TAccess anAccess);
	CDbCursor* OpenTableL(const TDesC& aName,RDbRowSet::TAccess anAccess);
private:
	RDbsObject iObject;
	};

//Secure shared database
NONSHARABLE_CLASS(CDbsSecureDatabase) : public CDbsDatabase
	{
public:
	static CDbsSecureDatabase* NewL(const RDbs& aDbs,const TDesC& aDatabase,const TDesC& aFormat);
	virtual ~CDbsSecureDatabase();

private:
	inline CDbsSecureDatabase();
	void ConstructL(const RDbs& aDbs,TIpcArgs& aArgs);

	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsNotifier) : public CDbNotifier
	{
public:
	static CDbsNotifier* NewL(const RDbsObject& aDatabase);
private:
	inline CDbsNotifier();
	~CDbsNotifier();
// from CDbObserver
	void Notify(TType aEvent,TRequestStatus& aStatus);
	void Cancel();
private:
	RDbsObject iObject;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsIncremental) : public CDbAsyncIncremental
	{
public:
	static CDbsIncremental* NewL(const RDbsObject& aDbs,TDbsFunction aFunction,TIpcArgs& aArgs,TInt& aStep);
	~CDbsIncremental();
private:
	inline CDbsIncremental();
// from CDbAsyncIncremental
	void Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus);
private:
	RDbsObject iObject;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsCursor) : public CDbCursor
	{
public:
	static CDbsCursor* NewL(const RDbsObject& aDbs,TDbsFunction aFunction,TIpcArgs& aArgs);
	~CDbsCursor();
private:
	CDbsCursor();
	void ConstructL(const RDbsObject& aDbs,TDbsFunction aFunction,TIpcArgs& aArgs);
//
	TDbColType Type(TDbColNo aCol) const;
	void ReplaceBlobL(TDbColumn& aCol);
	TBool RetrieveL(TDbsFunction aFunction,TInt aArg0=0);
// from CDbCursor
	void Reset();
	TBool EvaluateL();
	void Evaluate(TRequestStatus& aStatus);
	TBool Unevaluated();
	void SetIndexL(const TDesC* anIndex);
	TBool SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison);
	TBool AtBeginning();
	TBool AtEnd();
	TBool AtRow();
	TInt CountL(RDbRowSet::TAccuracy aAccuracy);
	TBool GotoL(RDbRowSet::TPosition aPosition);
	void Bookmark(TDbBookmark::TMark& aMark);
	void GotoL(const TDbBookmark::TMark& aMark);
	void GetL();
	void InsertL(TInsert aClearRow);
	void UpdateL();
	void Cancel();
	void PutL();
	void DeleteL();
	TInt ColumnCount();
	void ColumnsL(CDbColSet& aColSet);
	void ColumnDef(TDbCol& aCol,TDbColNo aColNo);
	TDbColType ColumnType(TDbColNo aCol);
	RDbRow* RowBuffer();
	TDbColumnC ColumnC(TDbColNo aCol);	// non-writeable column
	TDbColumn Column(TDbColNo aCol);		// writeable column
	void SetNullL(TDbColNo aCol);
	TInt ColumnSize(TDbColNo aCol);
	MStreamBuf* ColumnSourceL(TDbColNo aCol);
	MStreamBuf* ColumnSinkL(TDbColNo aCol);
	CDbRowConstraint* OpenConstraintL(const TDbQuery& aCriteria);
	TBool MatchL(CDbRowConstraint& aCursor);
	TInt FindL(RDbRowSet::TDirection aDirection,const TDbQuery& aCriteria);
private:
	enum TState {EUnknown,EAtBeginning,EAtEnd,EAtRow,ERetrieve,ERead,EWrite};
private:
	RDbsObject iObject;
	TState iState;
	TBool iChangedBlob;
	HDbsColumns* iColumns;
	RDbRow iRow;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsConstraint) : public CDbRowConstraint
	{
	friend class CDbsCursor;
private:
	inline CDbsConstraint();
	~CDbsConstraint();
private:
	RDbsObject iObject;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(HDbsBuf) : public TStreamBuf
	{
public:
	static HDbsBuf* NewL(const RDbsObject& aObject,TDbsFunction aFunction,TIpcArgs& aArgs);
	static HDbsBuf* NewLC(const RDbsObject& aObject,TDbsFunction aFunction,TIpcArgs& aArgs);
	static HDbsBuf* NewLC(const RDbs& aDbSession, TDbsFunction aFunction, TIpcArgs& aArgs);
	virtual inline ~HDbsBuf();
private:
	inline HDbsBuf();
	void ConstructL(const RDbsObject& aObject,TDbsFunction aFunction,TIpcArgs& aArgs);
	void ConstructL(const RDbs& aDbSession, TDbsFunction aFunction, TIpcArgs& aArgs);
// from TStreamBuf
	TInt UnderflowL(TInt aMaxLength);
	void OverflowL();
	void DoRelease();
	void DoSynchL();
	TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	void DoWriteL(const TAny* aPtr,TInt aLength);
	TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
private:
	inline void SetPos(TRead,TInt aPos);
	inline void SetPos(TWrite,TInt aPos);
	inline TInt Pos(TRead) const;
	inline TInt Pos(TWrite) const;
	inline TInt MovePos(TRead,TInt anOffset);
	inline TInt MovePos(TWrite,TInt anOffset);
	TInt IpcReadL(TAny* aPtr,TInt aMaxLength);
	void IpcWriteL(const TAny* aPtr,TInt aLength);
	TInt EndL();
//
	inline TInt Lag(TRead) const;
	inline TInt Lag(TWrite) const;
	inline TInt Mark(TRead) const;
	inline TInt Mark(TWrite) const;
private:
	RDbsObject iIpc;
	TInt iRPos;
	TInt iWPos;
	TDbsStreamBuf iBuf;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(HDbsReadBuf) : public TMemBuf
	{
public:
	static HDbsReadBuf* NewL(const TDesC8& aDes);
protected:
	inline HDbsReadBuf(const TDesC8& aDes);
	void DoRelease();
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsObserver) : public CActive
	{
public:
	/**
	@internalComponent
	*/
	class HObserver
		{
		friend class CDbsObserver;
	public:
		~HObserver();
		void Notify(const RMessage2& aMessage);		
		void Cancel();
	private:
		inline HObserver();
		void Complete(TInt aStatus);
		void Event(TInt aEvent);
	private:
		TDblQueLink iLink;
		TInt iPending;
		RMessagePtr2 iMessage;
		};
	friend class HObserver;
public:
	static CDbsObserver* NewL(CDbsSource& aSource);
	static CDbsObserver* Collect(CDbsObserver* aNotifier);
	HObserver* ObserverL();
private:
	inline CDbsObserver(CDbsSource& aSource);
	~CDbsObserver();
//
	void Queue();
// from CActive
	void RunL();
	void DoCancel();
private:
	CDbsSource& iSource;
	CDbNotifier* iNotifier;
	TDblQue<HObserver> iQueue;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsConnection) : public CDbContext
	{
	friend class RDbsSources;
public:
	inline CDbsSource& Source() const;
	static inline const CDbsConnection& Connection(const CDbObject& aObject);
	static inline CDbsSource& Source(const CDbObject& aObject);
private:
	inline CDbsConnection() {}
	~CDbsConnection();
	inline void Set(CDbsSource& aSource);
private:
	CDbsSource* iSource;
	};

/**
@internalComponent
*/
class CDbsDatabaseStub : public CDbObject
	{
public:
	static CDbsDatabaseStub* NewL();
	CDbDatabase* AuthenticateL();
private:
	inline CDbsDatabaseStub();
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsSource) : public CBase
	{
	friend class CDbsConnection;
	friend class CDbsObserver;
public:
	static CDbsSource* NewL(RFs& aFs,const TDesC& aSource);
//
	static inline TInt LinkOffset();
	TBool Is(const TDesC& aSource) const;
//
	inline CDbSource& Source();
	CDbsObserver::HObserver* ObserverL();
private:
	inline CDbsSource(const TDbFormat& aFormat);
	~CDbsSource();
	void Closed();
//
	inline void Open();
	void Close();
private:
	TDblQueLink iLink;
	HBufC* iName;
	const TDbFormat& iFormat;
	TInt iConnections;
	CDbSource* iSource;
	CDbsObserver* iObserver;
	};

/**
@internalComponent
*/
class RDbsSources
	{
private:
	typedef TDblQueIter<CDbsSource> TIter;
public:
	inline RDbsSources(RDbCache& aCache);
	inline void Close();
//
	CDbsConnection* OpenLC(RFs& aFs,const TDesC& aSource,const TDesC& aFormat);
private:
	TDblQue<CDbsSource> iSources;
	RDbCache iDrivers;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsServer) : public CServer2
	{
public:
	static CDbsServer* NewL();
//
	inline TDes& Name0();
	inline TDes& Name1();
	inline TDes& FileName();
	inline RDbsSources& Sources();
	inline RFs& Fs();
	inline RDriveSpaceCol& DriveSpaceCol();
	void RemoveSession();
//
	static CDbsServer* Instance();
	void Panic(const TDesC& aCategory,TInt aCode);	// .. panic client!

	inline CPolicyProxy& PolicyProxy() const;
	inline RDbPropsFactory& DbPropsFactory();

private:
	inline CDbsServer();
	~CDbsServer();
	void ConstructL();
// from CServer
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
private:
	enum {ECacheSize=8};
	enum {EServerPriority=0};
private:
	RDbCache iCache;
	RDbsSources iSources;
	TDbName iName0;		// used for message servicing
	TDbName iName1;
	TFileName iFileName;
	RFs iFs;
	CPolicyProxy* iPolicyProxy;
	RDbPropsFactory iDbPropsFactory;
	RDriveSpaceCol iDriveSpaceCol;
};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbsSession) : public CSession2
	{
private:
#if !defined(_SMALL_BUFFER)
	enum {EIndexGranularity=8};
#else
	enum {EIndexGranularity=1};
#endif
	struct TEntry
		{
		union
			{
			TAny* iObject;
			TInt iNext;
			};
		TUint8 iType;
		const MPolicy* iPolicy;
		TUint8 iMagic;
	public:
		void Release();
		DECLARE_DB_DUMP3(aFile)
	//
		inline TDbsType Type() const;
		inline CDbsDatabaseStub& DatabaseStub();
		inline CDbDatabase& Database();
		inline CDbIncremental& Incremental();
		inline CDbCursor& Cursor();
		inline CDbRowConstraint& Constraint();
		inline HDbsStream& Stream();
		inline CDbsObserver::HObserver& Observer();
		};
public:
	CDbsSession();
	virtual ~CDbsSession();
private:
//From CSession2
	virtual void CreateL();
// from CSession
	void ServiceL(const RMessage2& aMessage);	
	TInt ExtServiceL(const RMessage2& aMessage, TDbsFunction aDbsFunction);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	TInt CountResources();
//
	inline CDbsServer& Server() const;
	inline RDbsSources& Sources();
//
	const TDesC& ReadName0L(TInt aIndex,const RMessage2& aMessage);
	const TDesC& ReadName1L(TInt aIndex,const RMessage2& aMessage);
	const TDesC& ReadFileNameL(TInt aIndex, const RMessage2& aMessage);
	HBufC* ReadHBufLC(TInt aIndex,const RMessage2& aMessage);
	HBufC8* ReadHBuf8LC(TInt aIndex,const RMessage2& aMessage);
	CDbColSet* ColSetLC(TInt aIndex,const RMessage2& aMessage);
	CDbKey* KeyLC(TInt aIndex,const RMessage2& aMessage);
	TDbLookupKey* LookupKeyLC(TInt aIndex,TInt aSize,const RMessage2& aMessage);
	CDbObject* DoAuthenticateL(CDbsConnection* aDbsConnection, const RMessage2& aMessage);
	TInt DoOpenDatabaseL(const RMessage2& aMessage, const TDbProps& aDbProps);

	TDbProps* ExtractDbPropsLC(const RMessage2& aMessage);
	TInt OpenDatabaseL(const RMessage2& aMessage);	
	TInt CreateDatabaseL(const RMessage2& aMessage);
	TInt DoCreateDatabaseL(const RMessage2& aMessage, const TDbProps& aDbProps);
	void CopyDatabaseL(const RMessage2& aMessage);
	void DeleteDatabaseL(const RMessage2& aMessage);
	TInt GetDatabaseListL(const RMessage2& aMessage);	
	void GetSecurityPolicyL(const RMessage2& aMessage);	
	void ReadRowL(RDbRow& aRow,TInt aSize,const RMessage2& aMessage);
	void PutRowL(CDbCursor& aCursor,const RMessage2& aMessage);
	TInt WriteRowL(const RDbRow& aRow,const RMessage2& aMessage);
	TInt RetrieveRowL(CDbCursor& aCursor,const RMessage2& aMessage);
	TInt NewCursorL(CDbCursor* aCursor,const RMessage2& aMessage,const MPolicy* aTblSecurityPolicy);
	TInt NewIncrementalL(CDbIncremental* aCursor,TInt& aInit,const RMessage2& aMessage,const MPolicy* aPolicy);
	TInt NewStreamL(MStreamBuf* aHost,const RMessage2& aMessage,const MPolicy* aPolicy,TInt aExtent=-1);
	TInt NewStreamL(TAny* aPtr,TExternalizeFunction aExter,const RMessage2& aMessage,const MPolicy* aPolicy);
	void NewDbEntryL(CDbObject* aDbObject, const TDbPolicyRequest& aDbPolicyRequest, TInt& aDbHandle);
	
#ifdef __DBDUMP__
	void Dump();
#endif	

	void ReserveDriveSpaceL(TDriveNumber aDrive);
	void FreeReservedSpace(TDriveNumber aDrive);
	void GetReserveAccessL(TDriveNumber aDrive);
	void ReleaseReserveAccess(TDriveNumber aDrive);
	
	void GetBackupPathL(const RMessage2& aMessage);
	TInt GetBackupPathsL(const RMessage2& aMessage);

	inline TInt Add(CDbIncremental* aIncremental, const MPolicy* aPolicy);
	inline TInt Add(CDbCursor* aCursor, const MPolicy* aPolicy);
	inline TInt Add(CDbRowConstraint* aConstraint, const MPolicy* aPolicy);
	inline TInt Add(HDbsStream* aStream, const MPolicy* aPolicy);
	inline TInt Add(CDbsObserver::HObserver* aObserver, const MPolicy* aPolicy);

	TEntry& Object(TInt aHandle);
	void AllocL();
	TInt DoAdd(TAny* aObject,TDbsType aType, const MPolicy* aPolicy);
	void Free(TEntry& aEntry);

private:
	TEntry* iIx;
	TInt iSize;
	TInt iFree;
	RMap<TInt, TDbPolicyRequest> iDbPolicyRqColl;
    CDbsSessDriveSpace* iSessDriveSpace;
	};

/**
@internalComponent
*/
class HDbsStream
	{
public:
	inline HDbsStream(MStreamBuf* aHost,TInt aReadPos);
	inline ~HDbsStream();
//
	TInt ReadL(const RMessage2& aMessage);	
	void WriteL(const RMessage2& aMessage);	
	inline TInt SizeL();
	inline void SynchL();
private:
	MStreamBuf& iHost;
	TInt iRPos;
	TInt iWPos;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(HBufBuf) : public TBufBuf
	{
	enum {EGranularity=512};
public:
	static HBufBuf* NewLC();
private:
	inline HBufBuf();
	virtual inline ~HBufBuf();
	void DoRelease();
private:
	CBufBase* iBuf;
	};

#include "SD_STD.INL"



