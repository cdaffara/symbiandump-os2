// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stdio.h>
#include <new>
#include <pcstore/pcstore.h>
#include <pcstore/pcstoreexamplesdef.h>

// File names used in this program
const char* KStorePCName = "./examplestore";

void CreateStoreFile();
void RestoreStoreFile();
void RestoreRootStream(CStoreReadStream& aRs, TStreamId& aId2);

TInt32 main()
	{
	try 
		{
		CreateStoreFile();
		RestoreStoreFile();
		}
	catch (std::bad_alloc& ex )  
		{
		printf("Memory allocation failed: %s\n",ex.what());
		return -1;
		}
	catch (TStoreException& ex) 
		{
		printf("Library Exception: %d\n",ex.iErrorCode);
		return -1;
		}
	return 0;
	}
	
// Creates a store file.
// The store file has 2 streams. 
// All supported data type will be externalized to the root stream.
// Please note that the memory that ws is pointing to will be leaked 
// if an exception is thrown. The actual program should handle this 
// kind of leakage by deleting the pointer on exception or using auto_ptr.
void CreateStoreFile()
	{
	printf("Create a store file.\n");
	CDirectFileStore * pStore = CDirectFileStore::Replace(KStorePCName);
	
	//the first stream
	TStreamId id1;
	CStoreWriteStream * ws = pStore->CreateWriteStream(id1);
		
	(*ws) << KRefTInt8_1;
	delete ws;
	
	// the second stream, it would be the root stream of this store
	TStreamId id2;
	ws = pStore->CreateWriteStream(id2);
	
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
	// 8-bit descriptor
	(*ws) << KRefText8;
	
	// 16-bit descriptor, Unicode compression will be applied
	(*ws) << KRefText16;
	
	(*ws) << id1;
	
	delete ws;
	
	pStore->SetRoot(id2);
	delete pStore;
	
	printf("The store is created.\n\n");
}


// Restores data from the store file.
// Please note that the memory that rs is pointing to will be leaked 
// if an exception is thrown. The actual program should handle this 
// kind of leakage by deleting the pointer on exception or using auto_ptr.
void RestoreStoreFile()
	{
	printf("Restore from the store file.\n");
	printf("Open the root stream\n");
	CDirectFileStore * pStore = CDirectFileStore::Open(KStorePCName );
	TStreamId id1 = pStore->Root();
	TStreamId id2;
	
	CStoreReadStream * rs = pStore->CreateReadStream(id1);
	
	// restore the root stream
	RestoreRootStream(*rs,id2);	
	delete rs;

	// restore from another stream
	rs = pStore->CreateReadStream(id2);
	TInt8 inTInt8 = 0;
	(*rs) >> inTInt8;
	printf("Restore inTInt8: %d\n",inTInt8);
	
	delete rs; 
	
	delete pStore;
	}

// Restores data from the root stream of the store.
void RestoreRootStream(CStoreReadStream& aRs, TStreamId& aId2)
	{ 
	// Internalize data from the root stream
	TReal64 inTReal64 = 0;
	(aRs) >> inTReal64;
	printf("Restore TReal64: %f\n", inTReal64);
	
	TInt8 inTInt8 = 0;
	(aRs) >> inTInt8;
	printf("Restore inTInt8: %d\n", inTInt8);
	
	TInt16 inTInt16 = 0;
	(aRs) >> inTInt16;
	printf("Restore TInt16: %d\n", inTInt16);
	
	// Internalize 8-bit descriptor without header, in its maximum length
	CDes8 inCDes8(KRefText8.MaxLength());
	aRs.Read(inCDes8);
	printf("Restore CDes8\n") ;
	
	// Internalize 8-bit descriptor without header, in the specified length 
	// (half of it maximum length).
	CDes8 refText8(KRefText8.Ptr(), KRefText8.Length()/2);
	aRs.Read(inCDes8, KRefText8.Length()/2);
	printf("Restore CDes8 in half length.\n") ;

	TReal32 inTReal32 = 0;
	(aRs) >> inTReal32;
	printf("Restore TReal32: %f\n",inTReal32) ;
	
	TUint8 inTUint8 = 0;
	(aRs) >> inTUint8;
	printf("Restore TUint8: %u\n",inTUint8) ;
	
	TInt32 inTInt32 = 0;
	(aRs) >> inTInt32;
	printf("Restore TInt32: %d\n",inTInt32) ;
	
	// Internalize 16-bit descriptor without header
	CDes16 inCDes16(KRefText16.MaxLength());
	aRs.Read(inCDes16);
	printf("Restore CDes16\n") ;

	// Internalize 16-bit descriptor without header, in the specified length 
	// (half of it maximum length).
	CDes16 refText16(KRefText16.Ptr(), KRefText16.Length()/2);
	aRs.Read(inCDes16, KRefText16.Length()/2);
	printf("Restore CDes16 in specified length.\n") ;

	TUint16 inTUint16 = 0;
	(aRs) >> inTUint16;
	printf("Restore TUint16: %u\n",inTUint16) ;
	
	TUint32 inTUint32 = 0;
	(aRs) >> inTUint32;
	printf("Restore TUint32: %u\n",inTUint32) ;
	
	TUid inTUid = {0};
	(aRs) >> inTUid;
	printf("Restore TUid: %d\n",inTUid.iUid) ;
	
	// Internalize 8-bit descriptor
	CDes8 inCDes8_1(KRefText8.Length());
	(aRs) >> inCDes8_1;
	printf("Restore CDes8: length = %d\n",inCDes8_1.Length()) ;

	// Internalize 16-bit descriptor, Unicode compression will be applied
	CDes16 inCDes16_1(KRefText16.Length() );
	(aRs) >> inCDes16_1;
	printf("Restore CDes16: length = %d\n",inCDes16_1.Length()) ;
	
	(aRs) >> aId2;
	printf("Restore TStreamId: %d\n",aId2.Value()) ;
	}
