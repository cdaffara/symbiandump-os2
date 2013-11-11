/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/********************************************************************************
 *
 * Stat interfaces
 *
 ********************************************************************************/
#ifndef __MSTATAPITRANSPORT_H__
#define __MSTATAPITRANSPORT_H__

/********************************************************************************
 *
 * System Includes
 *
 ********************************************************************************/
#include <e32std.h>
#include <f32file.h>

/********************************************************************************
 *
 * Types
 *
 ********************************************************************************/
enum TTransportResult {
	KSTErrSuccess,
	KSTErrAsynchronous,
	KSTErrGeneralFailure,
	KSTErrReadFailure,
	KSTErrWriteFailure,
	KSTErrConnectFailure,
	KSTErrUnexpectedFragmentCommand,
	KSTErrResyncCommand
};

enum TCommStatus { 
	EIdle,
	EInitialising, 
	EInitialised, 
	EConnecting, 
	EConnected,
	EDisconnecting, 
	EDisconnected, 
	EReleasing, 
	ESendPending,
	EReceivePending,
	ELast
};

enum TReadWriteStatus {
		ENoRW,
		EWritePending,
		EReadPending
};

enum TStatConnectType { 
		ETCPIP, 
		ESerial, 
		EInfrared,
		EBluetooth,
		EUsb,
		// ...
		ENumConnectTypes,
};

/********************************************************************************
 *
 * MNotifyStatTransport - callback interface given to network on transport
 *
 ********************************************************************************/
class MNotifyStatTransport
{
public:
	virtual void HandleInitialise( TInt aResult ) = 0;
	virtual void HandleConnect( TInt aResult ) = 0;
	virtual void HandleSend( TInt aResult ) = 0;
	virtual void HandleReceive( TInt aResult, TDesC8 *aData, TUint aDataLength ) = 0;
	virtual void HandleDisconnect( TInt aResult ) = 0;
	virtual void HandleRelease( TInt aResult ) = 0;
	virtual void HandleError( TInt aError, void *aErrorData ) = 0;
	virtual void HandleInfo( const TDesC *aInfo ) = 0;
};

/********************************************************************************
 *
 * MDataSupplier - interface to contain a data buffer and means for users
 *					to access the data through this object.
 *					Its purpose is to prevent the need for many data buffers
 *					to be created by each object that needs data (instead
 *					one instance of this interface will be passed), and also
 *					it will hide from the user certain implementations such
 *					as reading data from a file when more data is needed 
 *					(this prevents the need to read all the data from a 
 *					file at one time).
 *					For STAT the purpose of this is to buffer data sent from
 *					the device to the PC.  The data is buffered when it is
 *					sent to the PC.
 *
 ********************************************************************************/
class MDataSupplier
{
public:
	// Destroy this object.
	// After this call the memory is deleted and any pointers to
	// the object become invalid.
	virtual void	Delete( void ) = 0;

	// Return the total size of the data available through
	// this data supplier.
	// The return value is one of the standard error codes:
	// 	KErrNone
	virtual TInt	GetTotalSize( TInt &aTotalSize ) = 0;
	// Return the size of the data that is remaining in this
	// data supplier.
	// The return value is one of the standard error codes:
	// 	KErrNone
	virtual TInt	GetRemainingSize( TInt &aRemainingSize ) = 0;

	// Set data in this data supplier object.
	// The nature of the source data buffer and the
	// storage used will be implementation specific to 
	// derived classes.
	// The return value is one of the standard error codes:
	// 	KErrNone
	// 	KErrNoMemory
	//	KErrNotFound
	virtual TInt	SetData( const TDesC8 &aSource ) = 0;

	// Retreives data from this data supplier and copies it to
	// the supplied buffer.
	// The implementattion will be specific to derived classes
	// but should ensure that if the user wants all data then it
	// should be available in one call.
	// If the length requested was more than the data remaining
	// the return value will indicate success and the out
	// parameter aActuallyCopied will hold the number of bytes 
	// copied.
	// The return value is one of the standard error codes:
	// 	KErrNone
	// 	KErrNoMemory
	//	KErrNotFound
	virtual TInt	GetData( HBufC8 &aDestination, TInt aLengthToCopy,
						TInt &aActuallyCopied ) = 0;

protected:
	MDataSupplier( void ) {;}
	virtual ~MDataSupplier( ) {;}
};

/********************************************************************************
 *
 * MDataConsumer - interface to contain a data buffer and means for users
 *					to access the data through this object.
 *					Its purpose is to prevent the need for many data buffers
 *					to be created by each object that needs data (instead
 *					one instance of this interface will be passed), and also
 *					it will hide from the user certain implementations such
 *					as reading data from a file when more data is needed 
 *					(this prevents the need to read all the data from a 
 *					file at one time).
 *					For STAT the purpose of this is to buffer recevied from
 *					the PC to the device.  The data is buffered when it is
 *					received on the device.
 *
 ********************************************************************************/
class MDataConsumer
{
public:
	// Destroy this object.
	// After this call the memory is deleted and any pointers to
	// the object become invalid.
	virtual void	Delete( void ) = 0;

	// Return the total size of the data available through
	// this data supplier.
	// The return value is one of the standard error codes:
	// 	KErrNone
	virtual TInt	GetTotalSize( TInt &aTotalSize ) = 0;

	// Add data in this data consumer object.
	// The nature of the source data buffer and the
	// storage used will be implementation specific to 
	// derived classes.
	// The return value is one of the standard error codes:
	// 	KErrNone
	// 	KErrNoMemory
	//	KErrNotFound
	virtual TInt	AddData( const TDesC8 &aSource ) = 0;

	// Retreives data from this data consumer and copies it to
	// the supplied buffer.
	// The return value is one of the standard error codes:
	// 	KErrNone
	// 	KErrNoMemory
	//	KErrNotFound
	virtual TInt	GetData( HBufC8 &aDestination ) = 0;

	// Retreives data from the data consumer as a
	// descriptor.  The data is not copied.
	virtual operator const TDesC8&( void ) const = 0;

	// Retreives data from this data consumer and copies it to
	// the supplied buffer.
	// The return value is one of the standard error codes:
	// 	KErrNone
	// 	KErrNoMemory
	//	KErrNotFound
	virtual TInt	SaveData( const TDesC &filePath ) = 0;

protected:
	MDataConsumer( void ) {;}
	virtual ~MDataConsumer( ) {;}
};

/********************************************************************************
 *
 * MNotifyLogMessage - callback interface to allow callers to log messages
 *
 ********************************************************************************/
class MNotifyLogMessage
{
public:
	virtual TInt Init(RFs &fsSession, TPtrC16 filename, CConsoleBase* console) = 0;
	virtual void Msg(TPtrC16 text, ...) = 0;
	virtual TBool IsInitialised() const = 0;
	virtual void CloseFile() = 0;
};

/********************************************************************************
 *
 * MNotifyStatEngine - callback interface given to transport on engine
 *
 ********************************************************************************/
class MNotifyStatEngine
{
public:
	virtual void HandleInitialise( TInt aResult ) = 0;
	virtual void HandleConnect( TInt aResult ) = 0;
	virtual void HandleSend( TInt aResult ) = 0;
	virtual void HandleReceive( TInt aResult, const TUint aCommand,
						MDataConsumer *const aDataConsumer ) = 0;
	virtual void HandleDisconnect( TInt aResult ) = 0;
	virtual void HandleRelease( TInt aResult ) = 0;
	virtual void HandleError( TInt aError, void *aErrorData ) = 0;
	virtual void HandleInfo( const TDesC *aInfo ) = 0;
	
};

/********************************************************************************
 *
 * MNotifyStatController - callback interface given to engine on controller
 *
 ********************************************************************************/
class MNotifyStatController
{
public:
	virtual void HandleStatusChange( TCommStatus aNewStatus ) = 0;
	virtual void HandleError( TInt aError, void *aErrorData ) = 0;
	virtual void HandleInfo( const TDesC *aInfo ) = 0;
};

/********************************************************************************
 *
 * MNotifyUI - callback interface given to the controller on the UI
 *
 ********************************************************************************/
class MNotifyUI
{
public:
	virtual void HandleStatusChange( TInt aSessionId, TCommStatus aNewStatus ) = 0;
	virtual void HandleError( TInt aError, void *aErrorData ) = 0;
	virtual void HandleInfo( const TDesC *aInfo ) = 0;
};

/********************************************************************************
 *
 * MStatApiNetwork - network interface called by transport layer
 *
 ********************************************************************************/
class MStatNetwork
{
public:
	virtual TInt GetPacketSize() = 0;
	virtual TInt InitialiseL( MNotifyStatTransport *transport ) = 0;
	virtual TInt ConnectL( TDesC *aRemoteHost ) = 0;
	virtual TInt RequestSend( TDesC8 *aData, const TUint aLength ) = 0;
	virtual TInt RequestReceive( TUint aByteCount ) = 0;
	virtual TInt Disconnect( void ) = 0;
	virtual TInt Release( void ) = 0;
	virtual TText8 *Error( void ) = 0;
	virtual TUint NtoHl( TUint aValue ) const = 0 ;
	virtual TUint HtoNl( TUint aValue ) const = 0;
};
	
/********************************************************************************
 *
 * MStatApiTransport - transport interface called by engine
 *
 ********************************************************************************/
class MStatApiTransport
{
public:
	virtual TInt InitialiseL() = 0;
	virtual TInt ConnectL( TDesC *aRemoteHost ) = 0;
	virtual TInt RequestSend( const TUint aCommand, MDataSupplier *const aDataSupplier) = 0;
	virtual TInt RequestReceive( void ) = 0;
	virtual TInt Disconnect( void ) = 0;
	virtual TInt Release( void ) = 0;
	virtual TText8 *Error( void ) = 0;
};

/********************************************************************************
 *
 * MStatEngine - engine interface called by controller
 *
 ********************************************************************************/
class MStatEngine
{
public:
	virtual void StartEngine( MStatApiTransport *aStatTransport, TStatConnectType iConnectType, TDesC *aRemoteHost) = 0;
	virtual void StopEngine() = 0;
};

/********************************************************************************
 *
 * MStatController - controller interface to app ui
 *
 ********************************************************************************/
class MStatController
{
public:
	virtual TInt StartSession( TStatConnectType aConnectType, TDesC *aConnectParams, MNotifyUI *aUI, RFs *const aSession, MNotifyLogMessage *const iMsg ) = 0;
	virtual TInt StopSession( TInt aSessionId ) = 0;
	virtual TInt SessionStatus( TInt aSessionId ) = 0;
};

#endif //__MSTATAPITRANSPORT_H__
