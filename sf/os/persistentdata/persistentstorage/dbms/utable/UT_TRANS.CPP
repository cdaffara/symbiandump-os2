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

// Class RDbTransaction::CNotifier

NONSHARABLE_CLASS(RDbTransaction::CNotifier) : public CDbNotifier
	{
public:
	inline CNotifier( RDbTransaction& aTransaction );
	~CNotifier();
//
	void Event( RDbNotifier::TEvent aEvent );
private:
	void Complete( TInt aStatus );
// from CDbNotifier
	void Notify( TType aEvent, TRequestStatus& aStatus );
	void Cancel();
private:
	RDbTransaction* iTransaction;
	TRequestStatus* iStatus;
	TInt iPending;
	};

inline RDbTransaction::CNotifier::CNotifier( RDbTransaction& aTransaction )
 :	iTransaction( &aTransaction )
	{}

RDbTransaction::CNotifier::~CNotifier()
//
// Cancel any outstanding request and extract from the transaction
//
	{
	Cancel();
	if ( iTransaction )
		{
		__ASSERT( iTransaction->iNotifier == this );
		iTransaction->iNotifier = 0;
		}
	}

void RDbTransaction::CNotifier::Complete( TInt aStatus )
	{
	if ( iStatus )
		{
		iPending = 0;
		User::RequestComplete( iStatus, aStatus );
		}
	}

void RDbTransaction::CNotifier::Notify( CDbNotifier::TType aType, TRequestStatus& aStatus )
//
// Request for future notification. If the database is closed complete immediately
//
	{
	__ASSERT( !iStatus );
	__ASSERT( iPending >= 0 );
	iStatus = &aStatus;
	if ( iPending > RDbNotifier::EUnlock )
		Complete( iPending );
	else if ( !iTransaction )
		Complete( RDbNotifier::EClose );
	else
		{
		iPending = aType;
		aStatus = KRequestPending;
		}
	}

void RDbTransaction::CNotifier::Cancel()
	{
	Complete( KErrCancel );
	}

void RDbTransaction::CNotifier::Event( RDbNotifier::TEvent aEvent )
	{
	if ( aEvent == RDbNotifier::EClose )
		iTransaction = 0;
	if ( iStatus )
		{
		__ASSERT( iPending < 0 );
		if (aEvent == RDbNotifier::EUnlock && iPending == CDbNotifier::EChange )
			;	// not interested in unlock events
		else
			Complete( aEvent );
		}
	else
		{
		__ASSERT( iPending >= 0 );
		if ( aEvent > iPending )
			iPending = aEvent;		// save the event
		}
	}


// Class RDbTransaction

#ifdef _ASSERTIONS

void RDbTransaction::_Invariant() const
//
// Invariance test
//
	{
	if ( iLockCount == 0 )
		{	// nothing must be happening in this state
		__ASSERT( iLockState == EDbReadLock );
		__ASSERT( iAction == EDbReadLock );
		__ASSERT( iUpdaters == 0 );
		return;
		}
	switch ( iLockState & EState )
		{
	default:
		__ASSERT( 0 );
	case EDbReadLock:
		{
		__ASSERT( iAction == EDbReadLock );
		__ASSERT( iLockCount > 0 );		// someone must have a lock
		__ASSERT( iLockCount <= iMaxLock );
		__ASSERT( iUpdaters == 0 );
		__ASSERT( iPrimary.iState != 0 );
		for (TInt ii = iLockCount - 1; --ii >= 0; )
			__ASSERT( iSharers[ii].iState != 0 );
		}
		break;
	case EDbCompactLock:		// not allowed in user-transactions
	case EDbRecoveryLock:
		__ASSERT( iAction == iLockState );
		__ASSERT( iLockCount == 1 );	// exactly one lock allowed
		__ASSERT( iUpdaters == 0 );
		__ASSERT( iPrimary.iState == 0 );
		break;
	case EDbXReadLock:	// intention to write. No updates but exclusive
		__ASSERT( iLockCount == 1 );	// exactly one lock allowed
		switch ( iAction )
			{
		default:
			__ASSERT( 0 );
		case EDbReadLock:	// must be in a transaction: cannot commit a write/schema mod when releasing a read lock
			__ASSERT( iUpdaters == 0 );
			__ASSERT( iPrimary.iState & static_cast<TUint>( ETransactionLock ) );
			break;
		case EDbWriteLock:
			__ASSERT( iUpdaters > 0 );
			break;
			}
		break;
	case EDbWriteLock:
	case EDbSchemaLock:
		__ASSERT( iLockCount == 1 );	// exactly one lock allowed
		switch ( iAction )
			{
		default:
			__ASSERT( 0 );
		case EDbReadLock:	// must be in a transaction: cannot commit a write/schema mod when releasing a read lock
			__ASSERT( iUpdaters == 0 );
			__ASSERT( iPrimary.iState & static_cast<TUint>( ETransactionLock ) );
			break;
		case EDbWriteLock:
			__ASSERT( iUpdaters > 0 );
			__ASSERT( ( iLockState & EState ) == EDbWriteLock || ( iPrimary.iState & static_cast<TUint>( ETransactionLock ) ) );
			break;
		case EDbSchemaLock:
			__ASSERT( ( iLockState & EState ) == EDbSchemaLock );
			__ASSERT( iUpdaters == 0 );
			break;
			}
		break;
		}
	}

template <class T> struct _InvariantFunc
	{
	static void Invariant( TAny* aPtr ) { ( (const T*)aPtr )->_Invariant(); }
	};

template <class T> inline TCleanupOperation _InvariantFunction( T* )
	{ return _InvariantFunc<T>::Invariant; }

struct _Invariant
	{
	inline _Invariant( TCleanupOperation aOp, TAny* aPtr )
	 :	iOp( aOp ), iPtr( aPtr )
		{ aOp( aPtr ); }
	inline ~_Invariant()
		{ iOp( iPtr ); }
private:
	TCleanupOperation iOp;
	TAny* iPtr;
	};

#ifndef __LEAVE_EQUALS_THROW
struct _InvariantL
	{
	inline _InvariantL( TCleanupOperation aOp, TAny* aPtr )
		{ aOp( aPtr ); CleanupStack::PushL( TCleanupItem( aOp, aPtr ) ); }
	inline ~_InvariantL()
        { CleanupStack::PopAndDestroy(); }
	};
#endif //__LEAVE_EQUALS_THROW__

#define __INVARIANT   struct _Invariant _invariant( _InvariantFunction( this ), this );

#ifdef __LEAVE_EQUALS_THROW__
	#define __INVARIANT_L __INVARIANT
#else
	#define __INVARIANT_L struct _InvariantL _invariant( _InvariantFunction( this ), this );
#endif //__LEAVE_EQUALS_THROW__

#else // _ASSERTIONS

#define __INVARIANT   ( (void)0 )
#define __INVARIANT_L ( (void)0 )

#endif // _ASSERTIONS

inline TDbLockType RDbTransaction::LockState() const
	{ return TDbLockType( iLockState & EState ); }

void RDbTransaction::Close()
	{
	__ASSERT( !IsLocked() );
	User::Free( iSharers );
	Event( RDbNotifier::EClose );
	}

void RDbTransaction::DoCommitL()
//
// Commit any changes
//
	{
	__ASSERT_ALWAYS( ( iPrimary.iState & ~ETransactionLock ) == 0, Panic( EDbStreamsPendingOnCommit ) );
	iLockState |= EFailed;
	Database().FlushL( LockState() );
	Database().SynchL( LockState() );
	Unlock( RDbNotifier::ECommit );
	}

void RDbTransaction::DoRollback()
//
// Rollback any changes
//
	{
	__ASSERT_ALWAYS( ( iPrimary.iState & ~ETransactionLock ) == 0, Panic( EDbStreamsPendingOnRollback ) );
	Database().Revert( LockState() );
	Database().Abandon( LockState() );
	if ( LockState() >= EDbWriteLock )
		++iRollback;
	Unlock( RDbNotifier::ERollback );
	}

// explicit transactions

void RDbTransaction::BeginL( const CDbObject& aObject )
//
// begin a user transaction. This first gains a shared read-lock
//
	{
	__INVARIANT_L;
	__ASSERT_ALWAYS( GetLock( aObject ) == 0, Panic( EDbBeginNestedTransaction ) );
	ReadyL();
	PrepareSLockL( aObject, TUint( ETransactionLock ) );
	__ASSERT( iAction == EDbReadLock );
	__ASSERT( iLockState == EDbReadLock );
	++iLockCount;
	}

void RDbTransaction::CommitL( const CDbObject& aObject )
//
// Commit a user transaction and release the lock
// All updates must be complete for a write-lock
//
	{
	__INVARIANT_L;
	__ASSERT_ALWAYS( InTransaction( aObject ), Panic( EDbNoCurrentTransaction ) );
	ReadyL();
	if ( iLockCount > 1 )
		{
		TLock* lock = GetLock( aObject );
		__ASSERT( lock );
		__ASSERT_ALWAYS( lock->iState == static_cast<TUint>( ETransactionLock ), Panic( EDbStreamsPendingOnCommit ) );
		Unlock( *lock );
		}
	else
		{
		__ASSERT_ALWAYS( iAction == EDbReadLock, Panic( EDbUpdatesPendingOnCommit ) );
		DoCommitL();
		}
	}

void RDbTransaction::Rollback( const CDbObject& aObject )
//
// Rollback a user transaction and release the lock
// All updates must be complete/aborted for a write-lock
//
	{
	__INVARIANT;
	__ASSERT_ALWAYS( InTransaction( aObject ), Panic( EDbNoCurrentTransaction ) );
	if ( iLockCount > 1 )
		{
		TLock* lock = GetLock( aObject );
		__ASSERT( lock );
		__ASSERT_ALWAYS( lock->iState == static_cast<TUint>( ETransactionLock ), Panic( EDbStreamsPendingOnRollback ) );
		Unlock( *lock );
		}
	else
		{
		__ASSERT_ALWAYS( iAction == EDbReadLock, Panic( EDbUpdatesPendingOnRollback ) );
		DoRollback();
		}
	}

void RDbTransaction::PrepareSLockL( const CDbObject& aObject, TUint aInitState )
//
// prepare to acquire a shared read lock
// if any holder has an exclusive lock this fails
//
	{
	__ASSERT( GetLock( aObject ) == 0 );	// cannot get a 2nd shared lock
//
	THolder h = aObject.Context();
	if ( iLockCount == 0 )
		{
		iPrimary.iHolder = h;				// first lock, no other checks required
		iPrimary.iState = aInitState;
		}
	else if ( iLockState != EDbReadLock )
		__LEAVE( KErrLocked );
	else
		{						// allocate a Sharers-slot
		TLock* share = iSharers;
		if ( iLockCount == iMaxLock )
			{
			TInt newsize = iMaxLock + ELockListGranularity;
			if ( newsize > EMaxLock )
				{
				__LEAVE( KErrLocked );
				return;
				}
			iSharers = share = ( TLock* )User::ReAllocL( share, ( newsize - 1 ) * sizeof( TLock ) );
			iMaxLock = TUint8( newsize );
			}
		share += iLockCount - 1;
		share->iHolder = h;
		share->iState = aInitState;
		}
	}

void RDbTransaction::PrepareXLockL( const CDbObject& aObject )
//
// prepare to acquire an exclusive lock
// if any other holder has a lock this fails
//
	{
	THolder h = aObject.Context();
	switch ( iLockCount )
		{
	case 0:					// no other holders, acquire the lock
		iPrimary.iHolder = h;
		iPrimary.iState = 0;		// this is not a transaction lock
		break;
	case 1:					// check we are the single Lock holder
		if (iPrimary.iHolder != h)
			__LEAVE( KErrLocked );
		break;
	default:				// cannot get XLock
		__LEAVE( KErrLocked );
		break;
		}
	}

void RDbTransaction::Unlock( RDbNotifier::TEvent aEvent )
//
// Remove the last lock and signal an event to the Notifier
//
	{
	__ASSERT( iLockCount == 1 );
	__ASSERT( ( iPrimary.iState & ~ETransactionLock ) == 0 );
	TDbLockType ls = LockState();
	Event( ls == EDbReadLock || ls == EDbXReadLock ? RDbNotifier::EUnlock : aEvent );
	iLockCount = 0;
	iAction = iLockState = EDbReadLock;
	iUpdaters = 0;
	Database().CheckIdle();
	}

void RDbTransaction::Unlock( RDbTransaction::TLock& aLock )
//
// Remove a shared lock holder from the list
//
	{
	__ASSERT( iLockCount > 1 );
	__ASSERT( LockState() == EDbReadLock );
	__ASSERT( ( aLock.iState & ~ETransactionLock ) == 0 );
	aLock = iSharers[--iLockCount - 1];
	}

RDbTransaction::TLock* RDbTransaction::GetLock( const CDbObject& aObject )
//
// Test if aObject holds any lock, and return it
//
	{
	const THolder h = aObject.Context();
	TInt lc = iLockCount;
	if ( --lc >= 0 )
		{
		if ( iPrimary.iHolder == h )
			return &iPrimary;
		if ( lc > 0 )
			{
			TLock* const base = iSharers;
			TLock* l = base + lc;
			do	{
				if ( ( --l )->iHolder == h )
					return l;
				} while ( l > base );
			}
		}
	return 0;
	}

TBool RDbTransaction::InTransaction( const CDbObject& aObject )
//
// Test if aObject holds a non-auto transaction
//
	{
	__INVARIANT;
	TLock* lock = GetLock( aObject );
	return lock ? lock->iState & static_cast<TUint>( ETransactionLock ) : 0;
	}

void RDbTransaction::ReadPrepareL( const CDbObject& aObject )
//
// Check that aObject can gain a shared read lock and allocate required resources
//
	{
	__INVARIANT_L;
	if ( GetLock( aObject ) == 0 )
		PrepareSLockL( aObject, 0 );		// prepare a S-Lock for the read
	else if ( iAction == EDbCompactLock )	// Cannot already hold a compaction lock
		__LEAVE( KErrAccessDenied );
	}

void RDbTransaction::ReadBegin( const CDbObject& aObject )
//
// Take a read-lock: ReadPrepareL(aObject) _must_ already have been called
//
	{
	__INVARIANT;
	TLock* lock = GetLock( aObject );
	if ( !lock )
		{
		++iLockCount;
		lock = GetLock( aObject );
		__ASSERT( lock );
		}
	++lock->iState;
	}

void RDbTransaction::ReadRelease( const CDbObject& aObject )
	{
	__INVARIANT;
	TLock* lock = GetLock( aObject );
	__ASSERT( lock );
	__ASSERT( ( lock->iState & ~ETransactionLock ) > 0 );
	if ( --lock->iState == 0 )
		{	// not transaction-lock
		if ( iLockCount > 1 )
			Unlock( *lock );
		else if ( iAction == EDbReadLock )	// no other locks to this client
			Unlock( RDbNotifier::EUnlock );
		}
	}

void RDbTransaction::DMLCheckL()
//
// Check that we can open a new rowset
//
	{
	__INVARIANT_L;
	ReadyL();
	if ( iAction > EDbCompactLock )	
		__LEAVE( KErrAccessDenied );
	}

void RDbTransaction::DMLPrepareL( const CDbObject& aObject )
//
// Check that we can do DML, this should be called immediately prior to DMLBegin
//
	{
	__INVARIANT_L;
	PrepareXLockL( aObject );
	if ( iAction>EDbWriteLock )
		__LEAVE( KErrAccessDenied );
	}

void RDbTransaction::DMLBegin()
//
// A Rowset begins an update
//
	{
	__INVARIANT;
	__ASSERT( iAction == EDbReadLock || iAction == EDbWriteLock );
	__ASSERT( ( iLockState & EFailed ) == 0 );
	__ASSERT( iLockCount <= 1 );
	if ( iAction == EDbReadLock )
		iAction = EDbWriteLock;
	if (iLockState == EDbReadLock )
		iLockState = EDbXReadLock;		// escalate lock to exclusive as we are now writing
	++iUpdaters;
	iLockCount = 1;
	}

void RDbTransaction::DMLTouch()
//
// This must be called prior to putting DML updates
//
	{
	__ASSERT( iAction == EDbWriteLock );
	__ASSERT( iUpdaters > 0 );
	TInt ls = iLockState;
	if ( ls == EDbXReadLock )
		ls = EDbWriteLock | EFailed;
	else
		ls |= EFailed;
	iLockState = TUint8( ls );
	}

void RDbTransaction::DMLBeginLC()
	{
	DMLBegin();
	CleanupStack::PushL( TCleanupItem( DMLAbandon, this ) );
	DMLTouch();
	}

void RDbTransaction::DMLCommitL()
//
// A rowset has completed an update
//
	{
	__INVARIANT_L;
	__ASSERT( iAction == EDbWriteLock && ( iLockState & EFailed ) );
	TInt updaters = iUpdaters - 1;
	if ( updaters == 0 )
		{
		if ( ( iPrimary.iState & static_cast<TUint>( ETransactionLock ) ) == 0 )
			{
			__ASSERT( iLockState == ( EDbWriteLock | EFailed ) );
			DoCommitL();		// automatic write-commit, release auto-lock
			return;
			}
		iAction = EDbReadLock;
		}
	iUpdaters = updaters;
	iLockState &= ~EFailed;
	}

void RDbTransaction::DMLRollback()
//
// Rollback a DML operation
//
	{
	__INVARIANT;
	__ASSERT( iAction == EDbWriteLock );
	TInt updates = iUpdaters - 1;
	if ( updates == 0 )
		{
		if ( ( iPrimary.iState & static_cast<TUint>( ETransactionLock ) ) == 0 )
			{
			__ASSERT( LockState() == EDbWriteLock || LockState() == EDbXReadLock );
			DoRollback();		// automatic rollback now (may panic)
			return;
			}
		iAction = EDbReadLock;
		}
	iUpdaters = updates;
	}

void RDbTransaction::DMLAbandon( TAny* aPtr )
	{
	STATIC_CAST( RDbTransaction*, aPtr )->DMLRollback();
	}

void RDbTransaction::DDLPrepareL( const CDbObject& aObject )
//
// Check that we can use the database for ddl and flush out any tables
// should be called before DDLBegin
//
	{
	__INVARIANT_L;
	ReadyL();
	PrepareXLockL( aObject );
	if ( iAction != EDbReadLock || ( IsLocked() && iPrimary.iState != static_cast<TUint>( ETransactionLock ) ) )
		__LEAVE( KErrAccessDenied );	// Cannot take sole ownership of the database
	TInt ls = iLockState;
	if ( ls >= EDbWriteLock )
		{	// ensure all table data is flushed as they may be "released"
		iLockState = TUint8( ls | EFailed );
		Database().FlushL( EDbWriteLock );
		iLockState = TUint8( ls );
		}
	}

void RDbTransaction::DDLBegin()
//
// A DDL object is about to start ops
//
	{
	__INVARIANT;
	__ASSERT( iAction == EDbReadLock );
	__ASSERT( ( iLockState & EFailed ) == 0 );
	__ASSERT( iLockCount <= 1 );
	iLockState = iAction = EDbSchemaLock;
	iLockCount = 1;
	}

void RDbTransaction::DDLBeginLC()
	{
	DDLBegin();
	CleanupStack::PushL( TCleanupItem( DDLAbandon, this ) );
	}

void RDbTransaction::DDLCommitL()
//
// A DDL incremental object has completed
//
	{
	__INVARIANT_L;
	__ASSERT( iAction == EDbSchemaLock );
	if ( ( iPrimary.iState & static_cast<TUint>( ETransactionLock ) ) == 0 )
		{
		__ASSERT( iLockState == EDbSchemaLock );
		DoCommitL();	// release auto-lock
		}
	else
		iAction = EDbReadLock;
	}

void RDbTransaction::DDLRollback()
//
// Rollback a DDL operation
//
	{
	__INVARIANT;
	__ASSERT( iAction == EDbSchemaLock );
	iLockState |= EFailed;
	if ( ( iPrimary.iState & static_cast<TUint>( ETransactionLock ) ) == 0 )
		{
		__ASSERT( iLockState == ( EDbSchemaLock | EFailed ) );
		DoRollback();		// release auto-lock
		}
	else
		iAction = EDbReadLock;
	}

void RDbTransaction::DDLAbandon( TAny* aPtr )
	{
	STATIC_CAST( RDbTransaction*, aPtr )->DDLRollback();
	}

// recovery. Nothing else can be done at the same time as this

void RDbTransaction::UtilityPrepareL( const CDbObject& aObject )
//
// Check that we are in a state to run a utility
//
	{
	__INVARIANT_L;
	ReadyL();
	PrepareXLockL( aObject );
	if ( IsLocked() )			// utilities not allowed in user transaction
		__LEAVE( KErrAccessDenied );
	}

void RDbTransaction::UtilityBegin( CDbDatabase::TUtility aType )
//
// Database Recovery object is about to start
//
	{
	__INVARIANT;
	__ASSERT( !IsLocked() );
	if ( aType == CDbDatabase::ERecover )
		iLockState = iAction = EDbRecoveryLock;
	else
		iLockState = iAction = EDbCompactLock;
	iLockCount = 1;
	}

void RDbTransaction::UtilityCommitL()
//
// Database Recovery has completed
//
	{
	__INVARIANT_L;
	Database().SynchL( LockState() );
	Unlock( iAction == EDbRecoveryLock ? RDbNotifier::ERecover : RDbNotifier::EUnlock );	// release auto-lock
	}

void RDbTransaction::UtilityRollback()
	{
	__INVARIANT;
	Database().Revert( LockState() );
	Unlock( RDbNotifier::EUnlock );	// release auto-lock
	}

CDbNotifier* RDbTransaction::NotifierL()
//
// Only support a single notifier for the database (server multiplexes)
//
	{
	if ( iNotifier )
		__LEAVE( KErrNotSupported );
	return iNotifier = new( ELeave ) CNotifier( *this );
	}

void RDbTransaction::Event( RDbNotifier::TEvent aEvent )
//
// Report an event to the Notifier
// If the lock was less than a write lock, report unlock only: no commit or rollback
//
	{
	if ( iNotifier )
		iNotifier->Event( aEvent );
	}


