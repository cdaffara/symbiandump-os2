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
// These tests are to allow the parser to parse a wide range of valid 
// documents allowing for greater coverage of its functionality.
// The fact that the documents are not parsed correctly is not the scope of
// these tests, but instead the stability of the parser is.
// The tests also incorporates indirect iTesting of the StringDictionaries
// as the parser uses these during its paring phase.
// If these iTest were to fail, it would give a good indication that the parser,
// dictionary, or document being parsed where at fault. As the document being
// parsed should never change (except for the addition of new documents),
// this should narrow the search.
// The output of what is parsed is withheld to speed up the processing of 
// this automated iTest. Uncommenting the macro __SHOW_MANUAL_OUTPUT__ within 
// the MMP file should allow for the output to be produced if the user so 
// wishes.
// Specific tests taking into account what the parser parses should also be
// supplied for a fully comprehensive test suite.
// 
//

#include <f32file.h>

#include <ecom/ecom.h>
#include <stringpool.h>
#include <e32test.h>

#include <xml/parserfeature.h>
#include <xml/xmlframeworkerrors.h>

#include "stabilitytestclass.h"
#include "testdocuments.h"

using namespace Xml;

#ifdef __COMPARE_OUTPUT__
#define INSERT_INTO_OUTPUT_BUFFER(output)  \
						iOutput->Insert(iOutput->Length(), output)
#define FORMAT_OUTPUT_BUFFER(format, data) \
						iFormat->Format(format, data)

#else
#define INSERT_INTO_OUTPUT_BUFFER (output)
#define FORMAT_OUTPUT_BUFFER (format, data)
#endif //__COMPARE_OUTPUT__

//
// The way data is formatted:
//   - Data is built up into iBuffer.
//   - This is then inserted into iFormat, adding formatting, 
//     e.g. start < and end >.
//   - If the test compares the data this formatted data is then 
//     inserted into iCompare for comparision.
//   - iBuffer is cleared and rebuilt on each element.
//   - iFormat is also used as a secondary buffer of iBuffer so as
//     to convert TDesC8s into TDesC16s.
//   - iOutput is only used for those tests that compare data,
//     i.e. what's generated to what's expected.
//



CStabilityTestClass* CStabilityTestClass::NewL(RTest& aTest, 
											   TBool aIsOomTest,
											   TInt aChunkSize)
	{
	CStabilityTestClass* self = new(ELeave) CStabilityTestClass(aTest, aIsOomTest, aChunkSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CStabilityTestClass::CStabilityTestClass(RTest& aTest, 
										 TBool aIsOomTest,
										 TInt aChunkSize)
:	iTest(aTest),
	iChunkSize(aChunkSize),
	iIsOomTest(aIsOomTest)
	{
	// do nothing;
	}


CStabilityTestClass::~CStabilityTestClass()
	{
	iFs.Close();
	iCurrentIndex.Reset();
	iEntries.ResetAndDestroy();

#ifdef __COMPARE_OUTPUT__
	delete iOutput;
#endif // __COMPARE_OUTPUT__

	delete iBuffer;

#ifdef __SHOW_MANUAL_OUTPUT__
	delete iFormat;
#endif // __SHOW_MANUAL_OUTPUT__
	}


void CStabilityTestClass::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());

#ifdef __COMPARE_OUTPUT__
	iOutput = new(ELeave) TBuf16<OUTPUT_SIZE>;
#endif // __COMPARE_OUTPUT__

	iBuffer = new(ELeave) TBuf16<BUFFER_SIZE>;

#ifdef __SHOW_MANUAL_OUTPUT__
	iFormat = new(ELeave) TBuf16<XBUFFER_SIZE>;
#endif // __SHOW_MANUAL_OUTPUT__
	}


// From MContentHandler

void CStabilityTestClass::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	iTest.Printf(_L("CStabilityTestClass::OnStartDocumentL Error code:%d\n"), aErrorCode);
	iError = aErrorCode;
	}

	
void CStabilityTestClass::OnEndDocumentL(TInt aErrorCode)
	{
	iTest.Printf(_L("\nCStabilityTestClass::OnEndDocumentL Error code:%d\n"), aErrorCode);
	iError = aErrorCode;
	}

	
#ifdef __SHOW_MANUAL_OUTPUT__
void CStabilityTestClass::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttribute, TInt aErrorCode)
#else
void CStabilityTestClass::OnStartElementL(const RTagInfo& /*aElement*/, const RAttributeArray& /*aAttribute*/, TInt aErrorCode)
#endif
	{
	iError = aErrorCode;
	
#ifdef __SHOW_MANUAL_OUTPUT__
	// element
	iBuffer->Copy (aElement.LocalName().DesC());
	
	// check for the namespace
	if (aElement.Uri().DesC() != KNullDesC8())
		{
		iBuffer->Append (_L(" xmlns=\'"));
		iFormat->Copy(aElement.Uri().DesC());
		iBuffer->Append (*iFormat);
		iBuffer->Append (_L("\'"));
		}

	if (aAttribute.Count() == 0)
		{
		RDebug::Print(_L("<%S>"), iBuffer);	
		
		FORMAT_OUTPUT_BUFFER(_L("<%S>"), iBuffer);
		INSERT_INTO_OUTPUT_BUFFER(*iFormat);
		}
	else
		{
		RPointerArray<TBuf16<256> > attr_list;

		for (TInt p=0; p<aAttribute.Count(); ++p)
			{
			// Keep list of namespaces
			
			// check for the namespace
			if ((aAttribute[p].Attribute().Uri().DesC() != aElement.Uri().DesC()) &&
				(aAttribute[p].Attribute().Uri().DesC() != KNullDesC8))
				{
				TBuf16<256> *attr_ns = new(ELeave) TBuf16<256>;
				TBool present=EFalse;
				TInt x=0;
				attr_ns->Copy(aAttribute[p].Attribute().Uri().DesC());

				while (!present && x<attr_list.Count())
					{
					if (attr_list[x]->Compare(*attr_ns)==0)
						{
						present=ETrue;
						}
					else
						{
						++x;
						}
					}

				if (!present)
					{
					attr_list.Append(attr_ns);
					}
				else
					{
					delete attr_ns;
					}
				}
			}
			
		for (TInt j=0; j<attr_list.Count(); ++j)
			{
			iBuffer->Append (_L(" xmlns='"));
			iBuffer->Append (*attr_list[j]);
			iBuffer->Append (_L("'"));
			}

		RDebug::Print(_L("<%S"), iBuffer);
		attr_list.ResetAndDestroy();

		FORMAT_OUTPUT_BUFFER(_L("<%S"), iBuffer);
		INSERT_INTO_OUTPUT_BUFFER(*iFormat);

		for (TInt i=0; i<aAttribute.Count(); ++i)
			{
			RDebug::Print(_L(" "));
			INSERT_INTO_OUTPUT_BUFFER(_L(" "));

			// attribute
			iBuffer->Copy(aAttribute[i].Attribute().LocalName().DesC());
			iBuffer->Append(_L("=\""));

			// value
			iFormat->Copy(aAttribute[i].Value().DesC());

			iBuffer->Append(*iFormat);
			iBuffer->Append(_L("\""));
			
			RDebug::Print(_L("%S"), iBuffer);
			
			FORMAT_OUTPUT_BUFFER(_L("%S"), iBuffer);
			INSERT_INTO_OUTPUT_BUFFER(*iFormat);
			}
		RDebug::Print(_L(">"));
		INSERT_INTO_OUTPUT_BUFFER( _L(">"));
		}
#endif // __SHOW_MANUAL_OUTPUT__
	}

	
#ifdef __SHOW_MANUAL_OUTPUT__
void CStabilityTestClass::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
#else
void CStabilityTestClass::OnEndElementL(const RTagInfo& /*aElement*/, TInt aErrorCode)
#endif
	{
	iError = aErrorCode;

#ifdef __SHOW_MANUAL_OUTPUT__
	iBuffer->Copy (aElement.LocalName().DesC());
	RDebug::Print(_L("</%S>"), iBuffer);

	FORMAT_OUTPUT_BUFFER(_L("</%S>"), iBuffer);
	INSERT_INTO_OUTPUT_BUFFER(*iFormat);

#endif // __SHOW_MANUAL_OUTPUT__
	}



void CStabilityTestClass::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
	{
	iError = aErrorCode;

  	iBuffer->Copy (aBytes);

   	if (aBytes.Length() && aBytes[0] == 0x02) // SyncML opaque data only
   		{
   		if (iParseMode & ERawContent)
	   		{
	   		// raw bytes	   		
	   		// Write to a new file to be parsed later
			RFile opaqueFile;
			CleanupClosePushL(opaqueFile);
			
			// loose the last '\'
			TPtrC path (iFileName.Path().Ptr(), iFileName.Path().Length()-1);
			TInt pos = 0;
			User::LeaveIfError(pos = path.LocateReverse('\\'));
			TPtrC mid = path.Mid(++pos);
			
			TFileName name(KOpaqueDirectory());
			name.Append (KDirSeperator());
	
			TInt err = KErrNone;
			err = iFs.MkDirAll(name);
			
			if (err != KErrNone && err != KErrAlreadyExists)
				{
				User::Leave(err);
				}

			name.Append (mid);
			name.Append (KUnderscore());
			name.Append (iFileName.NameAndExt());

			err = (opaqueFile.Replace(iFs, name, EFileWrite));
			User::LeaveIfError(opaqueFile.Write(aBytes));

			opaqueFile.Close();
	   		CleanupStack::Pop(&opaqueFile);

#ifdef __SHOW_MANUAL_OUTPUT__	   		
	   		// Print the data to the logs anyway, even if its unreadable
	   		for (TInt i=0; i<aBytes.Length(); ++i)
	   			{
	   			RDebug::Print(_L("%X"), aBytes[i]);
	   			
				FORMAT_OUTPUT_BUFFER(_L("%X"), aBytes[i]);
				INSERT_INTO_OUTPUT_BUFFER(*iFormat);
	   			}
#endif // __SHOW_MANUAL_OUTPUT__
	   		}
   		else
	   		{
	   		// utf8
#ifdef __SHOW_MANUAL_OUTPUT__
			RDebug::Print(_L("%S"), iBuffer);
			
			FORMAT_OUTPUT_BUFFER(_L("%S"), iBuffer);
			INSERT_INTO_OUTPUT_BUFFER(*iFormat);
			
#endif // __SHOW_MANUAL_OUTPUT__
	   		}
   		}
	else
		{
#ifdef __SHOW_MANUAL_OUTPUT__
		RDebug::Print(_L("%S"), iBuffer);

		FORMAT_OUTPUT_BUFFER(_L("%S"), iBuffer);
		INSERT_INTO_OUTPUT_BUFFER(*iFormat);

#endif // __SHOW_MANUAL_OUTPUT__
		}
	}


void CStabilityTestClass::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/)
	{
	// Not supported
	iTest(EFalse);
	}


void CStabilityTestClass::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
	{
	// Not supported
	iTest(EFalse);
	}


void CStabilityTestClass::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{
	// Not supported
	iTest(EFalse);
	}


void CStabilityTestClass::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
	{
	// Not supported
	iTest(EFalse);
	}


#ifdef __SHOW_MANUAL_OUTPUT__
void CStabilityTestClass::OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode)
#else
void CStabilityTestClass::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
#endif
	{
	iError = aErrorCode;

#ifdef __SHOW_MANUAL_OUTPUT__
	// attrstart	
	iFormat->Copy (aTarget);
	iFormat->Append(_L("=\""));
	iBuffer->Append(*iFormat);

	// attrvalue
	iFormat->Copy(aData);
	iFormat->Append(_L("\""));
	iBuffer->Append(*iFormat);
	
	RDebug::Print(_L("{{%S}}"), iBuffer);

	FORMAT_OUTPUT_BUFFER(_L("{{%S}}"), iBuffer);
	INSERT_INTO_OUTPUT_BUFFER(*iFormat);
	
#endif // __SHOW_MANUAL_OUTPUT__
	}


#ifdef __SHOW_MANUAL_OUTPUT__
void CStabilityTestClass::OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode)
#else
void CStabilityTestClass::OnExtensionL(const RString& /*aData*/, TInt /*aToken*/, TInt aErrorCode)
#endif
	{
	iError = aErrorCode;

#ifdef __SHOW_MANUAL_OUTPUT__
	iBuffer->Copy (aData.DesC());
	RDebug::Print(_L("[[0x%x : %S]]"), aToken, iBuffer);

	FORMAT_OUTPUT_BUFFER(_L("[[0x%x : "), aToken);
	INSERT_INTO_OUTPUT_BUFFER(*iFormat);
	FORMAT_OUTPUT_BUFFER(_L("%S]]"), iBuffer);
	INSERT_INTO_OUTPUT_BUFFER(*iFormat);

#endif // __SHOW_MANUAL_OUTPUT__
	}


void CStabilityTestClass::OnError(TInt aErrorCode)
	{
	iError = aErrorCode;
	iTest.Printf(_L("\nCStabilityTestClass::OnError Error code:%d\n"), aErrorCode);
	}


TAny* CStabilityTestClass::GetExtendedInterface(const TInt32 aUid)
/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
	{
	if (aUid == MWbxmlExtensionHandler::EExtInterfaceUid)
		{
		return static_cast<MWbxmlExtensionHandler*>(this);
		}
	return 0;
	}


//----------------------------------------------------------------------------
//Parser Tests


void CStabilityTestClass::StabilityTestL(const TDesC& aAbsoluteDirPath, const TDesC& aExt, 
										 ClassFuncPtrL aTestFuncL)
	{
	iTest.Next(_L("StabilityTestL"));

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	//--------------

	ParseDirL(aAbsoluteDirPath, aExt, aTestFuncL);
	iEntries.Close();
	iCurrentIndex.Close();


	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	iTest(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


void CStabilityTestClass::ParseDirL(const TDesC& aAbsoluteDirPath, 
									const TDesC& aExt, ClassFuncPtrL aTestFuncL)
	{
	iError = KErrNone;

	TFileName fileDirNameWithSep;
	fileDirNameWithSep = aAbsoluteDirPath;
	fileDirNameWithSep.Append(KDirSeperator);

	CDir *dirList = NULL;
	CDir *entries = NULL;

	User::LeaveIfError (iFs.GetDir(fileDirNameWithSep, KEntryAttNormal|KEntryAttDir, ESortNone, entries, dirList));
	delete dirList;
	dirList = NULL;

	iEntries.Append(entries);	
	iCurrentIndex.Append(entries->Count());		

	entries = NULL;
	CleanupStack::PushL(iEntries[iEntries.Count()-1]);

	// Point to last test that ran (OOM) or to be run
	CDir*& entry = iEntries[iEntries.Count()-1];
	TInt& ind = iCurrentIndex[iCurrentIndex.Count()-1];

	TParse entryName;
		
	// Also acts as base test for recursiveness
	while (--ind >= 0)
		{
		entryName.Set((*entry)[ind].iName, &fileDirNameWithSep, NULL);

		if (!(*entry)[ind].IsDir())
			{
			if (entryName.Ext() == aExt)
				{
				ParseEntryL(entryName.FullName(), aTestFuncL);
				}
			}
		else
			{
			ParseDirL(entryName.FullName(), aExt, aTestFuncL);
			}
		}
	
	CleanupStack::PopAndDestroy(iEntries[iEntries.Count()-1]);

	iCurrentIndex.Remove(iCurrentIndex.Count()-1);
	iEntries.Remove(iEntries.Count()-1);
	}


void CStabilityTestClass::ParseEntryL(const TDesC& aAbsoluteFilename,
									  ClassFuncPtrL aTestFuncL)
	{
	// Need to set this for OnContentL
	iFileName.Set (aAbsoluteFilename, NULL, NULL);
	iTest.Printf(_L("\n"));

	iParseMode = EErrorOnUnrecognisedTags|
				 ERawContent;

	if(iIsOomTest)
		{
		OomProcess(aTestFuncL);
		}
	else
		{
		(this->*aTestFuncL)(aAbsoluteFilename);
		}
	}


void CStabilityTestClass::TestWholeL(const TDesC& aFileName)
	{
	iTest.Next(_L("TestWholeL"));
	
	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	//--------------

	iTest.Printf(_L("\nParsing document: %S\n"),&aFileName);

	// Load the Parser and parse the buffer

	iParser = CParser::NewLC(KWbxmlParserDataType, *this);
	
	User::LeaveIfError(iParser->EnableFeature(iParseMode));

	// We parse to completion - parser will stop the ActiveSchedular
	ParseL(*iParser, iFs, aFileName); 

	CleanupStack::PopAndDestroy(iParser);
	REComSession::FinalClose(); // Don't want leaks outside the iTest 


	if(iIsOomTest)
		{
		// Only receives KErrNoMemory on OOM tests.
		// Do this so the calling function knows we need to keep parsing 
		// the same document.
		if (iError == KErrNoMemory)
			{
			User::Leave(iError);
			}
		}
	else
		{	
		iTest(iError == KErrNone);
		}

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	iTest(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


void CStabilityTestClass::TestChunkL(const TDesC& aFileName)
	{
	iTest.Next(_L("TestChunkL"));

	// Create the Parser without a uid list 
	// The data will be streamed a bit at a time
	// so as to iTest the reaction of the parser.
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	//--------------

	iTest.Printf(_L("\nParsing document: %S\n"),&aFileName);

	// Read the file into the buffer

	RFile xmlFile;
	CleanupClosePushL(xmlFile);

	User::LeaveIfError(xmlFile.Open(iFs, aFileName, EFileRead));

	TInt streamSize;
	User::LeaveIfError(xmlFile.Size(streamSize));

	// Load the Parser and parse the buffer

	iParser = CParser::NewLC(KWbxmlParserDataType, *this);
	
	User::LeaveIfError(iParser->EnableFeature(iParseMode));

	// Always size to the max as we can not choose, compilation errors.
	TBuf8<KMaxChunkSize> data;
	User::LeaveIfError(xmlFile.Read(data, iChunkSize));
	TInt length = data.Length();
	
	iError = KErrNone;
	
	while (length)
		{
		iParser->ParseL(data);
		
		// When no more data is read descriptors length is 0.
		// Will throw another KErrEof
		User::LeaveIfError(xmlFile.Read(data, iChunkSize));
		length = data.Length();
		}

	iParser->ParseEndL();

	// OnError should report only XML parsing specific errors
	iTest(iError == KErrNone || iError <= KErrXmlFirst && iError >= KErrXmlLast);

	CleanupStack::PopAndDestroy(iParser);
	CleanupStack::PopAndDestroy(&xmlFile);         // Closes as well
	REComSession::FinalClose(); // Don't want leaks outside the iTest 


	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	iTest(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}



void CStabilityTestClass::OomProcess(ClassFuncPtrL aTestFuncL)
	{	
	iTest.Next(_L("OomProcess test"));
	TInt err, tryCount = 0;
	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++tryCount);
		User::__DbgMarkStart(RHeap::EUser);
		TRAP(err, (this->*aTestFuncL)(iFileName.FullName()));
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(err==KErrNoMemory);

	if(err==KErrNone)
		{
		// Reset
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
		}
	else
		{
		User::Panic(_L("Unexpected leave reason"),err);
		}

	iTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}
	
	
void CStabilityTestClass::TestBehaviour(const TDesC& aSrc, TPassOrFailureSettings& aTestSettings)
	{
	iTest.Next(_L("TestBehaviour"));

	TRAPD(err, BehaviourTestL(aSrc, aTestSettings));
	iTest (err == aTestSettings.iExpectedCode);
	}
	
	
void CStabilityTestClass::BehaviourTestL(const TDesC& aSrc, TPassOrFailureSettings& aTestSettings)
	{
	iTest.Next(_L("BehaviourTestL"));

	// Test the parser with the values provided
	// ===========================================

	
	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	//--------------

	iError = 0;

	// iLoad the parser and parse the data

	iParser = CParser::NewLC(KWbxmlParserDataType, *this);

	iParser->AddPreloadedDictionaryL(*(aTestSettings.iStringDictionaryUri));

	User::LeaveIfError(iParser->EnableFeature(aTestSettings.iParseMode));

	if (aTestSettings.iDoParseDocument)
		{
		if (aTestSettings.iFilenameProvided)
			{
			ParseL(*iParser, iFs, aSrc);
			}
		else
			{
			TBuf8<256> buf8;
			
			// copy will ignore the upper byte if the byte-pair < 256, otherwise the value 1 is used.
			buf8.Copy(aSrc);
			
			// whole file should be in descriptor/
			ParseL(*iParser, buf8);
			}
		}
					
	CleanupStack::PopAndDestroy(iParser);

	REComSession::FinalClose(); // Don't want leaks outside the iTest 


	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	iTest(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;	


	User::LeaveIfError(iError);
	}
	
