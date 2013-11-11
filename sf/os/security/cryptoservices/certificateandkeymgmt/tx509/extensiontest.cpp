/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "x509constraintext.h"
#include "extensiontest.h"
#include "t_input.h"

_LIT(KFileNameStart, "<filename>");
_LIT(KExpectedDeviceIdList, "<device_id_list>");
_LIT(KExpectedDevice, "<device_id>");
_LIT(KExpectedSidList, "<sid_list>");
_LIT(KExpectedSid, "<sid>");
_LIT(KExpectedVidList, "<vid_list>");
_LIT(KExpectedVid, "<vid>");
_LIT(KExpectedCapabilities, "<capabilities>");
_LIT(KCorrupt, "<corrupt>");
_LIT(KMatch, "<match>");

CTestAction* CExtensionTest::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CExtensionTest::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CExtensionTest::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CExtensionTest* self = new(ELeave) CExtensionTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CExtensionTest::CExtensionTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), 
	iFs(aFs)
	{
	}

CExtensionTest::~CExtensionTest()
	{	
	iExpectedDeviceIdList.ResetAndDestroy();
	iExpectedDeviceIdList.Close();		
	iExpectedSidList.Close();
	iExpectedVidList.Close();
	}	

void CExtensionTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	TInt startPos = 0;
	TInt err = 0;
	
	HBufC* aBody = HBufC::NewLC(aTestActionSpec.iActionBody.Length());	
	aBody->Des().Copy(aTestActionSpec.iActionBody);
	
	// Filename of the certificate file
	TPtrC fileNameStart = Input::ParseElement(*aBody, KFileNameStart);
	iCertFileName.Copy(fileNameStart);

	// Whether we expect at least on of the extensions to be corrupt
	TPtrC corruptStart = Input::ParseElement(*aBody, KCorrupt);	
	if (corruptStart.CompareF(_L("True")) == 0)
		{
		iExpectedCorrupt = ETrue;
		}				
		
	// We expect the certificate to match the constraints
	TPtrC matchStart = Input::ParseElement(*aBody, KMatch);	
	if (matchStart.CompareF(_L("True")) == 0)
		{
		iExpectedMatch = ETrue;
		}						

	// Extract the lists of constraints that are expected to be present 		
	startPos = 0;
	TPtrC deviceListStart = Input::ParseElement(*aBody, KExpectedDeviceIdList, startPos, err);
	if (err >= 0)
		{
		BuildStringListL(iExpectedDeviceIdList, deviceListStart, KExpectedDevice);
		iDeviceIdsPresent = ETrue;		
		}

	startPos = 0;
	TPtrC sidListStart = Input::ParseElement(*aBody, KExpectedSidList, startPos, err);
	if (err >= 0)
		{
		BuildIntList(iExpectedSidList, sidListStart, KExpectedSid);
		iSidsPresent = ETrue;
		}	

	startPos = 0;
	TPtrC vidListStart = Input::ParseElement(*aBody, KExpectedVidList, startPos, err);
	if (err >= 0)
		{
		BuildIntList(iExpectedVidList, vidListStart, KExpectedVid);
		iVidsPresent = ETrue;
		}	
		
	startPos = 0;
	TPtrC capabilities = Input::ParseElement(*aBody, KExpectedCapabilities, startPos, err);
	if (err >= 0)
		{
		BuildCapabilitySet(iExpectedCapabilities, capabilities);
		iCapabilitiesPresent = ETrue;
		}	
	
	CleanupStack::PopAndDestroy(aBody);
	}	

void CExtensionTest::BuildStringListL(RPointerArray<HBufC>& aStrings, const TDesC& aBuf, const TDesC& aTag)
	{
	TInt pos = 0;
	TInt err = 0;	
	do
		{			
		// Find next value for the specified tag and add it to the string array
		// if it exists.
		TPtrC str = Input::ParseElement(aBuf, aTag, pos, err);
		if (err >= 0)
			{
			HBufC* string = str.AllocLC();			
			aStrings.AppendL(string);
			CleanupStack::Pop(string);
			}								
		}
	while (err >= 0);	
	}

void CExtensionTest::BuildIntList(RArray<TInt>& aInts, const TDesC& aBuf, const TDesC& aTag)
	{	
	TInt pos = 0;
	TInt err = 0;	
	do
		{			
		// Attempt to convert the contents of val to an int and store in  
		// the aInts array if it is a valid integer.
		TInt n = Input::ParseIntElement(aBuf, aTag, pos, err);
		if (err >= 0)
			{
			// This append should not fail as the parsing went fine. 
			// So, ignoring the leave just to satisfy non leaving method convention.
			TRAP_IGNORE(aInts.AppendL(n));
			}								
		}
	while (err >= 0);
	}

void CExtensionTest::BuildCapabilitySet(TCapabilitySet& aCapabilitySet, const TDesC& aBuf)	
	{
	aCapabilitySet.SetEmpty();
	TUint length = aBuf.Length();
	for (TUint i = 0; i < length && i < ECapability_Limit; i++) 
		{
		if (aBuf[i] == '1')
			{
			aCapabilitySet.AddCapability(static_cast<TCapability>(i));
			}			
		}
	}
			
void CExtensionTest::PerformAction(TRequestStatus& aStatus)
	{
	HBufC8* buf = NULL;
	TInt err = KErrNone;
	
	TRAP(err, buf = Input::ReadFileL(iCertFileName, iFs));
	if (err == KErrNotFound)
		{
		iResult = EFalse;
		iFinished = ETrue;
		SetScriptError(EFileNotFound, iCertFileName);
		TRequestStatus* status = &aStatus;
		iActionState = EPostrequisite;
		User::RequestComplete(status, KErrNone);
		return;
		}
	else if (err != KErrNone)
		{
		User::Leave(err);
		}
		
	CleanupStack::PushL(buf);
	CX509Certificate* cert = CX509Certificate::NewLC(buf->Des());

	Print(_L("Checking certificate extensions in file "));
	PrintLine(iCertFileName);
	
	TBool match = EFalse;
	TBool corrupt = EFalse;
	CheckExtensionsL(*cert, match, corrupt);
	if (corrupt)
		{
		iResult = iExpectedCorrupt;
		if (!iResult)
			{
			Print(_L("Found unexpected corrupt extension."));
			}
		}
	else 
		{
		// no error. test whether the certificate matched the
		// test script.
		iResult = (match == iExpectedMatch);
		}
	CleanupStack::PopAndDestroy(2, buf); // cert, buf	

	TRequestStatus* status = &aStatus;
	iActionState = EPostrequisite;
	User::RequestComplete(status, KErrNone);
	}	
	
void CExtensionTest::CheckExtensionsL(const CX509Certificate& cert, 
	TBool& match, TBool& corrupt)
	{
	TInt err = KErrNone;
	
	match = ETrue;
	corrupt = EFalse;

	TRAP(err, match &= CheckDeviceIdListL(cert));
	if (err == KErrArgument)
		{		
		PrintLine(_L("The device id constraint is corrupt."));
		corrupt = ETrue;
		}
	else if (err != KErrNone) 
		{
		User::Leave(err);
		}	
				
	TRAP(err, match &= CheckSidListL(cert));
	if (err == KErrArgument)
		{		
		PrintLine(_L("The secure id constraint is corrupt."));
		corrupt = ETrue;
		}
	else if (err != KErrNone) 
		{
		User::Leave(err);
		}
		
	TRAP(err, match &= CheckVidListL(cert));
	if (err == KErrArgument)
		{		
		PrintLine(_L("The vendor id constraint is corrupt."));
		corrupt = ETrue;
		}
	else if (err != KErrNone) 
		{
		User::Leave(err);
		}		
		
	TRAP(err, match &= CheckCapabilitiesL(cert));
	if (err == KErrArgument)
		{		
		PrintLine(_L("The capabilities constraint is corrupt."));
		corrupt = ETrue;
		}
	else if (err != KErrNone) 
		{
		User::Leave(err);
		}				
	}
	
TBool CExtensionTest::CheckDeviceIdListL(const CX509Certificate& cert) 
	{
	TBool match = ETrue;
	const CX509CertExtension* ext = cert.Extension(KDeviceIdListConstraint);	
	if (ext)
		{
		// use NewL because this covers NewLC as well
		CX509Utf8StringListExt* stringListExt = CX509Utf8StringListExt::NewL(ext->Data());	
		CleanupStack::PushL(stringListExt);
		if (! IsEqual(stringListExt->StringArray(), iExpectedDeviceIdList))
			{
			PrintLine(_L("Device Id list is different."));
			match = EFalse;
			}
		CleanupStack::PopAndDestroy(stringListExt);		
		}	
	else if (iDeviceIdsPresent) 
		{
		PrintLine(_L("Device Id constraint is missing."));
		match = EFalse;
		}	
	return match;
	}
	
TBool CExtensionTest::CheckSidListL(const CX509Certificate& cert) 
	{
	const CX509CertExtension* ext = cert.Extension(KSidListConstraint);
	TBool match = ETrue;
	if (ext)
		{				
		// use NewL because this covers NewLC as well
		CX509IntListExt* intListExt = CX509IntListExt::NewL(ext->Data());
		CleanupStack::PushL(intListExt);
		if (! IsEqual(intListExt->IntArray(), iExpectedSidList))
			{
			PrintLine(_L("SID list is different"));
			match = EFalse;
			}
		CleanupStack::PopAndDestroy(intListExt);		
		}	
	else if (iSidsPresent) 
		{
			PrintLine(_L("SID constraint is missing."));
			match = EFalse;
		}
	return match;	
	}
	
TBool CExtensionTest::CheckVidListL(const CX509Certificate& cert) 
	{
	const CX509CertExtension* ext = cert.Extension(KVidListConstraint);
	TBool match = ETrue;
	if (ext)
		{
		// use NewL because this covers NewLC as well
		CX509IntListExt* intListExt = CX509IntListExt::NewL(ext->Data());
		CleanupStack::PushL(intListExt);
								
		if (! IsEqual(intListExt->IntArray(), iExpectedVidList))
			{
			PrintLine(_L("VID list is different"));
			match = EFalse;
			}
		CleanupStack::PopAndDestroy(intListExt);		
		}	
	else if (iVidsPresent) 
		{
		PrintLine(_L("VID constraint is missing."));
		match = EFalse;
		}	
	return match;
	}	
	
TBool CExtensionTest::CheckCapabilitiesL(const CX509Certificate& cert) 
	{
	const CX509CertExtension* ext = cert.Extension(KCapabilitiesConstraint);
	TBool match = ETrue;
	if (ext)
		{		
		// use NewL because this covers NewLC as well
		CX509CapabilitySetExt* capabilitySetExt = CX509CapabilitySetExt::NewL(ext->Data());		
		CleanupStack::PushL(capabilitySetExt);
		
		const TCapabilitySet& capabilitySet = capabilitySetExt->CapabilitySet();		

		if (! (capabilitySet.HasCapabilities(iExpectedCapabilities)
			&& iExpectedCapabilities.HasCapabilities(capabilitySet)))
			{
			PrintLine(_L("Capability constraints are different."));
			match = EFalse;
			}
		CleanupStack::PopAndDestroy(capabilitySetExt);		
		}	
	else if (iCapabilitiesPresent) 
		{
		PrintLine(_L("Capability constraint is missing."));
		match = EFalse;
		}	
	return match;		
	}

TBool CExtensionTest::IsEqual(const RArray<TInt>& aArray1, const RArray<TInt>& aArray2) 
	{			
	if (aArray1.Count() == aArray2.Count())
		{
		TInt count = aArray1.Count();				
		for (TInt i = 0; i < count; i++)
			{
			if (aArray1[i] != aArray2[i])
				{
				return EFalse;
				}
			}
		return ETrue;
		}	
	return EFalse;
	}

TBool CExtensionTest::IsEqual(const RPointerArray<HBufC>& aArray1, const RPointerArray<HBufC>& aArray2) 
	{			
	if (aArray1.Count() == aArray2.Count())
		{
		TInt count = aArray1.Count();				
		for (TInt i = 0; i < count; i++)
			{
			if (aArray1[i]->Compare(*aArray2[i]) != 0)
				{
				return EFalse;
				}
			}
		return ETrue;
		}	
	return EFalse;
	}
	
void CExtensionTest::DoReportAction() 
	{	
	}
	
void CExtensionTest::DoCheckResult(TInt /*aError*/)
	{
	}

void CExtensionTest::Print(const TDesC& aText)
	{
	iConsole.Printf(aText);
	iOut.writeString(aText);			
	}
void CExtensionTest::PrintLine(const TDesC& aText)
	{
	iConsole.Printf(aText);
	iConsole.Printf(_L("\n"));
	
	iOut.writeString(aText);			
	iOut.writeNewLine();					
	}
