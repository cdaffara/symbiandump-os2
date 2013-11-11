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

#if (!defined __SQLFN_H__)
#define __SQLFN_H__
#include <test/testexecutestepbase.h>
#include "Te_SQL_SuiteStepBase.h"
#include "hashing.h"

#include<s32file.h>    // For RFileReadStream etc.

class CSQLFnStep : public CTe_SQL_SuiteStepBase
    {
public:
    CSQLFnStep();
    ~CSQLFnStep();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();
    
    // This big enumeration defines the values associated with particular
    // actions (as read from a config file and converted to the numeric
    // equivalent by the CSQLTEFAction class).
    enum TSqlDbFns { 
            Ectrl_endblock=0, Ectrl_newblock, Ectrl_function,
            Ectrl_waitA, Ectrl_signalA, Ectrl_waitB, Ectrl_signalB,
            Ectrl_sleep, Ectrl_eightbit, Ectrl_async,

            Efn_nop, Efn_create, Efn_createl,
            Efn_createsp,
            Efn_open, Efn_openl,
            Efn_attach, Efn_detach, Efn_copy,
            Efn_delete, Efn_close, Efn_exec,
            Efn_setisolationlevel, Efn_lasterrormessage,
            Efn_reservedrivespace, Efn_freereservedspace, 
            Efn_getreserveaccess, Efn_releasereserveaccess, 

            Erstmt_prepare, Erstmt_preparel, Erstmt_close,
            Erstmt_atrow, Erstmt_reset, Erstmt_exec, Erstmt_next,
            Erstmt_paramindex, Erstmt_colindex, Erstmt_coltype,
            Erstmt_colsize, Erstmt_bindnull, Erstmt_bindint,
            Erstmt_bindint64, Erstmt_bindreal, Erstmt_bindtext,
            Erstmt_bindbigtext, Erstmt_bindbinary, Erstmt_isnull,
            Erstmt_colint, Erstmt_colint64, Erstmt_colreal,
            Erstmt_coltextL, Erstmt_coltextP, Erstmt_coltextD,
            Erstmt_colbinL, Erstmt_colbinP, Erstmt_colbinD, 

            Esp_create, Esp_createl, Esp_close, Esp_setdbpolicy,
            Esp_setpolicy, Esp_externalizel, Esp_internalizel,
            
            Estreamwrite_bindtext, Estreamwrite_bindbinary,
            Estreamread_columntext, Estreamread_columnbinary,
            Edefineconfig,

            // Marks the last item in the enumeration.
            Efn_undefined,
                     };
    // Methods to handle our simple semaphore thread syncing..
    void SignalA(TInt ai) { isemA.Signal(ai); };
    void WaitA() { isemA.Wait(); };
    void SignalB(TInt ai) { isemB.Signal(ai); };
    void WaitB() { isemB.Wait(); };

    // Utility functions. These should move into a separate class, this
    // is untidy.
    TInt ActionNoToErrEnum(const TDesC&, const TInt, TPtrC &);
    TInt ErrStringToEnum(TPtrC &aerr);
    void ErrEnumToString(const TInt &, TPtrC &);
    const TPtrC SqlColumnTypeToString( TSqlColumnType & );
    TSqlColumnType StringToSqlColumnType( const TDesC & );
    void ReportOnError( const TDesC &, const TDesC &, const TDesC &,
                           const TInt, const TInt );
    TBool FromConfig( const TDesC &, const TDesC &, const TDesC &, TPtrC& );
    TBool FromConfig( const TDesC &, const TDesC &, const TDesC &, TInt& );
    TBool FromConfig( const TDesC &, const TDesC &, const TDesC &, TReal& );
    void CommaSeparated( const TPtrC &, TInt&, TInt& );
    void CommaSeparated( const TPtrC &, TInt&, TPtrC& );
    void CommaSeparated( const TPtrC &, TPtrC&, TPtrC& );
    TInt FileSize( const TPtrC & );
    TInt CompareBinaryAgainstFileL(const TDesC8&, const TFileName&);
    TInt CompareTextAgainstFileL(const TDesC&, const TFileName&);
    TInt CompareTextStreamAgainstFileL(RReadStream& , TInt, const TPtrC& );
    TInt CompareBinaryStreamAgainstFileL(RReadStream& , TInt, const TPtrC& );
    void WriteFileToStreamL(RWriteStream& , const TPtrC& );

    // Top-level method. The third parameter is the config block to
    // use.
    void SQLDbStepL(const TPtrC &);

    // Function=whatever. This function recognizes the third parameter and
    // calls a (code-driven) test.
    virtual void ResolveTestFunctionL( const TDesC&, const TInt, const TPtrC &)
                                                                          = 0;
    
    /*
     * These methods are wrappers around the RSqlDatabase methods of
     * the same (or similar) name. Includes error checking etc.
     */
    TBool Create(const TPtrC&,   const TDesC&, TInt);
    TBool CreateL_(const TPtrC&, const TDesC&, TInt);
    TBool CreateSP(const TPtrC&, const TDesC&, TInt);
    TBool Open(const TPtrC&,     const TDesC&, TInt);
    TBool OpenL_(const TPtrC&,   const TDesC&, TInt);
    void  Close();
    TBool Delete(const TPtrC&,   const TDesC&, TInt);

    /*
     * The next three methods are newer..
     */
    void Attach(const TPtrC&, const TDesC&, TInt);
    void Detach(const TPtrC&, const TDesC&, TInt);
    void Copy(const TPtrC&,   const TDesC&, TInt);

    void LastErrorMessage(const TPtrC&);
    void Exec(const TPtrC&,   const TDesC&, TInt);
    void SetIsolationLevel(const TPtrC&, const TDesC&, TInt);

    /*
     * These methods are wrappers around the RSqlStatement methods of
     * the same (or similar) name. Includes error checking etc.
     */
    void Prepare(const TPtrC &, const TDesC&, TInt);
    void PrepareL_(const TPtrC &, const TDesC&, TInt);
    void AtRow(const TPtrC&);
    void Reset(const TDesC&, TInt);
    void Close(TInt);
    void Next(TPtrC&, const TDesC&, TInt);
    TInt ParamIndex(const TDesC&, const TDesC&, TInt);
    TInt ColumnIndex(const TPtrC&, const TDesC&, TInt);
    void ColumnType(const TInt&, const TPtrC&);
    void ColumnSize(const TInt&, const TInt&);
    void BindNull(const TInt&, const TDesC&, const TInt);
    void BindInt(const TInt&, const TInt&, const TDesC&, const TInt);
    void BindInt64(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void BindReal(const TInt&, const TReal &, const TDesC&, const TInt);
    void BindText(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void BindBigTextL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void BindBinaryL(const TInt&, const TDesC&, const TDesC&, const TInt);

    void IsNull(const TInt&, const TPtrC&);
    void ColumnInt(const TInt&, const TInt&);
    void ColumnInt64(const TInt&, const TPtrC&);
    void ColumnReal(const TInt&, const TReal&);
    void ColumnTextL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void ColumnTextPL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void ColumnTextDL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void ColumnBinaryL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void ColumnBinaryPL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void ColumnBinaryDL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void SWBindTextL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void SWBindBinaryL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void SRColumnTextL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void SRColumnBinaryL(const TInt&, const TPtrC&, const TDesC&, const TInt);
    void ReserveDriveSpace(TInt, const TDesC&, const TInt);
    void FreeReservedSpace();
    void GetReserveAccess(const TDesC&, const TInt);
    void ReleaseReserveAccess();

    /*
     * These are the methods associated with setting up a security
     * policy for a secure database
     */
    TBool SPCreate(const TDesC&, const TInt);
    TBool SPCreate(const TPtrC&, const TDesC&, const TInt);
    void SPClose();
    TBool SPSetDBPolicy(const TPtrC&, const TDesC&, const TInt);
    TBool SPSetPolicy(const TPtrC&, const TDesC&, const TInt);
/*  void SPDefaultPolicy(const TDesC&, const TDesC&, const TInt);
    void SPDBPolicy(const TDesC&, const TDesC&, const TInt);
    void SPPolicy(const TDesC&, const TDesC&, const TInt);
*/  void SPExternalize(const TPtrC&, const TDesC&, const TInt);
    void SPInternalize(const TPtrC&, const TDesC&, const TInt);

    static const TInt KConfigItemMaxNameLength = 30;
    static const TInt KConfigItemMaxStringLength = 30;
    static const TInt KStatementFunMaxLength = 30;

private:
    // Arbitrary.
    static const TInt KTEFSQLSizeError = 98989898;
    static const TInt KTEFSQLUnknownError = 98989897;

protected:
    TBool i8bit;
    TBool iasync;
    TPtrC8* icfg;
    RSqlDatabase isqldb;
    RSqlStatement isqlst;
    RSqlSecurityPolicy isqlsp;

    // This is where we store the current column and parameter indices.
    RArray<TInt> icidxs, ipidxs;

    // Now the arrays of RBuf[8]s, these keep buffers in scope between
    // binding and exec'ing.
    RArray<RBuf> iBindRBufarr;
    RArray<RBuf8> iBindRBuf8arr;

    // Various methods do stuff with files, so we're often going to need
    // an RFs, lets create/destroy it in the constructor/destructor.
    RFs irfs;
    
    // This semaphore is used to synchronize things when we have multiple
    // threads running (as supported by the CONCURRENT keyword in TEF
    // scripts).
    // 'isemB' is a second semaphore required by DEF140385, there's small
    // possibility with a single semaphore one thread can run and finish
    // before the other thread(s) even start - the tests are not fully robust
    // without it.
    RSemaphore isemA, isemB;

    // This lets us associate errors (in string form) with their numeric
    // equivalent.
    CSQLErrHash *ierrhsh;
    // This lets us associate column type names (in string form) with their
    // numeric equivalent.
    CSQLColTypeHash *icoltypehsh;
    // This lets us associate actions read from a config file with the
    // TSQLDbFns enumeration above.
    CSQLTEFAction *iactionhsh;
    // This associates capability enums with capabilities read in from the
    // ini file
    CSQLCapability *icaphsh;
    // This associates the policy enum with policies read
    // from the ini file
    CSQLPolicy *ipolhsh;
    // This associates the database object enum with policies read
    // from the ini file
    CSQLObject *iobjhsh;
    };


// Data-driven.
_LIT(KSQLDDT, "CSQLDDT");

// Code driven.
_LIT(KSQLCDT, "CSQLCDT");

_LIT(SQLFN_KTEFSQLUnknownError, "KTEFSQLUnknownError" );
_LIT(SQLFN_KTEFSQLSizeError,    "KTEFSQLSizeError" );

#endif // SQLFN_H__
