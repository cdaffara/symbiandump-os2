// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by MJB, May 1995
// Updated Nov 1996 by PNJ for Unicode.
// Tests RResourceFile class
// 
//

#include <e32test.h>
#include <bautils.h>
#include <barsc.h>
#include <barsc2.h>
#include <trsc.rsg>

#include "T_RSC.H"

LOCAL_D RTest test(_L("T_RSC"));
LOCAL_D RFs TheFs;

_LIT(KRamResourceFile, "c:\\T_RSC.RSC");
_LIT(KRamResourceFile2, "c:\\NewRscFormat.RSC");
_LIT(KFailResourceFile,"z:\\system\\data\\nonexist.rsc");

LOCAL_C TInt compare( const TDesC8 &aresbuf, const TDesC &abuf, TBool aUnicode )
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Compare a buffer taken from a resource file with one that was defined in
// source code.  All resources read at this level are read as pure data.
// However, the resource file entries for this test code are defined as
// generic text, which may be 8 or 16 bit codes, depending on how the
// resource was compiled.  The third parameter indicates which is expected.
//
//
// Return a true value if the text matches, else false.
//
// ----------------------------------------------------------------------------
    {

    TInt count = abuf.Length();

    // exit immediately of the strings are of different length.
    if ( abuf.Size() != aresbuf.Size() ) return FALSE;

    // loop through character by character and exit if any comparison fails.

    if ( aUnicode )
        {
        while ( count > 0 )
            {
            TInt uch;
            TInt bi; // byte index
            count -=1;
            bi = count*2;
            uch = (aresbuf[bi+1]<<8) | aresbuf[bi];
            if ( uch != abuf[count] ) return FALSE;
            }
        }
    else
        {
        while ( count > 0 )
            {
            count -=1;
            if ( aresbuf[count] != abuf[count] ) return FALSE;
            }
        }

    return TRUE;

    } // end of compare function.


LOCAL_C void CopyFileL(const TDesC& aFrom,const TDesC& aTo)
//
// helper function
//
	{
	TheFs.Delete(aTo);
	CFileMan* man=CFileMan::NewL(TheFs);
	TInt r=man->Copy(aFrom,aTo);
	delete man;
	User::LeaveIfError(r);
	User::LeaveIfError(TheFs.SetAtt(aTo,0,KEntryAttReadOnly)); // clear RO
	}

LOCAL_C void DeleteFile(const TDesC& aFileName)
//
// helper function
//
	{
	// make sure the file is read/write
	TInt err = TheFs.SetAtt(aFileName, 0, KEntryAttReadOnly);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error changing attributes file = %d"),err);
		}
	// delete the file
	err = BaflUtils::DeleteFile(TheFs, aFileName);
	if(err != KErrNone)
		{
		RDebug::Print(_L("error deleting file = %d"),err);
		}
	}

LOCAL_C TInt FileSizeL(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(TheFs, aFileName, EFileRead));
	CleanupClosePushL(file);
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
	CleanupStack::PopAndDestroy(&file);
	return size;
	}

template <class T> class TestRsc
	{
protected:
	void TestReadL(T* aRscFile);
    };

class TestRRsc : public TestRsc<RResourceFile>
	{
public:
	void TestReadROldL(const TDesC &aTitle, const TDesC &aFileName);
	void TestReadRNewL(const TDesC &aTitle, const TDesC &aFileName, TUint aFileOffset, TUint aFileSize);
	void TestOpenR();
    };

class TestCRsc : public TestRsc<CResourceFile>
	{
public:
	void TestReadCNewL(const TDesC &aTitle, const TDesC &aFileName, TUint aFileOffset, TUint aFileSize);
	void TestOpenC();
    };

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0427
@SYMTestCaseDesc        RResourceFile class functionality test
@SYMTestPriority        Medium
@SYMTestActions         Attempts to read from an 8 and 16 bit resource file
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestRRsc::TestReadROldL(const TDesC &aTitle, const TDesC &aFileName)
	{
	test.Start(aTitle);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0427 "));
	RResourceFile rsc;
	CleanupClosePushL(rsc);
	rsc.OpenL(TheFs, aFileName);
	TestReadL(&rsc);
	CleanupStack::PopAndDestroy();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0428
@SYMTestCaseDesc        Tests for RResourceFile::OpenL(,,,) function
@SYMTestPriority        Medium
@SYMTestActions         Attempt to read from a resource file
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestRRsc::TestReadRNewL(const TDesC &aTitle, const TDesC &aFileName, TUint aFileOffset, TUint aFileSize)
	{
	test.Start(aTitle);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0428 "));
	RResourceFile rsc;
	CleanupClosePushL(rsc);
	rsc.OpenL(TheFs, aFileName, aFileOffset, aFileSize);
	TestReadL(&rsc);
	CleanupStack::PopAndDestroy();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0429
@SYMTestCaseDesc        Testing error recovery - R file
@SYMTestPriority        Medium
@SYMTestActions         Attempt to read a resouce file,should recover on error.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestRRsc::TestOpenR()
	{
	test.Start(_L("Testing error recovery - R file"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0429 "));
	RResourceFile rsc_file;
    TRAPD(err, rsc_file.OpenL(TheFs, KFailResourceFile));
    test(err != KErrNone);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0430
@SYMTestCaseDesc        Tests for RResourceFile::NewL() function
@SYMTestPriority        Medium
@SYMTestActions         Tests for reading the new resource file
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestCRsc::TestReadCNewL(const TDesC &aTitle, const TDesC &aFileName, TUint aFileOffset, TUint aFileSize)
	{
	test.Start(aTitle);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0430 "));
	CResourceFile* rsc = CResourceFile::NewL(TheFs, aFileName, aFileOffset, aFileSize);
	CleanupStack::PushL(rsc);
	TestReadL(rsc);
	CleanupStack::PopAndDestroy(rsc);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0431
@SYMTestCaseDesc        Testing error recovery - C file
@SYMTestPriority        Medium
@SYMTestActions         Tests for new open resource file,should recover on error
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestCRsc::TestOpenC()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0431 Testing error recovery - C file "));
	CResourceFile* rsc_file = NULL;
    TRAPD(err, rsc_file = CResourceFile::NewL(TheFs, KFailResourceFile, 0, 0));
    test(err != KErrNone);
	delete rsc_file;
	test.End();
	}

template <class T> void TestRsc<T>::TestReadL(T* aRscFile)
	{
	TInt ii;
    //
    // First test to see if we have an 8 or 16-bit text resource file.
    // This is done by looking at a signature resource that is defined
    // as an LTEXT item and seeing how its length computes.
    //

    test.Next(_L("Testing 8 or 16-bit signature"));


    TBool isUnicode = EFalse;

    HBufC8 *ps = aRscFile->AllocReadL(R_TEXT_SIGNATURE);
    TPtr8 sig = ps->Des();

    TInt l = sig.Length();		// length of the raw resource data
    TInt sig_count = sig[0];	// count byte at the start of the LTEXT

    if ( l == (1 + sig_count) )	// count + TText8 data
		{
		test.Printf(_L("8-bit resource text\n"));
		}
    else
	if ( l == (1+1+(sig_count*2)) )		// count + padding + TText16 data
		{
		isUnicode = ETrue;
		test.Printf(_L("Unicode resource text...\n"));
		test(sig[1]==0xAB);				// check for the defined padding byte
		}
	else
		test.Printf(_L("Invalid signature found\n"));

    User::Free(ps);

	for (ii=SYS_SPECIAL_CHARACTERS;ii<=SYS_PAGE_IS;ii++)
		{
		TPtrC des=TPtrC(S_Resource[ii]);
		TBuf<256> testbuf;
		testbuf.Format(_L("Testing ReadL for resource: \"%S\""),&des);
		test.Next(testbuf);
		TBuf8<256> buf;
		aRscFile->ReadL(buf,ii);
        test(compare(buf,des,isUnicode));
		}

	for (ii=SYS_SPECIAL_CHARACTERS;ii<=SYS_PAGE_IS;ii++)
		{
		TPtrC des=TPtrC(S_Resource[ii]);
		TBuf<256> testbuf;
		testbuf.Format(_L("Testing AllocReadL for resource: \"%S\""),&des);
		test.Next(testbuf);
		HBufC8 *p=aRscFile->AllocReadL(ii);
		TPtr8 wp=p->Des();
		test(compare(wp,des,isUnicode));
		User::Free(p);
		}

	for (ii=SYS_SPECIAL_CHARACTERS;ii<=SYS_PAGE_IS;ii++)
		{
		TPtrC des=TPtrC(S_Resource[ii]);
		TBuf<256> testbuf;
		testbuf.Format(_L("Testing AllocReadLC for resource: \"%S\""),&des);
		test.Next(testbuf);
		HBufC8 *p=aRscFile->AllocReadLC(ii);
		TPtr8 wp=p->Des();
		test(compare(wp,des,isUnicode));
		CleanupStack::PopAndDestroy();
		}
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0432
@SYMTestCaseDesc        Testing RResourceFile & CResourceFile classes
@SYMTestPriority        High
@SYMTestActions         Wrapper function, calls up test execute functions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void DoTestsL()
    {
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0432 "));
	CleanupClosePushL(TheFs);
	User::LeaveIfError(TheFs.Connect());

	test.Printf(_L("Copying resource files to RAM\r\n"));
	CopyFileL(KRomResourceFile,KRamResourceFile);
	CopyFileL(KRomResourceFile2,KRamResourceFile2);

	//The new resource format rsc file is compiled from RscHeader.bin, TRsc.rsc and 16RAMC.mbm.
	//So I want to know the offset and size of TRsc.rsc.
	TInt header_size = ::FileSizeL(KRomResourceFileHeader);
	TInt rsc_size1 = ::FileSizeL(KRomResourceFile);
	TInt rsc_size2 = ::FileSizeL(KRamResourceFile);

	TestRRsc t1;

	t1.TestReadROldL(_L("Testing ROM Resource file T_RSC.RSC"),KRomResourceFile);

	t1.TestReadRNewL(_L("Testing ROM Resource file NewRscFormat.RSC"),
							KRomResourceFile2,
							header_size,
							rsc_size1);

	t1.TestReadROldL(_L("Testing RAM Resource file T_RSC.RSC"),KRamResourceFile);

	t1.TestReadRNewL(_L("Testing RAM Resource file NewRscFormat.RSC"),
						KRamResourceFile2,
						header_size,
						rsc_size2);

	t1.TestOpenR();

	TestCRsc t2;

	t2.TestReadCNewL(_L("Testing ROM Resource file T_RSC.RSC"),KRomResourceFile, 0, 0);

	t2.TestReadCNewL(_L("Testing ROM Resource file NewRscFormat.RSC"),
						KRomResourceFile2,
						header_size,
						rsc_size1);

	t2.TestReadCNewL(_L("Testing RAM Resource file T_RSC.RSC"),KRamResourceFile, 0, 0);

	t2.TestReadCNewL(_L("Testing RAM Resource file NewRscFormat.RSC"),
						KRamResourceFile2,
						header_size,
						rsc_size2);

	t2.TestOpenC();

   	// tidy up
	DeleteFile(KRamResourceFile);
	DeleteFile(KRamResourceFile2);

	CleanupStack::PopAndDestroy(1, &TheFs);
    }

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	test.Title();
	test.Start(_L("Testing RResourceFile & CResourceFile"));
    TRAPD(err,DoTestsL());
    test.Printf(_L("Error code is %d\n"),err);
    test(err==KErrNone);
    test.Next(_L("/n"));
	test.End();
    test.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return(0);
    }
