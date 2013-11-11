/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* (c) 1999 Symbian Ltd
*
*/


/**
 @file
*/

#include <e32base.h>
#include <e32std.h>
#include <e32test.h>
#include <random.h>
#include <f32file.h>

RTest test(_L("Random Number Generator"));

TInt gTestPassCount = 0;
TInt gTestRequestCount = 10;
TInt gNumberOfRandomNumbers=10000;

/** Wraps a console and logs output to a file. */
class CTestConsole:public CConsoleBase
	{
	public:
		static CTestConsole* NewL(CConsoleBase* aCon, const TDesC& aFilename);
		TInt Create(const TDesC16& aTitle,TSize aSize) {return iCon->Create(aTitle,aSize);};
		void Read(TRequestStatus& aStatus) {iCon->Read(aStatus);};
		void ReadCancel(void) {iCon->ReadCancel();};
		void Write(const TDesC16& aString);
		TPoint CursorPos(void) const {return iCon->CursorPos();};
		void SetCursorPosAbs(const TPoint& aPos) {iCon->SetCursorPosAbs(aPos);};
		void SetCursorPosRel(const TPoint& aPos) {iCon->SetCursorPosRel(aPos);};
		void SetCursorHeight(TInt aHeight) {iCon->SetCursorHeight(aHeight);};
		void SetTitle(const TDesC16& aTitle) {iCon->SetTitle(aTitle);};
		void ClearScreen(void) {iCon->ClearScreen();};
		void ClearToEndOfLine(void) {iCon->ClearToEndOfLine();};
		TSize ScreenSize(void) const {return iCon->ScreenSize();};
		TKeyCode KeyCode(void) const {return iCon->KeyCode();};
		TUint KeyModifiers(void) const {return iCon->KeyModifiers();};
		~CTestConsole(void);
	private:
		CTestConsole(CConsoleBase* aCon);
	    void ConstructL(const TDesC& aFilename);
	    CConsoleBase* iCon;  ///< Pointer to wrapped console, we don't own this
	    RFs iFs;
	    RFile iFile;		 ///< Log file
	};

CTestConsole* CTestConsole::NewL(CConsoleBase* aCon, const TDesC& aFilename)

	{
	CTestConsole* self;
	self=new (ELeave) CTestConsole(aCon);
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	CleanupStack::Pop(self);
	return self;
	}

CTestConsole::CTestConsole(CConsoleBase* aCon) :
	CConsoleBase(), iCon(aCon)

	{
	}

void CTestConsole::ConstructL(const TDesC& aFilename)
	
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Replace(iFs,aFilename,EFileShareAny|EFileWrite));
	}

CTestConsole::~CTestConsole(void)

	{
	iFile.Close();
	iFs.Close();
	}

void CTestConsole::Write(const TDesC16& aString)

	{
	iCon->Write(aString);
	TUint8 space[200];
	TPtr8 ptr(space,200);
	ptr.Copy(aString);
	iFile.Write(ptr);
	}

void Monobit(const TUint8* aData)

	{
	const TInt bitcount[256]=
		{ 0,1,1,2,1,2,2,3,		// 00-07
		  1,2,2,3,2,3,3,4,		// 08-0f
		  1,2,2,3,2,3,3,4,		// 10-17
		  2,3,3,4,3,4,4,5,		// 18-1f
		  1,2,2,3,2,3,3,4,		// 20-27
		  2,3,3,4,3,4,4,5,		// 28-2f
		  2,3,3,4,3,4,4,5,		// 30-37
		  3,4,4,5,4,5,5,6,		// 38-3f

		  1,2,2,3,2,3,3,4,		// 40-47
		  2,3,3,4,3,4,4,5,		// 48-4f
		  2,3,3,4,3,4,4,5,		// 50-57
		  3,4,4,5,4,5,5,6,		// 58-5f
		  2,3,3,4,3,4,4,5,		// 60-67
		  3,4,4,5,4,5,5,6,		// 68-6f
		  3,4,4,5,4,5,5,6,		// 70-77
		  4,5,5,6,5,6,6,7,		// 78-7f

		  1,2,2,3,2,3,3,4,		// 80-87
		  2,3,3,4,3,4,4,5,		// 88-8f
		  2,3,3,4,3,4,4,5,		// 90-97
		  3,4,4,5,4,5,5,6,		// 98-9f
		  2,3,3,4,3,4,4,5,		// a0-a7
		  3,4,4,5,4,5,5,6,		// a8-af
		  3,4,4,5,4,5,5,6,		// b0-b7
		  4,5,5,6,5,6,6,7,		// b8-bf

		  2,3,3,4,3,4,4,5,		// c0-c7
		  3,4,4,5,4,5,5,6,		// c8-cf
		  3,4,4,5,4,5,5,6,		// d0-d7
		  4,5,5,6,5,6,6,7,		// d8-df
		  3,4,4,5,4,5,5,6,		// e0-e7
		  4,5,5,6,5,6,6,7,		// e8-ef
		  4,5,5,6,5,6,6,7,		// f0-f7
		  5,6,6,7,6,7,7,8		// f8-ff
		};
	TInt total=0;
	TInt i;
	for (i=0;i<2500;i++)
		{
		total+=bitcount[aData[i]];
		}
	test.Printf(_L("    Total bitcount %d\r\n"),total);
	if ((total>9654)&&(total<10346))
		{
		test.Printf(_L("    Passed Monobit\r\n"));
		}
	else
		{
		test.Printf(_L("    ***FAILED!\r\n"));
		User::Panic(_L("t_random.exe"), KErrGeneral);
		}
	}

void Poker(const TUint8* aData)
	
	{
	TInt f[16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	TInt i;
	for (i=0;i<2500;i++)
		{
		f[(aData[i]&0x0f)]++;
		f[((aData[i]&0xf0)>>4)]++;
		}
	TReal x=0;
	for (i=0;i<16;i++)
		{
		x+=f[i]*f[i];
		}
	x*=16;
	x/=5000;
	x-=5000;
	if ((x>1.03)&&(x<57.4))
		{
		test.Printf(_L("    Passed poker test\r\n"));
		}
	else
		{
		test.Printf(_L("    ***FAILED poker test\r\n"));
		User::Panic(_L("t_random.exe"), KErrGeneral);
		}
	}

void Runs(const TUint8* aData)

	{
	TInt i;
	TInt lastbit=0;
	TInt count[7][2]={
		{ 0,0 },
		{ 0,0 },
		{ 0,0 },
		{ 0,0 },
		{ 0,0 },
		{ 0,0 },
		{ 0,0 }
		};
	TInt thisrun=0;
	TInt longrun=0;
	for (i=0;i<2500;i++)
		{
		TInt bit;
		for (bit=0;bit<8;bit++)
			{
			if (((aData[i]>>bit)&1)==lastbit)
				{
				thisrun++;
				}
			else
				{
				if (thisrun<5)
					{
					count[thisrun][lastbit]++;
					}
				else
					{
					count[5][lastbit]++;
					}
				lastbit^=1;
				if (thisrun>longrun)
					{
					longrun=thisrun;
					}
				thisrun=0;
				}
			}
		}
	TInt bound[6][2]=
		{
			{2267,2733},
			{1079,1421},
			{ 502, 748},
			{ 223, 402},
			{  90, 223},
			{  90, 223}
		};
	TBool failed=EFalse;
	for (i=0;i<6;i++)
		{
		if (!((count[i][0]>bound[i][0])&&(count[i][0]<bound[i][1])))
			{
			test.Printf(_L("    ***FAILED runs test\r\n"));
			failed=ETrue;
			}
		if (!((count[i][1]>bound[i][0])&&(count[i][1]<bound[i][1])))
			{
			test.Printf(_L("    ***FAILED runs test\r\n"));
			failed=ETrue;
			}
		}
	if (!failed)
		{
		test.Printf(_L("    Passed runs test\r\n"));
		}
	if ( (longrun>34) || (failed) )
		{
		test.Printf(_L("    ***FAILED longrun test\r\n"));
		User::Panic(_L("t_random.exe"), KErrGeneral);
		}
	}

void FIPSTest(const TUint8* aData)
//	Run some basic tests to check it's returned some numbers
//	These will panic if a failure is detected
	{
	Monobit(aData);
	Poker(aData);
	Runs(aData);
	}

void WriteFile(const TUint8* aData,const TDesC& aFileName)

	{
	RFs fs;
	fs.Connect();
	RFile file;
	TInt err;
	err=file.Open(fs,aFileName,EFileShareAny|EFileWrite);
	if (err)
		{
		if (file.Create(fs,aFileName,EFileShareAny|EFileWrite))
			{
			return;
			}
		}
	TPtrC8 ptr(aData,gNumberOfRandomNumbers);
	TInt size;
	file.Size(size);
	file.Write(size,ptr);
	file.Close();
	fs.Close();
	FIPSTest(aData);
	}

void DoTestsL(void)

	{
	test.Printf(_L("Run random tests with normal salting\r\n"));    
	TInt i;
	TBuf8<16> buf2;
	for (i=0;i<16;i++)
		{
		buf2.SetLength(i);
		TRandom::RandomL(buf2);
		}
	
	HBufC8* buf=HBufC8::NewMaxL(gNumberOfRandomNumbers);
	CleanupStack::PushL(buf);
	TPtr8 buffer=buf->Des();
	test.Printf(_L("\nRequesting for random numbers.\n"));
	for (i=0;i<gTestRequestCount;i++)
		{
		TPtr8 thePtr(buf->Des());		
		//	Generate the random data	
		TRandom::RandomL(buffer);
		if (buf->Length()!=gNumberOfRandomNumbers)
		    {
			User::Leave(KErrGeneral);
		    }
		++gTestPassCount;
	    test.Printf(_L("."));
		}	
	
	// Request for Secure Random numbers.
	test.Printf(_L("\nRequesting for cryptographically secure random numbers.\n"));
    for (i=0;i<gTestRequestCount;i++)
        {
        TPtr8 thePtr(buf->Des());        
		// Generate the random data    
        TRAP_IGNORE(TRandom::SecureRandomL(buffer));
        if (buf->Length() != gNumberOfRandomNumbers)
            {
            User::Leave(KErrGeneral);
            }
        ++gTestPassCount;
        test.Printf(_L("."));
        }
	CleanupStack::PopAndDestroy(buf);
	}

void TestsL(void)
	{
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> logFile (sysDrive.Name());
	logFile.Append(_L("\\t_random.log"));
	CTestConsole* con = CTestConsole::NewL(test.Console(), logFile);
	test.SetConsole(con);
	
	DoTestsL();
	}

GLDEF_C TInt E32Main(void)

	{
	CTrapCleanup* cleanup;
	cleanup=CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-RANDOM-0001 Starting random number generator tests\r\n"));
	CConsoleBase* originalConsole = test.Console();

	TRAPD(ret,TestsL());   
    // Infor the user about the test cases' status.
    test.Printf(_L("\n%d tests failed out of %d\r\n"), ((2*gTestRequestCount) - gTestPassCount), 2*gTestRequestCount);
    
    test (ret == KErrNone);
	test.End();

	if (test.Console() != originalConsole)
		{
		delete test.Console();
		test.SetConsole(originalConsole);
		}	
	test.Close();
	
	__UHEAP_MARKEND;
	
	delete cleanup;
	return(KErrNone);
	}
