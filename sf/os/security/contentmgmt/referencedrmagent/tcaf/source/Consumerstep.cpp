/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <test/testexecutelog.h>
#include <caf/content.h>
#include <caf/data.h>
#include <caf/attribute.h>
#include "cafserver.h"
#include "bitset.h"
#include "Consumerstep.h"
#include "attributeset.h"
#include "stringattributeset.h"
#include <caf/virtualpath.h>
#include <e32hal.h>
#include <hal.h>

using namespace ContentAccess;

const TInt KCafTestMaxDataTypeLength = 255;
const TInt KCafTestSecToMS = 1000;

_LIT(KCDataThreadSemaphore, "CData_Thread_Test");

/* 
 * Step1 compares an agent 'size' call with that obtained by RFile
 *
 */

CCAFSizeStep::~CCAFSizeStep()
	{
	}

CCAFSizeStep::CCAFSizeStep(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFSizeStep);
	}

TVerdict CCAFSizeStep::doTestStepL()
	{
	__UHEAP_MARK;
	
	SetTestStepResult(EInconclusive);
	
	INFO_PRINTF1(_L("Basic Open Content Test"));
	
	TPtrC uri1;
	TPtrC uri2;
	GetStringFromConfig(ConfigSection(),_L("URI1"),uri1);
	GetStringFromConfig(ConfigSection(),_L("URI2"),uri2);

	INFO_PRINTF2(_L("Creating CContent object with uri: %S"), &uri1);
	CContent* content = CContent::NewLC(uri1);
	INFO_PRINTF1(_L("Creating CData object from content"));

	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	TInt size = 0;
	data->DataSizeL(size);
	INFO_PRINTF2(_L("Size of content from caf CContent::OpenContentL(): %d"),size);
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(content);

	INFO_PRINTF2(_L("Creating CData object with uri: %S"), &uri1);
	TInt size1 = 0;
	
	TBool withoutIntent;
	if (!GetBoolFromConfig(ConfigSection(),_L("WITHOUTINTENT"),withoutIntent))
		{
		INFO_PRINTF1(_L("With Intent"));
		data = CData::NewL(TVirtualPathPtr(uri1), EPeek, EContentShareReadOnly);			
		}
	else
		{
		INFO_PRINTF1(_L("Without Intent"));
		data = CData::NewL(TVirtualPathPtr(uri1), EContentShareReadOnly);
		data->SetProperty(EAgentPropertyAgentUI, 0);
		User::LeaveIfError(data->EvaluateIntent(EPeek));		
		}
	
	CleanupStack::PushL(data);
	data->DataSizeL(size1);
	INFO_PRINTF2(_L("Size of content from caf CData::NewL(): %d"),size1);
	CleanupStack::PopAndDestroy(data);

	INFO_PRINTF2(_L("Opening standard RFile interface with uri: %S"),&uri2);
	RFile file;
	User::LeaveIfError(file.Open(iParent.Fs(), uri2, EFileRead));

	TInt size2;
	file.Size(size2);
	file.Close();

	INFO_PRINTF2(_L("Size of content from f32: %d"),size2);

	if(size == size2 && size == size1)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
/* 
 * This is the 64bit version of CCAFSizeStep
 */

CCAFSizeStep64::~CCAFSizeStep64()
	{
	}

CCAFSizeStep64::CCAFSizeStep64(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFSizeStep);
	}

TVerdict CCAFSizeStep64::doTestStepL()
	{
	__UHEAP_MARK;
	
	SetTestStepResult(EInconclusive);
	
	INFO_PRINTF1(_L("Basic Open Content Test"));
	
	TPtrC uri1;
	TPtrC uri2;
	GetStringFromConfig(ConfigSection(),_L("URI1"),uri1);
	GetStringFromConfig(ConfigSection(),_L("URI2"),uri2);

	INFO_PRINTF2(_L("Creating CContent object with uri: %S"), &uri1);
	CContent* content = CContent::NewLC(uri1);
	INFO_PRINTF1(_L("Creating CData object from content"));

	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	TInt64 size = 0;
	data->DataSize64L(size);
	INFO_PRINTF2(_L("Size of content from caf CContent::OpenContentL(): %Ld"),size);
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(content);

	INFO_PRINTF2(_L("Creating CData object with uri: %S"), &uri1);
	TInt64 size1 = 0;
	
	TBool withoutIntent;
	if (!GetBoolFromConfig(ConfigSection(),_L("WITHOUTINTENT"),withoutIntent))
		{
		INFO_PRINTF1(_L("With Intent"));
		data = CData::NewL(TVirtualPathPtr(uri1), EPeek, EContentShareReadOnly);			
		}
	else
		{
		INFO_PRINTF1(_L("Without Intent"));
		data = CData::NewL(TVirtualPathPtr(uri1), EContentShareReadOnly);
		data->SetProperty(EAgentPropertyAgentUI, 0);
		User::LeaveIfError(data->EvaluateIntent(EPeek));		
		}
	
	CleanupStack::PushL(data);
	data->DataSize64L(size1);
	INFO_PRINTF2(_L("Size of content from caf CData::NewL(): %Ld"),size1);
	CleanupStack::PopAndDestroy(data);

	INFO_PRINTF2(_L("Opening standard RFile interface with uri: %S"),&uri2);
	RFile64 file;
	User::LeaveIfError(file.Open(iParent.Fs(), uri2, EFileRead));

	TInt64 size2;
	file.Size(size2);
	file.Close();

	INFO_PRINTF2(_L("Size of content from f32: %Ld"),size2);

	if(size == size2 && size == size1)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

/*
 * This step compares a seek and read using CAF with that using RFile
 *
 */

CCAFSeekReadStep::~CCAFSeekReadStep()
	{
	}

CCAFSeekReadStep::CCAFSeekReadStep(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFSeekReadStep);
	}

void CCAFSeekReadStep::StandardCheckReturnValue(TInt aReturnValue)
	{
	if (aReturnValue != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR Unexpected return value: %d"),aReturnValue);
		SetTestStepResult(EFail);
		}	
	}
TVerdict CCAFSeekReadStep::doTestStepL()
	{
	SetTestStepResult(EInconclusive);
	
	TBuf8<2> buf1;
	TBuf8<2> buf2;
	TRequestStatus status;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Basic Open Content Test"));
	
	TPtrC uri1;
	TPtrC uri2;
	GetStringFromConfig(ConfigSection(),_L("URI1"),uri1);
	GetStringFromConfig(ConfigSection(),_L("URI2"),uri2);

	INFO_PRINTF2(_L("Creating content object with uri: %S"), &uri1);
	CContent* content = CContent::NewLC(uri1);

	INFO_PRINTF1(_L("Creating data object from content"));
	CData* data = content->OpenContentL(EPeek);
	
	// don't need CContent any more
	CleanupStack::PopAndDestroy(content);
	CleanupStack::PushL(data);

	// Now, open the same file using RFile
	RFile file;
	INFO_PRINTF2(_L("Opening standard RFile interface with uri: %S"),&uri2);
	User::LeaveIfError(file.Open(iParent.Fs(), uri2, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);

	TInt size = 0;
	file.Size(size);
	INFO_PRINTF2(_L("Size of content from f32: %d"),size);
	
	// Seek and read from start using both CAF and RFile
	TInt pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	TInt pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	data->Read(buf1);
	file.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekStart test using CData: %d"), pos1);
	INFO_PRINTF2(_L("Position from ESeekStart test using RFile: %d"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != size/4)
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// set the location within the file then retrieve the current location within the file
	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent, pos2);

	INFO_PRINTF2(_L("Current position using CData: %d"), pos1);
	INFO_PRINTF2(_L("Current position using RFile: %d"), pos2);

	if(pos1 != pos2 || pos1 != (size/4 + buf1.Length()))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Seek and read from current using both CAF and RFile
	pos1 = size/4;
	data->Seek(ESeekCurrent, pos1);
	pos2 = size/4;
	file.Seek(ESeekCurrent, pos2);
	data->Read(buf1);
	file.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekCurrent test using CData: %d"), pos1);
	INFO_PRINTF2(_L("Position from ESeekCurrent test using RFile: %d"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != (size/2 + buf1.Length()))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Seek and read from end using both CAF and RFile
	pos1 = -size/4;
	data->Seek(ESeekEnd, pos1);
	pos2 = -size/4;
	file.Seek(ESeekEnd, pos2);
	data->Read(buf1);
	file.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekEnd test using CData: %d"), pos1);
	INFO_PRINTF2(_L("Position from ESeekEnd test using RFile: %d"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != (size - size/4))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Test other overloaded forms of CData::Read

	// Read only 1 byte using both CAF and RFile
	pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	data->Read(buf1,1);
	file.Read(buf2,1);
	INFO_PRINTF2(_L("Data Length from  CData::Read(TDesC& ,TInt Length) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from  RFile::Read(TDesC& , TInt Length): %d"), buf2.Length());
	
	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read asynchronously using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read from CAF and RFile"));
	pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	status = KRequestPending;
	data->Read(buf1,status);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(buf2,status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data length from  CData::Read(TDesC& ,TRequestStatus& aStatus): %d"), buf1.Length());
	INFO_PRINTF2(_L("Data length from  RFile::Read(TDesC& , TRequestStatus& aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read only 1 byte asynchronously using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read of 1 byte from CAF and RFile"));
	pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	status = KRequestPending;
	data->Read(buf1,1,status);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(buf2,1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TDesC& ,TInt Length, TRequestStatus aStatus) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// read past EOF
	pos1 = size+1;
	pos2 = size+1;
	data->Seek(ESeekStart, pos1);
	file.Seek(ESeekStart, pos2);
	data->Read(buf1);
	file.Read(buf2);

	if(buf1.Length() != 0 || buf1.Length() != 0)
		{
		INFO_PRINTF1(_L("ERROR data read past EOF"));
		SetTestStepResult(EFail);
		}

	
	// Read asynchronously supplying an offset using both CAF and RFile
	TInt err;
	INFO_PRINTF1(_L("Asynchronous read of 1 byte from CAF and RFile supplying an offset within the file"));
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf1,1,status);
	
	StandardCheckReturnValue(err);
	
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2, buf2,1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 !=buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}
		
	// Check asynchronous cancellation
	err = 0;
	INFO_PRINTF1(_L("Asynchronous read cancellation using both CAF and RFile with an offset within the file"));	
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf1,1,status);
	StandardCheckReturnValue(err);
	data->ReadCancel(status);
	User::WaitForRequest(status);
	TRequestStatus status2(KRequestPending);
	file.Read(pos2, buf2,1, status2);	
	file.ReadCancel();
	User::WaitForRequest(status2);	
	INFO_PRINTF3(_L("Data Length from CData::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d, status is %d"), buf1.Length(), status.Int());
	INFO_PRINTF3(_L("Data Length from RFile::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d, status is %d"), buf2.Length(), status2.Int());	
	if (status.Int() != status2.Int() || buf1 !=buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR status, buffers or position do not match"));
		SetTestStepResult(EFail);
		}
		
	// read past EOF
	pos1 = size+1;
	pos2 = size+1;
	status = KRequestPending;
	data->Read(pos1,buf1,1,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2,buf2,1,status);
	User::WaitForRequest(status);
	
	if(buf1.Length() != 0 || buf1.Length() != 0)
		{
		INFO_PRINTF1(_L("ERROR data read past EOF"));
		SetTestStepResult(EFail);
		}
		
	// read over the length of the buffer
	INFO_PRINTF1(_L("Asynchronous read of 5 bytes from CAF and RFile supplying an offset within the file"));
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf1,5,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	if(status.Int()!=KErrOverflow)
		{
		INFO_PRINTF2(_L("ERROR Unexpected status returned: %d"),status.Int());
		SetTestStepResult(EFail);
		}
	status = KRequestPending;
	file.Read(pos2, buf2,5, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf2.Length());

	if (buf1 !=buf2 ||  buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}
	// read negative position this should return KErrArgument, dont supply -ve pos to RFile as will panic test
	INFO_PRINTF1(_L("Asynchronous read from CAF supplying a negative offset within the file"));
	pos1 = -1;
	status = KRequestPending;
	err = data->Read(pos1,buf1,5,status);
	if(err!=KErrArgument)
		{
		INFO_PRINTF2(_L("ERROR Unexpected return value: %d"),err);
		SetTestStepResult(EFail);
		}
		
	// Read asynchronously supplying an offset of 0 and a length greater than size of file using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read of length greater than size of file from CAF and RFile supplying an offset of 0 within the file"));
	TBuf8<256> buf3;
	TBuf8<256> buf4;
	pos1 = 0;
	pos2 = 0;
	status = KRequestPending;
	err = data->Read(pos1,buf3,size+1,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2, buf4,size+1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf3.Size());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf4.Size());

	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf3 != buf4 || pos1 != pos2 || buf3.Length() != buf4.Length() || pos1 != size || 
									pos2 != size || buf3.Size() != pos1 || buf4.Size() != pos2)
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read asynchronously supplying an offset of size/4 and a length greater than size of file using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read of length greater than size of file from CAF and RFile supplying an offset within the file"));
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf3,size+1,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2, buf4,size+1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf3.Size());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf4.Size());

	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf3 != buf4 || pos1 != pos2 || buf3.Size() != buf4.Size() || pos1 != size || pos2 != size)
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}	
		
	CleanupStack::PopAndDestroy(2, data);

	__UHEAP_MARKEND;

	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}


#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
/*
 * This is the 64bit version of CCAFSeekReadStep. 
*/

CCAFSeekReadStep64::~CCAFSeekReadStep64()
	{
	}

CCAFSeekReadStep64::CCAFSeekReadStep64(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFSeekReadStep);
	}

void CCAFSeekReadStep64::StandardCheckReturnValue(TInt aReturnValue)
	{
	if (aReturnValue != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR Unexpected return value: %d"),aReturnValue);
		SetTestStepResult(EFail);
		}	
	}
TVerdict CCAFSeekReadStep64::doTestStepL()
	{
	SetTestStepResult(EInconclusive);
	
	TBuf8<2> buf1;
	TBuf8<2> buf2;
	TRequestStatus status;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Basic Open Content Test"));
	
	TPtrC uri1;
	TPtrC uri2;
	GetStringFromConfig(ConfigSection(),_L("URI1"),uri1);
	GetStringFromConfig(ConfigSection(),_L("URI2"),uri2);

	INFO_PRINTF2(_L("Creating content object with uri: %S"), &uri1);
	CContent* content = CContent::NewLC(uri1);

	INFO_PRINTF1(_L("Creating data object from content"));
	CData* data = content->OpenContentL(EPeek);
	
	// don't need CContent any more
	CleanupStack::PopAndDestroy(content);
	CleanupStack::PushL(data);

	// Now, open the same file using RFile
	RFile64 file;
	INFO_PRINTF2(_L("Opening standard RFile interface with uri: %S"),&uri2);
	User::LeaveIfError(file.Open(iParent.Fs(), uri2, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);

	TInt64 size = 0;
	file.Size(size);
	INFO_PRINTF2(_L("Size of content from f32: %Ld"),size);
	
	// Seek and read from start using both CAF and RFile
	TInt64 pos1 = size/4;
	data->Seek64(ESeekStart, pos1);
	//pos2 needs to be modified to TInt64 when file server supports 64bit
	TInt64 pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	data->Read(buf1);
	file.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekStart test using CData: %Ld"), pos1);
	INFO_PRINTF2(_L("Position from ESeekStart test using RFile: %Ld"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != size/4)
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// set the location within the file then retrieve the current location within the file
	pos1=0;
	pos2=0;
	data->Seek64(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent, pos2);

	INFO_PRINTF2(_L("Current position using CData: %Ld"), pos1);
	INFO_PRINTF2(_L("Current position using RFile: %Ld"), pos2);

	if(pos1 != pos2 || pos1 != (size/4 + buf1.Length()))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Seek and read from current using both CAF and RFile
	pos1 = size/4;
	data->Seek64(ESeekCurrent, pos1);
	pos2 = size/4;
	file.Seek(ESeekCurrent, pos2);
	data->Read(buf1);
	file.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekCurrent test using CData: %Ld"), pos1);
	INFO_PRINTF2(_L("Position from ESeekCurrent test using RFile: %Ld"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != (size/2 + buf1.Length()))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Seek and read from end using both CAF and RFile
	pos1 = -size/4;
	data->Seek64(ESeekEnd, pos1);
	pos2 = -size/4;
	file.Seek(ESeekEnd, pos2);
	data->Read(buf1);
	file.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekEnd test using CData: %Ld"), pos1);
	INFO_PRINTF2(_L("Position from ESeekEnd test using RFile: %Ld"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != (size - size/4))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Test other overloaded forms of CData::Read

	// Read only 1 byte using both CAF and RFile
	pos1 = size/4;
	data->Seek64(ESeekStart, pos1);
	pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	data->Read(buf1,1);
	file.Read(buf2,1);
	INFO_PRINTF2(_L("Data Length from  CData::Read(TDesC& ,TInt Length) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from  RFile::Read(TDesC& , TInt Length): %d"), buf2.Length());
	
	pos1=0;
	pos2=0;
	data->Seek64(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read asynchronously using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read from CAF and RFile"));
	pos1 = size/4;
	data->Seek64(ESeekStart, pos1);
	pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	status = KRequestPending;
	data->Read(buf1,status);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(buf2,status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data length from  CData::Read(TDesC& ,TRequestStatus& aStatus): %d"), buf1.Length());
	INFO_PRINTF2(_L("Data length from  RFile::Read(TDesC& , TRequestStatus& aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek64(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read only 1 byte asynchronously using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read of 1 byte from CAF and RFile"));
	pos1 = size/4;
	data->Seek64(ESeekStart, pos1);
	pos2 = size/4;
	file.Seek(ESeekStart, pos2);
	status = KRequestPending;
	data->Read(buf1,1,status);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(buf2,1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TDesC& ,TInt Length, TRequestStatus aStatus) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek64(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// read past EOF
	pos1 = size+1;
	pos2 = size+1;
	data->Seek64(ESeekStart, pos1);
	file.Seek(ESeekStart, pos2);
	data->Read(buf1);
	file.Read(buf2);

	if(buf1.Length() != 0 || buf1.Length() != 0)
		{
		INFO_PRINTF1(_L("ERROR data read past EOF"));
		SetTestStepResult(EFail);
		}

	
	// Read asynchronously supplying an offset using both CAF and RFile
	TInt err;
	INFO_PRINTF1(_L("Asynchronous read of 1 byte from CAF and RFile supplying an offset within the file"));
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf1,1,status);
	
	StandardCheckReturnValue(err);
	
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2, buf2,1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek64(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf1 !=buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}
		
	// Check asynchronous cancellation
	err = 0;
	INFO_PRINTF1(_L("Asynchronous read cancellation using both CAF and RFile with an offset within the file"));	
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf1,1,status);
	StandardCheckReturnValue(err);
	data->ReadCancel(status);
	User::WaitForRequest(status);
	TRequestStatus status2(KRequestPending);
	file.Read(pos2, buf2,1, status2);	
	file.ReadCancel();
	User::WaitForRequest(status2);	
	INFO_PRINTF3(_L("Data Length from CData::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d, status is %d"), buf1.Length(), status.Int());
	INFO_PRINTF3(_L("Data Length from RFile::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d, status is %d"), buf2.Length(), status2.Int());	
	if (status.Int() != status2.Int() || buf1 !=buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR status, buffers or position do not match"));
		SetTestStepResult(EFail);
		}
		
	// read past EOF
	pos1 = size+1;
	pos2 = size+1;
	status = KRequestPending;
	data->Read(pos1,buf1,1,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2,buf2,1,status);
	User::WaitForRequest(status);
	
	if(buf1.Length() != 0 || buf1.Length() != 0)
		{
		INFO_PRINTF1(_L("ERROR data read past EOF"));
		SetTestStepResult(EFail);
		}
		
	// read over the length of the buffer
	INFO_PRINTF1(_L("Asynchronous read of 5 bytes from CAF and RFile supplying an offset within the file"));
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf1,5,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	if(status.Int()!=KErrOverflow)
		{
		INFO_PRINTF2(_L("ERROR Unexpected status returned: %d"),status.Int());
		SetTestStepResult(EFail);
		}
	status = KRequestPending;
	file.Read(pos2, buf2,5, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf2.Length());

	if (buf1 !=buf2 ||  buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}
	// read negative position this should return KErrArgument, dont supply -ve pos to RFile as will panic test
	INFO_PRINTF1(_L("Asynchronous read from CAF supplying a negative offset within the file"));
	pos1 = -1;
	status = KRequestPending;
	err = data->Read(pos1,buf1,5,status);
	if(err!=KErrArgument)
		{
		INFO_PRINTF2(_L("ERROR Unexpected return value: %d"),err);
		SetTestStepResult(EFail);
		}
		
	// Read asynchronously supplying an offset of 0 and a length greater than size of file using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read of length greater than size of file from CAF and RFile supplying an offset of 0 within the file"));
	TBuf8<256> buf3;
	TBuf8<256> buf4;
	pos1 = 0;
	pos2 = 0;
	status = KRequestPending;
	err = data->Read(pos1,buf3,size+1,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2, buf4,size+1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf3.Size());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf4.Size());

	pos1=0;
	pos2=0;
	data->Seek64(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf3 != buf4 || pos1 != pos2 || buf3.Length() != buf4.Length() || pos1 != size || 
									pos2 != size || buf3.Size() != pos1 || buf4.Size() != pos2)
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read asynchronously supplying an offset of size/4 and a length greater than size of file using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read of length greater than size of file from CAF and RFile supplying an offset within the file"));
	pos1 = size/4;
	pos2 = size/4;
	status = KRequestPending;
	err = data->Read(pos1,buf3,size+1,status);
	StandardCheckReturnValue(err);
	User::WaitForRequest(status);
	status = KRequestPending;
	file.Read(pos2, buf4,size+1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus) : %d"), buf3.Size());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TInt64 aPos, TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf4.Size());

	pos1=0;
	pos2=0;
	data->Seek64(ESeekCurrent, pos1);
	file.Seek(ESeekCurrent,pos2);
	if (buf3 != buf4 || pos1 != pos2 || buf3.Size() != buf4.Size() || pos1 != size || pos2 != size)
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}	
	
	CleanupStack::PopAndDestroy(2, data);
	
	__UHEAP_MARKEND;
	
	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
/*
 * This step retrieves the attributes of a file using the CAF framework
 *
 */

CCAFAttributesStep::~CCAFAttributesStep()
	{
	}

CCAFAttributesStep::CCAFAttributesStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFAttributesStep);
	}


TVerdict CCAFAttributesStep::doTestStepL()
	{
	CContent *content;
	CAttribute *attrs; 
	TPtrC fileName;
	TBool Protected;
	TBool Forwardable;
	
	
	TBool Value;

	SetTestStepResult(EFail);

	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	GetBoolFromConfig(ConfigSection(),_L("Protected"),Protected);
	GetBoolFromConfig(ConfigSection(),_L("Forwardable"),Forwardable);


	INFO_PRINTF2(_L("Check attributes of file: %S"), &fileName);	

	__UHEAP_MARK;

	// Open a file and retrieve the attributes
	content = CContent::NewLC(fileName);
	attrs = content->NewAttributeL(ETrue);
	CleanupStack::PushL(attrs);

	// Assume all the attributes are what we expect them to be
	SetTestStepResult(EPass);

	Value = attrs->ResponseSet().IsSet(EIsProtected);
	if(Value != Protected)
		{
		SetTestStepResult(EFail);
		Protected ? INFO_PRINTF1(_L("File is Protected")) : INFO_PRINTF1(_L("File is not Protected"));
		}

	Value = attrs->ResponseSet().IsSet(EIsForwardable);
	if(Value != Forwardable)
		{
		SetTestStepResult(EFail);
		Forwardable ? INFO_PRINTF1(_L("File is Forwardable")) : INFO_PRINTF1(_L("File is not Forwardable"));
		}

	CleanupStack::PopAndDestroy(2, content); 

	__UHEAP_MARKEND;
	return TestStepResult();
	}


/*
 * This step retrieves the string attributes of a file using the CAF framework
 *
 */

CCAFStringAttributesStep::~CCAFStringAttributesStep()
	{
	}

CCAFStringAttributesStep::CCAFStringAttributesStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFStringAttributesStep);
	}


TVerdict CCAFStringAttributesStep::doTestStepL()
	{
	TInt expectedResult;
	TInt attribute;
	TPtrC expectedValue;
	TBuf <256> value; //this will be used to test KErrOverFlow when transactionid is 257char long.
	TInt result = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("stringattribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetStringFromConfig(ConfigSection(),_L("value"),expectedValue);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);
	CData *data = content->OpenContentL(EPeek, uniqueId);
	result = data->GetStringAttribute(attribute, value);
	delete data;
	if(result != expectedResult)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CData::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);
		}
	if(value != expectedValue)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CData::GetStringAttribute() Expected value: %S, actual result: %S"), &expectedValue, &value);
		}

	CleanupStack::PopAndDestroy(content);		
    		
	__UHEAP_MARKEND;
	return TestStepResult();

	}




/* 
 * Step reads from a CData from multiple threads
 *
 */

CCAFMultiThreadCDataStep::~CCAFMultiThreadCDataStep()
	{
	}

CCAFMultiThreadCDataStep::CCAFMultiThreadCDataStep(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFMultiThreadCDataStep);
	}

TVerdict CCAFMultiThreadCDataStep::doTestStepL()
	{
	TBuf8<2> buf;

	__UHEAP_MARK;
	_LIT(KCDataThread,"CData_Thread");

	SetTestStepResult(EInconclusive);
	
	INFO_PRINTF1(_L("Multi Thread CData Test"));
	
	TPtrC uri;
	GetStringFromConfig(ConfigSection(),_L("URI"),uri);

	INFO_PRINTF1(_L("Creating data object from content"));
	CContent* content = CContent::NewLC(uri);
	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	TInt size, pos;

	// Create a mutex for communication between our thread and the new thread
	RSemaphore threadSemaphore;
	threadSemaphore.CreateGlobal(KCDataThreadSemaphore(), 1, EOwnerProcess);
	threadSemaphore.Wait(); 

	// fire up a new thread, stack size and heap size 8k, might need to be bigger in the future 
	RThread readerThread;
	User::LeaveIfError(readerThread.Create(KCDataThread(),CCAFMultiThreadCDataStep::ReaderThreadEntry,8192, KMinHeapSize, 8192, (TAny *) data, EOwnerProcess));
	
	// request notification of thread completion
	TRequestStatus stat;
	readerThread.Logon(stat);
	readerThread.Resume();	
	
	data->DataSizeL(size);
	pos = size/2;
	data->Seek(ESeekStart,pos);
	data->Read(buf);

	threadSemaphore.Wait(); // wait for thread function to run and signal the semaphore
	threadSemaphore.Signal(); // finished with semaphore
	threadSemaphore.Close(); // so close

	User::WaitForRequest(stat); // wait for thread to finish
	TInt error=(readerThread.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	User::LeaveIfError(error);
	readerThread.Close();  // close the thread
	
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(content);

	__UHEAP_MARKEND;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CCAFMultiThreadCDataStep::ReaderThreadEntry(TAny *aPtr)
	{
	TBuf8 <2> buf;
	CData *data = reinterpret_cast <CData *> (aPtr);

	// create a trap handler
	CTrapCleanup* cleanup = CTrapCleanup::New();

	RSemaphore threadSemaphore;
	threadSemaphore.OpenGlobal(KCDataThreadSemaphore() ,EOwnerProcess);

	data->Read(buf);
	threadSemaphore.Signal(); // allow original thread to continue
	threadSemaphore.Close();
	
	delete cleanup;
	return KErrNone;
	}


/* 
 * Obtains the mime type from a CData
 *
 */

CCAFMimeTypeCDataStep::~CCAFMimeTypeCDataStep()
	{
	}

CCAFMimeTypeCDataStep::CCAFMimeTypeCDataStep(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFMimeTypeCDataStep);
	}

TVerdict CCAFMimeTypeCDataStep::doTestStepL()
	{
	__UHEAP_MARK;


	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("CData MimeType Test"));
	
	TPtrC uri;
	TPtrC expectedMimeType;
	TBool mimeTypeKnown=EFalse;

	// Load INI parameters
	GetStringFromConfig(ConfigSection(),_L("URI"),uri);
	GetStringFromConfig(ConfigSection(),_L("ContentMimeType"),expectedMimeType);
	GetBoolFromConfig(ConfigSection(),_L("Known"),mimeTypeKnown);

	// convert to Des8
	HBufC8 *expected = ConvertDes16toHBufC8LC(expectedMimeType);

	// Create CContent and CData
	INFO_PRINTF1(_L("Creating data object from content"));
	CContent* content = CContent::NewLC(uri);
	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	// Get mime type from CData
	INFO_PRINTF1(_L("Checking mime type"));
	HBufC8 *buffer = HBufC8::NewLC(KCafTestMaxDataTypeLength);
	TPtr8 ptr = buffer->Des();
	TBool r = data->GetMimeTypeL(ptr);

	// check if result matches expected result
	if(r == mimeTypeKnown && *buffer == *expected)
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(4,expected);
	__UHEAP_MARKEND;
	return TestStepResult();
	}

	/*
 * This step tests file sharing modes
 *
 */

CCAFShareModeStep::~CCAFShareModeStep()
	{
	}

CCAFShareModeStep::CCAFShareModeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFShareModeStep);
	}


TVerdict CCAFShareModeStep::doTestStepL()
	{
	TVerdict verdict = EFail;
	TRAPD(err, verdict = doShareModeStepL());
	if(err != KErrNone)
		{
		if(err != KErrInUse)
			{
			User::Leave(err);
			}
		else
			{
			User::Leave(KErrGeneral);
			}
		}
	return verdict;	
	}

TVerdict CCAFShareModeStep::doShareModeStepL()	
	{
	CAttribute *attr = NULL;
	RFile file;	
	RFs fs;
	TPtrC fileName;
	TInt err = 0;
	
	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("FileName"),fileName);

	// This function works if I step through everything but fails
	// when run or step over the TRAP
//	User::Leave(KErrGeneral);
	

	__UHEAP_MARK;
	
	INFO_PRINTF2(_L("Creating Content object for file: %S"), &fileName);	
	CContent* content= CContent::NewL(fileName);	
	CData *data = NULL;
	
	fs.Connect();
	CleanupClosePushL(fs);


	INFO_PRINTF1(_L("Testing EContentShareReadOnly"));	
	User::LeaveIfError(file.Open(fs, fileName, EFileShareReadersOnly | EFileRead | EFileStream));
	CleanupClosePushL(file);
	data = content->OpenContentL(EPeek);
	delete data;
	data = NULL;
	data = content->OpenContentL(EPeek, EContentShareReadOnly);
	delete data;
	data = NULL;
	TRAP(err, data = content->OpenContentL(EPeek, EContentShareReadWrite));
	delete data;
	data = NULL;
	
	// We can open read-write because CAF never writes.
	
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		}
	
	// However, exclusive mode should be impossible.
	
	TRAP(err, data = content->OpenContentL(EPeek, EContentShareExclusive));
	delete data;
	data = NULL;
	if(err != KErrInUse)
		{
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&file); 
	delete content;

	
	INFO_PRINTF1(_L("Testing EContentShareReadWrite"));	
	content = CContent::NewL(fileName, EContentShareReadWrite);
	User::LeaveIfError(file.Open(fs, fileName, EFileShareReadersOrWriters | EFileRead | EFileStream));
	CleanupClosePushL(file);
	data = content->OpenContentL(EPeek, EContentShareReadWrite);
	delete data;
	data = NULL;
	
	// We should be able to open read-only
	
	TRAP(err, data = content->OpenContentL(EPeek, EContentShareReadOnly));
	delete data;
	data = NULL;
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		}
		
	// but not share exclusive
		
	TRAP(err, data = content->OpenContentL(EPeek, EContentShareExclusive));
	delete data;
	data = NULL;
	if(err != KErrInUse)
		{
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&file);
	
	
	// Reopen the file with write attributes, and make sure ReadOnly becomes impossible 
	User::LeaveIfError(file.Open(fs, fileName, EFileShareReadersOrWriters | EFileRead | EFileWrite | EFileStream));
	CleanupClosePushL(file);
	
	TRAP(err, data = content->OpenContentL(EPeek, EContentShareReadOnly));
	delete data;
	data = NULL;
	if(err != KErrInUse)
		{
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(&file);
	delete content;
	
	
	INFO_PRINTF1(_L("Testing EContentShareExclusive"));	
	User::LeaveIfError(file.Open(fs, fileName, EFileShareExclusive | EFileRead | EFileStream));
	CleanupClosePushL(file);
	TRAP(err, data = CData::NewL(TVirtualPathPtr(fileName), EPeek, EContentShareReadOnly));
	delete data;
	data = NULL;
	if(err != KErrInUse)
		{
		SetTestStepResult(EFail);
		}
	TRAP(err, data = CData::NewL(TVirtualPathPtr(fileName), EPeek, EContentShareReadWrite));
	delete data;
	data = NULL;
	if(err != KErrInUse)
		{
		SetTestStepResult(EFail);
		}
	TRAP(err, data = CData::NewL(TVirtualPathPtr(fileName), EPeek, EContentShareExclusive));
	delete data;
	data = NULL;
	if(err != KErrInUse)
		{
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&file); 
	
	INFO_PRINTF1(_L("Testing CContent::NewAttributeL"));	
	content = CContent::NewL(fileName, EContentShareReadWrite);
	User::LeaveIfError(file.Open(fs, fileName, EFileShareAny | EFileRead | EFileStream));
	CleanupClosePushL(file);
	attr = content->NewAttributeL(ETrue,EContentShareReadWrite);
	delete attr;
	attr = NULL;
	TRAP(err, attr = content->NewAttributeL(ETrue));
	delete attr;
	attr = NULL;
	if(err != KErrInUse)
		{
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(2, &fs); // fs, file
	delete content;

	__UHEAP_MARKEND;
	return TestStepResult();
	}

CCAFHandleSizeStep::CCAFHandleSizeStep(CCAFServer& aParent)
:iParent(aParent)
{
	SetTestStepName(KCAFHandleSizeStep);	
}

CCAFHandleSizeStep::~CCAFHandleSizeStep()
{
	
	
}	

TVerdict CCAFHandleSizeStep::doTestStepL()
	{
	__UHEAP_MARK;
	
	SetTestStepResult(EInconclusive);
	
	INFO_PRINTF1(_L("Basic Open Content Test"));
	
	TPtrC uri1;
	TPtrC uri2;
	GetStringFromConfig(ConfigSection(),_L("URI1"),uri1);
	GetStringFromConfig(ConfigSection(),_L("URI2"),uri2);

	INFO_PRINTF2(_L("Creating content object with uri: %S"), &uri1);
	
	RFile file1;
	User::LeaveIfError(file1.Open(iParent.Fs(), uri1, EFileRead));	
	CleanupClosePushL(file1);	
		
	CContent* content = CContent::NewLC(file1);

	INFO_PRINTF1(_L("Creating data object from content"));

	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	TInt size = 0;
	data->DataSizeL(size);
	INFO_PRINTF2(_L("Size of content from caf: %d"),size);
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(content);
	
	TInt pos = 0;
	file1.Seek(ESeekStart, pos);

	INFO_PRINTF2(_L("Creating CData object with uri: %S"), &uri1);
	TInt size1 = 0;
	data = CData::NewL(file1, KDefaultContentObject(), EPeek);
	CleanupStack::PushL(data);
	data->DataSizeL(size1);
	INFO_PRINTF2(_L("Size of content from caf CData::NewL(): %d"),size1);
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(&file1);
	
	INFO_PRINTF2(_L("Opening standard RFile interface with uri: %S"),&uri2);
	RFile file2;
	User::LeaveIfError(file2.Open(iParent.Fs(), uri2, EFileRead));
	
	TInt size2;
	file2.Size(size2);
	file2.Close();

	INFO_PRINTF2(_L("Size of content from f32: %d"),size2);

	if(size == size2 && size1 == size2)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}


CCAFHandleSeekReadStep::~CCAFHandleSeekReadStep()
{
	
}

CCAFHandleSeekReadStep::CCAFHandleSeekReadStep(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFHandleSeekReadStep);
	}
	
TVerdict CCAFHandleSeekReadStep::doTestStepL()
	{
	SetTestStepResult(EInconclusive);
	
	TBuf8<2> buf1;
	TBuf8<2> buf2;
	TRequestStatus status;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Basic Open Content Test"));
	
	TPtrC uri1;
	TPtrC uri2;
	GetStringFromConfig(ConfigSection(),_L("URI1"),uri1);
	GetStringFromConfig(ConfigSection(),_L("URI2"),uri2);

	INFO_PRINTF2(_L("Creating content object with uri: %S"), &uri1);
	RFile file1;
	User::LeaveIfError(file1.Open(iParent.Fs(), uri1, EFileRead | EFileShareReadersOnly));	
	CleanupClosePushL(file1);		
	CContent* content = CContent::NewLC(file1);

	INFO_PRINTF1(_L("Creating data object from content"));
	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	// Now, open the same file using RFile
	RFile file2;
	INFO_PRINTF2(_L("Opening standard RFile interface with uri: %S"),&uri2);
	//User::LeaveIfError(file2.Open(iParent.Fs(), uri2, EFileRead | EFileShareReadersOnly));
	TInt res = file2.Open(iParent.Fs(), uri2, EFileRead | EFileShareReadersOnly);
	CleanupClosePushL(file2);

	TInt size = 0;
	file2.Size(size);
	INFO_PRINTF2(_L("Size of content from f32: %d"),size);
	
	// Seek and read from start using both CAF and RFile
	TInt pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	TInt pos2 = size/4;
	file2.Seek(ESeekStart, pos2);
	data->Read(buf1);
	file2.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekStart test using CData: %d"), pos1);
	INFO_PRINTF2(_L("Position from ESeekStart test using RFile: %d"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != size/4)
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// set the location within the file then retrieve the current location within the file
	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file2.Seek(ESeekCurrent, pos2);

	INFO_PRINTF2(_L("Current position using CData: %d"), pos1);
	INFO_PRINTF2(_L("Current position using RFile: %d"), pos2);

	if(pos1 != pos2 || pos1 != (size/4 + buf1.Length()))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Seek and read from current using both CAF and RFile
	pos1 = size/4;
	data->Seek(ESeekCurrent, pos1);
	pos2 = size/4;
	file2.Seek(ESeekCurrent, pos2);
	data->Read(buf1);
	file2.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekCurrent test using CData: %d"), pos1);
	INFO_PRINTF2(_L("Position from ESeekCurrent test using RFile: %d"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != (size/2 + buf1.Length()))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Seek and read from end using both CAF and RFile
	pos1 = -size/4;
	data->Seek(ESeekEnd, pos1);
	pos2 = -size/4;
	file2.Seek(ESeekEnd, pos2);
	data->Read(buf1);
	file2.Read(buf2);
	INFO_PRINTF2(_L("Position from ESeekEnd test using CData: %d"), pos1);
	INFO_PRINTF2(_L("Position from ESeekEnd test using RFile: %d"), pos2);
	if (buf1 != buf2 || pos1 != pos2 || pos1 != (size - size/4))
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Test other overloaded forms of CData::Read

	// Read only 1 byte using both CAF and RFile
	pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	pos2 = size/4;
	file2.Seek(ESeekStart, pos2);
	data->Read(buf1,1);
	file2.Read(buf2,1);
	INFO_PRINTF2(_L("Data Length from  CData::Read(TDesC& ,TInt Length) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from  RFile::Read(TDesC& , TInt Length): %d"), buf2.Length());
	
	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file2.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read asynchronously using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read from CAF and RFile"));
	pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	pos2 = size/4;
	file2.Seek(ESeekStart, pos2);
	status = KRequestPending;
	data->Read(buf1,status);
	User::WaitForRequest(status);
	status = KRequestPending;
	file2.Read(buf2,status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data length from  CData::Read(TDesC& ,TRequestStatus& aStatus): %d"), buf1.Length());
	INFO_PRINTF2(_L("Data length from  RFile::Read(TDesC& , TRequestStatus& aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file2.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// Read only 1 byte asynchronously using both CAF and RFile
	INFO_PRINTF1(_L("Asynchronous read of 1 byte from CAF and RFile"));
	pos1 = size/4;
	data->Seek(ESeekStart, pos1);
	pos2 = size/4;
	file2.Seek(ESeekStart, pos2);
	status = KRequestPending;
	data->Read(buf1,1,status);
	User::WaitForRequest(status);
	status = KRequestPending;
	file2.Read(buf2,1, status);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Data Length from CData::Read(TDesC& ,TInt Length, TRequestStatus aStatus) : %d"), buf1.Length());
	INFO_PRINTF2(_L("Data Length from RFile::Read(TDesC& , TInt Length, TRequestStatus aStatus): %d"), buf2.Length());

	pos1=0;
	pos2=0;
	data->Seek(ESeekCurrent, pos1);
	file2.Seek(ESeekCurrent,pos2);
	if (buf1 != buf2 || pos1 != pos2 || buf1.Length() != buf2.Length())
		{
		INFO_PRINTF1(_L("ERROR buffers or position do not match"));
		SetTestStepResult(EFail);
		}

	// read past EOF
	pos1 = size+1;
	pos2 = size+1;
	data->Seek(ESeekStart, pos1);
	file2.Seek(ESeekStart, pos2);
	data->Read(buf1);
	file2.Read(buf2);

	if(buf1.Length() != 0 || buf1.Length() != 0)
		{
		INFO_PRINTF1(_L("ERROR data read past EOF"));
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(4);

	__UHEAP_MARKEND;

	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

/* 
 * Step reads from a CData from multiple threads using RFile handle
 *
 */
 
CCAFHandleMultiThreadCDataStep::~CCAFHandleMultiThreadCDataStep()
 {
  	
 }
   
 CCAFHandleMultiThreadCDataStep::CCAFHandleMultiThreadCDataStep(CCAFServer& aParent)
 :iParent(aParent)
 {
 	
 	SetTestStepName(KCAFHandleMultiThreadCDataStep);
 }
 
TVerdict CCAFHandleMultiThreadCDataStep::doTestStepL()
	 {
	TBuf8<2> buf;

	__UHEAP_MARK;
	_LIT(KCDataThread,"CData_Thread");

	SetTestStepResult(EInconclusive);
	
	INFO_PRINTF1(_L("Multi Thread CData Test using RFile Handle"));
	
	TPtrC uri;
	GetStringFromConfig(ConfigSection(),_L("URI"),uri);

	INFO_PRINTF1(_L("Creating data object from content"));
	RFile file;
	User::LeaveIfError(file.Open(iParent.Fs(), uri, EFileRead));	
		
	CleanupClosePushL(file);
		
	CContent* content = CContent::NewLC(file);
	CData* data = content->OpenContentL(EPeek);
	CleanupStack::PushL(data);

	TInt size, pos;

	// Create a mutex for communication between our thread and the new thread
	RSemaphore threadSemaphore;
	threadSemaphore.CreateGlobal(KCDataThreadSemaphore(), 1, EOwnerProcess);
	threadSemaphore.Wait(); 

	// fire up a new thread, stack size and heap size 8k, might need to be bigger in the future 
	RThread readerThread;
	User::LeaveIfError(readerThread.Create(KCDataThread(),CCAFMultiThreadCDataStep::ReaderThreadEntry,8192, KMinHeapSize, 8192, (TAny *) data, EOwnerProcess));
	
	// request notification of thread completion
	TRequestStatus stat;
	readerThread.Logon(stat);
	readerThread.Resume();	
	

	data->DataSizeL(size);
	pos = size/2;
	data->Seek(ESeekStart,pos);
	data->Read(buf);

	threadSemaphore.Wait(); // wait for thread function to run and signal the semaphore
	threadSemaphore.Signal(); // finished with semaphore
	threadSemaphore.Close(); // so close

	User::WaitForRequest(stat); // wait for thread to finish
	TInt error=(readerThread.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	User::LeaveIfError(error);
	readerThread.Close();  // close the thread
	
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PopAndDestroy(content);
	CleanupStack::PopAndDestroy(&file); //file
	

	__UHEAP_MARKEND;
	
	SetTestStepResult(EPass);
	return TestStepResult();	
}

TInt CCAFHandleMultiThreadCDataStep::ReaderThreadEntry(TAny *aPtr)
	{
	TBuf8 <2> buf;
	CData *data = reinterpret_cast <CData *> (aPtr);

	// create a trap handler
	CTrapCleanup* cleanup = CTrapCleanup::New();

	RSemaphore threadSemaphore;
	threadSemaphore.OpenGlobal(KCDataThreadSemaphore() ,EOwnerProcess);

	data->Read(buf);
	threadSemaphore.Signal(); // allow original thread to continue
	threadSemaphore.Close();
	
	delete cleanup;
	return KErrNone;
	}

/* 
 * Data attribute step
 *
 */

CCAFDataAttributeStep::~CCAFDataAttributeStep()
	{
	}

CCAFDataAttributeStep::CCAFDataAttributeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFDataAttributeStep);
	}

TVerdict CCAFDataAttributeStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  
	TInt attribute;
	TInt value = KErrNone;
	TInt expectedValue = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("value"),expectedValue);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);
	CData *data = content->OpenContentL(EPeek, uniqueId);
	User::LeaveIfError(data->GetAttribute(attribute, value));
	delete data;
	if(expectedValue != value)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CData::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);
		}
	
	CleanupStack::PopAndDestroy(content);		    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Data attributeset step
 *
 */

CCAFDataAttributeSetStep::~CCAFDataAttributeSetStep()
	{
	}

CCAFDataAttributeSetStep::CCAFDataAttributeSetStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFDataAttributeSetStep);
	}

TVerdict CCAFDataAttributeSetStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

	TInt expectedValue1;
	TInt expectedValue2;
	TInt value1;
	TInt value2;
	TInt attribute1;
	TInt attribute2;
	TInt result = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetIntFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetIntFromConfig(ConfigSection(),_L("value2"),expectedValue2);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);
	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	attributeSet.AddL(attribute1);
	attributeSet.AddL(attribute2);

	CData *data = content->OpenContentL(EPeek, uniqueId);
	result = data->GetAttributeSet(attributeSet);
	delete data;
	if(result != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CData::GetAttribute() failed"));
		}
	User::LeaveIfError(attributeSet.GetValue(attribute1, value1));
	User::LeaveIfError(attributeSet.GetValue(attribute2, value2));
	if(value1 != expectedValue1 || value2 != expectedValue2 || attributeSet.Count() != 2)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CData::GetAttributeSet() values don't match expected values"));
		}
	CleanupStack::PopAndDestroy(&attributeSet);		
	CleanupStack::PopAndDestroy(content);		
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/* 
 * Data string attribute step
 *
 */

CCAFDataStringAttributeStep::~CCAFDataStringAttributeStep()
	{
	}

CCAFDataStringAttributeStep::CCAFDataStringAttributeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFDataStringAttributeStep);
	}

TVerdict CCAFDataStringAttributeStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

	TInt expectedResult;
	TInt attribute;
	TPtrC expectedValue;
	TBuf <200> value;
	TInt result = KErrNone;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetIntFromConfig(ConfigSection(),_L("attribute"),attribute);
	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetStringFromConfig(ConfigSection(),_L("value"),expectedValue);
	GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);
	CData *data = content->OpenContentL(EPeek, uniqueId);
	result = data->GetStringAttribute(attribute, value);
	delete data;
	if(result != expectedResult)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CData::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);
		}
	if(value != expectedValue)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("CData::GetStringAttribute() Expected value: %S, actual result: %S"), &expectedValue, &value);
		}

	CleanupStack::PopAndDestroy(content);		
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * Data StringAttributeSet step
 *
 */

CCAFDataStringAttributeSetStep::~CCAFDataStringAttributeSetStep()
	{
	}

CCAFDataStringAttributeSetStep::CCAFDataStringAttributeSetStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFDataStringAttributeSetStep);
	}

TVerdict CCAFDataStringAttributeSetStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)     
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  
	TPtrC expectedValue1;
	TPtrC expectedValue2;
	TBuf <200> value1;
	TBuf <200> value2;
	TInt attribute1;
	TInt attribute2;
	TInt result = KErrNone;
	TInt result1;
	TInt result2;
	TPtrC uri;
	TPtrC uniqueId;

	SetTestStepResult(EPass);

	GetStringFromConfig(ConfigSection(),_L("uri"),uri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);
	GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);
	GetStringFromConfig(ConfigSection(),_L("value1"),expectedValue1);
	GetStringFromConfig(ConfigSection(),_L("value2"),expectedValue2);
	GetIntFromConfig(ConfigSection(),_L("result1"),result1);
	GetIntFromConfig(ConfigSection(),_L("result2"),result2);

	__UHEAP_MARK;
	CContent *content = CContent::NewLC(uri);
	RStringAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	attributeSet.AddL(attribute1);
	attributeSet.AddL(attribute2);
	
	CData *data = content->OpenContentL(EPeek, uniqueId);
	result = data->GetStringAttributeSet(attributeSet);
	delete data;
	if(result != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CData::GetAttribute() failed"));
		}
	if(result1 != attributeSet.GetValue(attribute1, value1))
		{
		INFO_PRINTF1(_L("RStringAttributeSet::GetValue failed"));
		}
	if(result2 != attributeSet.GetValue(attribute2, value2))
		{
		INFO_PRINTF1(_L("RStringAttributeSet::GetValue failed"));
		}
	if( value1 != expectedValue1 || value2 != expectedValue2 || attributeSet.Count() != 2)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CData::GetAttributeSet() values don't match expected values"));
		}
	CleanupStack::PopAndDestroy(&attributeSet);		
	CleanupStack::PopAndDestroy(content);		
	
    		
	__UHEAP_MARKEND;
	return TestStepResult();
	}


CCAFExecuteIntentStep::~CCAFExecuteIntentStep()
	{
	}

CCAFExecuteIntentStep::CCAFExecuteIntentStep(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFSizeStep);
	}

TVerdict CCAFExecuteIntentStep::doTestStepL()
	{
	__UHEAP_MARK;
	
	SetTestStepResult(EInconclusive);
	
	INFO_PRINTF1(_L("Execute Intent Test"));
	
	TPtrC fileName;
	TPtrC uniqueId;
	TInt intent = EPeek;
	TInt expectedResult = KErrNone;
	
	GetStringFromConfig(ConfigSection(),_L("FileName"),fileName);
	GetStringFromConfig(ConfigSection(),_L("UniqueId"),uniqueId);
	GetIntFromConfig(ConfigSection(),_L("Intent"),intent);
	GetIntFromConfig(ConfigSection(),_L("ExpectedResult"),expectedResult);
	
	INFO_PRINTF3(_L("Creating CData object with filename: %S, UniqueId: %S"), &fileName, &uniqueId);
	
	CData* data = NULL;
	
	TRAPD(err, data = CData::NewL(TVirtualPathPtr(fileName, uniqueId),static_cast<TIntent>(intent), EContentShareReadOnly));
	
	if(err != expectedResult)
		{
		INFO_PRINTF2(_L("Unexpected result, EvaluateIntent() returned %d"), err);
		}
	else
		{
		SetTestStepResult(EPass);
		if(err == KErrNone)
			{
			err = data->ExecuteIntent(static_cast<TIntent>(intent));
			if(err == KErrNone)
				{
				TBuf8 <128> buffer;
				TInt length = 128;
			
				// read the entire content object
				while(err == KErrNone && length > 0)
					{
					err = data->Read(buffer);
					length = buffer.Length();
					}
				if(err != KErrEof && err != KErrNone)
					{
					INFO_PRINTF2(_L("Unexpected error while reading content object: %d"), err);
					SetTestStepResult(EFail);
					}
				}
			else
				{
				INFO_PRINTF2(_L("Unexpected error, EvaluteIntent succeeded but ExecuteIntent failed with error: %d"), err);
				SetTestStepResult(EFail);
				}
			}
		}
		
	delete data;
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}

/* 
 * DRM File Open Performance Test Step
 *
 */

CCAFDRMFileOpenPerformanceTest::~CCAFDRMFileOpenPerformanceTest()
	{
	iIterationSum = 0;
	}

CCAFDRMFileOpenPerformanceTest::CCAFDRMFileOpenPerformanceTest(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFDRMFileOpenPerformanceStep);
	}

TVerdict CCAFDRMFileOpenPerformanceTest::doTestStepL()
	{
	__UHEAP_MARK;

	INFO_PRINTF1(_L("DRM File Open Performance Test"));
	
	TPtrC inputFileName;
	TInt maxValue = 0 ,iterCount = 0;

	GetStringFromConfig(ConfigSection(),_L("FileName"),inputFileName);
	GetIntFromConfig(ConfigSection(),_L("IterationCount"),iterCount);
	GetIntFromConfig(ConfigSection(),_L("MaxTimeLimitInMS"),maxValue);
		
	iIterationSum=0;
	// iterated iterCount times to get the average time taken to open the file.
	for (TInt i = 0; i < iterCount; ++i)
		{
		OpenAndReadFileContentL(inputFileName);
		}
		
	TInt tickPeriod;
	HAL::Get(HAL::ENanoTickPeriod, tickPeriod);
	TInt nTickPeriodMS = tickPeriod / KCafTestSecToMS;

	TInt avgTimeForFileOpen = (iIterationSum * nTickPeriodMS) / iterCount;
	if((avgTimeForFileOpen > maxValue))
		{
		INFO_PRINTF1(_L("Performance Test failed."));
		SetTestStepResult(EFail);
		}

	INFO_PRINTF2(_L("Expected maximum time to open DRM file in millisecs is : %d"), maxValue);
	INFO_PRINTF2(_L("Obtained time in millisecs is: %d"), avgTimeForFileOpen);
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
	
void CCAFDRMFileOpenPerformanceTest::OpenAndReadFileContentL(const TDesC& aFileName)
	{
	TPtrC uniqueIdentifier;

	GetStringFromConfig(ConfigSection(),_L("UniqueId"),uniqueIdentifier);
	CContent *content = CContent::NewLC(aFileName);
	CData *data = NULL;
	TUint32 tickCounterValBeforeFileOpen, tickCounterValAfterFileOpen;
	if(uniqueIdentifier.Length() > 0)
		{
		tickCounterValBeforeFileOpen = User::NTickCount();
		data = content->OpenContentL(EPeek,uniqueIdentifier);
		tickCounterValAfterFileOpen = User::NTickCount();
		}
	else
		{
		tickCounterValBeforeFileOpen = User::NTickCount();
		data = content->OpenContentL(EPeek);
		tickCounterValAfterFileOpen = User::NTickCount();
		}
	delete data;
	// finished with Data object
	CleanupStack::PopAndDestroy(content);	
	iIterationSum += tickCounterValAfterFileOpen - tickCounterValBeforeFileOpen;
	}
	
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
CWmdrmCAFContentStep::~CWmdrmCAFContentStep()     
    {     
    }     
      
CWmdrmCAFContentStep::CWmdrmCAFContentStep()     
    {     
    SetTestStepName(KWmdrmCAFContentStep);     
    }     
      
      
TVerdict CWmdrmCAFContentStep::doTestStepL()     
    {     
    SetTestStepResult(EFail);     
    
    TBool protectedVal;
    GetBoolFromConfig(ConfigSection(),_L("attribValue1"), protectedVal);     
    TBool forwardable;     
    GetBoolFromConfig(ConfigSection(),_L("attribValue2"), forwardable);     
         
    TBool headerDataPresent = ETrue;     
    GetBoolFromConfig(ConfigSection(),_L("headerDataFlag"), headerDataPresent);     
         
    INFO_PRINTF1(_L("Verify CContent APIs for WMDRM content"));      
         
    __UHEAP_MARK;     
         
    CContent* content = NULL;     
         
    // this condition verifies that CContent construction leaves with KErrMissingWmdrmHeaderData,     
    // when no header data is provided.     
    if(!headerDataPresent)     
        {     
        content = CContent::NewL(KNullDesC8());     
        return EPass;     
        }     
         
    // create a dummy header data.     
    HBufC8* headerData = CreateWmdrmHeaderLC();     
         
    // Open a file and retrieve the attributes.     
    // If headerDataPresent is false, the code will never reach here.s     
    content = CContent::NewL(*headerData);     
    CleanupStack::PushL(content);     
    CAttribute* attrs = content->NewAttributeL(ETrue);     
    CleanupStack::PushL(attrs);     
      
    TBool value1 = attrs->ResponseSet().IsSet(EIsProtected);     
    TBool value2 = attrs->ResponseSet().IsSet(EIsForwardable);     
    if(value1 == protectedVal && value2 == forwardable)     
        {     
        SetTestStepResult(EPass);     
        }     
      
    CleanupStack::PopAndDestroy(3, headerData);      
      
    __UHEAP_MARKEND;     
    return TestStepResult();     
    }     
      
      
CWmdrmCAFDataStep::~CWmdrmCAFDataStep()     
    {     
    }     
      
CWmdrmCAFDataStep::CWmdrmCAFDataStep()     
    {     
    SetTestStepName(KWmdrmCAFDataStep);     
    }     
      
TVerdict CWmdrmCAFDataStep::doTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TInt attribute;     
    TInt expectedValue = KErrNone;     
      
    GetIntFromConfig(ConfigSection(),_L("attribute"), attribute);     
    GetIntFromConfig(ConfigSection(),_L("value"), expectedValue);     
         
__UHEAP_MARK;     
      
    HBufC8* headerData = CreateWmdrmHeaderLC();     
         
    CContent *content = CContent::NewLC(*headerData);     
    CData *data = content->OpenContentL(EPeek);     
    TInt value = KErrNone;     
    User::LeaveIfError(data->GetAttribute(attribute, value));     
    delete data;     
         
    if(expectedValue == value)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF3(_L("CData::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);        
        }     
         
    CleanupStack::PopAndDestroy(2, headerData);                      
__UHEAP_MARKEND;     
         
    return TestStepResult();     
    }     
      
      
CWmdrmCAFReadStep::~CWmdrmCAFReadStep()     
    {     
    }     
      
CWmdrmCAFReadStep::CWmdrmCAFReadStep()     
    {     
    SetTestStepName(KWmdrmCAFReadStep);     
    }     
      
      
TVerdict CWmdrmCAFReadStep::doTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TBool synchronous = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("synchronous"), synchronous);     
    TBool intent = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("intent"), intent);     
    TPtrC inputPacket;     
    GetStringFromConfig(ConfigSection(),_L("inputPacket"), inputPacket);     
    TPtrC expectedOutput;     
    GetStringFromConfig(ConfigSection(),_L("outputPacket"), expectedOutput);     
         
__UHEAP_MARK;     
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
         
    TInt result = 0;     
    if(!synchronous)     
        {     
        result = TestAsynchronousReadL(*headerData, intent, inputPacket, expectedOutput);     
        }     
    else     
        {     
        result = TestSynchronousReadL(*headerData, intent, inputPacket, expectedOutput);     
        }     
      
    if(result == KErrNone)     
        {     
        SetTestStepResult(EPass);     
        }     
             
    CleanupStack::PopAndDestroy(headerData);     
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
         
TInt CWmdrmCAFReadStep::TestSynchronousReadL(const TDesC8& aHeaderData, TBool aIntent, TDesC& aInputPacket, TDesC& aExpectedOutput)     
    {     
__UHEAP_MARK;     
    CData* data = NULL;     
    TInt returnVal = KErrGeneral;     
         
    if(aIntent)     
        {     
        data = CData::NewL(aHeaderData, EPeek);     
        }     
    else     
        {     
        data = CData::NewL(aHeaderData);     
        }     
         
    CleanupStack::PushL(data);     
         
    if(aInputPacket.Length() <= 0)     
        {     
        // dummy output buffer.This loop essentially tests the error condition KErrInsufficientDataPacketLength.     
        TBuf8<2> outputBuffer;     
        User::LeaveIfError(data->Read(KNullDesC8(), outputBuffer));     
        }     
             
    else     
        {     
        HBufC8* inputPacket = ConvertDes16toHBufC8LC(aInputPacket);     
        HBufC8* outputPacket = HBufC8::NewLC(aInputPacket.Length());     
        TPtr8 outputPacketPtr = outputPacket->Des();     
         
        User::LeaveIfError(data->Read(*inputPacket, outputPacketPtr));     
        HBufC8* expectedOutput = ConvertDes16toHBufC8LC(aExpectedOutput);     
        if(expectedOutput->Compare(*outputPacket) == KErrNone)     
            {     
            returnVal = KErrNone;        
            }     
         
        CleanupStack::PopAndDestroy(3, inputPacket);         
        }     
         
    CleanupStack::PopAndDestroy(data);     
__UHEAP_MARKEND;     
         
    return returnVal;     
    }     
         
TInt CWmdrmCAFReadStep::TestAsynchronousReadL(const TDesC8& aHeaderData, TBool aIntent, TDesC& aInputPacket, TDesC& aExpectedOutput)     
    {     
__UHEAP_MARK;     
    CData* data = NULL;     
    TInt returnVal = KErrGeneral;     
         
    if(aIntent)     
        {     
        data = CData::NewL(aHeaderData, EPeek);     
        }     
    else     
        {     
        data = CData::NewL(aHeaderData);     
        }     
         
    CleanupStack::PushL(data);     
         
    if(aInputPacket.Length() <= 0)     
        {     
        TBuf8<2> outputBuffer;     
        User::LeaveIfError(data->Read(KNullDesC8(), outputBuffer));     
        }     
             
    else     
        {     
        HBufC8* inputPacket = ConvertDes16toHBufC8LC(aInputPacket);     
        HBufC8* outputPacket = HBufC8::NewLC(aInputPacket.Length());     
        TPtr8 outputPacketPtr = outputPacket->Des();     
         
        TRequestStatus status;     
        data->Read(*inputPacket, outputPacketPtr, status);     
        User::WaitForRequest(status);     
         
        if(status.Int() != KErrNone)     
            {     
            CleanupStack::PopAndDestroy(3, data);     
            return status.Int();     
            }     
             
        HBufC8* expectedOutput = ConvertDes16toHBufC8LC(aExpectedOutput);     
        if(expectedOutput->Compare(*outputPacket) == KErrNone)     
            {     
            returnVal = KErrNone;     
            }     
         
        CleanupStack::PopAndDestroy(3, inputPacket);     
        }     
         
    CleanupStack::PopAndDestroy(data);     
__UHEAP_MARKEND;     
         
    return returnVal;     
    }     
         
TVerdict CCAFDataAttributeStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TInt attribVal;     
    GetIntFromConfig(ConfigSection(),_L("attribute"), attribVal);     
         
    TInt expectedValue;     
    GetIntFromConfig(ConfigSection(),_L("value"), expectedValue);     
      
__UHEAP_MARK;     
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
             
    CContent *content = CContent::NewLC(*headerData);     
    CData *data = content->OpenContentL(EPeek);     
    TInt value;     
    User::LeaveIfError(data->GetAttribute(attribVal, value));     
    delete data;     
         
    if(expectedValue == value)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF3(_L("CData::GetAttribute() Expected value: %d, actual value: %d"), expectedValue, value);        
        }     
         
    CleanupStack::PopAndDestroy(2, headerData);                      
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
         
      
TVerdict CCAFDataAttributeSetStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TInt attribute1;     
    GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);     
         
    TInt attribute2;     
    GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);     
         
    TInt expectedValue1;     
    GetIntFromConfig(ConfigSection(),_L("value1"),expectedValue1);     
         
    TInt expectedValue2;     
    GetIntFromConfig(ConfigSection(),_L("value2"),expectedValue2);     
      
__UHEAP_MARK;     
      
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
         
    CContent *content = CContent::NewLC(*headerData);     
    CData *data = content->OpenContentL(EPeek);     
                     
    RAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
    attributeSet.AddL(attribute1);     
    attributeSet.AddL(attribute2);     
             
    TInt result = data->GetAttributeSet(attributeSet);     
    delete data;     
         
    if(result == KErrNone)     
        {     
        TInt value1;         
        User::LeaveIfError(attributeSet.GetValue(attribute1, value1));     
             
        TInt value2;     
        User::LeaveIfError(attributeSet.GetValue(attribute2, value2));     
             
        if(expectedValue1 == value1 && expectedValue2 == value2 && attributeSet.Count() == 2)     
            {     
            SetTestStepResult(EPass);     
            }     
        else     
            {     
            INFO_PRINTF1(_L("CData::GetAttributeSet() values don't match expected values"));     
            }     
        }     
         
    else     
        {     
        INFO_PRINTF1(_L("CData::GetAttributeSet() failed"));     
        }     
             
    CleanupStack::PopAndDestroy(3, headerData);          
         
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
      
TVerdict CCAFDataStringAttributeStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TInt attribVal;     
    GetIntFromConfig(ConfigSection(),_L("attribute"),attribVal);     
         
    TPtrC expectedValue;     
    GetStringFromConfig(ConfigSection(),_L("value"),expectedValue);     
         
    TInt expectedResult;     
    GetIntFromConfig(ConfigSection(),_L("result"),expectedResult);     
         
__UHEAP_MARK;     
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
         
    CContent *content = CContent::NewLC(*headerData);     
    CData *data = content->OpenContentL(EPeek);     
                 
    TBuf <200> value;     
    TInt result = data->GetStringAttribute(attribVal, value);     
    delete data;     
         
    if(result == expectedResult && value == expectedValue)     
        {     
        SetTestStepResult(EPass);     
        }     
    else     
        {     
        INFO_PRINTF3(_L("CData::GetStringAttribute() Expected result: %d, actual result: %d"), expectedResult, result);     
        INFO_PRINTF3(_L("CData::GetStringAttribute() Expected value: %S, actual value: %S"), &expectedValue, &value);     
        }     
             
    CleanupStack::PopAndDestroy(2, headerData);          
             
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
      
TVerdict CCAFDataStringAttributeSetStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
      
    TInt attribute1;         
    GetIntFromConfig(ConfigSection(),_L("attribute1"),attribute1);     
         
    TInt attribute2;     
    GetIntFromConfig(ConfigSection(),_L("attribute2"),attribute2);     
         
    TPtrC expectedValue1;     
    GetStringFromConfig(ConfigSection(),_L("value1"),expectedValue1);     
         
    TPtrC expectedValue2;     
    GetStringFromConfig(ConfigSection(),_L("value2"),expectedValue2);     
         
__UHEAP_MARK;     
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
         
    CContent *content = CContent::NewLC(*headerData);     
    CData *data = content->OpenContentL(EPeek);     
         
    RStringAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
    attributeSet.AddL(attribute1);     
    attributeSet.AddL(attribute2);     
             
    TInt result = data->GetStringAttributeSet(attributeSet);     
    delete data;     
             
    TBuf <200> value1;     
    TBuf <200> value2;     
    if(result == KErrNone)     
        {     
        TInt result3 = attributeSet.GetValue(attribute1, value1);     
        TInt result4 = attributeSet.GetValue(attribute2, value2);     
                 
        if(value1 == expectedValue1 && value2 == expectedValue2 && attributeSet.Count() == 2     
         && result3 == KErrNone && result4 == KErrNone)     
            {     
            SetTestStepResult(EPass);     
            }     
        else     
            {     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute1.Expected value: %S, actual value: %S"), &expectedValue1, &value1);     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute2.Expected value: %S, actual value: %S"), &expectedValue2, &value2);     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute1. Expected result: %d, actual result: %d"), 0, result3);     
            INFO_PRINTF3(_L("RStringAttributeSet::GetValue() for attribute2. Expected result: %d, actual result: %d"), 0, result4);      
            }     
        }     
    else     
        {     
        INFO_PRINTF1(_L("CData::GetStringAttributeSet() failed"));     
        }        
         
    CleanupStack::PopAndDestroy(3, headerData);          
      
__UHEAP_MARKEND;     
      
    return TestStepResult();     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
