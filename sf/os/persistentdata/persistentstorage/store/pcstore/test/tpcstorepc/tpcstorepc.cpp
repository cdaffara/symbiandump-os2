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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <memory>
#include <new>
#include <pcstore/pcstore.h>
#include "pcstoretestdef.h"

using namespace std;

// Exceptions thrown by the test program
enum ETestError
	{
	ETestFileOpenError = 1,
	ETestFileReadError,
	ETestFileSeekError,
	ETestFileTellError,
	ETestFileSizeDiff,
	ETestFileDataDiff,
	ETestDataRestoreError,
	ETestExceptionTestFailure
	};

// File names used in this test
const char* KStorePCName = "/epoc32/winscw/c/stor-tst/pcstore/store_pc1";

// New store file name to be used to trigger exceptions
const char* KStorePCNameExc = "/epoc32/winscw/c/stor-tst/pcstore/store_pc2";
const char* KStoreSymName = "/epoc32/winscw/c/stor-tst/pcstore/store_ref1";
const char* KStoreSymNameNoStream = "/epoc32/winscw/c/stor-tst/pcstore/store_ref2";

const char* KNonStoreFileName = "/epoc32/winscw/c/stor-tst/pcstore/nonstorefile.dat";

// Store file name not existing
const char* KStorePCInValidName = "/epoc32/winscw/c/stor-tst/pcstore1/store_pc1";


// Data for 8-bit descriptor API test
const TUint8 KString8[] = "0123456789";
const TInt32 KString8Length = 10;
const TInt32 KString8Size = 10;
const TInt32 KString8HalfLength = 5;
const TInt32 KString8HalfSize = 5;

const TUint8 KString8_app[] = "9876543210";
const TInt32 KString8Length_app = 10;

const TUint8 KString8_aft_app[] = "01234567899876543210";
const TInt32 KString8Length_aft_app = 20;
const TInt32 KString8Size_aft_app = 20;

// Data for 16-bit descriptor API test
const TUint16 KString16[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x00};
const TInt32 KString16Length = 10;
const TInt32 KString16Size = 20;
const TInt32 KString16HalfLength = 5;
const TInt32 KString16HalfSize = 10;

const TUint16 KString16_app[] = {0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x00};
const TInt32 KString16Length_app = 10;

const TUint16 KString16_aft_app[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,  
					0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x00};
const TInt32 KString16Length_aft_app = 20;
const TInt32 KString16Size_aft_app = 40;


void CreateStoreFile();
void RestoreStoreFile();
void ExceptionsTests();
void DescriptorsAPIs();
void TestForDEF113838();
void TestForDEF113845();

// Compares 2 files in size and content.
// They are identical if this function completes without exception
void FileCompare(const char* f1, const char* f2)
	{
	FILE * fp1 = NULL, * fp2 = NULL;
	int f_size1, f_size2;
	int err;

	assert(f1 != NULL && f2 != NULL);
	
	try
		{
		// Opens file f1 and gets its size
		fp1 = fopen(f1, "rb");
		if(fp1 == NULL) 
			{
			printf("File comparison: failed to open file: %s\n", f1);
			throw ETestFileOpenError;
			}
		printf("File comparison: file: %s is opened\n", f1);
		
		err = fseek(fp1, 0, SEEK_END);
		if(err != 0)
			{
			printf("File comparison: failed to seek file end: %s\n", f1);
			throw ETestFileSeekError;
			}
		f_size1 = ftell(fp1);
		if(f_size1 == -1)
			{
			printf("File comparison: failed to get file size: %s\n", f1);
			throw ETestFileTellError;
			}
		err = fseek(fp1, 0, SEEK_SET);
		if(err != 0)
			{
			printf("File comparison: failed to seek file start: %s\n", f1);
			throw ETestFileSeekError;
			}
		printf("File comparison: Length of file 1: %d\n", f_size1);
		
		
		// Opens file f2 and gets its size
		fp2 = fopen(f2, "rb");
		if(fp2 == NULL) 
			{
			printf("File comparison: failed to open file: %s\n", f2);
			throw ETestFileOpenError;
			}
		printf("File comparison: file: %s is opened\n", f2);
		
		err = fseek(fp2, 0, SEEK_END);
		if(err != 0)
			{
			printf("File comparison: failed to seek file end: %s\n", f1);
			throw ETestFileSeekError;
			}
		f_size2 = ftell(fp2);
		if(f_size2 == -1)
			{
			printf("File comparison: failed to get file size: %s\n", f2);
			throw ETestFileTellError;
			}
		err = fseek(fp2, 0, SEEK_SET);
		if(err != 0)
			{
			printf("File comparison: failed to seek file start: %s\n", f2);
			throw ETestFileSeekError;
			}
		printf("File comparison: Length of file 2: %d\n", f_size2);
		
		
		// Compares their sizes
		if(f_size1 != f_size2) 
			{
			printf("File comparison: Files are different in sizes!\n");
			throw ETestFileSizeDiff;
			}
		
		// Compares their contens byte by byte
		for(TInt32 i = 0; i < f_size1; i ++) 
			{
			TUint8 val1, val2;
			err = fread(&val1,sizeof(TUint8), 1, fp1);
			if (err != 1) 
				{
				printf("File comparison: failed to read file: %s\n", f1);
				throw ETestFileReadError;
				}
			
			err = fread(&val2,sizeof(TUint8), 1, fp2);
			if (err != 1) 
				{
				printf("File comparison: failed to read file: %s\n", f2);
				throw ETestFileReadError;
				}
			
			if( val1 != val2) 
				{
				fclose(fp1);
				fclose(fp2);
				printf("File comparison: Files differ at location: 0x%x.\n", i);
				throw ETestFileDataDiff;
				}
			}
		}
	catch (ETestError)
		{
		if(fp1 != NULL) 
			{
			fclose(fp1);
			}
		if(fp2 != NULL) 
			{
			fclose(fp2);
			}
		throw;  // rethrow the exception
		}

	fclose(fp1);
	fclose(fp2);
	printf("File comparison: Files are identical.\n");
}

TInt32 main()
	{
	try 
		{
		CreateStoreFile();
		RestoreStoreFile();
		ExceptionsTests();
		DescriptorsAPIs();
		TestForDEF113838();
		TestForDEF113845();
		}

	catch (std::bad_alloc& ex )  
		{
		printf("Memory allocation failed: %s\n",ex.what());
		return -1;
		}

	catch (TStoreException& ex) 
		{
		printf("Library Exception %d\n",ex.iErrorCode);
		return -1;
		}

	catch (ETestError& err)
		{
		printf("Test error: %d\n", err);
		return -1;
		}
	printf("PC Store tests passed.\n");
	return 0;
	}

// Creates a store file to externalise some data and compares it to the sample
// store file which is created by Symbian STORE component.
// See the test specification for the details of the data's value and 
// externalization order.
//
// Pointers are defined as auto_ptr to avoid memory leakage. However, due the  
// limitation of stream concurrency in the PCStore library, the pointers will 
// be explicitly deleted before some certain functions, instead of after their 
// lifetimes end.
void CreateStoreFile()
	{
	printf("Test of externalization starts.\n");
	auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Replace(KStorePCName));
	
	//the first stream
	TStreamId id1;
	auto_ptr <CStoreWriteStream> ws (pStore->CreateWriteStream(id1));
	
	(*ws) << KRefTInt8_1;

	// Deletes ws before the new stream can be created.
	delete ws.release();
	
	// the second stream, it would be the root stream of this store
	TStreamId id2;
	ws = auto_ptr <CStoreWriteStream> (pStore->CreateWriteStream(id2));
	
	(*ws) << KRefTReal64;
	(*ws) << KRefTInt8_2;
	(*ws) << KRefTInt16;
	
	// Externalize 8-bit descriptor without header
	// It will be externalized in full length and half length for tests of different APIs
	ws->Write(KRefText8);
	ws->Write(KRefText8, KRefText8.Length()/2);
	
	(*ws) << KRefTReal32;
	(*ws) << KRefTUint8;
	(*ws) << KRefTInt32;
	
	// Externalize 16-bit descriptor without header
	// It will be externalized in full length and half length for tests of different APIs
	ws->Write(KRefText16);
	ws->Write(KRefText16, KRefText16.Length()/2);

	(*ws) << KRefTUint16;
	(*ws) << KRefTUint32;
	(*ws) << KRefTUid;
	
	// Externalize descriptors with headers
	// 8-bit descriptor in various lengths	
	// Append the KRefText8 a few times to make its length more than 8192
	TInt appends = (8192 / KRefText8.Length()) + 1;
	CDes8 refCDes8(KRefText8.Length() * appends);
	while(appends--)
		{
		refCDes8.Append(KRefText8);
		}


	// 8-bit descriptor in various lengths
	refCDes8.SetLength(CARDINAL_1_BYTE_END);
	(*ws) << refCDes8;
	
	refCDes8.SetLength(CARDINAL_2_BYTES_START);
	(*ws) << refCDes8;
	
	refCDes8.SetLength(CARDINAL_2_BYTES_END);
	(*ws) << refCDes8;
	
	refCDes8.SetLength(CARDINAL_4_BYTES_START);
	(*ws) << refCDes8;
	
	// 16-bit descriptor in various lengths, Unicode compression will be applied
	// Append the KRefText16 a few times to make its length more than 8192
	appends = (8192 / KRefText16.Length()) + 1;
	CDes16 refCDes16(KRefText16.Length() * appends);
	while(appends--)
		{
		refCDes16.Append(KRefText16);
		}

	refCDes16.SetLength(CARDINAL_1_BYTE_END);
	(*ws) << refCDes16;
	
	refCDes16.SetLength(CARDINAL_2_BYTES_START);
	(*ws) << refCDes16;
	
	refCDes16.SetLength(CARDINAL_2_BYTES_END);
	(*ws) << refCDes16;
	
	refCDes16.SetLength(CARDINAL_4_BYTES_START);
	(*ws) << refCDes16;
	
	(*ws) << id1;

	// Deletes ws before the new stream can be created in function SetRoot()
	delete ws.release();
	
	pStore->SetRoot(id2);

	// Deletes pStore to close the file before comparison.
	delete pStore.release();
	
	FileCompare(KStorePCName, KStoreSymName);
	
	printf("Test of externalization passed.\n\n");
}
// Restores the root stream from the reference store.
// Checks the restored value with the reference value
// and throws exception ETestDataRestoreError if they do not
// match.
void RestoreRootStream(CStoreReadStream& aRs, TStreamId& aId2)
	{ 
	// Internalize data from the root stream
	TReal64 inTReal64 = 0;
	(aRs) >> inTReal64;
	printf("Restore TReal64: %f\n", inTReal64);
	if(inTReal64!= KRefTReal64 ) 
		{
		printf("File Internalization: TReal64 failed.\n");
		throw ETestDataRestoreError;
		}
	
	TInt8 inTInt8 = 0;
	(aRs) >> inTInt8;
	printf("Restore inTInt8: %d\n", inTInt8);
	if(inTInt8 != KRefTInt8_2) 
		{
		printf("File Internalization: TInt8 failed.\n");
		throw ETestDataRestoreError;
		}
	
	TInt16 inTInt16 = 0;
	(aRs) >> inTInt16;
	printf("Restore TInt16: %d\n", inTInt16);
	if(inTInt16 != KRefTInt16)
		{
		printf("File Internalization: TInt16 failed.\n");
		throw ETestDataRestoreError;
		}
	
	// Internalize 8-bit descriptor without header, in its maximum length
	CDes8 inCDes8(KRefText8.MaxLength());
	aRs.Read(inCDes8);
	printf("Restore CDes8\n") ;
	if(inCDes8 != KRefText8)
		{
		printf("File Internalization: CDes8 failed.\n");
		throw ETestDataRestoreError;
		}
	
	// Internalize 8-bit descriptor without header, in the specified length 
	// (half of it maximum length).
	CDes8 refText8(KRefText8.Ptr(), KRefText8.Length()/2);
	aRs.Read(inCDes8, KRefText8.Length()/2);
	printf("Restore CDes8 in half length.\n") ;
	if(inCDes8 != refText8)
		{
		printf("File Internalization: CDes8 in half length failed.\n");
		throw ETestDataRestoreError;
		}

	TReal32 inTReal32 = 0;
	(aRs) >> inTReal32;
	printf("Restore TReal32: %f\n",inTReal32) ;
	if(inTReal32 != KRefTReal32 ) 
		{
		printf("File Internalization: TReal32 failed.\n");
		throw ETestDataRestoreError;
		}
	
	TUint8 inTUint8 = 0;
	(aRs) >> inTUint8;
	printf("Restore TUint8: %u\n",inTUint8) ;
	if(inTUint8 != KRefTUint8) 
		{
		printf("File Internalization: TUint8 failed.\n");
		throw ETestDataRestoreError;
		}
	
	TInt32 inTInt32 = 0;
	(aRs) >> inTInt32;
	printf("Restore TInt32: %d\n",inTInt32) ;
	if(inTInt32 != KRefTInt32) 
		{
		printf("File Internalization: TInt32 failed.\n");
		throw ETestDataRestoreError;
		}
	
	// Internalize 16-bit descriptor without header
	CDes16 inCDes16(KRefText16.MaxLength());
	aRs.Read(inCDes16);
	printf("Restore CDes16\n") ;
	if(inCDes16 != KRefText16) 
		{
		printf("File Internalization: CDes16 failed.\n");
		throw ETestDataRestoreError;
		}
	

	// Internalize 16-bit descriptor without header, in the specified length 
	// (half of it maximum length).
	CDes16 refText16(KRefText16.Ptr(), KRefText16.Length()/2);
	aRs.Read(inCDes16, KRefText16.Length()/2);
	printf("Restore CDes16 in specified length.\n") ;
	if(inCDes16 != refText16)
		{
		printf("File Internalization: CDes16 in specified length failed.\n");
		throw ETestDataRestoreError;
		}
	

	TUint16 inTUint16 = 0;
	(aRs) >> inTUint16;
	printf("Restore TUint16: %u\n",inTUint16) ;
	if(inTUint16 != KRefTUint16) 
		{
		printf("File Internalization: TUint16 failed.\n");
		throw ETestDataRestoreError;
		}
	
	TUint32 inTUint32 = 0;
	(aRs) >> inTUint32;
	printf("Restore TUint32: %u\n",inTUint32) ;
	if(inTUint32 != KRefTUint32 ) 
		{
		printf("File Internalization: TUint32 failed.\n");
		throw ETestDataRestoreError;
		}
	
	TUid inTUid = {0};
	(aRs) >> inTUid;
	printf("Restore TUid: %d\n",inTUid.iUid) ;
	if(inTUid != KRefTUid) 
		{
		printf("File Internalization: TUid failed.\n");
		throw ETestDataRestoreError;
		}
	
	// Internalize 8-bit descriptor in various lengths	

	// Append the KRefText8 a few times to make length more than 8192
	TInt appends = (8192 / KRefText8.Length()) + 1;
	CDes8 refCDes8(KRefText8.Length() * appends);
	while(appends--)
		{
		refCDes8.Append(KRefText8);
		}

	CDes8 inCDes8_1(CARDINAL_4_BYTES_START);
	
	refCDes8.SetLength(CARDINAL_1_BYTE_END);
	(aRs) >> inCDes8_1;
	printf("Restore CDes8: length = %d\n",inCDes8_1.Length()) ;
	
	if(refCDes8!= inCDes8_1) 
		{
		printf("File Internalization: CDes8 (len 63) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	
	refCDes8.SetLength(CARDINAL_2_BYTES_START);
	(aRs) >> inCDes8_1;
	printf("Restore CDes8: length = %d\n",inCDes8_1.Length()) ;
	
	if(refCDes8 != inCDes8_1) 
		{
		printf("File Internalization: CDes8 (len 64) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	
	refCDes8.SetLength(CARDINAL_2_BYTES_END);
	(aRs) >> inCDes8_1;
	printf("Restore CDes8: length = %d\n",inCDes8_1.Length()) ;
	
	if(refCDes8 != inCDes8_1) 
		{
		printf("File Internalization: CDes8 (len 8191) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	
	refCDes8.SetLength(CARDINAL_4_BYTES_START);
	(aRs) >> inCDes8_1;
	printf("Restore CDes8: length = %d\n",inCDes8_1.Length()) ;
	if(refCDes8 != inCDes8_1) 
		{
		printf("File Internalization: CDes8 (len 8192) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	

	// Internalize 16-bit descriptor in various lengths, Unicode compression will be applied
	// Append the KRefText16 a few times to make its length more than 8192
	appends = (8192 / KRefText16.Length()) + 1;
	CDes16 refCDes16(KRefText16.Length() * appends);
	while(appends--)
		{
		refCDes16.Append(KRefText16);
		}

	CDes16 inCDes16_1(CARDINAL_4_BYTES_START);
	
	refCDes16.SetLength(CARDINAL_1_BYTE_END);
	(aRs) >> inCDes16_1;
	printf("Restore CDes16: length = %d\n",inCDes16_1.Length()) ;
	if(refCDes16 != inCDes16_1) 
		{
		printf("File Internalization: CDes16 (len 63) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	
	refCDes16.SetLength(CARDINAL_2_BYTES_START);
	(aRs) >> inCDes16_1;
	printf("Restore CDes16: length = %d\n",inCDes16_1.Length()) ;
	if(refCDes16 != inCDes16_1) 
		{
		printf("File Internalization: CDes16 (len 64) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	
	refCDes16.SetLength(CARDINAL_2_BYTES_END);
	(aRs) >> inCDes16_1;
	printf("Restore CDes16: length = %d\n",inCDes16_1.Length()) ;
	if(refCDes16 != inCDes16_1) 
		{
		printf("File Internalization: CDes16 (len 8191) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	
	refCDes16.SetLength(CARDINAL_4_BYTES_START);
	(aRs) >> inCDes16_1;
	printf("Restore CDes16: length = %d\n",inCDes16_1.Length()) ;
	if(refCDes16 != inCDes16_1) 
		{
		printf("File Internalization: CDes16 (len 8192) with header  failed.\n");
		throw ETestDataRestoreError;
		}
	
	(aRs) >> aId2;
	printf("Restore TStreamId: %d\n",aId2.Value()) ;
	}


// Restores data from the store file created by Symbian test program with
// the STORE component and compares the restored data to the reference data.
//
// Pointers are defined as auto_ptr to avoid memory leakage. However, due the  
// limitation of stream concurrency in the PCStore library, the pointers will 
// be explicitly deleted before some certain functions, instead of after their 
// lifetimes end.

void RestoreStoreFile()
	{
	printf("Test of internalization starts\n");
	printf("Opens the root stream\n");
	auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Open(KStoreSymName));
	TStreamId id1 = pStore->Root();
	TStreamId id2;

	auto_ptr <CStoreReadStream> rs (pStore->CreateReadStream(id1));

	// restore the root stream
	RestoreRootStream(*rs,id2);	
	delete rs.release();

	// restore another stream
	rs = auto_ptr<CStoreReadStream> (pStore->CreateReadStream(id2));
	TInt8 inTInt8 = 0;
	(*rs) >> inTInt8;
	printf("Restore inTInt8: %d\n",inTInt8);
	
	if(inTInt8 != KRefTInt8_1) 
		{
		printf("File Internalization: TInt8 failed.\n");
		throw ETestDataRestoreError;
		}
	printf("Test of Internalization passed.\n\n");
	}


// CDirectFileStore should fail to be initialized for writing 
// with an invalid file name. Exception EFileOpenError will be caught.
void ExceptionsTest1()
	{
	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Replace(KStorePCInValidName));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EFileOpenError)
			{
			return;
			}
		}
	throw ETestExceptionTestFailure;
	}

// Read stream should fail to be created on the CDirectFileStore which has been 
// created for writing. Exception EStoreModeError will be caught 
void ExceptionsTest2()
	{
	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Replace(KStorePCNameExc));
		TStreamId id = pStore->Root();
		auto_ptr <CStoreReadStream> rs (pStore->CreateReadStream(id));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EStoreModeError)
			{
			return;
			}
		}
		
	throw ETestExceptionTestFailure;
	}

// No more than one write stream can be created on a CDirectFileStore.
// Exception EStreamExclusionError will be caught on the creation of 
// the second write stream.
void ExceptionsTest3()
	{
	try
		{
		TStreamId id1, id2;
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Replace(KStorePCNameExc));
	    auto_ptr <CStoreWriteStream> ws1 (pStore->CreateWriteStream(id1));
		auto_ptr <CStoreWriteStream> ws2 (pStore->CreateWriteStream(id2));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EStreamExclusionError)
			{
			return;
			}
		}
		
	throw ETestExceptionTestFailure;
	}


// CDirectFileStore should fail to be initialized for reading
// with an invalid file name. Exception EFileOpenError will be caught 
void ExceptionsTest4()
	{
	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Open(KStorePCInValidName));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EFileOpenError)
			{
			return;
			}
		}
		
	throw ETestExceptionTestFailure;
	}


// CDirectFileStore should fail to be initialized for reading
// if given a name of an invalid store file. Exception EStoreNotSupported will be caught 
void ExceptionsTest5()
	{
 	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Open(KNonStoreFileName));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EStoreNotSupported)
			{
			return;
			}
		}
		
	throw ETestExceptionTestFailure;
	}

// CDirectFileStore should fail to be initialized for reading
// if given a name of a store file containing no stream. 
// Exception EStoreNotSupported will be caught 
void ExceptionsTest6()
	{
	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Open(KStoreSymNameNoStream));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EStoreNotSupported)
			{
			return;
			}
		}
		
	throw ETestExceptionTestFailure;
	}

// Write stream cannot be created on the CDirectFileStore which has been 
// created for reading. Exception EStoreModeError will be caught 
void ExceptionsTest7()
	{
	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Open(KStorePCName));
		TStreamId id;
		auto_ptr <CStoreWriteStream> ws (pStore->CreateWriteStream(id));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EStoreModeError)
			{
			return;
			}
		}
		
	throw ETestExceptionTestFailure;
	} 

// No more than one read stream can be created on a CDirectFileStore.
// Exception EStreamExclusionError will be caught on the creation of 
// the second read stream.
void ExceptionsTest8()
	{
	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Open(KStorePCName));
		TStreamId id = pStore->Root();
		auto_ptr <CStoreReadStream> rs1 (pStore->CreateReadStream(id));
		auto_ptr <CStoreReadStream> rs2 (pStore->CreateReadStream(id));
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EStreamExclusionError)
			{
			return;
			}
		}
	
	throw ETestExceptionTestFailure;
	} 


// Read error should be detected if the program try to read more data 
// after all data has been read from the store file. Exception EFileReadError 
// will be caught on any exra read operation.
void ExceptionsTest9()
	{
	try
		{
		auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Open(KStorePCName));

		TStreamId id = pStore->Root(), id2 = 0;
		auto_ptr <CStoreReadStream> rs (pStore->CreateReadStream(id));

		// Restore the root stream. There should not be any data
		// in the store after this function
		RestoreRootStream(*rs, id2);
		rs->ReadInt8();
		}
	catch (TStoreException& ex) 
		{
		if(ex.iErrorCode == TStoreException::EFileReadError)
			{
			return;
			}
		}
	
	throw ETestExceptionTestFailure;
	} 

// Tests of Error Handling
// Test cases emulate happening conditions of exceptions to trigger them.
void ExceptionsTests()
	{
	printf("Test of Exceptions starts.\n\n");
	ExceptionsTest1();
	ExceptionsTest2();
	ExceptionsTest3();
	ExceptionsTest4();
	ExceptionsTest5();
	ExceptionsTest6();
	ExceptionsTest7();
	ExceptionsTest8();
	ExceptionsTest9();
	printf("Test of Exceptions passed.\n\n");
	}

// Test of descriptors' constructors
// Test cases create descriptors with different constructors
void DesConstructors()
	{
	// Construct 8-bit descriptors
	auto_ptr <CDes8>  conDes8;

	//test CDes8::CDes8() 
	conDes8 = auto_ptr <CDes8> (new CDes8);
	if(! ((conDes8->Length() == 0) && (conDes8->MaxLength() == 0) && (conDes8->Size() == 0)))
		{
		printf("Test of Constructor CDes8::CDes8() failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::CDes8(TInt32 iMaxLength) 
	conDes8 = auto_ptr <CDes8> (new CDes8(KString8Length));
	if(! ((conDes8->Length() == 0) && (conDes8->MaxLength() == KString8Length) 
		&& (conDes8->Size() == KString8Size)))
		{
		printf("Test of Constructor CDes8::CDes8(TInt32 iMaxLength) failed.\n");
		throw ETestExceptionTestFailure;
		}
	
	//test CDes8::CDes8(const TUint8* aPtr) 
	conDes8 = auto_ptr <CDes8> (new CDes8(KString8));
	if(! ((conDes8->Length() == KString8Length) && (conDes8->MaxLength() == KString8Length) 
		&& (conDes8->Size() == KString8Size) &&  memcmp(conDes8->Ptr(), KString8, KString8Length) == 0))
		{
		printf("Test of Constructor CDes8::CDes8(const TUint8* aPtr) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::CDes8(const TUint8* aPtr, TInt32 aLength) 
	conDes8 = auto_ptr <CDes8> (new CDes8(KString8, KString8HalfLength));
	if(! ((conDes8->Length() == KString8HalfLength) && (conDes8->MaxLength() == KString8HalfLength)
		&& (conDes8->Size() == KString8HalfSize) &&  memcmp(conDes8->Ptr(), KString8, KString8HalfLength) == 0))
		{
		printf("Test of Constructor CDes8::CDes8(const TUint8* aPtr, TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::CDes8(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength) 
	conDes8 = auto_ptr <CDes8> (new CDes8(KString8, KString8HalfLength, KString8Length));
	if(! ((conDes8->Length() == KString8HalfLength) && (conDes8->MaxLength() == KString8Length)
		&& (conDes8->Size() == KString8Size) &&  memcmp(conDes8->Ptr(), KString8, KString8HalfLength) == 0))
		{
		printf("Test of Constructor CDes8::CDes8(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::CDes8(const CDes8& aDes) 
	auto_ptr <CDes8> conDes8_1 (new CDes8(*conDes8));
	if(! ((conDes8_1->Length() == conDes8->Length()) && (conDes8_1->MaxLength() == conDes8->MaxLength())
		&& (conDes8->Size() == conDes8->Size()) &&  memcmp(conDes8->Ptr(), conDes8->Ptr(), conDes8->Length()) == 0))
		{
		printf("Test of Constructor CDes8(const CDes8& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	// Construct 16-bit descriptors
	auto_ptr <CDes16> conDes16;

	//test CDes16::CDes16() 
	conDes16 = auto_ptr <CDes16> (new CDes16);
	if(! ((conDes16->Length() == 0) && (conDes16->MaxLength() == 0) && (conDes16->Size() == 0)))
		{
		printf("Test of Constructor CDes16::CDes16() failed.\n");
		throw ETestExceptionTestFailure;
		}
	
	//test CDes16::CDes16(TInt32 iMaxLength) 
	conDes16 = auto_ptr <CDes16> (new CDes16(KString16Length));
	if(! ((conDes16->Length() == 0) && (conDes16->MaxLength() == KString16Length) 
		&& (conDes16->Size() == KString16Size)))
		{
		printf("Test of Constructor CDes16::CDes16(TInt32 iMaxLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes16::CDes16(const TUint16* aPtr) 
	conDes16 = auto_ptr <CDes16> (new CDes16(KString16));
	if(! ((conDes16->Length() == KString16Length) && (conDes16->MaxLength() == KString16Length) 
		&& (conDes16->Size() == KString16Size) &&  memcmp(conDes16->Ptr(), KString16, KString16Length) == 0))
		{
		printf("Test of Constructor CDes16::CDes16(const TUint16* aPtr) failed.\n");
		throw ETestExceptionTestFailure;
		}
	
	//test CDes16::CDes16(const TUint16* aPtr, TInt32 aLength) 
	conDes16 = auto_ptr <CDes16> (new CDes16(KString16, KString16HalfLength));
	if(! ((conDes16->Length() == KString16HalfLength) && (conDes16->MaxLength() == KString16HalfLength)
		&& (conDes16->Size() == KString16HalfSize) &&  memcmp(conDes16->Ptr(), KString16, KString16HalfLength) == 0))
		{
		printf("Test of Constructor CDes16::CDes16(const TUint16* aPtr, TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}
	
	//test CDes16::CDes16(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength) 
	conDes16 = auto_ptr <CDes16> (new CDes16(KString16, KString16HalfLength, KString16Length));
	if(! ((conDes16->Length() == KString16HalfLength) && (conDes16->MaxLength() == KString16Length)
		&& (conDes16->Size() == KString16Size) &&  memcmp(conDes16->Ptr(), KString16, KString16HalfLength) == 0))
		{
		printf("Test of Constructor CDes16::CDes16(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes16::CDes16(const CDes16& aDes) 
	auto_ptr <CDes16> conDes16_1 (new CDes16(*conDes16));
	if(! ((conDes16_1->Length() == conDes16->Length()) && (conDes16_1->MaxLength() == conDes16->MaxLength())
		&& (conDes16->Size() == conDes16->Size()) &&  memcmp(conDes16->Ptr(), conDes16->Ptr(), conDes16->Length()) == 0))
		{
		printf("Test of Constructor CDes16::CDes16(const CDes16& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}
	}

// Test of descriptors' modifiers
// Test cases changes descriptors with the modifiers
void DesModifiers()
	{
	// Modifiers of CDes8
	CDes8 modDes8, modDes8_1(KString8);

	//test CDes8& operator=(const CDes8& aDes)
	modDes8 = modDes8_1;
	if(! ( (modDes8.Length() == modDes8_1.Length()) && (modDes8.MaxLength() == modDes8_1.MaxLength()) 
		&& (modDes8.Size() == modDes8.Size()) &&  memcmp(modDes8.Ptr(), modDes8_1.Ptr(), modDes8.Length()) == 0))
		{
		printf("Test of Modifier CDes8& operator=(const CDes8& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::Set(const CDes8& aDes)
	modDes8.Set(modDes8_1);
	if(! ( (modDes8.Length() == modDes8_1.Length()) && (modDes8.MaxLength() == modDes8_1.MaxLength()) 
		&& (modDes8.Size() == modDes8.Size()) &&  memcmp(modDes8.Ptr(), modDes8_1.Ptr(), modDes8.Length()) == 0))
		{
		printf("Test of Modifier CDes8::Set(const CDes8& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}
	
	//test CDes8::Set(const TUint8* aPtr, TInt32 aLength)
	modDes8.Set(KString8, KString8HalfLength);
	if(! ( (modDes8.Length() == KString8HalfLength) && (modDes8.MaxLength() == KString8HalfLength) 
		&& (modDes8.Size() == KString8HalfSize ) &&  memcmp(modDes8.Ptr(), KString8, KString8HalfLength) == 0))
		{
		printf("Test of Modifier CDes8::Set(const TUint8* aPtr, TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::Set(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength)
	modDes8.Set(KString8, KString8HalfLength, KString8Length);
	if(! ( (modDes8.Length() == KString8HalfLength) && (modDes8.MaxLength() == KString8Length) 
		&& (modDes8.Size() == KString8Size ) &&  memcmp(modDes8.Ptr(), KString8, KString8HalfLength) == 0))
		{
		printf("Test of Modifier CDes8::Set(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::SetLength(TInt32 aLength) 
	modDes8.SetLength(KString8HalfLength / 2);
	if(! ( (modDes8.Length() == KString8HalfLength / 2) && (modDes8.MaxLength() == KString8Length) 
		&& (modDes8.Size() == KString8Size) &&  memcmp(modDes8.Ptr(), KString8, KString8Length /2) == 0))
		{
		printf("Test of Modifier SetLength(TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::Append(const CDes8& aDes)
	CDes8 modDes8_app(KString8Length_aft_app);
	modDes8_app.Append(modDes8_1);
	if(! ( (modDes8_app.Length() == KString8Length) && (modDes8_app.MaxLength() == KString8Length_aft_app) 
		&& (modDes8_app.Size() == KString8Size_aft_app) &&  memcmp(modDes8_app.Ptr(), KString8, KString8Length) == 0))
		{
		printf("Test of Modifier CDes8::Append(const CDes8& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes8::Append(const CDes8& aDes)
	modDes8_app.Append(KString8_app, KString8Length_app);
	if(! ( (modDes8_app.Length() == KString8Length_aft_app) && (modDes8_app.MaxLength() == KString8Length_aft_app ) 
		&& (modDes8_app.Size() == KString8Size_aft_app) &&  memcmp(modDes8_app.Ptr(), KString8_aft_app, KString8Length_aft_app) == 0))
		{
		printf("Test of Modifier CDes8::Append(const TUint8* aPtr, TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}


	// Modifiers of CDes16
	CDes16 modDes16, modDes16_1(KString16);

	//test CDes16::Set(const CDes16& aDes)
	modDes16 = modDes16_1;
	if(! ( (modDes16.Length() == modDes16_1.Length()) && (modDes16.MaxLength() == modDes16_1.MaxLength()) 
		&& (modDes16.Size() == modDes16.Size()) 
		&& wmemcmp(reinterpret_cast<const wchar_t *>(modDes16.Ptr()), reinterpret_cast<const wchar_t *>(modDes16_1.Ptr()), modDes16.Length()) == 0))
		{
		printf("Test of Modifier CDes16& operator=(const CDes16& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes16::Set(const CDes16& aDes)
	modDes16.Set(modDes16_1);
	if(! ( (modDes16.Length() == modDes16_1.Length()) && (modDes16.MaxLength() == modDes16_1.MaxLength()) 
		&& (modDes16.Size() == modDes16.Size()) 
		&& wmemcmp(reinterpret_cast<const wchar_t *>(modDes16.Ptr()), reinterpret_cast<const wchar_t *>(modDes16_1.Ptr()), modDes16.Length()) == 0))
		{
		printf("Test of Modifier CDes16::Set(const CDes16& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}
	
	//test CDes16::Set(const TUint16* aPtr, TInt32 aLength)
	modDes16.Set(KString16, KString16HalfLength);
	if(! ( (modDes16.Length() == KString16HalfLength) && (modDes16.MaxLength() == KString16HalfLength) 
		&& (modDes16.Size() == KString16HalfSize ) 
		&&  wmemcmp(reinterpret_cast<const wchar_t *>(modDes16.Ptr()), reinterpret_cast<const wchar_t *>(KString16), KString16HalfLength) == 0))
		{
		printf("Test of Modifier CDes16::Set(const TUint16* aPtr, TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes16::Set(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength)
	modDes16.Set(KString16, KString16HalfLength, KString16Length);
	if(! ( (modDes16.Length() == KString16HalfLength) && (modDes16.MaxLength() == KString16Length) 
		&& (modDes16.Size() == KString16Size) 
		&&  wmemcmp(reinterpret_cast<const wchar_t *>(modDes16.Ptr()), reinterpret_cast<const wchar_t *>(KString16), KString16HalfLength) == 0))
		{
		printf("Test of Modifier CDes16::Set(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes16::SetLength(TInt32 aLength) 
	modDes16.SetLength(KString16HalfLength / 2);
	if(! ( (modDes16.Length() == KString16HalfLength / 2) && (modDes16.MaxLength() == KString16Length) 
		&& (modDes16.Size() == KString16Size) 
		&&  wmemcmp(reinterpret_cast<const wchar_t *>(modDes16.Ptr()), reinterpret_cast<const wchar_t *>(KString16), KString16HalfLength / 2) == 0))
		{
		printf("Test of Modifier SetLength(TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes16::Append(const CDes16& aDes)
	CDes16 modDes16_app(KString16Length_aft_app);
	modDes16_app.Append(modDes16_1);
	if(! ( (modDes16_app.Length() == KString16Length) && (modDes16_app.MaxLength() == KString16Length_aft_app) 
		&& (modDes16_app.Size() == KString16Size_aft_app) 
		&&  wmemcmp(reinterpret_cast<const wchar_t *>(modDes16_app.Ptr()), reinterpret_cast<const wchar_t *>(KString16), KString16Length) == 0))
		{
		printf("Test of Modifier CDes16::Append(const CDes16& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	//test CDes16::Append(const TUint16* aPtr, TInt32 aLength)
	modDes16_app.Append(KString16_app, KString16Length_app);
	if(! ( (modDes16_app.Length() == KString16Length_aft_app) && (modDes16_app.MaxLength() == KString16Length_aft_app ) 
		&& (modDes16_app.Size() == KString16Size_aft_app) 
		&&  wmemcmp(reinterpret_cast<const wchar_t *>(modDes16_app.Ptr()), reinterpret_cast<const wchar_t *>(KString16_aft_app), KString16Length_aft_app) == 0))
		{
		printf("Test of Modifier CDes16::Append(const TUint16* aPtr, TInt32 aLength) failed.\n");
		throw ETestExceptionTestFailure;
		}
	}

// Test of descriptors' comparison operators
// Test cases comapares descriptors with the comparison operators
void DesCompOperators()
	{
	// Comparison operators of CDes8
	CDes8 opeDes8_1(KString8), opeDes8_2(KString8), opeDes8_3(KString8_app);

	if( (! (opeDes8_1 == opeDes8_2)) || opeDes8_1 == opeDes8_3)
		{
		printf("Test of Comparison operator CDes8::operator==(const CDes8& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	if( (opeDes8_1 != opeDes8_2) || (!(opeDes8_1 != opeDes8_3)))
		{
		printf("Test of Comparison operator CDes8::operator!=(const CDes8& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	// Comparison operators of CDes16
	CDes16 opeDes16_1(KString16), opeDes16_2(KString16), opeDes16_3(KString16_app);

	if( (! (opeDes16_1 == opeDes16_2)) || opeDes16_1 == opeDes16_3)
		{
		printf("Test of Comparison operator CDes16::operator==(const CDes16& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}

	if( (opeDes16_1 != opeDes16_2) || (!(opeDes16_1 != opeDes16_3)))
		{
		printf("Test of Comparison operator CDes16::operator!=(const CDes16& aDes) failed.\n");
		throw ETestExceptionTestFailure;
		}
	}

// Descriptors' APIs tests
// Call all the descriptors' APIs to verify whether they behave properly
void DescriptorsAPIs()
	{
	printf("Test of Descriptors APIs starts.\n");
	DesConstructors();
	DesModifiers();
	DesCompOperators();
	printf("Test of Descriptors APIs passed.\n\n");
	}

/**
@SYMTestCaseID          SYSLIB-STORE-UT-4002
@SYMTestCaseDesc        Testing the fix for DEF113838: PCStore does not permit setting store uid
@SYMTestPriority        High
@SYMTestActions         Run newly overloaded CDirectFileStore::Replace with valid and invalid uid types.
@SYMTestExpectedResults The method should functioning correctly, a exception should be threw when invalid uid
						type is passed in.
@SYMDEF                 DEF113838
*/
void TestForDEF113838()
{
	printf("  @SYMTestCaseID:SYSLIB-STORE-UT-4002  Test of new CDirectFileStore::Replace() starts.\n");
	
	const TUid KTestTUid = {0x100}; // a random uid
	const TUidType KTestTUidType (KDirectFileStoreLayoutUid, KTestTUid, KTestTUid); // valid
	const TUidType KTestTUidTypeWithNullUid (KDirectFileStoreLayoutUid); // valid
	const TUidType KInvalidTUidType (KTestTUid); // invalid

	auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Replace(KStorePCName, KTestTUidType));
	TStreamId id;
	auto_ptr <CStoreWriteStream> ws (pStore->CreateWriteStream(id));
	delete ws.release();
	pStore->SetRoot(id);
	delete pStore.release();
	//reopen the store, this will call CDirectFileStore::CheckType()
	pStore = auto_ptr <CDirectFileStore> (CDirectFileStore::Open(KStorePCName));
	delete pStore.release();

	pStore = auto_ptr <CDirectFileStore> (CDirectFileStore::Replace(KStorePCName, KTestTUidTypeWithNullUid));
	ws = auto_ptr <CStoreWriteStream> (pStore->CreateWriteStream(id));
	delete ws.release();
	pStore->SetRoot(id);
	delete pStore.release();
	pStore = auto_ptr <CDirectFileStore> (CDirectFileStore::Open(KStorePCName));
	delete pStore.release();

	// Test with a invalid uid type, an EStoreNotSupported exception should be threw
	try
		{
		pStore = auto_ptr <CDirectFileStore> (CDirectFileStore::Replace(KStorePCName, KInvalidTUidType));
		}
	catch (TStoreException& ex)
		{
		if(ex.iErrorCode != TStoreException::EStoreNotSupported)
			{
			throw ex;
			}
		delete pStore.release();
		printf("Test of new CDirectFileStore::Replace() passed.\n\n");
		return;
		}
	delete pStore.release();
	throw ETestExceptionTestFailure; // in case that no exception is threw. 
}

/**
@SYMTestCaseID          SYSLIB-STORE-UT-4003
@SYMTestCaseDesc        Testing the fix for DEF113845: PCstore not usable because TInt64 not available
@SYMTestPriority        High
@SYMTestActions         Run newly added externalize and internalize functions for newly defined TInt64 and TUint64.
						Try writing 64-bit integer data to stream and then restore it from stream.
@SYMTestExpectedResults The restored data should be same as the original
@SYMDEF                 DEF113845
*/
void TestForDEF113845()
{
	printf("@SYMTestCaseID:SYSLIB-STORE-UT-4003  Test of 64-bit integer APIs starts.\n");
	
	auto_ptr <CDirectFileStore> pStore (CDirectFileStore::Replace(KStorePCName));
	TStreamId id;
	auto_ptr <CStoreWriteStream> ws (pStore->CreateWriteStream(id));
	
	(*ws) << KRefTInt64;
	(*ws) << KRefTUint64;
	
	delete ws.release();
	pStore->SetRoot(id);
	delete pStore.release();
	
	// reopen
	pStore = auto_ptr <CDirectFileStore> (CDirectFileStore::Open(KStorePCName));
	id = pStore->Root();
	auto_ptr <CStoreReadStream> rs (pStore->CreateReadStream(id));
	
	TInt64 inTInt64 = 0;
	(*rs) >> inTInt64;
	printf("Restore TInt64: %I64d\n", inTInt64);
	if(inTInt64!= KRefTInt64 ) 
		{
		printf("Restore TInt64 failed.\n");
		throw ETestDataRestoreError;
		}

	TUint64 inTUint64 = 0;
	(*rs) >> inTUint64;
	printf("Restore TUint64: %I64u\n", inTUint64);
	if(inTUint64!= KRefTUint64 ) 
		{
		printf("Restore TUInt64 failed.\n");
		throw ETestDataRestoreError;
		}

	delete rs.release();
	delete pStore.release();
	
	printf("Test of 64-bit integer APIs passed.\n\n");
}
