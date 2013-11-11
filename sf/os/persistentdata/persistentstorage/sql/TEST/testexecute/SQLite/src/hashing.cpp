// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hashing.h"
#include "sqlfn.h"
#include "cdtest.h"
#include <sqldb.h>

//
// This code is for setting up and accessing hashes. These are used within
// the test harness to associate a word (for example "KErrNone") with an
// integer value (0 in this case). The base class CSQLHashUtil contains the
// accessor and destructor methods which do 99% of the work. All of the
// other classes merely contain constructors to set up their hashes.
//

// Basic functionality first...
TPtrC* CSQLHashUtil::GetStringFromNum(TInt aErrNum)
    {
    return ihash.Find(aErrNum);
    }

TInt CSQLHashUtil::GetNumFromString(TPtrC aErrMsg)
    {
    THashMapIter<TInt, TPtrC> it(ihash);
    it.NextValue();
    while (it.CurrentValue())
        {
        if (*it.CurrentValue() == aErrMsg)
            return *it.CurrentKey();
        it.NextValue();
        }
    return (KErrNotFound);
    }

CSQLHashUtil::~CSQLHashUtil()
    {
    while(ihash.Count())
        {
        THashMapIter<TInt, TPtrC> it(ihash);
        const TInt *curkey = it.NextKey();
        ihash.Remove(*curkey);
        }
    }

// Now setup OS errors..
CSQLOsErrHash::CSQLOsErrHash()
    {
    // error codes from e32err.h
    ihash.Insert(KErrNone, _L("KErrNone"));
    ihash.Insert(KErrNotFound, _L("KErrNotFound"));
    ihash.Insert(KErrGeneral, _L("KErrGeneral"));
    ihash.Insert(KErrCancel, _L("KErrCancel"));
    ihash.Insert(KErrNoMemory, _L("KErrNoMemory"));
    ihash.Insert(KErrNotSupported, _L("KErrNotSupported"));
    ihash.Insert(KErrArgument, _L("KErrArgument"));
    ihash.Insert(KErrTotalLossOfPrecision, _L("KErrTotalLossOfPrecision"));
    ihash.Insert(KErrBadHandle, _L("KErrBadHandle"));
    ihash.Insert(KErrOverflow, _L("KErrOverflow"));
    ihash.Insert(KErrUnderflow, _L("KErrUnderflow"));
    ihash.Insert(KErrAlreadyExists, _L("KErrAlreadyExists"));
    ihash.Insert(KErrPathNotFound, _L("KErrPathNotFound"));
    ihash.Insert(KErrDied, _L("KErrDied"));
    ihash.Insert(KErrInUse, _L("KErrInUse"));
    ihash.Insert(KErrServerTerminated, _L("KErrServerTerminated"));
    ihash.Insert(KErrServerBusy, _L("KErrServerBusy"));
    ihash.Insert(KErrCompletion, _L("KErrCompletion"));
    ihash.Insert(KErrNotReady, _L("KErrNotReady"));
    ihash.Insert(KErrUnknown, _L("KErrUnknown"));
    ihash.Insert(KErrCorrupt, _L("KErrCorrupt"));
    ihash.Insert(KErrAccessDenied, _L("KErrAccessDenied"));
    ihash.Insert(KErrLocked, _L("KErrLocked"));
    ihash.Insert(KErrWrite, _L("KErrWrite"));
    ihash.Insert(KErrDisMounted, _L("KErrDisMounted"));
    ihash.Insert(KErrEof, _L("KErrEof"));
    ihash.Insert(KErrDiskFull, _L("KErrDiskFull"));
    ihash.Insert(KErrBadDriver, _L("KErrBadDriver"));
    ihash.Insert(KErrBadName, _L("KErrBadName"));
    ihash.Insert(KErrCommsLineFail, _L("KErrCommsLineFail"));
    ihash.Insert(KErrCommsFrame, _L("KErrCommsFrame"));
    ihash.Insert(KErrCommsOverrun, _L("KErrCommsOverrun"));
    ihash.Insert(KErrCommsParity, _L("KErrCommsParity"));
    ihash.Insert(KErrTimedOut, _L("KErrTimedOut"));
    ihash.Insert(KErrCouldNotConnect, _L("KErrCouldNotConnect"));
    ihash.Insert(KErrCouldNotDisconnect, _L("KErrCouldNotDisconnect"));
    ihash.Insert(KErrDisconnected, _L("KErrDisconnected"));
    ihash.Insert(KErrBadLibraryEntryPoint, _L("KErrBadLibraryEntryPoint"));
    ihash.Insert(KErrBadDescriptor, _L("KErrBadDescriptor"));
    ihash.Insert(KErrAbort, _L("KErrAbort"));
    ihash.Insert(KErrTooBig, _L("KErrTooBig"));
    ihash.Insert(KErrDivideByZero, _L("KErrDivideByZero"));
    ihash.Insert(KErrBadPower, _L("KErrBadPower"));
    ihash.Insert(KErrDirFull, _L("KErrDirFull"));
    ihash.Insert(KErrHardwareNotAvailable, _L("KErrHardwareNotAvailable"));
    ihash.Insert(KErrSessionClosed, _L("KErrSessionClosed"));
    ihash.Insert(KErrPermissionDenied, _L("KErrPermissionDenied"));
    ihash.Insert(KErrExtensionNotSupported, _L("KErrExtensionNotSupported"));
    TInt err = ihash.Insert(KErrCommsBreak, _L("KErrCommsBreak"));
    if( err == KErrNone )
        return;
    User::Panic(_L("Unable to create OS error hash"), 1411);
    }
    
// Now setup SQL errors..
CSQLErrHash::CSQLErrHash()
    {
    // error codes from sqldb.h
    ihash.Insert(KSqlErrGeneral, _L("KSqlErrGeneral"));
    ihash.Insert(KSqlErrInternal, _L("KSqlErrInternal"));
    ihash.Insert(KSqlErrPermission, _L("KSqlErrPermission"));
    ihash.Insert(KSqlErrAbort, _L("KSqlErrAbort"));
    ihash.Insert(KSqlErrBusy, _L("KSqlErrBusy"));
    ihash.Insert(KSqlErrLocked, _L("KSqlErrLocked"));
    ihash.Insert(KSqlErrReadOnly, _L("KSqlErrReadOnly"));
    ihash.Insert(KSqlErrInterrupt, _L("KSqlErrInterrupt"));
    ihash.Insert(KSqlErrIO, _L("KSqlErrIO"));
    ihash.Insert(KSqlErrCorrupt, _L("KSqlErrCorrupt"));
    ihash.Insert(KSqlErrNotFound, _L("KSqlErrNotFound"));
    ihash.Insert(KSqlErrFull, _L("KSqlErrFull"));
    ihash.Insert(KSqlErrCantOpen, _L("KSqlErrCantOpen"));
    ihash.Insert(KSqlErrProtocol, _L("KSqlErrProtocol"));
    ihash.Insert(KSqlErrEmpty, _L("KSqlErrEmpty"));
    ihash.Insert(KSqlErrSchema, _L("KSqlErrSchema"));
    ihash.Insert(KSqlErrTooBig, _L("KSqlErrTooBig"));
    ihash.Insert(KSqlErrConstraint, _L("KSqlErrConstraint"));
    ihash.Insert(KSqlErrMismatch, _L("KSqlErrMismatch"));
    ihash.Insert(KSqlErrMisuse, _L("KSqlErrMisuse"));
    ihash.Insert(KSqlErrRange, _L("KSqlErrRange"));
    ihash.Insert(KSqlErrNotDb, _L("KSqlErrNotDb"));
    ihash.Insert(KSqlErrStmtExpired, _L("KSqlErrStmtExpired"));
    // The last two aren't strictly errors, but it is convenient to treat
    // them as such to process the output from RSqlStatement::Next().
    ihash.Insert(KSqlAtRow, _L("KSqlAtRow"));
    TInt err = ihash.Insert(KSqlAtEnd, _L("KSqlAtEnd"));
    if( err == KErrNone )
        return;
    User::Panic(_L("Unable to create SQL error hash"), 1412);
    }

// Now do the SQL enumerations.. 
CSQLColTypeHash::CSQLColTypeHash()
    {
    // Enumerations for sqldb.h
    ihash.Insert(ESqlNull, _L("ESqlNull"));
    ihash.Insert(ESqlInt, _L("ESqlInt"));
    ihash.Insert(ESqlInt64, _L("ESqlInt64"));
    ihash.Insert(ESqlReal, _L("ESqlReal"));
    ihash.Insert(ESqlText, _L("ESqlText"));
    ihash.Insert(ESqlBinary, _L("ESqlBinary"));
    TInt err = ihash.Insert(ESqlNull, _L("ESqlNull"));
    if( err == KErrNone )
        return;

    User::Panic(_L("Unable to create column type hash"), 1413);
    }

// Now do the enumerations for all of the functions in sqlfn..
CSQLTEFAction::CSQLTEFAction()
    {
    // Enumerations from sqlfn.h
    ihash.Insert(CSQLFnStep::Efn_undefined, _L("fn_undefined"));
    ihash.Insert(CSQLFnStep::Efn_nop, _L("NoOperation"));
    ihash.Insert(CSQLFnStep::Efn_create, _L("Create"));
    ihash.Insert(CSQLFnStep::Efn_createl, _L("CreateL"));
    ihash.Insert(CSQLFnStep::Efn_createsp, _L("CreateSP"));
    ihash.Insert(CSQLFnStep::Efn_open, _L("Open"));
    ihash.Insert(CSQLFnStep::Efn_openl, _L("OpenL"));
    ihash.Insert(CSQLFnStep::Efn_attach, _L("Attach"));
    ihash.Insert(CSQLFnStep::Efn_detach, _L("Detach"));
    ihash.Insert(CSQLFnStep::Efn_copy, _L("Copy"));
    ihash.Insert(CSQLFnStep::Efn_delete, _L("Delete"));
    ihash.Insert(CSQLFnStep::Efn_close, _L("Close"));
    ihash.Insert(CSQLFnStep::Efn_exec, _L("Exec"));
    ihash.Insert(CSQLFnStep::Efn_setisolationlevel, _L("SetIsolationLevel"));
    ihash.Insert(CSQLFnStep::Efn_lasterrormessage, _L("LastErrorMessage"));
    ihash.Insert(CSQLFnStep::Efn_reservedrivespace, _L("ReserveDriveSpace"));
    ihash.Insert(CSQLFnStep::Efn_freereservedspace, _L("FreeReservedSpace"));
    ihash.Insert(CSQLFnStep::Efn_getreserveaccess, _L("GetReserveAccess"));
    ihash.Insert(CSQLFnStep::Efn_releasereserveaccess, _L("ReleaseReserveAccess"));
    ihash.Insert(CSQLFnStep::Erstmt_prepare, _L("Prepare"));
    ihash.Insert(CSQLFnStep::Erstmt_preparel, _L("PrepareL"));
    ihash.Insert(CSQLFnStep::Erstmt_close, _L("St_Close"));
    ihash.Insert(CSQLFnStep::Erstmt_atrow, _L("AtRow"));
    ihash.Insert(CSQLFnStep::Erstmt_reset, _L("Reset"));
    ihash.Insert(CSQLFnStep::Erstmt_exec, _L("St_Exec"));
    ihash.Insert(CSQLFnStep::Erstmt_next, _L("Next"));
    ihash.Insert(CSQLFnStep::Erstmt_paramindex, _L("ParameterIndex"));
    ihash.Insert(CSQLFnStep::Erstmt_colindex, _L("ColumnIndex"));
    ihash.Insert(CSQLFnStep::Erstmt_coltype, _L("ColumnType"));
    ihash.Insert(CSQLFnStep::Erstmt_colsize, _L("ColumnSize"));
    ihash.Insert(CSQLFnStep::Erstmt_bindnull, _L("BindNull"));
    ihash.Insert(CSQLFnStep::Erstmt_bindint, _L("BindInt"));
    ihash.Insert(CSQLFnStep::Erstmt_bindint64, _L("BindInt64_"));
    ihash.Insert(CSQLFnStep::Erstmt_bindreal, _L("BindReal"));
    ihash.Insert(CSQLFnStep::Erstmt_bindtext, _L("BindText"));
    ihash.Insert(CSQLFnStep::Erstmt_bindbigtext, _L("BindTextFromFile"));
    ihash.Insert(CSQLFnStep::Erstmt_bindbinary, _L("BindBinary"));
    ihash.Insert(CSQLFnStep::Erstmt_isnull, _L("IsNull"));
    ihash.Insert(CSQLFnStep::Erstmt_colint, _L("ColumnInt"));
    ihash.Insert(CSQLFnStep::Erstmt_colint64, _L("ColumnInt64_"));
    ihash.Insert(CSQLFnStep::Erstmt_colreal, _L("ColumnReal"));
    ihash.Insert(CSQLFnStep::Erstmt_coltextL, _L("ColumnTextL"));
    ihash.Insert(CSQLFnStep::Erstmt_coltextP, _L("ColumnTextP"));
    ihash.Insert(CSQLFnStep::Erstmt_coltextD, _L("ColumnTextD"));
    ihash.Insert(CSQLFnStep::Erstmt_colbinL, _L("ColumnBinaryL"));
    ihash.Insert(CSQLFnStep::Erstmt_colbinP, _L("ColumnBinaryP"));
    ihash.Insert(CSQLFnStep::Erstmt_colbinD, _L("ColumnBinaryD"));
    ihash.Insert(CSQLFnStep::Esp_create, _L("SPCreate"));
    ihash.Insert(CSQLFnStep::Esp_createl, _L("SPCreateL"));
    ihash.Insert(CSQLFnStep::Esp_close, _L("SPClose"));
    ihash.Insert(CSQLFnStep::Esp_setdbpolicy, _L("SetDBPolicy"));
    ihash.Insert(CSQLFnStep::Esp_setpolicy, _L("SetPolicy"));
    ihash.Insert(CSQLFnStep::Esp_externalizel, _L("SPExternalizeL"));
    ihash.Insert(CSQLFnStep::Esp_internalizel, _L("SPInternalizeL"));
    ihash.Insert(CSQLFnStep::Estreamwrite_bindtext, _L("StreamWriteBindText"));
    ihash.Insert(CSQLFnStep::Estreamwrite_bindbinary, _L("StreamWriteBindBin"));
    ihash.Insert(CSQLFnStep::Estreamread_columntext, _L("StreamReadColText"));
    ihash.Insert(CSQLFnStep::Estreamread_columnbinary, _L("StreamReadColBin"));
    ihash.Insert(CSQLFnStep::Edefineconfig, _L("DefineConfig"));
    ihash.Insert(CSQLFnStep::Ectrl_newblock, _L("NewBlock"));
    ihash.Insert(CSQLFnStep::Ectrl_function, _L("Function"));
    ihash.Insert(CSQLFnStep::Ectrl_waitA, _L("WaitA"));
    ihash.Insert(CSQLFnStep::Ectrl_signalA, _L("SignalA"));
    ihash.Insert(CSQLFnStep::Ectrl_waitB, _L("WaitB"));
    ihash.Insert(CSQLFnStep::Ectrl_signalB, _L("SignalB"));
    ihash.Insert(CSQLFnStep::Ectrl_sleep, _L("Sleep"));
    ihash.Insert(CSQLFnStep::Ectrl_eightbit, _L("EightBit"));
    ihash.Insert(CSQLFnStep::Ectrl_async, _L("Async"));
    TInt err = ihash.Insert(CSQLFnStep::Ectrl_endblock, _L("EndBlock"));
    if( err == KErrNone )
        return;

    User::Panic(_L("Unable to create action hash"), 1414);
    }

CSQLCapability::CSQLCapability()
	{
    ihash.Insert(ECapabilityTCB, _L("ECapabilityTCB"));
    ihash.Insert(ECapabilityCommDD, _L("ECapabilityCommDD"));
    ihash.Insert(ECapabilityPowerMgmt, _L("ECapabilityPowerMgmt"));
    ihash.Insert(ECapabilityMultimediaDD, _L("ECapabilityMultimediaDD"));
    ihash.Insert(ECapabilityReadDeviceData, _L("ECapabilityReadDeviceData"));
    ihash.Insert(ECapabilityWriteDeviceData, _L("ECapabilityWriteDeviceData"));
    ihash.Insert(ECapabilityDRM, _L("ECapabilityDRM"));
	ihash.Insert(ECapabilityTrustedUI, _L("ECapabilityTrustedUI"));
    ihash.Insert(ECapabilityProtServ, _L("ECapabilityProtServ"));
    ihash.Insert(ECapabilityDiskAdmin, _L("ECapabilityDiskAdmin"));
    ihash.Insert(ECapabilityNetworkControl, _L("ECapabilityNetworkControl"));
    ihash.Insert(ECapabilityAllFiles, _L("ECapabilityAllFiles"));
    ihash.Insert(ECapabilitySwEvent, _L("ECapabilitySwEvent"));
    ihash.Insert(ECapabilityNetworkServices, _L("ECapabilityNetworkServices"));
    ihash.Insert(ECapabilityLocalServices, _L("ECapabilityLocalServices"));
    ihash.Insert(ECapabilityReadUserData, _L("ECapabilityReadUserData"));
    ihash.Insert(ECapabilityWriteUserData, _L("ECapabilityWriteUserData"));
    ihash.Insert(ECapabilityLocation, _L("ECapabilityLocation"));
    ihash.Insert(ECapabilitySurroundingsDD, _L("ECapabilitySurroundingsDD"));
    ihash.Insert(ECapabilityUserEnvironment, _L("ECapabilityUserEnvironment"));
    ihash.Insert(ECapability_Limit, _L("ECapability_Limit"));
    ihash.Insert(ECapability_HardLimit, _L("ECapability_HardLimit"));
    ihash.Insert(ECapability_None, _L("ECapability_None"));
    TInt err = ihash.Insert(ECapability_Denied, _L("ECapability_Denied"));
    if( err == KErrNone )
        return;

    User::Panic(_L("Unable to create capability hash"), 1414);
    }

CSQLPolicy::CSQLPolicy()
	{
	ihash.Insert(RSqlSecurityPolicy::ESchemaPolicy, _L("ESchemaPolicy"));
    ihash.Insert(RSqlSecurityPolicy::EReadPolicy, _L("EReadPolicy"));
    TInt err = ihash.Insert(RSqlSecurityPolicy::EWritePolicy, _L("EWritePolicy"));
    if( err == KErrNone )
        return;

    User::Panic(_L("Unable to create SQL policy hash"), 1414);
	}

CSQLObject::CSQLObject()
	{
	TInt err = ihash.Insert(RSqlSecurityPolicy::ETable, _L("ETable"));
    if( err == KErrNone )
        return;

    User::Panic(_L("Unable to create SQL policy hash"), 1414);
	}

CSQLSFSTEFAction::CSQLSFSTEFAction()
    {
    ihash.Insert(ESFS_SelectIntL, _L("SelectIntL"));
    ihash.Insert(ESFS_SelectInt64L, _L("SelectInt64L"));
    ihash.Insert(ESFS_SelectRealL, _L("SelectRealL"));
    ihash.Insert(ESFS_SelectTextL, _L("SelectTextL"));

    TInt err = ihash.Insert(ESFS_SelectBinaryL, _L("SelectBinaryL"));
    if( err == KErrNone )
        return;

    User::Panic(_L("Unable to create action hash"), 1414);
    }

