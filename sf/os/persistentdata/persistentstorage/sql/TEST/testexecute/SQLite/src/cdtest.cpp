// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cdtest.h"
#include "hashing.h"
#include "common.h"
#include<bautils.h>

//Constants taken from SqlSrvConfig.h
#ifdef __WINSCW__
		const TInt KDefaultSoftHeapLimitKb = 1024;
#else
		const TInt KDefaultSoftHeapLimitKb = 8192;
#endif	

// Includes any code required for 'Code-Driven' testing, generally tests
// that cannot be data-driven (or not completely).

CSQLCDT::~CSQLCDT()
    {
    }

CSQLCDT::CSQLCDT()
    {
    SetTestStepName(KSQLCDT);
    }


// Look at 'arg' and call whichever function is required.
void CSQLCDT::ResolveTestFunctionL(const TDesC &acfgblk, const TInt acnnum,
                                  const TPtrC &arg )
    {
    _LIT(KTestFunction, "cdtest::ResolveTestFunction");
    INFO_PRINTF3(_L("In %S, arg is %S"), &KTestFunction, &arg);

    if(arg == _L("WriteBigTable"))
        WriteBigTableL(acfgblk, acnnum);
    else if(arg == _L("ReadBigTable"))
        ReadBigTableL(acfgblk);

    // This assumes that a ParameterIndex and ColumnIndex call has been
    // set up previously - the apidxs/apicxs array access below will
    // certainly PANIC if you haven't done the require preparation..
    else if(arg == _L("CopyCellsUsingStreams"))
        CopyCellsUsingStreamsL(acfgblk);
    else if(arg == _L("WriteIntsToStream"))
        WriteIntsToStream(acfgblk, acnnum);
    else if(arg == _L("NearFillDisk"))
        NearFillDisk(acfgblk);
    else if(arg == _L("ScalarFullSelect"))
        ScalarFullSelectL(acfgblk, acnnum);
    else if(arg == _L("FilesDifferBySize"))
        FilesDifferBySize(acfgblk, acnnum);
    else if(arg == _L("SecurityPolicyCheck"))
        SecurityPolicyCheck(acfgblk, acnnum);
    else if(arg == _L("CollationTest"))
        CollationTest(acfgblk, acnnum);
    
    else User::Panic(_L("Unknown Function"), 42);
    }

// ------------------------------------------------------------------------ 
//
// There's some hardwired nastiness in here - which we might be able to remove
// if we can restructure the code (with the parameter index array as a
// member variable in sqlfn), but until we can get reporting (INFO_PRINTF etc)
// working in new objects that isn't going to happen.
// There should have been a 
// 'Insert into t(what, ever, whatever) values (:FInt, :FReal, :FText)'
// ... before this is called. Those on the right are the nasty hardwiring,
// and should correspond to integer, real, and text fields.
void CSQLCDT::WriteBigTableL(const TDesC &acfgblk, TInt acnnum )
    {
    _LIT(KTestFunction, "WriteBigTable");

    // Parameters for the loop.
    TInt low=1, high=10, step=1, experr = KErrNone;
    TReal mult; TPtrC text, experrS;
    FromConfig(KTestFunction, acfgblk, _L("LowCount"), low);
    FromConfig(KTestFunction, acfgblk, _L("HighCount"), high);
    FromConfig(KTestFunction, acfgblk, _L("CountStep"), step);
    FromConfig(KTestFunction, acfgblk, _L("Multiplier"), mult);
    FromConfig(KTestFunction, acfgblk, _L("Text"), text);
    if( FromConfig(KTestFunction, acfgblk, _L("EventuallyExpectedError"), experrS) )
        experr = ErrStringToEnum(experrS);

    // First work out if out text is actually a filename..
    TInt textfromfile=0;
    textfromfile = BaflUtils::FileExists(irfs, text);

    // Ahoy! Nasty hardwiring ahead!
    TInt pidxi = ParamIndex(_L(":FInt"), acfgblk, 0 );
    TInt pidxr = ParamIndex(_L(":FReal"), acfgblk, 0 );
    TInt pidxt = ParamIndex(_L(":FText"), acfgblk, 0 );

    TInt i;
    for(i=low ; i<=high ; i+=step)
        {
        TInt err=KErrNone;
        // Can use a stream write for speed to write the text to the disk. Don't
        // make the file too big or a memory error (server side) will result.
        if(textfromfile)
            SWBindTextL(pidxt, text, acfgblk, acnnum);
        else
            {
            err = isqlst.BindText(pidxt, text);
            ReportOnError( KTestFunction, _L("BindText"), acfgblk, acnnum, err );
            }

        TReal tr = i * mult;
        err = isqlst.BindInt(pidxi, i);
        ReportOnError( KTestFunction, _L("BindInt"), acfgblk, acnnum, err );
        err = isqlst.BindReal(pidxr, tr);
        ReportOnError( KTestFunction, _L("BindReal"), acfgblk, acnnum, err);

        err = isqlst.Exec();
        if((err != KErrNone) && (err == experr))
            {
            INFO_PRINTF1(HTML_GREEN);
            INFO_PRINTF3(_L("Loop dropped out with expected error %S, i=%d"), &experrS, i );
            INFO_PRINTF1(HTML_COLOUR_OFF);
            break;
            }
        else if(err < 0)  // <0 a real error we weren't expecting.
            {
            ReportOnError( KTestFunction, _L("Exec"), acfgblk, acnnum, err );
            INFO_PRINTF3(_L("%S: counter i is %d"), &KTestFunction, i );
            break;
            }

        err = isqlst.Reset();
        ReportOnError( KTestFunction, _L("Reset"), acfgblk, acnnum, err );
        acnnum++;
        }
    isqlst.Close();

    return;
    }
void CSQLCDT::ReadBigTableL(const TDesC &acfgblk)
    {
    _LIT(KTestFunction, "ReadBigTable");

    // Parameters for the loop.
    TInt low=1, high=10, step=1, err;
    TReal mult; TPtrC text;
    FromConfig(KTestFunction, acfgblk, _L("LowCount"), low);
    FromConfig(KTestFunction, acfgblk, _L("HighCount"), high);
    FromConfig(KTestFunction, acfgblk, _L("CountStep"), step);
    FromConfig(KTestFunction, acfgblk, _L("Multiplier"), mult);
    FromConfig(KTestFunction, acfgblk, _L("Text"), text);

    // First work out if out text is actually a filename..
    TInt textfromfile=0;
    textfromfile = BaflUtils::FileExists(irfs, text);

    // Ahoy! Nasty hardwiring ahead!
    TInt cidxi = ColumnIndex(_L("Someint"), acfgblk, 0 );
    TInt cidxr = ColumnIndex(_L("Somereal"), acfgblk, 0 );
    TInt cidxt = ColumnIndex(_L("Sometext"), acfgblk, 0 );

    for(TInt i=low ; i<=high ; i+=step)
        {
        TReal tr = i * mult;
// INFO_PRINTF3(_L("CFGBLK: %S        COUNT: %d"), &acfgblk, i);

        TInt cint = isqlst.ColumnInt(cidxi);
        TReal creal = isqlst.ColumnReal(cidxr);
        if((cint != i) || (creal != tr))
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF4(_L("%S: ColumnInt gave %d, wanted %d"),
                           &KTestFunction, cint, i );
            ERR_PRINTF4(_L("%S: ColumnReal gave %f, wanted %f"),
                           &KTestFunction, creal, tr );
            INFO_PRINTF1(HTML_COLOUR_OFF);
            break;
            }
        // Now check the text..
        if(textfromfile)
            SRColumnTextL(cidxt, text, acfgblk, -1 );
        else
            ColumnTextL(cidxt, text, acfgblk, -1 );
        if(isqlst.Next()==KSqlAtEnd)
            {
            // This expected error was for *writing* the table - we don't
            // get an error reading it back, just KSqlAtEnd. But lets assume
            // that an expected error on write implies a short table and
            // so no failure if we get KSqlAtEnd early.
            INFO_PRINTF1(HTML_GREEN);
            INFO_PRINTF3(_L("%S: Next gave KSqlAtEnd, i is %d"),
                                                   &KTestFunction, i );
            INFO_PRINTF1(HTML_COLOUR_OFF);
            break;
            }
        }

    if( (err = isqlst.Next()) != KSqlErrMisuse )
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Next gave %d, is there some table left? Expected KSqlErrMisuse"), &KTestFunction, err );
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    return;
    }


// Copy a single cell in a table using streams. We'll use the BindBinary
// and ColumnBinary methods of RSqlParamWriteStream and RSqlColumnReadStream
// respectively. Don't try to copy integers or reals, as the API spec
// says very clearly in a table, converting most of these to Binary
// gives KNullDesc8.
void CSQLCDT::CopyCellsUsingStreamsL(const TDesC &acfgblk) 
    {
    _LIT(KTestFunction, "CopyCellUsingStreams");

    // We only have one RSqlStatement around, and we must have two - a
    // source and destination, so lets get another one - we'll write to
    // this.
    RSqlStatement sqlst2;
    
    // The prepare statement used to add to the second table.
    TPtrC prep;
    FromConfig(KTestFunction, acfgblk, _L("PrepareStatement"), prep);
    // The parameter name (e.g :freddy) in the above.
    TPtrC paramname;
    FromConfig(KTestFunction, acfgblk, _L("ParamName"), paramname);

    // Prepare and get pidx.
    TInt err = sqlst2.Prepare(isqldb, prep);
    TInt pidx = sqlst2.ParameterIndex(paramname);

    // Whilst we're reading 
    while(isqlst.Next() == KSqlAtRow)
        {
        // First lets find a cell to copy. This assumes there is a single
        // column selected..
        // Set up where we're reading from. ColumnIndex will be zero.
        // Obviously a prepare must already have been done.
        RSqlColumnReadStream sqlr;
        err = sqlr.ColumnText(isqlst, 0);
    
        // Read a cell from the database as a stream. Pass that stream to
        // another stream, an RSqlParamWriteStream to copy the cell into
        // another db.
    
        // Get our writable stream..
        RSqlParamWriteStream sqlw;
    
        // Set up where we're writing to.
        err = sqlw.BindText(sqlst2, pidx);
    
        // Write.
        sqlw.WriteL(sqlr);
        sqlw.Close();
        err = sqlst2.Exec();
        err = sqlst2.Reset();
        }
    sqlst2.Close();
    return;
    }

// Write to 32-bit signed integers to a stream (a cell in a table) until
// the write operation Leaves with a KErrNoMem. A 'prepare' statement
// must already have been run, and a parameterindex also. We assume
// that the parameterindex is zero.
// If the user wants to do an Exec and Reset, that's up to them. That
// would end up sticking a NULL into the current cell though.
void CSQLCDT::WriteIntsToStream(const TDesC &acfgblk, const TInt acnnum)
                                   
    {
    _LIT(KTestFunction, "WriteIntsToStream");

    // Get our writable stream..
    RSqlParamWriteStream sqlw;
    // Find out how many integers to write..
    TInt count;
    FromConfig(KTestFunction, acfgblk, _L("Count"), count);
    if(count == -1) count = 2000000000;

    // Assume only one 'ParameterIndex' has been run..
    sqlw.BindBinary(isqlst,0);

    TInt i, leavecode;
    for(i=0 ; i<count ; i++)
        {
        // A fast way to send a lot of data down a stream..
        TRAP(leavecode, sqlw.WriteInt32L(i));
        if(leavecode != KErrNone) break;
        }
    // Close the stream immediately. If we've run out of memory (a common test)
    // this will free it up and allow (e.g) LogEngine to function so we get
    // proper error reports.
    sqlw.Close();

    ReportOnError( KTestFunction, _L("Stream Write"), acfgblk, acnnum, leavecode );
    INFO_PRINTF3(_L("%S bound %d integers"), &KTestFunction, i );

    return;
    }

//This function is needed for drives with more free space than KMaxTUint32 (4GB -1) 
//This is because in a FAT32 file system, the maximum file size is (4GB-1)
//In the case where the free space is larger than this limit, this function will create additional 
//file(s)until the disk is under the (4GB-1) limit. After that CSQLCDT::NearFillDisk will fill the 
//disk up to the required amount.
void CSQLCDT::PrepareLargeDisk(const TDesC& /*acfgblk*/, const TDriveUnit atdu, TInt64 &atowrite)
    {
    _LIT(KTestFunction, "PrepareLargeDisk");
    _LIT(KFillDiskName,"\\FillDisk");
    _LIT(KFillDiskExt,".txt");
    TFileName fname;
    TInt count = 1;
    TInt err = 0;
    
    while(atowrite > KMaxTUint32)
        {
        RFile64 fillDiskFile;
        fname.Copy(atdu.Name());
        fname.Append(KFillDiskName);
        fname.AppendNum(count);
        fname.Append(KFillDiskExt);
        
        if( (err = fillDiskFile.Create(irfs, fname, EFileWrite)) != KErrNone )
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF4(_L("%S: Failed to open RFile64 for file %S, err %d"),
                               &KTestFunction, &fname, err );
            INFO_PRINTF1(HTML_COLOUR_OFF);
            return;
            }
        
        if( (err = fillDiskFile.SetSize(KMaxTUint32)) != KErrNone )
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF4(_L("%S: Failed on RFile64::SetSize for file %S, err %d"),
                               &KTestFunction, &fname, err );
            INFO_PRINTF1(HTML_COLOUR_OFF);
            fillDiskFile.Close();
            return;
            }
        
        fillDiskFile.Close();
        atowrite-=KMaxTUint32;
        count++;
        }
    }

// Create a file specified by 'FillFile' in the config and write to
// it until 'DiskFree' bytes remain. Note that because files use whole
// sectors (512/1024/2048/4096 or whatever bytes), that attempting to leave
// (e.g) 1023 bytes could result in 512 bytes remaining on a file system
// with 512 byte sectors, or zero bytes remaining if the sectors are bigger.
void CSQLCDT::NearFillDisk(const TDesC &acfgblk)
    {
    _LIT(KTestFunction, "NearFillDisk");

    // What file should we use? Requires a full path.
    TPtrC fillfile;
    (void)FromConfig(KTestFunction, acfgblk, _L("FillFile"), fillfile);
    irfs.Delete(fillfile);

    // Get the drive number. This method ignores trailing text.
    // Probably wants upper case.
    TDriveUnit tdu(fillfile);

    // Find out how much disk we want left.. 
    TInt free, err;
    (void)FromConfig(KTestFunction, acfgblk, _L("DiskFree"), free);

    // Find out how much disk space currently remains..
    TVolumeInfo vol;
    if((err = irfs.Volume(vol, tdu )) != KErrNone)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: Failed to get volume info for %S, err %d"),
                           &KTestFunction, &fillfile, err );
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }

    // So how many bytes do we need to write?
    TInt64 towrite = vol.iFree - free;
    INFO_PRINTF4(_L("%S: Disk writing %Ld, free %Ld"), &KTestFunction, 
                    towrite, vol.iFree );
    INFO_PRINTF3(_L("%S: free %Ld"), &KTestFunction, vol.iFree );
    INFO_PRINTF3(_L("%S: writing %Ld"), &KTestFunction, towrite );

    if( towrite < 0 )
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: Disk wanted remaining less than current(%Ld)"),
                           &KTestFunction, vol.iFree );
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    
    //In case the disk is large (i.e >4G -1 bytes) we need another function to 
    //create more files to fill it
    PrepareLargeDisk(acfgblk, tdu, towrite);
    
    // Get a file.
    RFile64 myfile;
    if( (err = myfile.Create(irfs, fillfile, EFileWrite)) != KErrNone )
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: Failed to open RFile64 for file %S, err %d"),
                           &KTestFunction, &fillfile, err );
        INFO_PRINTF1(HTML_COLOUR_OFF);
        return;
        }
    // Write it.

    // We seem to hit trouble if we just try to write 'towrite' bytes, so
    // here we write 50% of them and check the remainder repeatedly until
    // the right amount remains. Actually it is unlikely to be exactly the
    // right amount - depending on sector sizes and other factors the
    // remaining space tends to be a kilobyte or two less than requested.
    // Obviously this is likely to be different between file system types,
    // between hardware and emulator and so on.
    
    
    TInt64 size = 0;
    while(towrite > 0)
        {
        if(towrite < 1024) break;
        TInt64 tow = towrite/2;
        if(towrite < 4096) tow = towrite;
		size += tow;
        if( (err = myfile.SetSize(size)) != KErrNone )
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF4(_L("%S: Failed on RFile64::SetSize for file %S, err %d"),
                               &KTestFunction, &fillfile, err );
            INFO_PRINTF1(HTML_COLOUR_OFF);
			break;
            }
        err = irfs.Volume(vol, tdu );
        if(err != KErrNone)
        	{
            SetTestStepResult(EFail);
            INFO_PRINTF1(HTML_RED);
            ERR_PRINTF3(_L("%S: 2-RFs::Volume() has failed, err=%d"),
                               &KTestFunction, err);
            INFO_PRINTF1(HTML_COLOUR_OFF);
            break;
        	}
        towrite = vol.iFree - free;
        }

    err = irfs.Volume(vol, tdu );
    if(err != KErrNone)
    	{
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF3(_L("%S: 3-RFs::Volume() has failed, err=%d"),
                           &KTestFunction, err);
        INFO_PRINTF1(HTML_COLOUR_OFF);
    	}
    INFO_PRINTF3(_L("%S: Disk remaining is %Ld"), &KTestFunction, vol.iFree );
	myfile.Close();
    return;
    }

// This method exercises the TSqlScalarFullSelectQuery class, which is
// just a wrapper. Its methods expect a select statement whose result will
// be a single cell. Perhaps this should be in sqlfn.cpp.
void CSQLCDT::ScalarFullSelectL(const TDesC &acfgblk, const TInt acnnum)
                                   
    {
    _LIT(KTestFunction, "ScalarFullSelect");

    // Look for an integer called 'Use_SetNN'.
    TInt useset;
    TBuf<KConfigItemMaxNameLength> conS(_L("Use_Set"));
    conS.AppendNum(acnnum);
    FromConfig(KTestFunction, acfgblk, conS, useset);
    // Look for a string called 'MethodNN'.
    TPtrC meth;
    TBuf<KConfigItemMaxNameLength> methS(_L("Method"));
    methS.AppendNum(acnnum);
    FromConfig(KTestFunction, acfgblk, methS, meth);
    // Look for a string called 'StatementNN'.
    TPtrC stmt;
    TBuf<KConfigItemMaxNameLength> stmtS(_L("Statement"));
    stmtS.AppendNum(acnnum);
    FromConfig(KTestFunction, acfgblk, stmtS, stmt);
    // Convert the SQL statement to an UTF-8 version.
    RBuf8 stmt8;
    stmt8.Create(stmt.Length());
    stmt8.Copy(stmt);
    CleanupClosePushL(stmt8);

    // Look for a string called 'ResultNN'.
    TPtrC res;
    TBuf<KConfigItemMaxNameLength> resS(_L("Result"));
    resS.AppendNum(acnnum);
    FromConfig(KTestFunction, acfgblk, resS, res);
    // We may want to convert the expected result to a real or int or something
    TLex conv = res;

    // Ok, now create a TSqlScalarFullSelectQuery object.
    TSqlScalarFullSelectQuery *asfs;
    TSqlScalarFullSelectQuery one(isqldb);
    TSqlScalarFullSelectQuery two;
    two.SetDatabase(isqldb);
    // Use one of the above objects.
    asfs = useset ? &two : &one;

    // Get the action hash for this..
    CSQLSFSTEFAction *cs = new CSQLSFSTEFAction();
    TInt action = cs->GetNumFromString(meth);
    switch(action)
        {
        case CSQLSFSTEFAction::ESFS_SelectIntL:
            {
            TInt actual;
            if(i8bit)
                actual = asfs->SelectIntL(stmt8);
            else
                actual = asfs->SelectIntL(stmt);
				
            TInt expected;
			if (res.CompareF(_L("DEFAULT_SOFT_HEAP_LIMIT")) == 0)
				{
				expected = KDefaultSoftHeapLimitKb;
				}
			else
				{
				conv.Val(expected);
				}
				
            if( actual != expected )
                {
                SetTestStepResult(EFail);
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF4(_L("%S: SelectIntL gave %d, wanted %d"),
                       &KTestFunction, actual,  expected);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            else
                {
                INFO_PRINTF1(HTML_GREEN);
                INFO_PRINTF3(_L("%S: SelectIntL gave %d, as expected"),
                       &KTestFunction, actual);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            }
            break;

        case CSQLSFSTEFAction::ESFS_SelectInt64L:
            {
            TInt64 actual;
            if(i8bit)
                actual = asfs->SelectInt64L(stmt8);
            else
                actual = asfs->SelectInt64L(stmt);
            TInt64 expected;
            conv.Val(expected);
            if( actual != expected )
                {
                SetTestStepResult(EFail);
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF4(_L("%S: SelectInt64L gave %d, wanted %d"),
                       &KTestFunction, actual,  expected);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            else
                {
                INFO_PRINTF1(HTML_GREEN);
                INFO_PRINTF3(_L("%S: SelectInt64L gave %d, as expected"),
                       &KTestFunction, actual);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            }
            break;

        case CSQLSFSTEFAction::ESFS_SelectRealL:
            {
            TReal actual;
            if(i8bit)
                actual = asfs->SelectRealL(stmt8);
            else
                actual = asfs->SelectRealL(stmt);
            TReal expected;
            conv.Val(expected);
            if( actual != expected )
                {
                SetTestStepResult(EFail);
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF4(_L("%S: SelectRealL gave %f, wanted %f"),
                       &KTestFunction, actual,  expected);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            else
                {
                INFO_PRINTF1(HTML_GREEN);
                INFO_PRINTF3(_L("%S: SelectRealL gave %f, as expected"),
                       &KTestFunction, actual);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            }
            break;

        case CSQLSFSTEFAction::ESFS_SelectTextL:
            {
            RBuf actual;
            actual.Create(32768);
            CleanupClosePushL(actual);
            TInt rc = KErrNone;
            if(i8bit)
                rc = asfs->SelectTextL(stmt8, actual);
            else
                rc = asfs->SelectTextL(stmt, actual);
            if( actual != res )
                {
                SetTestStepResult(EFail);
                INFO_PRINTF1(HTML_RED);
                ERR_PRINTF4(_L("%S: SelectTextL gave %S, wanted %S"),
                       &KTestFunction, &actual,  &res);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            else
                {
                INFO_PRINTF1(HTML_GREEN);
                INFO_PRINTF3(_L("%S: SelectTextL gave %S, as expected"),
                       &KTestFunction, &actual);
                INFO_PRINTF1(HTML_COLOUR_OFF);
                }
            ReportOnError( KTestFunction, _L("SelectTextL"), acfgblk, acnnum,
                                                                          rc );
            CleanupStack::PopAndDestroy(1, &actual);
        }
        break;

        case CSQLSFSTEFAction::ESFS_SelectBinaryL:
            {
            RBuf8 actual;
            actual.Create(32768);
            CleanupClosePushL(actual);
            TInt rc = KErrNone;
            if(i8bit)
                rc = asfs->SelectBinaryL(stmt8, actual);
            else
                rc = asfs->SelectBinaryL(stmt, actual);
            ReportOnError( KTestFunction, _L("SelectBinaryL"), acfgblk, acnnum, rc );
            if(!rc)
                {
                TInt rc2 = CompareBinaryAgainstFileL(actual, res);
                if(rc2)
                    {
                    SetTestStepResult(EFail);
                    INFO_PRINTF1(HTML_RED);
                    ERR_PRINTF3(_L("%S: File compare gave error %d"),
                                     &KTestFunction, rc2 );
                    INFO_PRINTF1(HTML_COLOUR_OFF);
                    }
                else
                    {
                    INFO_PRINTF1(HTML_GREEN);
                    _LIT(KSelectBinaryStr, "SelectBinaryL");
                    ERR_PRINTF3(_L("%S: File compare successful, %S"), &KTestFunction, &KSelectBinaryStr);
                    INFO_PRINTF1(HTML_COLOUR_OFF);
                    }

                }
            CleanupStack::PopAndDestroy(1, &actual);
            }
            break;

        default: User::Panic(_L("Unknown Function"), 49);

        }
    CleanupStack::PopAndDestroy(1, &stmt8);
    }

// Verifies that two files differ by size.
void CSQLCDT::FilesDifferBySize(const TDesC &acfgblk, const TInt acnnum)
                                   
    {
    _LIT(KTestFunction, "FilesDifferBySize");

    // Look for a string called 'FileANN'.
    TPtrC filea;
    TBuf<KConfigItemMaxNameLength> fileaS(_L("FileA"));
    fileaS.AppendNum(acnnum);
    FromConfig(KTestFunction, acfgblk, fileaS, filea);
    // Look for a string called 'FileBNN'.
    TPtrC fileb;
    TBuf<KConfigItemMaxNameLength> filebS(_L("FileB"));
    filebS.AppendNum(acnnum);
    FromConfig(KTestFunction, acfgblk, filebS, fileb);

    TInt fza = FileSize(filea);
    TInt fzb = FileSize(fileb);
    if(fza == fzb)
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: File A %d, File B %d"), &KTestFunction, fza,  fzb);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    else
        {
        INFO_PRINTF1(HTML_GREEN);
        INFO_PRINTF6(_L("%S: Files %S and %S differ in size as expected, %d, %d"), &KTestFunction, &fileaS, &filebS, fza, fzb);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    }

// Tests the method that retrive security policies.
void CSQLCDT::SecurityPolicyCheck(const TDesC &/*acfgblk*/, const TInt /*acnnum*/)
    {
//    _LIT(KTestFunction, "SecurityPolicyCheck");
    
    // The methods to be tested here have been tested in the developer test:
    // t_sqlsecurityXX.cpp. The work here has been deferred because it has been 
    // duplicated in the unit tests.
    }

// Tests for Locale change
// This test has been deferred pending a defect fix, defect is:
// DEF091753 "Initialize locale" should be part of the system startup
void CSQLCDT::CollationTest(const TDesC &/*acfgblk*/, const TInt /*acnnum*/)
    {
    TExtendedLocale myExtendedLocale;
    myExtendedLocale.LoadSystemSettings();

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
    
    TBuf<100> DllLanguage; 
    DllLanguage.Copy(_L("elocl_lan.003"));
    
    TBuf<100> DllRegion; 
    DllRegion.Copy(_L("elocl_reg.056"));
    
    TBuf<100> DllCollation; 
    DllCollation.Copy(_L("elocl_col.003"));
    
    // Change the locale 
    TInt err = myExtendedLocale.LoadLocale(DllLanguage, DllRegion, DllCollation);
    
#else
    
    TBuf<100> DllName; 
    DllName.Copy(_L("elocl.sc"));
    
    // Change the locale to Scandinavian Locale 
    TInt err = myExtendedLocale.LoadLocale(DllName);    
    
#endif
    if( err != KErrNone )
        {
        _LIT(KTestFunction, "CollationTest");
        SetTestStepResult(EFail);
        INFO_PRINTF1(HTML_RED);
        ERR_PRINTF4(_L("%S: TExtendedLocale::LoadLocale gave %d, wanted %d"),
                          &KTestFunction, err,  KErrNone);
        INFO_PRINTF1(HTML_COLOUR_OFF);
        }
    
    // Save the changes to system settings
    myExtendedLocale.SaveSystemSettings();

    // Wait
    User::After(1000000);
    }


