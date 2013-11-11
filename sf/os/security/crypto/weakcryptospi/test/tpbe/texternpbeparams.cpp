/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32mem.h>
#include <s32file.h>
#include "texternpbeparams.h"


CTestAction* CExternPbeParams::NewL(
	RFs& aFs, CConsoleBase& aConsole,
	Output& aOut, const TTestActionSpec& aTestActionSpec)
/**
	Factory function allocates new instance of CExternPbeParams and extracts
	the element body from the supplied test action spec.

	@param	aFs				Used to parse XML script file.
	@param	aConsole		Required by CTestAction.
	@param	aOut			Required by CTestAction.
	@param	aTestActionSpec	Action specification contains type, name, and
							XML contents.
	@return					New instance of CExternPbeParams, which is owned
							by the caller.
 */
	{
	CExternPbeParams* self = new(ELeave) CExternPbeParams(aConsole, aOut, aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CExternPbeParams::CExternPbeParams(CConsoleBase& aConsole, Output& aOut, RFs& aFs)
/**
	This constructor exists to record the file server session and to initialize
	the CTestAction superclass.
	
	@param	aConsole		Required by CTestAction.
	@param	aOut			Required by CTestAction.
	@param	aFs				Used to read from and write to files in PerformAction,
							which stores and restores the externalized params.
 */
:	CTestAction(aConsole, aOut),
	iFs(aFs)
	{
	// empty.
	}

void CExternPbeParams::ConstructL(const TTestActionSpec& aTestActionSpec)
/**
	Second phase initialization initializes the superclass and
	makes a copy of the test element.
	
	@param	aTestActionSpec	Action specification contains type, name, and
							XML contents.	
 */
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = aTestActionSpec.iActionBody.AllocL();
	
	// iBody is deconstructed in DoPerformPrerequisite
	}

CExternPbeParams::~CExternPbeParams()
/**
	Free resources allocated in ConstructL.  Specifically,
	deletes the copy of the element body text.
 */
	{
	delete iBody;
	}

// -------- implement CTestAction --------

void CExternPbeParams::DoPerformPrerequisite(TRequestStatus& aStatus)
/**
	Override CTestAction by deconstructing element body allocated
	in ConstructL.
	
	If this function fails then DoPerformPostrequisite must still
	be called.
	
	@param	aStatus			This status is completed when the prerequisite
							has finished.  (This implementation is actually
							synchronous so the request will already be completed
							when it returns.)
 */
	{
	TRAPD(r, DoPerformPrerequisiteL());
	
	iActionState = CTestAction::EAction;
	TRequestStatus* ps = &aStatus;
	User::RequestComplete(ps, r);
	}

void CExternPbeParams::DoPerformPrerequisiteL()
/**
	Helper function for DoPerformPrerequisite contains resource allocation
	functions which can leave.
	
	Extracts cipher, salt, iv, iter count, and kdf values.
 */
	{
	_LIT8(KOrigFileName, "orig-filename");
	iOrigFileName = ReadStringLC(*iBody, KOrigFileName);
	CleanupStack::Pop(iOrigFileName);
	
	_LIT8(KExpCipherElemName, "expected-cipher");
	iExpCipher = ReadDecStringL(*iBody, KExpCipherElemName);
	_LIT8(KExpSaltElemName, "expected-salt");
	iExpSalt = ReadHexStringL(*iBody, KExpSaltElemName);
	_LIT8(KExpIvElemName, "expected-iv");
	iExpIv = ReadHexStringL(*iBody, KExpIvElemName);	
	_LIT8(KExpIterCountElemName, "expected-iter-count");
	iExpIterCount = ReadDecStringL(*iBody, KExpIterCountElemName);
	_LIT8(KExpKdfElemName, "expected-kdf");
	iExpKdf = ReadDecStringL(*iBody, KExpKdfElemName);
	}

void CExternPbeParams::DoPerformPostrequisite(TRequestStatus& aStatus)
/**
	Implements CTestAction by cleaning up data allocated in DoPerformPrerequisiteL.
	
	@param	aStatus			This status is completed to indicate the
							postrequisite has finished.  (This function
							is synchronous so the status is completed before
							this function returns.)
 */
	{
	delete iExpIv;
	iExpIv = 0;
	delete iExpSalt;
	iExpSalt = 0;
	delete iOrigFileName;
	iOrigFileName = NULL;
	
	iFinished = ETrue;
	TRequestStatus* ps = &aStatus;
	User::RequestComplete(ps, KErrNone);
	}

void CExternPbeParams::PerformAction(TRequestStatus& aStatus)
/**
	Implements CTestAction by running the actual tests.  This
	consists of:
	
	Reading an externalized CPBEncryptParms object and testing the
	cipher, salt, iv, iteration count, and KDF are as expected.
	
	Externalizing the object to memory.
	
	Testing the two externalizations are binary identical.
	
	Creating an equivalent object from scratch and externalizing it.
	
	Testing the externalizations are binary identical.
	
	As well as testing the objects can be stored reliably, this
	test also ensures that old (pre-PKCS#12) files can still be
	read and, and that objects are stored in the old format if they
	do not use any PKCS#12-specific features.  (I.e., they use the
	default PKCS#5 KDF.)
	
	@param	aStatus			This request status is completed when
							the action has finished, successfully
							or otherwise.  This implementation is
							synchronous, and so the status is actually
							completed before this function returns.
 */
	{
	TFileName fn;
	fn.Copy(*iOrigFileName);	// convert from narrow

	// ensure reference file matches re-externalized form
	
	TRAPD(r,
		TestDecodeMatchesScriptL(fn);
		TestReExternMatchesL(fn);
		TestScratchExternL(fn) );
		
	iResult = (r == KErrNone);
	iActionState = CTestAction::EPostrequisite;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

CPBEncryptParms* CExternPbeParams::InternalizeEncryptionParamsLC(const TDesC& aFileName)
/**
	Construct a CPBEncryptParms object from the externalized
	form in the named file.
	
	@param	aFileName		File which contains externalized form.
	@return					Internalized encryption parameters object
							which is placed on the cleanup stack.
 */
	{
	RFileReadStream frs;
	TInt r = frs.Open(iFs, aFileName, EFileStream | EFileRead);
	User::LeaveIfError(r);
	CleanupClosePushL(frs);	
	CPBEncryptParms* pbep = CPBEncryptParms::NewL(frs);
	CleanupStack::PopAndDestroy(&frs);
	CleanupStack::PushL(pbep);
	return pbep;
	}

void CExternPbeParams::TestDecodeMatchesScriptL(const TDesC& aFileName)
/**
	Test whether the encryption parameters which were externalized
	to the supplied file match those specified in the script file.
	
	@param	aFileName		Name of file which contains externalized form.
	@leave KErrGeneral		The internalized form doesn't match the parameters
							in the script.
 */
	{
	CPBEncryptParms* pbep = InternalizeEncryptionParamsLC(aFileName);
	
	TBool match =
			pbep->Cipher() == iExpCipher
		&&	pbep->Salt() == *iExpSalt
		&&	pbep->Iterations() == iExpIterCount;
	
	match = match && pbep->Kdf() == iExpKdf;

	if (! match)
		User::Leave(KErrGeneral);
	
	CleanupStack::PopAndDestroy(pbep);	
	}

void CExternPbeParams::CompareAgainstTestFileL(
	const TDesC& aFileName, const CPBEncryptParms& aParams)
/**
	Externalize the supplied parameters object and ensure it matches the
	test file.
	
	@param	aFileName		File which contains externalized parameters.
	@param	aParams			Test object to externalize.
	@leave KErrGeneral The externalized forms do not match.
 */
	{
 	// open a file stream on the externalized form
	RFileReadStream frs;
	TInt r = frs.Open(iFs, aFileName, EFileStream | EFileRead);
	User::LeaveIfError(r);
	CleanupClosePushL(frs);
	
	// externalize the object to memory
	const TInt KMaxBufferLen = 128;
	HBufC8* reExtBuf = HBufC8::NewLC(KMaxBufferLen);
	TPtr8 reDes = reExtBuf->Des();
	RDesWriteStream dws(reDes);
	CleanupClosePushL(dws);
	aParams.ExternalizeL(dws);
	dws.CommitL();
	
	// ensure the externalized forms are equal
	RDesReadStream drs(reDes);
	TInt fLen = frs.Source()->SizeL();
	TInt mLen = drs.Source()->SizeL();
	if (fLen != mLen)
		User::Leave(KErrGeneral);	

	TBuf8<1> fByte;
	TBuf8<1> mByte;
	for (TInt i = 0; i < fLen; ++i)
		{
		frs.ReadL(fByte, 1);
		drs.ReadL(mByte, 1);
		if (fByte != mByte)
			User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(3, &frs);	// frs, reExtBuf, dws	
	}

void CExternPbeParams::TestReExternMatchesL(const TDesC& aFileName)
/**
	Read the CPBEncryptParms object which is externalized in
	the named file, re-externalize it, and check the two
	representations are binary equivalent.
	
	@param	aFileName		Name of file which contains externalized form.
	@leave	KErrGeneral		The externalized forms are different.
 */
	{
 	CPBEncryptParms* pbep = InternalizeEncryptionParamsLC(aFileName);
 	
 	CompareAgainstTestFileL(aFileName, *pbep);
 	
	CleanupStack::PopAndDestroy(pbep);
	}

void CExternPbeParams::TestScratchExternL(const TDesC& aFileName)
/**
	Construct a CPBEncryptParams object from the parameter values
	in the script file.  Test it matches the test file.
	
	@param	aFileName		Test file which contains externalized parameters.
 */
	{
	CPBEncryptParms* pbep = CPBEncryptParms::NewLC(
		static_cast<TPBECipher>(iExpCipher),
		*iExpSalt,
		*iExpIv,
		iExpIterCount);
	
	pbep->SetKdf(static_cast<CPBEncryptParms::TKdf>(iExpKdf));

	CompareAgainstTestFileL(aFileName, *pbep);

	CleanupStack::PopAndDestroy(pbep);
	}

void CExternPbeParams::DoReportAction(void)
/**
	Implements CTestAction but is empty.
 */
	{
	// empty.
	}

void CExternPbeParams::DoCheckResult(TInt /*aError*/)
/**
	Implements CTestAction but is empty.
 */
	{
	// empty.
	}


// -------- support functions --------


HBufC8* CExternPbeParams::ReadHexStringL(const TDesC8& aBody, const TDesC8& aTag)
/**
	Convert a string in the test script to an 8-bit buffer.  The string
	is a sequence of hex digits, e.g. "abcdef01", which is converted to a
	descriptor containing the matching bytes {0xab, 0xcd, 0xef, 0x01}.
	
	@param	aBody			Body of parent element.
	@param	aTag			Bare tag name.  This function extracts the text
							between "<aTag>" and "</aTag>".
	@return					Newly-allocated buffer containing matching bytes.
							This is owned by the caller.
 */
	{
 	HBufC8* scriptString = ReadStringLC(aBody, aTag);

	TInt textLen = scriptString->Length();
	if ((textLen % 2) != 0)
		User::Leave(KErrCorrupt);
	TInt byteCount = textLen / 2;
	HBufC8* procString = HBufC8::NewMaxLC(byteCount);
	TPtr8 procDes = procString->Des();
	for (TInt i = 0; i < byteCount; ++i)
		{
		TUint8 byteVal;
		TInt r = TLex8(scriptString->Mid(i * 2, 2)).Val(byteVal, EHex);
		User::LeaveIfError(r);
		procDes[i] = byteVal;
		}
	
	CleanupStack::Pop(procString);
	CleanupStack::PopAndDestroy(scriptString);
	return procString;
	}

TInt CExternPbeParams::ReadDecStringL(const TDesC8& aBody, const TDesC8& aTag)
/**
	Finds a decimal text string in the script and returns the
	integer value which it represents.
	
	@param	aBody			Body of parent element.
	@param	aTag			Bare tag name.  This function extracts the text
							between "<aTag>" and "</aTag>".
	@return					Integer value encoded in the script file.
 */
 	{
 	HBufC8* scriptString = ReadStringLC(aBody, aTag);
 	
 	TInt value;
	User::LeaveIfError(TLex8(*scriptString).Val(value));
	CleanupStack::PopAndDestroy(scriptString);
	return value;
	}

HBufC8* CExternPbeParams::ReadStringLC(const TDesC8& aBody, const TDesC8& aTag)
/**
	Extracts a string from the supplied script file.
	
	@param	aBody			Body of parent element.
	@param	aTag			Bare tag name.  This function extracts the text
							between "<aTag>" and "</aTag>".
	@return					A copy of the string allocated on the heap.  The
							string is placed on the cleanup stack.
 */
	{
	TBuf8<32> startTag;
	startTag.Format(_L8("<%S>"), &aTag);
	TBuf8<32> endTag;
	endTag.Format(_L8("</%S>"), &aTag);
	
	TInt pos = 0;
	TInt r;
	const TPtrC8 contents = Input::ParseElement(
		aBody, startTag, endTag, pos, r);
	User::LeaveIfError(r);
	
	return contents.AllocLC();
	}

/**
	This code was originally in PerformAction to create the initial
	data files.
	
	// GENERATE PKCS5 TEST PARAMS FILE
	RFileWriteStream fws;
	r = fws.Replace(iFs, _L("c:\\tpbe\\pkcs5-orig.dat"), EFileStream | EFileWrite);
	User::LeaveIfError(r);
	CleanupClosePushL(fws);
	
	_LIT8(KSalt, "SALT4567");
	_LIT8(KIv, "IV23456789abcdef");
	const TInt KIterCount = 1234;
	CPBEncryptParms* pbep = CPBEncryptParms::NewLC(
		ECipherAES_CBC_256, KSalt, KIv, KIterCount);
	pbep->ExternalizeL(fws);
	fws.CommitL();
	CleanupStack::PopAndDestroy(2, &fws);

#ifdef SYMBIAN_PKCS12
	// GENERATE PKCS12 TEST PARAMS FILE
	RFileWriteStream fws;
	r = fws.Replace(iFs, _L("c:\\tpbe\\pkcs12-first.dat"), EFileStream | EFileWrite);
	User::LeaveIfError(r);
	CleanupClosePushL(fws);
	
	_LIT8(KSalt, "SALT4567");
	_LIT8(KIv, "IV23456789abcdef");
	const TInt KIterCount = 1234;
	CPBEncryptParms* pbep = CPBEncryptParms::NewLC(
		ECipherAES_CBC_256, KSalt, KIv, KIterCount);
	pbep->SetKdf(CPBEncryptParms::EKdfPkcs12);
	pbep->ExternalizeL(fws);
	fws.CommitL();
	CleanupStack::PopAndDestroy(2, &fws);
#endif	// #ifdef SYMBIAN_PKCS12

 */
