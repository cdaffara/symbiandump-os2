// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "zipfile.h"

#include <e32cons.h>
#include <s32file.h>
#include <s32strm.h> 
#include <utf.h>
#include <e32test.h>

/*
	class CZipTest - defines text UI
*/
static RTest test(_L("ZIPTEST"));

class CZipTest : public CBase
	{
public:
	// execute UI until exit command processed
	void ExecuteL();
private:
	// destructor
	~CZipTest();
	// other functions
	void TrapDoTest();
	void DoTestL();
	void ExtractFileL(RFs&,  const CZipFileMember*, CZipFile*, const TFileName*, RFileWriteStream&);
	void ExpandMember();
	void TheZipTestL(RFs&, TFileName*, RFileWriteStream&);
	
private:
	CConsoleBase*	iConsole;
	RFile			iZipOut;
	TBool			iNoFiles;
	};

_LIT(KNoFilesFound,"No appropriate files located in this folder.\r\n");
_LIT(KOpeningFile,"Opening file ");
_LIT(KDots,"...");
_LIT(KOutputFileName,"C:\\test\\zip\\extracts\\zip_out.txt");
_LIT(KErrFileNotOpened, "File could not be opened.");
_LIT(KOk, "Ok");
_LIT(KZipName,"Name:		");
_LIT(KZipCRC32,"CRC32:		");
_LIT(KZipCSize,"Compressed Size:	");
_LIT(KZipUCSize,"Uncompressed Size:	");
_LIT(KOpenedSuccessfully, "%S opened sucessfully.\n");
_LIT(KLineSpace, "\r\n");

_LIT(KExtractZipPath, "C:\\test\\zip\\extracts\\");

#if !defined(__WINS__)
_LIT(KPath, "Z:\\test\\zip\\");
#else
_LIT(KPath, "C:\\test\\zip\\");
#endif

void CZipTest::ExecuteL()
	{
	//text constant declarations
	_LIT(KTxtZipTest,"Zip Test");
		
	iConsole=Console::NewL(KTxtZipTest,
		TSize(KConsFullScreen,KConsFullScreen));
	DoTestL();
	}

CZipTest::~CZipTest()
	{
	delete iConsole;
	}
void CZipTest::TrapDoTest()
	{
	TInt q=10;

	while(q<2000)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, q);
		__UHEAP_MARK;
		TRAPD(err, DoTestL());
		test(err==KErrNone);
		__UHEAP_MARKEND;
		q++;
		}
	}

void CZipTest::DoTestL()
	{
	
	TFileName*				FileName;
	RFs						Fs;
	
	iNoFiles=ETrue;
	
	User::LeaveIfError(Fs.Connect()); //Connect to file session
	CleanupClosePushL(Fs);
	User::LeaveIfError(Fs.SetSessionPath(KPath)); //Set Session Path to direcrt containing test zip files
	
	Fs.MkDirAll(KOutputFileName);
	iZipOut.Replace(Fs, KOutputFileName, EFileShareAny|EFileWrite);
	RFileWriteStream stream(iZipOut);
	CleanupClosePushL(stream);
	CDir* fileList;
	
	Fs.GetDir(KPath, KEntryAttNormal, ESortByName, fileList);
	CleanupStack::PushL(fileList);
	TInt count = fileList->Count(); 
	for (TInt y=0; y<count; y++)
		{

		iNoFiles=EFalse;
		
		TBuf<256> dirFileName((*fileList)[y].iName);

		FileName= &dirFileName;
		stream.WriteL(KLineSpace);
		stream.WriteL(KOpeningFile);
		stream.WriteL(*FileName);
		stream.WriteL(KDots);
		TheZipTestL(Fs, FileName, stream);
	/*	for(TInt t=1;t<50;t++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, t);
		__UHEAP_MARK;

		TRAPD(err,TheZipTestL(Fs, FileName, stream));
		
		if (err!=KErrNoMemory) break;
		__UHEAP_MARKEND;
		}
*/
	}
	if (iNoFiles) 
		{
		iConsole->Printf(KNoFilesFound);
		stream.WriteL(KNoFilesFound);
		}
	CleanupStack::PopAndDestroy(3);
/*	CleanupStack::Pop();
	Fs.Close();
*/	}

#if defined(__GCC32__)
typedef wchar_t __TText;
#elif defined(__VC32__)
typedef TUint16 __TText;
#elif defined(__CW32__)
typedef TUint16 __TText;
#elif !defined(__TText_defined)
#error  no typedef for __TText
#endif
const __TText* KShortData1252  = L"\x20AC\u00FC\u00E9";
const __TText* KShortData850   = L"\u00C7\u00FC\u00E9";
const __TText* KLongData1252   = L"\u00E7\u00F8d\u00EB\u00DE\u00E3g\u00EA1252";
const __TText* KLongData850    = L"\u00E7\u00F6d\u00EBp\u00E4g\u00EA850";
const __TText* KShortWin2K1252 = L"CP1252Win2kShort.zip"; // created on win2k with winzip
const __TText* KShortWinXP1252 = L"CP1252WinXPShort.zip"; // created on XP with winzip
const __TText* KShortWin2K850  = L"CP850Win2kShort.zip";
const __TText* KShortWinXPMS850= L"CP850WinXPMSShort.zip";// created on XP with Compressed Folders
const __TText* KShortWinXP850  = L"CP850WinXPShort.zip";
const __TText* KLongWin2K      = L"CPMixWin2kLong.zip";
const __TText* KLongWinXP      = L"CPMixWinXPLong.zip";
const __TText* KLongWinXPMS    = L"CPMixWinXPMSLong.zip";
struct TUnzipResultsArray
	{
	const __TText*  iZipFileName;
	TInt iZipFileMemberIndex;
	const __TText*  iZipFileMemberExpectedName;
	};
const TUnzipResultsArray KUnzipResults[] =
	{
	{KShortWin2K1252, 1, KShortData1252},
	{KShortWinXP1252, 1, KShortData1252},
	{KShortWin2K850,  1, KShortData850},
	{KShortWinXPMS850,1, KShortData850},
	{KShortWinXP850,  1, KShortData850},
	{KLongWin2K,      1, KLongData1252},
	{KLongWin2K,      2, KLongData850},
	{KLongWinXP,      1, KLongData850},
	{KLongWinXP,      2, KLongData1252},
	{KLongWinXPMS,    1, KLongData850},
	{KLongWinXPMS,    2, KLongData1252}
	};
void TestEntry(const TDesC16& aFileName, TInt aIndex, const TDesC16& aEntryName)
// Test for correct decoding of filenames containing extended characters (DEF083102)	
// Cycle through data defined above looking for matching filenames and data
	{	
 	for (TInt i = 0; i != sizeof(KUnzipResults)/sizeof(KUnzipResults[0]); ++i)
 		{
 		TPtrC filename(reinterpret_cast<const TText16*>(KUnzipResults[i].iZipFileName));
 		if(aFileName==filename && aIndex==KUnzipResults[i].iZipFileMemberIndex)
 			{
	 		TPtrC entryname(reinterpret_cast<const TText16*>(KUnzipResults[i].iZipFileMemberExpectedName));
 			test.Next(_L("DEF083102"));
 			test(aEntryName==entryname);
 			}
 		}
	}
	
void CZipTest::TheZipTestL(RFs& aFs, 
						  TFileName* aFileName, 
						  RFileWriteStream& aStream)
	{
	CZipFile*				ZipFile=0;
	CZipFileMember*			Member=0;
	CZipFileMemberIterator*	FileMembers;

	iConsole->Printf(_L("Test file: %S\r\n"), aFileName);
	TRAPD(err, ZipFile = CZipFile::NewL(aFs,*aFileName));
	
	if(err==KErrNoMemory) 
		User::Leave(err); // leave here if error is no memory
	
	if (err!=KErrNone)
		{
		aStream.WriteL(KErrFileNotOpened);
		aStream.WriteL(KLineSpace);
	
		if (err == CZipArchive::KZipArchiveError)
			aStream.WriteL(_L("Error: Zip Archive Error.\r\n"));
		else if (err == CZipArchive::KCentralDirectoryTrailerNotFound)
			aStream.WriteL(_L("Error: Central Directory Trailer Not Found.\r\n"));
		else if (err == CZipArchive::KCentralDirectoryTrailerInvalid)
			aStream.WriteL(_L("Error: KCentral Directory Trailer Invalid.\r\n"));
		else if (err == CZipArchive::KCompressionMethodNotSupported)
			aStream.WriteL(_L("Error: Compression Method Not Supported.\r\n"));
		else if (err == CZipArchive::KMultiDiskArchivesNotSupported)
			aStream.WriteL(_L("Error: Multi Disk Archives Not Supported.\r\n"));
		else if (err == CZipArchive::KMemberNotFound)
			aStream.WriteL(_L("Error: Member Not Found.\r\n"));
		else if (err == CZipArchive::KZipArchiveMinError)
			aStream.WriteL(_L("Error: Zip Archive Min Error.\r\n"));

		User::Leave(err);
		}
	else
		{
		CleanupStack::PushL(ZipFile);
		aStream.WriteL(KOk);
		aStream.WriteL(KLineSpace);
			aStream.WriteL(KLineSpace);
		iConsole->Printf(KOpenedSuccessfully, aFileName);
	
		iNoFiles=EFalse;
			
		FileMembers = ZipFile->GetMembersL();
		CleanupStack::PushL(FileMembers);
		Member = FileMembers->NextL();
		TInt index=1;// have to provide this - the one in the iterator is private
		while (Member != NULL)
			{
			CleanupStack::PushL(Member);
			TBuf<16> CRC32;
			CRC32.NumUC((TUint)Member->CRC32(), EHex);
			TBuf<16> UnComp;
			UnComp.NumUC((TUint)Member->UncompressedSize());
			TBuf<16> Comp;
			Comp.NumUC((TUint)Member->CompressedSize());
			aStream.WriteL(KZipName);
			aStream.WriteL(*Member->Name());
			aStream.WriteL(KLineSpace);
			aStream.WriteL(KZipCRC32);
			aStream.WriteL(CRC32);
			aStream.WriteL(KLineSpace);
			aStream.WriteL(KZipCSize);
			aStream.WriteL(Comp);
			aStream.WriteL(KLineSpace);
			aStream.WriteL(KZipUCSize);
			aStream.WriteL(UnComp);
			aStream.WriteL(KLineSpace);
			aStream.CommitL();
			ExtractFileL(aFs, Member, ZipFile, aFileName, aStream);
			TestEntry(*aFileName,index++,*Member->Name());
			aFs.SetSessionPath(KPath);
			CleanupStack::PopAndDestroy(Member); 
			Member = FileMembers->NextL();
			}
		CleanupStack::PopAndDestroy(FileMembers); 
		CleanupStack::PopAndDestroy(ZipFile);
		}
}

void CZipTest::ExtractFileL(RFs& aFs, 
							const CZipFileMember* aMember, 
							CZipFile* aZipFile, 
							const TFileName* aFileName,
							RFileWriteStream& aStream)
	{
	TInt loop=0;
	HBufC* name = aMember->Name()->AllocLC();
	while (loop<name->Length())
		{
		if ((*name)[loop] == '/')
			{
			name->Des()[loop] = '\\';
			}
		loop++;
		}
	
	TFileName fn;
	fn.Append(KExtractZipPath);
	fn.Append(*aFileName);
	fn.Append('\\');
	fn.Append(*name);
//	
	RFile expandedMember;
	CleanupClosePushL(expandedMember);
//
	aFs.SetSessionPath(KExtractZipPath);
	aFs.MkDirAll(fn);
	TInt retVal = expandedMember.Replace(aFs,fn, EFileShareAny|EFileWrite);
	if(retVal != KErrNone)
	{
	//Could be a directory name which we are planning to write to.
	CleanupStack::PopAndDestroy(2,name); //expanded member, name
	expandedMember.Close();
	return;
	}

//
	RZipFileMemberReaderStream* fileStream;
	TInt error = aZipFile->GetInputStreamL(aMember, fileStream);
	if (error != KErrNone)
		{
		_LIT(KCompressionNotSupported, "Error: Compression Method Not Supported");
		aStream.WriteL(KCompressionNotSupported);
		aStream.WriteL(KLineSpace);
		CleanupStack::PopAndDestroy(2,name); //expanded member and name
		aFs.Delete(fn);
		return;
		}
	CleanupStack::PushL(fileStream);
//
	TUint32 size = aMember->UncompressedSize();
	HBufC8* bytes = HBufC8::New(size);
	CleanupStack::PushL(bytes);
	TPtr8 ptr = bytes->Des();
	User::LeaveIfError(fileStream->Read(ptr,size));
	User::LeaveIfError(expandedMember.Write(ptr));
	CleanupStack::PopAndDestroy(4,name); // bytes, fileStream, expanded member and name
	expandedMember.Close();	
	}
	
static void executeUiL()
	{
	CZipTest* ziptest=new (ELeave) CZipTest;
	CleanupStack::PushL(ziptest);
	ziptest->ExecuteL();
	CleanupStack::PopAndDestroy();
	}
/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4299
@SYMTestCaseDesc	    Test CZipFile functionality like open, close, size of the file.
@SYMTestPriority 	    High
@SYMTestActions  	    Creates a new CZipFile object using the supplied file server session and a valid file handle using CZipFile::NewL() 
						Open zip file using CZipFile::OpenL()
						Check the size of the zip file using CZipFile::Size()
						Close zip file using CZipFile::Close()
@SYMTestExpectedResults The test succeeds with no errors i.e. KErrNone 
@SYMDEF                 REQ8024
*/
static void TestCZipFileMethodsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4299 "));
	RFs	rFs;
	
	User::LeaveIfError(rFs.Connect()); //Connect to file session
	CleanupClosePushL(rFs);
	User::LeaveIfError(rFs.SetSessionPath(KPath)); //Set Session Path to direcrt containing test zip files
		
	TFileName fileName(_L("rfc2459.zip"));

	test.Printf(_L("Test file: %S\r\n"), &fileName);

	CZipFile* ZipFile = 0;
	TRAPD(err, ZipFile = CZipFile::NewL(rFs, fileName));
	
	CleanupStack::PushL(ZipFile);
	
	TRAP(err, ZipFile->OpenL());
	test(err == KErrNone);
	
	TInt fileSize;
	TRAP(err, ZipFile->Size(fileSize));
	test(err == KErrNone && fileSize > 0);
	
	TRAP(err, ZipFile->Close());
	test(err == KErrNone);

	CleanupStack::PopAndDestroy(2);// rFs, ZipFile
	}

GLDEF_C TInt E32Main() // main function called by E32
    {
	test.Title();
	__UHEAP_MARK; // mark heap state
	test.Start(_L("Test ZIP"));
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, executeUiL()); // do most stuff under cleanup stack
	test(error==KErrNone);
	test.Next(_L("Test CZipFile functionality like open, close, size of the file"));
	TRAP(error, TestCZipFileMethodsL());
	test.End();
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND; // check no memory leak
	test.Close();
	return 0; // and return
    }

