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
// It would have been nice to create a new object every time we jump to
// a new config block but reporting (INFO_PRINTF and ERR_PRINTF) doesn't
// work from sub-objects. All in all the structure isn't what I'd like,
// perhaps I'm missing some TEF functionality which would get around this. 
// Various bits of repetition may be removed into utility class(es).
// Some utility methods probably should go into utility class(es).
// Unimplemented 8-bit methods, e.g Exec8.
// 
//

#include "sqlfn.h"
#include "Te_SQL_SuiteDefs.h"
#include "common.h"

// Contains code to perform functions on SQLite databases - what functions
// and in what order is determined by the content of the config (.ini) file.

CSQLFnStep::~CSQLFnStep()
/**
 * Destructor
 */
    {
    // Get rid of the RFs object.. Note this isn't set up in the constructor
    // but in doTestStepL.
    irfs.Close();

    // Get rid of the semaphore objects.
    isemA.Close();
    isemB.Close();

    // Get rid of the hashes. These are originally set up in doTestStepL.
    delete ierrhsh;
    delete icoltypehsh;
    delete iactionhsh;
    delete icaphsh;
    delete ipolhsh;
    delete iobjhsh;

    // Get rid of the config item.
    if(icfg)delete icfg;
    }
CSQLFnStep::CSQLFnStep()
    {
    // Create a global semaphore to be used by all instances of this framework
    // to be used for synchronising separate threads when 'CONCURRENT' is
    // used. If it already exists, then perhaps another thread already has it
    // which is fine - in that case just open it.
    TInt err = isemA.CreateGlobal(_L("SQLiteSemA"), 0);
    if(err == KErrAlreadyExists)
    	{
        err = isemA.OpenGlobal(_L("SQLiteSemA"));
    	}
    if(err != KErrNone)
    	{
		INFO_PRINTF2(_L("Error %d creating semaphore"), err);
		__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
    	}

	//
	// Second semaphore require for DEF140385.
	//
    err = isemB.CreateGlobal(_L("SQLiteSemB"), 0);
    if(err == KErrAlreadyExists)
    	{
        err = isemB.OpenGlobal(_L("SQLiteSemB"));
    	}
    if(err != KErrNone)
    	{
		INFO_PRINTF2(_L("Error %d creating semaphoreB"), err);
		__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
    	}
    }
TVerdict CSQLFnStep::doTestStepPostambleL()
    {
    // Try to make sure that the database and statement resources have been
    // properly closed (in case of problems).
    isqlst.Close();
    isqldb.Close();
    return TestStepResult();
    }
TVerdict CSQLFnStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual. Our implementation only gets called
 * if the base class doTestStepPreambleL() did not leave. That being the case,
 * the current test result value will be EPass.
 */
    {
    // Create the RFs object so we can talk to the file-system when necessary.
    // Moved from the constructor to shut up leavescan.
    User::LeaveIfError(irfs.Connect());
    irfs.ShareProtected();

    // Make sure the database and statement objects get cleaned up..
    CleanupClosePushL(isqldb);
    CleanupClosePushL(isqlst);

    // Make sure that the icfg member is definitely unset when we start.
    icfg = NULL;

    // Get the hashes we use to associate words with numbers (e.g
    // KErrNone with 0). If these fail due to lack of memory they will
    // PANIC, which is fine. If we're that short of memory nothing is
    // going to work anyway.
    ierrhsh = new CSQLErrHash();
    icoltypehsh = new CSQLColTypeHash();
    iactionhsh = new CSQLTEFAction();
    icaphsh = new CSQLCapability();
    ipolhsh = new CSQLPolicy();
    iobjhsh = new CSQLObject();
     
    // Set the test result to PASS to start with and call the main block..
    SetTestStepResult(EPass);    
    SQLDbStepL(ConfigSection());

    // Clean up the database and statement objects.
    CleanupStack::PopAndDestroy(2, &isqldb);

    return TestStepResult();
    }
// This is our 'main' function. It works out what method (e.g RSqlStatement::
// Close) the user wants (based on the configuration file) and then calls
// the appropriate wrapper function which runs the wanted method and reports
// on any unexpected errors.
void CSQLFnStep::SQLDbStepL(const TPtrC& acfgblk)
    {
    _LIT(KTestFunction, "SQLDbStep");

    /*
     * Go through all of the actions defined in the configuration file
     * acting on each. The counter will keep incrementing until we
     * fail to find a config item called 'CreateNN', or 'OpenNN' etc.
     * The two arrays hold Parameter and Column indices for use in
     * any method that needs one. E.G..
     */
    TInt ended=0;

    iasync = i8bit = EFalse;
    for(TInt count=0 ; ; count++)
        {
        TPtrC argument;
        TInt whatfun=Efn_undefined;
        for(TInt i=0 ; i < Efn_undefined ; i++)
            {
            // Construct something like 'ColumnInt37'
            TBuf<KStatementFunMaxLength> stfn(*(iactionhsh->GetStringFromNum(i)));
            stfn.AppendNum(count);

            // Does it exist in the config file? If not try e.g 'ColumnReal37'
            if(!GetStringFromConfig(acfgblk, stfn, argument))
                continue;

            whatfun = i;
            if(whatfun == Ectrl_endblock)
                ended = 1;
            // The GetString was successful, so we drop out anyway.
            break;
            }
        // If we hit an EndBlock marker or couldn't find any keyword with
        // the current counter number then drop out.
        if((whatfun == Efn_undefined) || (whatfun == Ectrl_endblock))
            break;

        // If there's a comma in the argument, split it up. We do
        // this here (rather than, more logically, in the called methods)
        // because we'd end up repeating the 'CommaSeparated' call in
        // all of the wrapper methods. Also, we need the indices for
        // Column and Parameter index resolution.
        TInt arg1, arg2;
        TPtrC arg3;
        CommaSeparated(argument, arg1, arg2);
        CommaSeparated(argument, arg1, arg3);

        TInt err=0;
        switch(whatfun)
            {
                case Efn_nop: break;
            // First the RSqlDatabase methods...
                case Efn_create:
                        Create(argument, acfgblk, count);
                        break;
                case Efn_createl:
                        CreateL_(argument, acfgblk, count);
                        break;
                case Efn_createsp:
                        CreateSP(argument, acfgblk, count);
                        break;
                case Efn_open:
                        Open(argument, acfgblk, count);
                        break;
                case Efn_openl:
                        OpenL_(argument, acfgblk, count);
                        break;
                case Efn_attach:
                        Attach(argument, acfgblk, count);
                        break;
                case Efn_detach:
                        Detach(argument, acfgblk, count);
                        break;
                case Efn_copy:
                        Copy(argument, acfgblk, count);
                        break;
                case Efn_close:
                        Close();
                        break;
                case Efn_delete:
                        Delete(argument, acfgblk, count);
                        break;
                case Efn_lasterrormessage:
                        LastErrorMessage(argument);
                        break;
                case Efn_exec:
                        Exec(argument, acfgblk, count);
                        break;
                case Efn_setisolationlevel:
                        SetIsolationLevel(argument, acfgblk, count);
                        break;
                case Efn_reservedrivespace:
                        ReserveDriveSpace(arg1, acfgblk, count);
                        break;
                case Efn_freereservedspace:
                        FreeReservedSpace();
                        break;
                case Efn_getreserveaccess:
                        GetReserveAccess(acfgblk, count);
                        break;
                case Efn_releasereserveaccess:
                        ReleaseReserveAccess();
                        break;

            // Now the RSqlStatement methods...
                case Erstmt_prepare:
                        Prepare(argument, acfgblk, count);
                        break;
                case Erstmt_preparel:
                        PrepareL_(argument, acfgblk, count);
                        break;
                case Erstmt_close:
                        Close(1);
                        break;
                case Erstmt_atrow:
                        AtRow(argument);
                        break;
                case Erstmt_reset:
                        err = isqlst.Reset();
                        ReportOnError(KTestFunction, _L("Reset"), acfgblk,
                                                                 count, err);
                        break;
                case Erstmt_exec:
                        {
                            TBuf<KConfigItemMaxNameLength> apiname(_L("st_exec"));
                            if(!iasync)
                                err = isqlst.Exec();
                            else
                                {
                                TChar ch = 'A';
                                apiname.Append(ch);
                                TRequestStatus trs;
                                isqlst.Exec(trs);
                                User::WaitForRequest(trs);
                                err = trs.Int();
                                }
                            ReportOnError(KTestFunction, apiname,
                                                        acfgblk, count, err);
                        }
                        break;
                case Erstmt_next:
                        Next(argument, acfgblk, count);
                        break;
                case Erstmt_paramindex:
                            {
                            TInt pidx = ParamIndex(argument, acfgblk, count);
                            // The test designer will have to remember how many
                            // param indices have been stuck in this array..
                            if(pidx >= 0)ipidxs.Append(pidx);
                            }
                        break;
                case Erstmt_colindex:
                            {
                            TInt cidx = ColumnIndex(argument, acfgblk, count);
                            // The test designer will have to remember how many
                            // column indices have been stuck in this array..
                            if(cidx >= 0)icidxs.Append(cidx);
                            }
                        break;
                case Erstmt_coltype:
                        // ColumnType needs the ColumnIndex (the last arg)
                        // and also the expected result, which it will get from
                        // the config file. We have to deal with the ColumnIndex
                        // here because it lives in our scope, not that of the
                        // method we're calling..
                        // The test designer will have to remember how many
                        // column indices have been stuck in this array..
                        ColumnType(icidxs[arg1], arg3);
                        break;
                case Erstmt_colsize:
                        ColumnSize(icidxs[arg1], arg2);
                        break;
                case Erstmt_bindnull:
                        BindNull(ipidxs[arg1], acfgblk, count);
                        break;
                case Erstmt_bindint:
                        BindInt(ipidxs[arg1], arg2, acfgblk, count);
                        break;
                case Erstmt_bindint64:
                        BindInt64(ipidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_bindreal:
                        {
                        TLex tl = arg3;
                        TReal tr;
                        tl.Val(tr);
                        BindReal(ipidxs[arg1], tr, acfgblk, count);
                        }
                        break;
                case Erstmt_bindtext:
                        BindText(ipidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_bindbigtext:
                        // Not an RSqlStatement method, but calls BindText
                        // after reading from a file.
                        BindBigTextL(ipidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_bindbinary:
                        BindBinaryL(ipidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_isnull:
                        IsNull(icidxs[arg1], arg3);
                        break;
                case Erstmt_colint:
                        ColumnInt(icidxs[arg1], arg2);
                        break;
                case Erstmt_colint64:
                        ColumnInt64(icidxs[arg1], arg3);
                        break;
                case Erstmt_colreal:
                        {
                        TLex tl = arg3;
                        TReal tr2;
                        tl.Val(tr2);
                        ColumnReal(icidxs[arg1], tr2);
                        }
                        break;
                case Erstmt_coltextL:
                        ColumnTextL(icidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_coltextP:
                        ColumnTextPL(icidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_coltextD:
                        ColumnTextDL(icidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_colbinL:
                        ColumnBinaryL(icidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_colbinP:
                        ColumnBinaryPL(icidxs[arg1], arg3, acfgblk, count);
                        break;
                case Erstmt_colbinD:
                        ColumnBinaryDL(icidxs[arg1], arg3, acfgblk, count);
                        break;

                case Esp_create:
                        SPCreate(acfgblk, count);
                        break;
                case Esp_createl:
                        SPCreate(argument, acfgblk, count);
                        break;
                case Esp_close:
                        SPClose();
                        break;
                case Esp_setdbpolicy:
                        SPSetDBPolicy(argument, acfgblk, count);
                        break;
                case Esp_setpolicy:
                        SPSetPolicy(argument, acfgblk, count);
                        break;
                case Esp_externalizel:
                        SPExternalize(argument, acfgblk, count);
                        break;
                case Esp_internalizel:
                        SPInternalize(argument, acfgblk, count);
                        break;

                case Estreamwrite_bindtext:
                        SWBindTextL(ipidxs[arg1], arg3, acfgblk, count);
                        break;
                case Estreamwrite_bindbinary:
                        SWBindBinaryL(ipidxs[arg1], arg3, acfgblk, count);
                        break;
                case Estreamread_columntext:
                        SRColumnTextL(icidxs[arg1], arg3, acfgblk, count);
                        break;
                case Estreamread_columnbinary:
                        SRColumnBinaryL(icidxs[arg1], arg3, acfgblk, count);
                        break;
                case Edefineconfig:
                        {
                        if(icfg)
                            delete icfg;
                        TInt len = argument.Length();
                        if(len)
                            {
                            // At the time of writing, configuration strings
                            // are limited to 255 bytes.
                            TBuf8<256> arg;
                            arg.Copy(argument);
                            icfg = new TPtrC8(arg);
                            }
                        else
                            icfg = NULL;
                        }
                        break;

            // Actions that aren't direct method calls..
                case Ectrl_newblock:
                        // Continue executing from another configuration
                        // block. Obviously this restarts the step
                        // counter at zero. It's unfortunate that we can't
                        // create a new test object here, that would be
                        // so much neater. But logging doesn't work in
                        // sub-objects (it could be bodged around but it
                        // really would be a bodge). A shame, we could have
                        // lots of member vars holding all this junk we're
                        // passing around. Note that because we don't create
                        // a new object we are playing with the same
                        // RSqlDatabase and RSqlStatement objects.
                        SQLDbStepL(argument);
                        break;
                case Ectrl_function:
                        // Pure virtual. Lives elsewhere..
                        ResolveTestFunctionL(acfgblk, count, argument);
                        break;

                case Ectrl_waitA:
                        // Wait for the isem member semaphore to receive
                        // arg1 signals for this thread. Obviously this assumes
                        // there's another thread that's going to execute
                        // a 'Signal' at some point.
						for(TInt ii=0 ; ii<arg1 ; ++ii)
                            {
                            WaitA();
                            }
                        break;
                case Ectrl_waitB:
                        // Wait for the isemB member semaphore to receive
                        // arg1 signals for this thread. Obviously this assumes
                        // there's another thread that's going to execute
                        // a 'SignalB' at some point.
						for(TInt ii=0 ; ii<arg1 ; ++ii)
                            {
                            WaitB();
                            }
                        break;
                case Ectrl_signalA:
                        // E.G Signal37=6 to wake up six threads that are
                        // waiting on isem.
                        SignalA(arg1);
                        break;
                case Ectrl_signalB:
                        // E.G SignalB37=6 to wake up six threads that are
                        // waiting on isemB..
                        SignalB(arg1);
                        break;
                case Ectrl_sleep:
                        INFO_PRINTF2(_L("Sleeping for %d microseconds"), arg1);
                        User::After(arg1);
                        break;
                case Ectrl_eightbit:
                        if((argument == _L("On")) || (argument == _L("True")))
                            i8bit = ETrue;
                        else
                            i8bit = EFalse;
                        break;
                case Ectrl_async:
                        if((argument == _L("On")) || (argument == _L("True")))
                            iasync = ETrue;
                        else
                            iasync = EFalse;
                        break;

            // Big problems if this stuff executes.
                case Efn_undefined:
                        SetTestStepResult(EFail);
                        INFO_PRINTF1(HTML_RED);
                        ERR_PRINTF1(_L("This should never happen. A"));
                        INFO_PRINTF1(HTML_COLOUR_OFF);
                        break;
                default:
                        SetTestStepResult(EFail);
                        INFO_PRINTF1(HTML_RED);
                        ERR_PRINTF1(_L("This should never happen. B"));
                        INFO_PRINTF1(HTML_COLOUR_OFF);
                        break;
            }
        // Let's get rid of any colour. Can get tangled in multi-threaded
        // tests.
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    // If we get to this point and the 'ended' flag hasn't been set then
    // we haven't seen an 'EndBlockNN=' line in the config file. That
    // usually means the test has a missing <keyword><number> item
    // which is a test  failure.
    if(!ended)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF2(_L("Put an 'EndBlock' marker at the end of all config blocks. This is intended to\nspot missing numbers (which cause the test to drop out) failing to generate a failure (%S)"), &acfgblk);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    }

// ----------Methods to exercise RSqlDatabase methods ------------------------

TBool CSQLFnStep::Create(const TPtrC& adbnm,
                         const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "Create");
    INFO_PRINTF3(_L("%S: Database name is %S "), &KTestFunction, &adbnm);

    // Try to create the database.
    TInt rc = isqldb.Create(adbnm, icfg);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }
TBool CSQLFnStep::CreateL_(const TPtrC& adbnm,
                           const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "CreateL");
    INFO_PRINTF3(_L("%S: Database name is %S "), &KTestFunction, &adbnm);

    // Try to create the database. Trap any leave (we're actually duplicating
    // what is in the non-leaving Create method, but hey, this is black box
    // testing, we're not supposed to know that..)
    TInt rc=KErrNone;
    TRAP(rc, isqldb.CreateL(adbnm, icfg));
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }

TBool CSQLFnStep::CreateSP(const TPtrC& adbnm, const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "CreateSP");
    INFO_PRINTF3(_L("%S: Database name is %S "), &KTestFunction, &adbnm);

    TInt rc = isqldb.Create(adbnm, isqlsp, icfg);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }
    
TBool CSQLFnStep::Open(const TPtrC& adbnm,
                       const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "Open");
    INFO_PRINTF3(_L("%S: Database name is %S "), &KTestFunction, &adbnm);

    // Try to open the database.
    TInt rc = isqldb.Open(adbnm, icfg);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }
TBool CSQLFnStep::OpenL_(const TPtrC& adbnm,
                         const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "OpenL");
    INFO_PRINTF3(_L("%S: Database name is %S "), &KTestFunction, &adbnm);

    // Try to open the database.
    TInt rc = KErrNone;
    TRAP(rc, isqldb.OpenL(adbnm, icfg));
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }
// Close the current database.
void CSQLFnStep::Close()
    {
//  _LIT(KTestFunction, "Close");
    isqldb.Close();
    return;
    }
void CSQLFnStep::Attach(const TPtrC &arg,
                        const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "Attach");
    TPtrC sqldb, sqldbname;
    // Break arg into 'sqldb', the path/filename of the database to attach
    // and sqldbname, the name by which the database will be referred to
    // through this Attach.
    CommaSeparated(arg, sqldb, sqldbname);
    TInt rc = isqldb.Attach(sqldb, sqldbname);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    return;
    }
void CSQLFnStep::Detach(const TPtrC &arg,
                        const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "Detach");
    TInt rc = isqldb.Detach(arg);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    return;
    }
void CSQLFnStep::Copy(const TPtrC &arg,
                      const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "Copy");
    TPtrC sqldb1, sqldb2;
    CommaSeparated(arg, sqldb1, sqldb2);
    TInt rc = isqldb.Copy(sqldb1, sqldb2);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    return;
    }
TBool CSQLFnStep::Delete(const TPtrC& adbnm,
                         const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "Delete");
    INFO_PRINTF3(_L("%S: Database name is %S "), &KTestFunction, &adbnm);

    // Try to delete the database.
    TInt rc = isqldb.Delete(adbnm);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }
void CSQLFnStep::LastErrorMessage(const TPtrC& arg)
                
    {
    _LIT(KTestFunction, "LastErrorMessage");

    TPtrC lem = isqldb.LastErrorMessage();
    if(arg.Length() == 0)
        {
        INFO_PRINTF3(_L("%S: '%S'"), &KTestFunction, &lem);
        return; // No particular error message was expected, so just return.
        }
    if(lem != arg)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: Expected Message '%S', got '%S'"), &KTestFunction,
                                                                   &arg, &lem);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected Message '%S'"), &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }
void CSQLFnStep::Exec(const TPtrC& arg,
                      const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "Exec");
    TBuf<KConfigItemMaxNameLength> apiname(KTestFunction);
    TInt rc;
    if(i8bit == EFalse)
        {
        if(!iasync)
            rc = isqldb.Exec(arg);
        else
            {
            TChar ch = 'A';
            apiname.Append(ch);
            TRequestStatus trs;
            isqldb.Exec(arg, trs);
            User::WaitForRequest(trs);
            rc = trs.Int();
            }
        }
    else
        {
        apiname.AppendNum(8);
        RBuf8 b8;
        b8.Create(arg.Length());
        b8.Copy(arg);
        if(!iasync)
            rc = isqldb.Exec(b8);
        else
            {
            TChar ch = 'A';
            apiname.Append(ch);
            TRequestStatus trs;
            isqldb.Exec(b8, trs);
            User::WaitForRequest(trs);
            rc = trs.Int();
            }
        b8.Close();
        }
    ReportOnError(KTestFunction, apiname, acfgblk, acnnum, rc);
    return;
    }
void CSQLFnStep::SetIsolationLevel(const TPtrC& arg,
                                   const TDesC &acfgblk, TInt acnnum)
    {
    _LIT(KTestFunction, "SetIsolationLevel");
    // Get the expected error code..
    TPtrC experrS;
    TInt experr = ActionNoToErrEnum(acfgblk, acnnum, experrS);

    INFO_PRINTF2(_L("SetIsolationLevel: %S"), &arg);
    RSqlDatabase::TIsolationLevel sil;
    if(arg == _L("EReadUncommitted"))
        sil = RSqlDatabase::EReadUncommitted;
    else if(arg == _L("EReadCommitted"))
        sil = RSqlDatabase::EReadCommitted;
    else if(arg == _L("ERepeatableRead"))
        sil = RSqlDatabase::ERepeatableRead;
    else if(arg == _L("ESerializable"))
        sil = RSqlDatabase::ESerializable;
    else
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Unrecognized TIsolationLevel '%S'"), 
                                                    &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    TInt rc = isqldb.SetIsolationLevel(sil);
    TPtrC err;
    ErrEnumToString(rc,err);
    if(rc != experr)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("Unexpected SetIsolationLevel error %d/%S"), rc, &err);
        TPtrC lem = isqldb.LastErrorMessage();
        ERR_PRINTF2(_L(" - Last Error Message: %S"), &lem);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    else
        INFO_PRINTF2(_L("SetIsolation level, got error %S as expected"), &err);
    return;
    }

void CSQLFnStep::ReserveDriveSpace(TInt ares,
                                   const TDesC& acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "ReserveDriveSpace");

    // Try to reserve space..
    TInt rc = isqldb.ReserveDriveSpace(ares);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    return;
    }
void CSQLFnStep::FreeReservedSpace()
    {
//    _LIT(KTestFunction, "FreeReservedSpace");
    isqldb.FreeReservedSpace();
    return;
    }
void CSQLFnStep::GetReserveAccess(const TDesC& acfgblk,
                                  const TInt acnnum)
    {
    _LIT(KTestFunction, "GetReserveAccess");

    // Try to reserve space..
    TInt rc = isqldb.GetReserveAccess();
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    return;
    }
void CSQLFnStep::ReleaseReserveAccess()
    {
//    _LIT(KTestFunction, "ReleaseReserveSpace");
    isqldb.ReleaseReserveAccess();
    return;
    }

// ----------Methods to exercise RSqlStatement methods ------------------------
//
// Execute a Close on the current RSqlStatement. This also clears out the
// arrays of RBufs and RBuf8s which are used for BindText and
// BindBinary (just a way of keeping the buffers in scope until the 
// Exec/Close happens) and loses all of the ParameterIndex and ColumnIndex's.
// The 'TInt' argument is just to differentiate between the RSqlDatabase
// Close wrapper, and this RSqlStatement Close wrapper.
void CSQLFnStep::Close(TInt)
    {
//    _LIT(KTestFunction, "St_Close");

    // Close the RSqlStatement.
    isqlst.Close();

    // Empty the arrays where we keep references to BindXXX buffers,
    // closing those buffers as we go.
    for(TInt count = iBindRBufarr.Count() - 1 ; count >= 0; count--)
        {
        iBindRBufarr[count].Close();
        iBindRBufarr.Remove(count);
        }
    for(TInt count = iBindRBuf8arr.Count() - 1 ; count >= 0 ; count--)
        {
        iBindRBuf8arr[count].Close();
        iBindRBuf8arr.Remove(count);
        }
    if((iBindRBuf8arr.Count() != 0) || (iBindRBufarr.Count() != 0))
        {
        User::Panic(_L("RBuf arrays not empty"), 512);
        }

    // Empty the ParameterIndex and ColumnIndex arrays.
    while(ipidxs.Count()) ipidxs.Remove(0);
    while(icidxs.Count()) icidxs.Remove(0);

    return;
    }
void CSQLFnStep::Next(TPtrC& arg,
                      const TDesC &acfgblk, TInt acnnum=-1)
    {
    _LIT(KTestFunction, "Next");
    TInt rc = isqlst.Next();

    // If arg is not zero length it will be KSqlAtEnd/KSqlAtRow, turn that
    // into the enumeration.
    if(arg.Length())
        {
        TInt expn = ErrStringToEnum(arg);
        TPtrC errS;
        ErrEnumToString(rc, errS); // Convert the actual rc to a string.
        if(expn != rc)
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF7(_L("%S/%S: Got %S/%d, expected %S/%d"), &KTestFunction,
                                    &acfgblk, &errS, rc, &arg, expn );
            INFO_PRINTF1(HTML_COLOUR_OFF);
            }
        }
    ReportOnError(KTestFunction, _L("Next"), acfgblk, acnnum, rc);
    return;
    }

// Call the RSqlStatement method 'AtRow'. This returns a boolean. This
// method expects the config file to contain a line resembling
// 'AtRow57=false'. The return value is checked against the config
// value and error/info is reported.
void CSQLFnStep::AtRow(const TPtrC &arg)

    {
    _LIT(KTestFunction, "AtRow");
    TBuf<8> atrres(arg);
    atrres.LowerCase();
    TBool expected = EFalse;
    if(atrres == _L("false"))
        expected = EFalse;
    else if(atrres == _L("true"))
        expected = ETrue;
    else
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: expected config item to be true/false, got %S"),
                      &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    TBool atr = isqlst.AtRow();
    if(atr != expected)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: expected AtRow to return %S, got %d"),
                           &KTestFunction, &atrres, atr);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %S"), &KTestFunction, &atrres);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }
// Call the RSqlStatement method 'Prepare'. This returns an int. This
// method expects the config file to contain a line resembling
// 'Prepare43=Create Table tbl3(f1 etc)'. The return value is checked
// against the expected error (in ReportOnError).
void CSQLFnStep::Prepare(const TPtrC &arg,
                         const TDesC &acfgblk, TInt acnnum=-1)
    {
    _LIT(KTestFunction, "Prepare");
    INFO_PRINTF3(_L("%S: Prepare command is %S"), &KTestFunction, &arg);

    TInt rc;
    if(i8bit == EFalse)
        rc = isqlst.Prepare(isqldb, arg);
    else
        {
        RBuf8 b8;
        b8.Create(arg.Length());
        b8.Copy(arg);
        rc = isqlst.Prepare(isqldb, b8);
        b8.Close();
        }
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    }
void CSQLFnStep::PrepareL_(const TPtrC &arg,
                           const TDesC &acfgblk, TInt acnnum=-1)
    {
    _LIT(KTestFunction, "PrepareL");
    INFO_PRINTF3(_L("%S: PrepareL command is %S"), &KTestFunction, &arg);

    TInt rc=KErrNone;
    if(i8bit == EFalse)
        {
        TRAP(rc, isqlst.PrepareL(isqldb, arg));
        }
    else
        {
        RBuf8 b8;
        b8.Create(arg.Length());
        b8.Copy(arg);
        TRAP(rc, isqlst.PrepareL(isqldb, b8));
        b8.Close();
        }
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    }
// Call the RSqlStatement method 'ParamIndex'. This returns an int. This
// method expects the config file to contain a line resembling
// 'ParamIndex12=:Frog'. The return value is returned.
TInt CSQLFnStep::ParamIndex(const TDesC &arg,
                            const TDesC &acfgblk, TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ParameterIndex");

    // If the test specifies ':?' was the parameter index, we'll assume that
    // ':?' was given in the SELECT (a nameless parameter), this always gives
    // '1' for the paramIndex.
    if(arg == _L(":?"))
        return 1;

    // If arg resembles '23,*explicit*', then return the leading integer.
    // This is so we can call BindEtc with bad values for PANIC testing
    // or otherwise generate a specific parameter index.
    TInt pidx=0;
    TPtrC rhs;
    CommaSeparated(arg, pidx, rhs);
    if(rhs == _L("*explicit*"))
        {
        INFO_PRINTF3(_L("%S: Returning explicit Parameter Index %d"),
                                                      &KTestFunction, pidx);
        return pidx;
        }

    // Ok, run ParameterIndex.
    pidx = isqlst.ParameterIndex(arg);

    // ParameterIndex returns a non-negative integer on success. If the
    // return is negative, we have a problem. We cannot know what the
    // return is if the operation has succeeded, so checking the error
    // code is limited to required errors, i.e < 0.
    // ReportOnError will set test result to failure if the error doesn't
    // match our expected error.
    if(pidx < 0)
        ReportOnError(KTestFunction, _L("ParameterIndex"),
                       acfgblk, acnnum, pidx);
    return pidx;
    }
// Call the RSqlStatement method 'ColumnIndex'. This returns an int. This
// method expects the config file to contain a line resembling
// 'ColumnIndex12=Fld3'. The return value is returned.
TInt CSQLFnStep::ColumnIndex(const TPtrC& arg,
                             const TDesC &acfgblk, TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ColumnIndex");

    // If no column is specified then return zero - this may be necessary
    // if the test is for example counting lines in a table ...
    // >select count(*) from mytbl;
    // In this case obviously there is no namable column, but the Api just
    // requires that the index for ColumnInt will be zero.
    if(arg.Length() == 0) return 0;

    // If arg resembles '23,*explicit*', then return the leading integer.
    // This is so we can call ColumnEtc with bad values for PANIC testing
    // or otherwise generate a specific parameter index.
    TInt colIndex=0;
    TPtrC rhs;
    CommaSeparated(arg, colIndex, rhs);
    if(rhs == _L("*explicit*"))
        {
        INFO_PRINTF3(_L("%S: Returning explicit Column Index %d"),
                                                      &KTestFunction, colIndex);
        return colIndex;
        }
    colIndex = isqlst.ColumnIndex(arg);
    // ColumnIndex returns a non-negative integer on success. If the
    // return is negative, we have a problem. We cannot know what the
    // return is if the operation has succeeded, so checking the error
    // code is limited to required errors, i.e < 0.
    // ReportOnError will set test result to failure if the error doesn't
    // match our expected error.
    if(colIndex < 0)
        ReportOnError(KTestFunction, _L("ColumnIndex"),
                       acfgblk, acnnum, colIndex);
    return colIndex;
    }
// Call the RSqlStatement method 'ColumnType'.
void CSQLFnStep::ColumnType(const TInt &acidx, const TPtrC &aexp)

    {
    _LIT(KTestFunction, "ColumnType");
    TSqlColumnType gottype = isqlst.ColumnType(acidx);
    TPtrC got(SqlColumnTypeToString(gottype));

    if((aexp.Length() ==0) || (StringToSqlColumnType(aexp)==gottype))
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got Column type %S"), &KTestFunction, &got); 
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    // If the expected type hasn't been specified then just display what
    // we've got.
    INFO_PRINTF1(HTML_RED);
    ERR_PRINTF4(_L("%S: Got Column type %S, expected %S"),
                          &KTestFunction, &got, &aexp);
    SetTestStepResult(EFail);
    INFO_PRINTF1(HTML_COLOUR_OFF);
    return;
    }
// Call the RSqlStatement method 'ColumnSize'. Check it against the
// expected size specified in the config file.
void CSQLFnStep::ColumnSize(const TInt& acolidx, const TInt &axexp)
    {
    _LIT(KTestFunction, "ColumnSize");
    TInt csize = isqlst.ColumnSize(acolidx);

    if((axexp != -1) && (axexp != csize))
        {
        INFO_PRINTF1(HTML_RED);
        SetTestStepResult(EFail);
        ERR_PRINTF4(_L("%S: Got Column size %d, expected %d"),
                       &KTestFunction, csize, axexp);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    // If colsize is -1 display what we have.
    INFO_PRINTF3(_L("%S: Got Column size %d"), &KTestFunction, csize); 

    return;
    }
// Onto the Bind methods...
void CSQLFnStep::BindNull(const TInt& apidx,
                          const TDesC &acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "BindNull");
     
    TInt err = isqlst.BindNull(apidx);
    ReportOnError(KTestFunction, _L("BindNull"), acfgblk, acnnum, err);

    return;
    }
void CSQLFnStep::BindInt(const TInt& apidx, const TInt& atob,
                         const TDesC &acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "BindInt");
     
    TInt err = isqlst.BindInt(apidx, atob);
    ReportOnError(KTestFunction, _L("BindInt"), acfgblk, acnnum, err);

    return;
    }
void CSQLFnStep::BindInt64(const TInt& apidx, const TPtrC& atob,
                           const TDesC &acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "BindInt64");
    TInt64 bind;
    TLex tl = atob;
    tl.Val(bind);
    TInt err = isqlst.BindInt64(apidx, bind);
    ReportOnError(KTestFunction, _L("BindInt64"), acfgblk, acnnum, err);

    return;
    }
void CSQLFnStep::BindReal(const TInt& apidx, const TReal& areal,
                          const TDesC &acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "BindReal");
    TInt err = isqlst.BindReal(apidx, areal);
    ReportOnError(KTestFunction, _L("BindReal"), acfgblk, acnnum, err);

    return;
    }
// BindText from the config line...
// May be modified to return a ref which we can keep on the
// stack in the main loop. Then, when we hit a 'Next' that can be cleared.
// This is necessary because there are scoping problems with text and
// binarys - the SQL code expects buffers to remain in scope until
// the Exec/Next.
void CSQLFnStep::BindText(const TInt& apidx, const TPtrC& atxt,
                          const TDesC &acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "BindText");
    TInt err = isqlst.BindText(apidx, atxt);
    ReportOnError(KTestFunction, _L("BindText"), acfgblk, acnnum, err);

    return;
    }
// An additional method to let us bind more than one line of text from
// a config file...
// If the Bind is successful the buffer which has been bound is appended
// to the 'iBindRBufarr' array. This is necessary to keep it in scope until
// the 'Next'/'Exec' actions.
void CSQLFnStep::BindBigTextL(const TInt& apidx, const TPtrC& arg,
                              const TDesC &acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "BindBigText");
    RBuf buf;

    TInt fsize = FileSize(arg);
    if(fsize < 0) return;  // This will have reported an error if necessary.
    
#ifdef _UNICODE
    fsize >>= 1;  // We're assuming here that one character is two bytes..
#endif

    // Create a buffer big enough for the text in the file.
    TInt err = buf.Create(fsize);
    if(err != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Can't allocate file buffer %S"), &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    // Don't push buf onto the cleanup stack. We'll keep a reference to it
    // which get removed on RSqlStatement::Reset.

    // Use an RFileReadStream because we need to worry about characters,
    // not just bytes.
    RFileReadStream rflrs;
    if(rflrs.Open(irfs, arg, EFileRead) != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Can't open file %S"), &KTestFunction, &arg);
        buf.Close();
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    CleanupClosePushL(rflrs);
    rflrs.ReadL(buf);

    // Do the bind...
    err = isqlst.BindText(apidx, buf);
    ReportOnError(KTestFunction, _L("BindText"), acfgblk, acnnum, err);
    // Drop out if it failed.
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &rflrs);
        buf.Close();
        return;
        }
    CleanupStack::PopAndDestroy(1, &rflrs);

    // Tack the buffer onto the internal array. This keeps it in 
    // scope until a RSqlStatement::Close is performed when it will get
    // destroyed.
    iBindRBufarr.Append(buf);

    return;
    }
// If the Bind is successful the buffer which has been bound is appended
// to the 'iBindRBuf8arr' array. This is necessary to keep it in scope until
// the 'Next'/'Exec' actions.
void CSQLFnStep::BindBinaryL(const TInt& apidx, const TDesC& arg,
                              const TDesC &acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "BindBinary");
    TInt fsize = FileSize(arg);
    if(fsize < 0)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Can't find file %S"), &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    // Create the buffer we're going to bind from. We'll keep a reference
    // to this so it doesn't go out of scope (it mustn't
    // until Exec/Reset) so don't put it on the cleanup stack.
    RBuf8 ap;
    TInt err = ap.Create(fsize);
    ReportOnError(KTestFunction, _L("BufferCreate"), acfgblk, acnnum, err);
    if(err != KErrNone) return;

    // Now open the file specified in the argument.
    RFile file;
    TFileName fn = arg;
    err = file.Open(irfs, fn, 0);
    ReportOnError(KTestFunction, _L("FileOpen"), acfgblk, acnnum, err);
    if(err != KErrNone)
        {
        ap.Close();
        return;
        }
    CleanupClosePushL(file);

    // Attempt to read from the file.
    err = file.Read(ap, fsize);
    ReportOnError(KTestFunction, _L("FileRead"), acfgblk, acnnum, err);
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &file);
        ap.Close();
        return;
        }

    // Do the bind...
    err = isqlst.BindBinary(apidx, ap);
    ReportOnError(KTestFunction, _L("BindBinary"), acfgblk, acnnum, err);

    // Drop out if it failed.
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &file);
        ap.Close();
        return;
        }

    CleanupStack::PopAndDestroy(1, &file);

    // Ok things seemed to have worked. Tack the buffer onto our internal
    // RBuf8 array to keep it in scope. It will finally get trashed when
    // we do the next RSqlStatement::Reset.
    iBindRBuf8arr.Append(ap);

    return;
    }
void CSQLFnStep::IsNull(const TInt& apidx, const TPtrC& atxt)
    {
    _LIT(KTestFunction, "IsNull");
    TBuf<8> isn(atxt);
    isn.LowerCase();
    TBool expected;
    if(isn == _L("false"))
        expected = EFalse;
    else if(isn == _L("true"))
        expected = ETrue;
    else
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: expected argument item to be true/false, got %S"),
                     &KTestFunction, &atxt);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    if(isqlst.IsNull(apidx) != expected)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: expected IsNull to return %S"), &KTestFunction,
                            &atxt);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %S"), &KTestFunction, &atxt);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }
void CSQLFnStep::ColumnInt(const TInt& acidx, const TInt& aint)
    {
    _LIT(KTestFunction, "ColumnInt");

    TInt got = isqlst.ColumnInt(acidx);
    if(got != aint)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: expected ColumnInt to return %d, got %d"),
                           &KTestFunction, aint, got);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %d"), &KTestFunction,
                           got);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }
void CSQLFnStep::ColumnInt64(const TInt& acidx, const TPtrC& aintS)
    {
    _LIT(KTestFunction, "ColumnInt64");

    TLex tl(aintS);
    TInt64 aint;
    tl.Val(aint);
    TInt64 got = isqlst.ColumnInt64(acidx);
    if(got != aint)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: expected ColumnInt to return %S"),
                                   &KTestFunction, &aint);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %ld"), &KTestFunction,
                           got);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }
void CSQLFnStep::ColumnReal(const TInt& acidx, const TReal& areal)
    {
    _LIT(KTestFunction, "ColumnReal");

    TReal got = isqlst.ColumnReal(acidx);
    if(got != areal)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: expected ColumnReal to return %f, got %f"),
                           &KTestFunction, areal, got);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %f"), &KTestFunction, got);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }
void CSQLFnStep::ColumnTextL(const TInt& acidx, const TPtrC& atxt,
                             const TDesC &acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ColumnTextL");

    TPtrC got = isqlst.ColumnTextL(acidx);
    TInt err;

    // First the simplest, does the text match the config text?
    if(got == atxt)
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %S"), &KTestFunction,
                           &got);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    // Perhaps 'atxt' is a file, CompareTextAgainstFile will
    // return KErrNotFound if it can't find the file.
    if((err = CompareTextAgainstFileL(got, atxt)) == KErrNone)
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Text match with file %S"), &KTestFunction,
                           &atxt);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    ReportOnError(KTestFunction, _L("ColumnTextL"), acfgblk, acnnum, err);

    return;
    }
void CSQLFnStep::ColumnTextPL(const TInt& acidx, const TPtrC &arg,
                              const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ColumnTextP");

    TPtrC got;
    TInt err = isqlst.ColumnText(acidx, got);
    ReportOnError(KTestFunction, _L("ColumnTextP"), acfgblk, acnnum, err);

    // First the simplest, does the text match the config text?
    if(got == arg)
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %S"), &KTestFunction,
                           &got);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    // Perhaps 'arg' is a file, CompareTextAgainstFile will
    // return KErrNotFound if it can't find the file.
    if((err = CompareTextAgainstFileL(got, arg)) == KErrNone)
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Text match with file %S"), &KTestFunction,
                           &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    ReportOnError(KTestFunction, _L("ColumnTextP"), acfgblk, acnnum, err);
    }
void CSQLFnStep::ColumnTextDL(const TInt& acidx, const TPtrC &arg,
                              const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ColumnTextD");
// Masses of duplication.. We should have a common method to get around
// this, but perhaps when time permits..

    // How big is this item? This is measured in bytes, not characters.
    TInt colsize = isqlst.ColumnSize(acidx);

    // Allocate a buffer.
    RBuf buf;
    TInt err = buf.Create(colsize);
    INFO_PRINTF3(_L("%S: colsize %d"), &KTestFunction, colsize);
    if(err != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Failed to allocate %d"), &KTestFunction,
                           colsize);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    CleanupClosePushL(buf);

    // Call ColumnText(TInt aColumnIndex, TDes& aDest);
    err = isqlst.ColumnText(acidx, buf);
    ReportOnError(KTestFunction, _L("ColumnTextD"), acfgblk, acnnum, err);
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &buf);
        return;
        }

    // First the simplest, does the text match the config text?
    if(buf == arg)
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Got expected result, %S"), &KTestFunction,
                           &buf);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        CleanupStack::PopAndDestroy(1, &buf);
        return;
        }

    // Perhaps 'arg' is a file, CompareTextAgainstFile will
    // return KErrNotFound if it can't find the file.
    if((err = CompareTextAgainstFileL(buf, arg)) == KErrNone)
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Text match with file %S"), &KTestFunction,
                           &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        CleanupStack::PopAndDestroy(1, &buf);
        return;
        }

    ReportOnError(KTestFunction, _L("ColumnTextD"), acfgblk, acnnum, err);
    CleanupStack::PopAndDestroy(1, &buf);
    return;
    }
void CSQLFnStep::ColumnBinaryL(const TInt& acidx, const TPtrC &arg,
                               const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ColumnBinaryL");

    // Get the output from ColumnBinary.
    TPtrC8 colb = isqlst.ColumnBinaryL(acidx);
    INFO_PRINTF3(_L("%S: Got length %d"), &KTestFunction, colb.Length());
    // If both are zero length, then we're expected nothing, which is a
    // reasonable possibility.
    if((colb.Length() == 0) && (arg.Length() == 0))
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF2(_L("%S: Got expected empty buffer."), &KTestFunction);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    // Compare ColumnBinary return against a file.
    TInt err = CompareBinaryAgainstFileL(colb, arg);
    ReportOnError(KTestFunction, _L("ColumnBinaryL"), acfgblk, acnnum, err);

    return;
    }
void CSQLFnStep::ColumnBinaryPL(const TInt& acidx, const TPtrC &arg,
                                const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ColumnBinaryP");

    TInt csize = isqlst.ColumnSize(acidx);
    INFO_PRINTF3(_L("%S: Colsize %d"), &KTestFunction, csize);
#ifdef _UNICODE
    if(isqlst.ColumnType(acidx) == ESqlText) csize <<= 1;
#endif

    RBuf8 data;
    TInt err;
    if((err = data.Create(csize)) != KErrNone)
        {
        ReportOnError(KTestFunction, _L("Createbuf"), acfgblk, acnnum, err);
        return;
        }
    CleanupClosePushL(data);
    err = isqlst.ColumnBinary(acidx, data);
    ReportOnError(KTestFunction, _L("ColumnBinaryP"), acfgblk, acnnum, err);
    if((data.Length()==0) && (arg.Length()==0))
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF2(_L("%S: Got expected empty buffer."), &KTestFunction);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        CleanupStack::PopAndDestroy(1, &data);
        return;
        }
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &data);
           return;
        }

    // Compare ColumnBinary return against a file.
    err = CompareBinaryAgainstFileL(data, arg);
    ReportOnError(KTestFunction, _L("FileCompare"), acfgblk, acnnum, err);
    CleanupStack::PopAndDestroy(1, &data);
    return;
    }
void CSQLFnStep::ColumnBinaryDL(const TInt& acidx, const TPtrC &arg,
                                const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "ColumnBinaryD");

    // How big is this item?
    TInt colsize = isqlst.ColumnSize(acidx);
    INFO_PRINTF3(_L("%S: colsize %d"), &KTestFunction, colsize);
#ifdef _UNICODE
    if(isqlst.ColumnType(acidx) == ESqlText) colsize <<= 1;
#endif

    // Allocate a buffer.
    RBuf8 buf;
    TInt err = buf.Create(colsize);
    if(err != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Failed to allocate %d bytes"), &KTestFunction,
                           colsize);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    CleanupClosePushL(buf);

    // Call ColumnBinary(TInt aColumnIndex, TDes8& aDest);
    err = isqlst.ColumnBinary(acidx, buf);
    ReportOnError(KTestFunction, _L("ColumnBinaryD"), acfgblk, acnnum, err);
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &buf);
        return;
        }
    if((buf.Length()==0) && (arg.Length()==0))
        {
        CleanupStack::PopAndDestroy(1, &buf);
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF2(_L("%S: Got expected empty buffer."), &KTestFunction);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    // Compare ColumnBinary return against a file.
    err = CompareBinaryAgainstFileL(buf, arg);
    ReportOnError(KTestFunction, _L("ColumnBinaryD"), acfgblk, acnnum, err);
    CleanupStack::PopAndDestroy(1, &buf);

    return;
    }

// The following four methods, SWBindText, SWBindBinary, SRColumnText and
// SRColumnBinary are Stream-Write and Stream-Read methods.
// In each case 'arg' specifies a file which is opened as a source (SW)
// of data, or else as another stream (SR) to compare against.
void CSQLFnStep::SWBindTextL(const TInt& apidx, const TPtrC &arg,
                             const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "SWBindText");
    RSqlParamWriteStream sqlw;
    // Get the WriteStream to stuff data down..
    TInt err = sqlw.BindText(isqlst, apidx);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, err);
    if(err != KErrNone) return;
    WriteFileToStreamL(sqlw, arg);
    sqlw.Close();
    return;
    }
void CSQLFnStep::SWBindBinaryL(const TInt& apidx, const TPtrC &arg,
                               const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "SWBindBinary");
    RSqlParamWriteStream sqlw;
    // Get the WriteStream to stuff data down..
    TInt err = sqlw.BindBinary(isqlst, apidx);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, err);
    if(err != KErrNone) return;
    WriteFileToStreamL(sqlw, arg);
    sqlw.Close();
    return;
    }
void CSQLFnStep::SRColumnTextL(const TInt& acidx, const TPtrC &arg,
                              const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "SRColumnText");

    // First find out how much data is in this cell..
    TInt dsize = isqlst.ColumnSize(acidx);
    INFO_PRINTF3(_L("%S: ColumnSize is %d"), &KTestFunction, dsize);

    RSqlColumnReadStream sqlr;
    TInt err = sqlr.ColumnText(isqlst, acidx);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, err);
    if(err != KErrNone) return;
    // Ok, we have a Read Stream..
    CleanupClosePushL(sqlr);

    // Compare it..
    TInt rc = CompareTextStreamAgainstFileL(sqlr, dsize, arg);
    if(rc)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Stream comparison failure, file %S"),
                           &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Stream comparison success, file %S"),
                           &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    CleanupStack::PopAndDestroy(1,&sqlr);
    return;
    }
void CSQLFnStep::SRColumnBinaryL(const TInt& acidx, const TPtrC &arg,
                                 const TDesC& acfgblk, const TInt acnnum=-1)
    {
    _LIT(KTestFunction, "SRColumnBinary");

    // First find out how much data is in this cell..
    TInt dsize = isqlst.ColumnSize(acidx);
    INFO_PRINTF3(_L("%S: Colsize is %d"), &KTestFunction, dsize);
#ifdef _UNICODE
    if(isqlst.ColumnType(acidx) == ESqlText) dsize <<= 1;
#endif

    // Get our RReadStream and check for errors.
    RSqlColumnReadStream sqlr;
    TInt err = sqlr.ColumnBinary(isqlst, acidx);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, err);
    if(err != KErrNone) return;
    // Ok, we have a Read Stream..
    CleanupClosePushL(sqlr);

    // Compare it..
    TInt rc = CompareBinaryStreamAgainstFileL(sqlr, dsize, arg);
    if(rc)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Stream comparison failure, file %S"),
                           &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF3(_L("%S: Stream comparison success, file %S"),
                           &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    CleanupStack::PopAndDestroy(1,&sqlr);
    return;
    }

TBool CSQLFnStep::SPCreate(const TDesC &acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "SPCreate");
    
    TSecurityPolicy defaultPolicy;
    
    // Try to create the SQLDB security policy.
    TInt rc = isqlsp.Create(defaultPolicy);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }

TBool CSQLFnStep::SPCreate(const TPtrC&, const TDesC &acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "SPCreate");
    
    TSecurityPolicy defaultPolicy;
    
    // Try to create the SQLDB security policy.
    TInt rc = KErrNone;
    TRAP(rc, isqlsp.CreateL(defaultPolicy));
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }

void CSQLFnStep::SPClose()
    {
    isqlsp.Close();
    return;
    }

TBool CSQLFnStep::SPSetDBPolicy(const TPtrC& apol, const TDesC &acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "SPSetDBPolicy");

    //extract the policy level and capability from the argument passed in
    TPtrC level, cap;
    CommaSeparated(apol, level, cap);

    //create the security policy object with the supplied capability
    TSecurityPolicy sp((TCapability)(icaphsh->GetNumFromString(cap)));

    INFO_PRINTF2(_L("SetDBPolicy: %S"), &level);
    INFO_PRINTF2(_L("Capabilities are: %S"), &cap);
    
    TInt rc = isqlsp.SetDbPolicy(((RSqlSecurityPolicy::TPolicyType)ipolhsh->GetNumFromString(level)), sp);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }

TBool CSQLFnStep::SPSetPolicy(const TPtrC& apol, const TDesC &acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "SPSetPolicy");

    //extract the policy level and capability from the argument passed in
    TPtrC arg, arg2, object, name, level, caps;
    CommaSeparated(apol, object, arg);
    CommaSeparated(arg, name, arg2);
    CommaSeparated(arg2, level, caps);

    //create the security policy object with the supplied capability
    TSecurityPolicy sp((TCapability)(icaphsh->GetNumFromString(caps)));

    INFO_PRINTF2(_L("SetPolicy: %S"), &level);
    INFO_PRINTF2(_L("Capabilities are: %S"), &caps);
    INFO_PRINTF2(_L("Object type is: %S"), &object);
    INFO_PRINTF2(_L("Object name is: %S"), &name);
    
    TInt rc = isqlsp.SetPolicy(((RSqlSecurityPolicy::TObjectType)iobjhsh->GetNumFromString(object)), name, ((RSqlSecurityPolicy::TPolicyType)ipolhsh->GetNumFromString(level)), sp);
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    if(rc == KErrNone) return ETrue;
    return EFalse;
    }

void CSQLFnStep::SPExternalize(const TPtrC &arg, const TDesC &acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "SPExternalize");
    
    RFileWriteStream rfws;

    TInt err = rfws.Create(irfs, arg, EFileStream);
    if(err != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: Can't open file %S, err %d"), &KTestFunction, &arg,
                         err );
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    
    TInt rc = KErrNone;
    TRAP(rc, isqlsp.ExternalizeL(rfws));
    TRAP(rc, rfws.CommitL());
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    rfws.Close();    
    return;
    }

void CSQLFnStep::SPInternalize(const TPtrC &arg, const TDesC &acfgblk, const TInt acnnum)
    {
    _LIT(KTestFunction, "SPInternalize");
    
    RFileReadStream rfrs;
    
    TInt err = rfrs.Open(irfs, arg, EFileStream);
    if(err != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Failed to open stream from file %S"), &KTestFunction, &arg);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    
    TInt rc = KErrNone;
    TRAP(rc, isqlsp.InternalizeL(rfrs));
        
    ReportOnError(KTestFunction, KTestFunction, acfgblk, acnnum, rc);
    rfrs.Close();     
    return;
    }

// - Utility functions... ---  we should remove these into a separate class -- 
//
// Stream functions - should we have a persistent stream? Here it goes
// out of scope once the method completes, but what about a case where for
// example we write to a cell, then write some more later to the same cell?
//
// Loads more duplication..
TInt CSQLFnStep::CompareTextStreamAgainstFileL(RReadStream &as, TInt asiz,
                                               const TPtrC &afile)
    {
    _LIT(KTestFunction, "CompareTextStreamAgainstFile");
    // Get the file size. This is in bytes, so for Unicode will be
    // out by a factor of two. Also, if created with notepad or similar
    // will have a 'FEFF' two byte marker at the start (which SQLite
    // strips when it sees it).
    TInt fsize = FileSize(afile);
    if(fsize < 0) return fsize;;
    // We don't divide asiz by two: This will have originated in ColumnSize
    // which returns the number of characters in a cell.
    TInt textlen = asiz;
#ifdef _UNICODE
    fsize >>= 1;
#endif

    // If fsize is 1 different (2 bytes) from textlen, then we'll expect
    // a unicode marker. If not 1 or zero give up straight away.
    TInt diff = fsize - textlen;
    TInt ucmark = (diff == 1);
    if((diff>1) || (diff<0))
        {
        INFO_PRINTF1(HTML_COLOUR_OFF);
        ERR_PRINTF4(_L("%S Size mismatch. Expected %d, got %d"),
                                                &KTestFunction, fsize, textlen);
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return KTEFSQLSizeError;
        }

    // Open the reference file specified in the argument.
    // Use an RFileReadStream because we need to worry about characters,
    // not just bytes.
    RFileReadStream rflrs;
    TInt err = rflrs.Open(irfs, afile, EFileRead);
    if(err != KErrNone)
        return err;
    CleanupClosePushL(rflrs);

    // Dumps the FEFF marker bytes for unicode files.
    // SQLite does the same for text.
    if(ucmark)
        {
        TInt16 mark = rflrs.ReadInt16L();  // byte order?
        if((mark != (TInt16)0xfeff) && (mark != (TInt16)0xfffe))
            {
            CleanupStack::PopAndDestroy(1, &rflrs);
            return KTEFSQLSizeError;
            }
        }

    // For each 32768 chars in the text buffer...
    const TInt slice = 32768;
    RBuf fbuf, abuf;
    err = fbuf.Create(slice);
    if(err == KErrNone) err = abuf.Create(slice);
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &rflrs);
        return KErrNoMemory;
        }
    CleanupClosePushL(fbuf);
    CleanupClosePushL(abuf);

    TInt rc = KErrNone;
    for(TInt pos=0; pos < textlen ; pos += slice)
        {
        TInt toread = textlen - pos;
        if(toread > slice)
            toread = slice;

        // Read 'toread' characters from the file and from the passed in
        // data.
        as.ReadL(abuf, toread);
        TPtrC txtslice = abuf.Left(toread);
        rflrs.ReadL(fbuf, toread);
        TPtrC fileslice = fbuf.Left(toread);

        // Compare ..
        if (fileslice != txtslice)
            { 
            rc = KTEFSQLSizeError;
            break;
            }
        }
    CleanupStack::PopAndDestroy(3, &rflrs);
    return rc;
    }

// Loads more duplication..
TInt CSQLFnStep::CompareBinaryStreamAgainstFileL(RReadStream &as, TInt asiz,
                                                const TPtrC &afile)
    {
    _LIT(KTestFunction, "CompareBinaryStreamAgainstFile");
    // Get the file size. This is in bytes.
    TInt fsize = FileSize(afile);
    if(fsize < 0) return fsize;;

    // If sizes differ give up immediately.
    if(fsize - asiz)
        {
        INFO_PRINTF1(HTML_COLOUR_OFF);
        ERR_PRINTF4(_L("%S Size mismatch. Expected %d, got %d"),
                                                &KTestFunction, fsize, asiz);
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return KTEFSQLSizeError;
        }

    // Open the reference file specified in the argument.
    // Use an RFileReadStream because we need to worry about characters,
    // not just bytes.
    RFileReadStream rflrs;
    TInt err = rflrs.Open(irfs, afile, EFileRead);
    if(err != KErrNone) return err;
    CleanupClosePushL(rflrs);

    // For each 32768 chars in the text buffer...
    const TInt slice = 32768;
    RBuf8 fbuf, abuf;
    err = fbuf.Create(slice);
    if(err == KErrNone) err = abuf.Create(slice);
    if(err != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &rflrs);
        return err;
        }
    CleanupClosePushL(fbuf);
    CleanupClosePushL(abuf);

    TInt rc = KErrNone;
    for(TInt pos=0; pos < asiz ; pos += slice)
        {
        TInt toread = asiz - pos;
        if(toread > slice)
            toread = slice;

        // Read 'toread' characters from the file and from the passed in
        // data. Do we really need to chop out only the read bits for
        // comparison? Wouldn't comparing fbuf and abuf work?
        rflrs.ReadL(fbuf, toread);
        TPtr8 fslice = fbuf.LeftTPtr(toread);
        as.ReadL(abuf, toread);
        TPtr8 aslice = abuf.LeftTPtr(toread);

        // Compare .. (does this compare only what's been read?)
        if (fslice != aslice)
            { 
            rc = KTEFSQLSizeError;
            break;
            }
        }
    CleanupStack::PopAndDestroy(3, &rflrs);
    return rc;
    }
void CSQLFnStep::WriteFileToStreamL(RWriteStream &as1, const TPtrC &afile)
    {
    _LIT(KTestFunction, "WriteFileToStream");
    // Open the reference file specified in the argument.
    RFileReadStream rflrs;
    TInt err = rflrs.Open(irfs, afile, EFileRead);
    // Do we want to return anything?
    if(err != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Failed to open stream from file %S"),
                           &KTestFunction, &afile);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    CleanupClosePushL(rflrs);
    as1.WriteL(rflrs);
    CleanupStack::PopAndDestroy(1, &rflrs);
    return;
    }

TInt CSQLFnStep::CompareBinaryAgainstFileL(const TDesC8 &abuf,
                                          const TFileName& afile)
    {
    _LIT(KTestFunction, "CompareBinaryAgainstFile");
    // Get the file size.
    TInt fsize = FileSize(afile);
    if(fsize < 0) return fsize;
    // How much binary do we have?
    TInt binlen = abuf.Length();

    INFO_PRINTF4(_L("%S: Filelen %d, Binlen %d"), &KTestFunction, fsize, binlen);

    // If sizes differ drop out immediately.
    if(fsize - binlen)
        return KTEFSQLSizeError;

    // Open the reference file specified in the argument.
    // Use an RFileReadStream because we need to worry about characters,
    // not just bytes.
    RFileReadStream rflrs;
    TInt err = rflrs.Open(irfs, afile, EFileRead);
    if(err != KErrNone)
        return err;
    CleanupClosePushL(rflrs);

    // For each 32768 chars in the text buffer...
    const TInt slice = 32768;
    RBuf8 fbuf;
    fbuf.Create(slice);
    CleanupClosePushL(fbuf);

    TInt rc = KErrNone;
    for(TInt pos=0; pos < binlen ; pos += slice)
        {
        TInt toread = binlen - pos;
        if(toread > slice)
            toread = slice;

        // Read 'toread' bytes from the file and from the passed in
        // data.
        rflrs.ReadL(fbuf, toread);
        TPtrC8 fileslice = fbuf.Left(toread);
        TPtrC8 binslice = abuf.Mid(pos, toread);

        // Compare ..
        if (fileslice != binslice)
            { 
            rc = KTEFSQLSizeError;
            break;
            }
        }

    INFO_PRINTF2(_L("%S: Comparison successful"), &KTestFunction);
    CleanupStack::PopAndDestroy(2, &rflrs);
    return rc;
    }

// Tested and working..
TInt CSQLFnStep::CompareTextAgainstFileL(const TDesC &atxt,
                                        const TFileName& afile)
    {
    // Get the file size. This is in bytes, so for Unicode will be
    // out by a factor of two. Also, if created with notepad or similar
    // will have a 'FEFF' two byte marker at the start (which SQLite
    // strips when it sees it).
    TInt fsize = FileSize(afile);
    if(fsize < 0) return fsize;
#ifdef _UNICODE
    fsize >>= 1;
#endif
    // How much text do we have?
    TInt textlen = atxt.Length();

    // If fsize is 1 different (2 bytes) from textlen, then we'll expect
    // a unicode marker. If not 1 or zero give up straight away.
    TInt diff = fsize - textlen;
    TInt ucmark = (diff == 1);
    if((diff>1) || (diff<0))
        {
        ERR_PRINTF3(_L("FSIZE is %d, textlen is %d"), fsize, textlen);
        return KTEFSQLSizeError;
        }

    // Open the reference file specified in the argument.
    // Use an RFileReadStream because we need to worry about characters,
    // not just bytes.
    RFileReadStream rflrs;
    TInt err = rflrs.Open(irfs, afile, EFileRead);
    if(err != KErrNone)
        return err;
    CleanupClosePushL(rflrs);

    // Dumps the FEFF marker bytes for unicode files.
    // SQLite does the same for text.
    if(ucmark)
        {
        TInt16 mark = rflrs.ReadInt16L();  // byte order?
        if((mark != (TInt16)0xfeff) && (mark != (TInt16)0xfffe))
            {
            CleanupStack::PopAndDestroy(1, &rflrs);
            return KTEFSQLSizeError;
            }
        }

    // For each 32768 chars in the text buffer...
    const TInt slice = 32768;
    RBuf fbuf;
    fbuf.Create(slice);
    CleanupClosePushL(fbuf);

    TInt rc = KErrNone;
    for(TInt pos=0; pos < textlen ; pos += slice)
        {
        TInt toread = textlen - pos;
        if(toread > slice)
            toread = slice;

        // Read 'toread' characters from the file and from the passed in
        // data.
        rflrs.ReadL(fbuf, toread);
        TPtrC txtslice = atxt.Mid(pos, toread);

        // Compare ..
        if (fbuf != txtslice)
            { 
            rc = KTEFSQLSizeError;
            break;
            }
        }
    CleanupStack::PopAndDestroy(2, &rflrs);
    return rc;
    }

// Get the expected error code for the current action. Assume KErrNone if it
// isn't in the config file. We might have (in the config file)
// ExpectedError27=KSqlErrPermission
int CSQLFnStep::ActionNoToErrEnum(const TDesC& acfgsec, const TInt aActionNum,
                                  TPtrC& aes)
    {
    TBuf<KConfigItemMaxNameLength> cnfgerr(_L("ExpectedError"));
    if(aActionNum != -1) cnfgerr.AppendNum(aActionNum);
    if(!GetStringFromConfig(acfgsec, cnfgerr, aes))
        aes.Set(_L("KErrNone"));
    return(ErrStringToEnum(aes));
}
TInt CSQLFnStep::ErrStringToEnum(TPtrC &aerr)
    {
    return(ierrhsh->GetNumFromString(aerr));
    }
void CSQLFnStep::ErrEnumToString(const TInt &aerr, TPtrC &aptrstr)
    {
    aptrstr.Set(*(ierrhsh->GetStringFromNum(aerr)));
    return;
    }
const TPtrC CSQLFnStep::SqlColumnTypeToString(TSqlColumnType &asqltype)
    {
    return *icoltypehsh->GetStringFromNum(asqltype);
    }
TSqlColumnType CSQLFnStep::StringToSqlColumnType(const TDesC &atype)
    {
    return (TSqlColumnType) icoltypehsh->GetNumFromString(atype);
    }
/*
 * A helper function to report on an error. This won't say anything if the
 * error received (aerr) is equal to the error expected as defined in the
 * configuration file.
 */
void CSQLFnStep::ReportOnError(const TDesC &afnnam, const TDesC &apinam,
                                const TDesC &acfgblk, const TInt acfgno,
                                const TInt aerr)
    {
    // Get the expected error.
    TPtrC experrS;
    TInt experr = ActionNoToErrEnum(acfgblk, acfgno, experrS);

    // Some methods such as Exec and Next return a positive value on
    // success. If we're not expecting an error and the actual error code
    // is positive just return, everything is cool.
    if((experr == KErrNone) && (aerr >= 0))
        return;

    // Is the actual error the same as the expected error?
    if(aerr != experr)
        {
        INFO_PRINTF1(HTML_RED);
        SetTestStepResult(EFail);
        TPtrC errS;
        ErrEnumToString(aerr, errS); // Convert the actual error to a string.

        ERR_PRINTF7(_L("%S: %S gave error %d/%S, expected %d/%S"),
                        &afnnam, &apinam, aerr, &errS, experr, &experrS);
        // Run 'LastErrorMessage' if we unexpectedly have 'KSqlErrGeneral',
        // often what it has to say is very helpful.
        if(aerr == KSqlErrGeneral)
            LastErrorMessage(_L(""));

        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    else if(aerr != KErrNone)
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF5(_L("%S: %S got expected error %d/%S"), &afnnam, &apinam,
                              aerr, &experrS);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }
TBool CSQLFnStep::FromConfig(const TDesC &afnnam, const TDesC &acfgblk,
                             const TDesC &acfgname, TPtrC &acfgval)
    {
    if(!GetStringFromConfig(acfgblk, acfgname, acfgval))
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: Failed to get %S:%S parameter."), &afnnam, &acfgblk,
                                                              &acfgname);
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return EFalse;
        }
    return ETrue;
    }
TBool CSQLFnStep::FromConfig(const TDesC &afnnam, const TDesC &acfgblk,
                             const TDesC &acfgname, TInt &acfgval)
    {
    if(!GetIntFromConfig(acfgblk, acfgname, acfgval))
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: Failed to get %S:%S parameter."), &afnnam, &acfgblk,
                           &acfgname);
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return EFalse;
        }
    return ETrue;
    }
TBool CSQLFnStep::FromConfig(const TDesC &afnnam, const TDesC &acfgblk,
                             const TDesC &acfgname, TReal &acfgval)
    {
    TPtrC gotS;
    if(!GetStringFromConfig(acfgblk, acfgname, gotS))
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: Failed to get %S:%S parameter."), &afnnam, &acfgblk,
                                                              &acfgname);
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return EFalse;
        }
    TLex tl = gotS;
    if(tl.Val(acfgval) != KErrNone)
        {
        ERR_PRINTF5(_L("%S:%S Failed to convert %S:%S to real."),
                           &afnnam, &acfgblk, &acfgname, &gotS);
        return EFalse;
        }

    return ETrue;
    }
// Looking for, e.g, "9876,1234" 
void CSQLFnStep::CommaSeparated(const TPtrC& ainp, TInt &aint, TInt &aint2)
    {
//    _LIT(KTestFunction, "CommaSeparated");

    // Read in what kind of binds we'll be doing..
    TInt origlen = ainp.Length();
    TChar comma(',');
    TInt comoffset = ainp.Locate(comma);
    if(comoffset != KErrNotFound)
        {
        TPtrC left = ainp.Left(comoffset);
        TLex tl(left);
        tl.Val(aint);
        TInt rightlen = origlen - comoffset - 1;
        TPtrC right = ainp.Right(rightlen);
        tl = right;
        tl.Val(aint2);
        }
    else
        {
        TLex tl(ainp);
        tl.Val(aint);
        aint2=-1;
        }
    }
// Looking for, e.g, "9876,some words"
void CSQLFnStep::CommaSeparated(const TPtrC& ainp, TInt &aint, TPtrC &astr)
    {
//    _LIT(KTestFunction, "CommaSeparated");

    TInt origlen = ainp.Length();
    TChar comma(',');
    TInt comoffset = ainp.Locate(comma);
    if(comoffset != KErrNotFound)
        {
        TPtrC left = ainp.Left(comoffset);
        TLex tl(left);
        tl.Val(aint);
        TInt rightlen = origlen - comoffset - 1;
        astr.Set(ainp.Right(rightlen));
        }
    else
        {
        TLex tl(ainp);
        tl.Val(aint);
        astr.Set(_L(""));
        }
    }
void CSQLFnStep::CommaSeparated(const TPtrC& ainp, TPtrC &aleft, TPtrC &aright)
    {
//    _LIT(KTestFunction, "CommaSeparated");
    aleft.Set(ainp);
    aright.Set(_L(""));

    TInt origlen = ainp.Length();
    TChar comma(',');
    TInt comoffset = ainp.Locate(comma);
    if(comoffset != KErrNotFound)
        {
        aleft.Set(ainp.Left(comoffset));
        TInt rightlen = origlen - comoffset - 1;
        aright.Set(ainp.Right(rightlen));
        }
    else
        return;
    }
TInt CSQLFnStep::FileSize(const TPtrC &afile)
    {
    // First open the file specified in the argument.
    // This lot gets duplicated a lot.
    RFile file;
    TFileName fn = afile;
    TInt err;
    if((err = file.Open(irfs, fn, 0)) != KErrNone)
        {
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("Cannot open %S to get filesize, err %d"), &afile, err);
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_COLOUR_OFF);

        return err;
        }
    TInt fsize;
    file.Size(fsize);
    file.Close();
    return fsize;
    }

