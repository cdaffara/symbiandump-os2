/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32mem.h>
#include <caf/caf.h>
#include <caf/rightsinfo.h>
#include <caf/manager.h>
#include <caf/supplieroutputfile.h>
#include "cafserver.h"
#include "CafUtilsStep.h"

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
#include "refcafhelper.h"
#include <caf/cafhelper.h>     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

using namespace ContentAccess;


CCafCDirStreamStep::~CCafCDirStreamStep()
	{
	}

CCafCDirStreamStep::CCafCDirStreamStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFCDirStreamStep);
	}


TVerdict CCafCDirStreamStep::doTestStepL()
	{
	SetTestStepResult(EPass);

	_LIT(KTcafDir, "C:\\tcaf\\");
	
	INFO_PRINTF1(_L("CDirStreamable test"));
	

	__UHEAP_MARK;
	
	CManager *manager = CManager::NewLC();
	
	CDir *fileList = NULL;
	User::LeaveIfError(manager->GetDir(KTcafDir(),KNullUid, ESortByName, fileList));
	
	CDirStreamable *newFileList = CDirStreamable::NewL(*fileList);
	delete fileList;
	
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	newFileList->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CDirStreamable from the stream
	CDirStreamable *streamDir = CDirStreamable::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	for(TInt i = 0; i < newFileList->Count(); i++)
		{
		if((*newFileList)[i].iName != (*streamDir)[i].iName)
			{
			SetTestStepResult(EFail);
			}
		if((*newFileList)[i].iType != (*streamDir)[i].iType)
			{
			SetTestStepResult(EFail);
			}
		}
	
	CleanupStack::PopAndDestroy(buf);
	
	delete newFileList;
	delete streamDir;
	CleanupStack::PopAndDestroy(manager);
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

CCafRAttributeSetStreamStep::~CCafRAttributeSetStreamStep()
	{
	}

CCafRAttributeSetStreamStep::CCafRAttributeSetStreamStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRAttributeSetStreamStep);
	}


TVerdict CCafRAttributeSetStreamStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("CRAttributeSet streaming test"));
	
	__UHEAP_MARK;
	

	RAttributeSet aSet;
	CleanupClosePushL(aSet);
	aSet.AddL(EIsProtected);
	aSet.AddL(EIsForwardable);
	aSet.AddL(EIsForwardable);
	
	aSet.SetValue(EIsProtected, (TInt) ETrue, KErrNone);
	aSet.SetValue(EIsForwardable, (TInt) EFalse, KErrNone);
		
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aSet.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CDirStreamable from the stream
	RAttributeSet bSet;
	CleanupClosePushL(bSet);
	bSet.InternalizeL(readStream);
		
	TInt valueA, valueB;
	if(aSet.Count() != bSet.Count())
		{
		SetTestStepResult(EFail);
		}
	
	for(TInt i = 0; i < aSet.Count(); i++)
		{
		valueA = -1;
		valueB = -1;
		if(aSet.GetValue(aSet[i],valueA) != bSet.GetValue(aSet[i],valueB))
			{
			SetTestStepResult(EFail);
			}
			
		if(valueA != valueB)
			{
			SetTestStepResult(EFail);
			}
		}
	
	CleanupStack::PopAndDestroy(&bSet);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(&aSet);
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}


CCafRStringAttributeSetStreamStep::~CCafRStringAttributeSetStreamStep()
	{
	}

CCafRStringAttributeSetStreamStep::CCafRStringAttributeSetStreamStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRStringAttributeSetStreamStep);
	}


TVerdict CCafRStringAttributeSetStreamStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("CRStringAttributeSet streaming test"));
	
	_LIT(KMimeType,"mime/type");

	__UHEAP_MARK;
	
	RStringAttributeSet aSet;
	CleanupClosePushL(aSet);
	aSet.AddL(EMimeType);
	aSet.AddL(EDescription);
	
	// Set some values, normally be done by the agent
	aSet.SetValue(EMimeType, KMimeType(), KErrNone);	
	aSet.SetValue(EDescription, KNullDesC(), KErrNotSupported);	
	
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aSet.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CDirStreamable from the stream
	RStringAttributeSet bSet;
	CleanupClosePushL(bSet);
	bSet.InternalizeL(readStream);
		
	TBuf <1024> valueA;
	TBuf <1024> valueB;
	if(aSet.Count() != bSet.Count())
		{
		SetTestStepResult(EFail);
		}
	
	for(TInt i = 0; i < aSet.Count(); i++)
		{
		valueA.SetLength(0);
		valueB.SetLength(0);
		if(aSet.GetValue(aSet[i],valueA) != bSet.GetValue(aSet[i],valueB))
			{
			SetTestStepResult(EFail);
			}
			
		if(valueA != valueB)
			{
			SetTestStepResult(EFail);
			}
		}
	
	CleanupStack::PopAndDestroy(&bSet);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(&aSet);
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}



CCafSupplierOutputFileStreamStep::~CCafSupplierOutputFileStreamStep()
	{
	}

CCafSupplierOutputFileStreamStep::CCafSupplierOutputFileStreamStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFSupplierOutputFileStreamStep);
	}


TVerdict CCafSupplierOutputFileStreamStep::doTestStepL()
	{
	SetTestStepResult(EPass);

	_LIT(KDummyName, "C:\\tcaf\\supplieroutputfile.txt");
	_LIT8(KDummyType, "something//caf");
	
	INFO_PRINTF1(_L("CSupplierOutputFile test"));
	

	__UHEAP_MARK;
	
	CSupplierOutputFile *outputFile = CSupplierOutputFile::NewL(KDummyName(), EContent, KDummyType());
	
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	outputFile->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CSupplierOutputFile from the stream
	CSupplierOutputFile *outputFile2= CSupplierOutputFile::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	if(outputFile->FileName() != outputFile2->FileName())
			{
			SetTestStepResult(EFail);
			}
	if(outputFile->OutputType() != outputFile2->OutputType())
			{
			SetTestStepResult(EFail);
			}
	if(outputFile->MimeTypeL() != outputFile2->MimeTypeL())
			{
			SetTestStepResult(EFail);
			}
	
	CleanupStack::PopAndDestroy(buf);
	
	delete outputFile;
	delete outputFile2;
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

CCafMetaDataArrayStep::~CCafMetaDataArrayStep()
	{
	}

CCafMetaDataArrayStep::CCafMetaDataArrayStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFMetaDataArrayStep);
	}


TVerdict CCafMetaDataArrayStep::doTestStepL()
	{
	SetTestStepResult(EPass);

	_LIT(KTest1, "Cat");
	_LIT(KValue1, "Sat");

	_LIT8(KTest2, "Mellow");
	_LIT8(KValue2, "Yellow");

	_LIT8(KTest3, "Tree");
	_LIT8(KValue3, "House");

	
	INFO_PRINTF1(_L("CMetaDataArray test"));
	

	__UHEAP_MARK;
	
	CMetaDataArray *array = CMetaDataArray::NewLC();
	array->AddL(KTest1(), KValue1());
	array->AddL(KTest2(), KValue2());
	array->AddL(KTest3(), KValue3());
	
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	array->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CSupplierOutputFile from the stream
	CMetaDataArray *array2= CMetaDataArray::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	
	if(array->Count() != array2->Count() || (*array)[0].Field() != KTest1() || (*array)[1].Data8() != KValue2())
			{
			SetTestStepResult(EFail);
			}
	for(TInt i = 0; i < array->Count(); i++)
		{
		const CMetaData &a = (*array)[i];
		const CMetaData &b = (*array2)[i];
		if(a.Field() != b.Field())
			{
			SetTestStepResult(EFail);
			}
		if(a.Field8() != b.Field8())
			{
			SetTestStepResult(EFail);
			}
		if(a.Data() != b.Data())
			{
			SetTestStepResult(EFail);
			}
		if(a.Data8() != b.Data8())
			{
			SetTestStepResult(EFail);
			}
		}
	
	delete array2;
		
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(array);
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}


CCafEmbeddedObjectStep::~CCafEmbeddedObjectStep()
	{
	}

CCafEmbeddedObjectStep::CCafEmbeddedObjectStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFEmbeddedObjectStep);
	}


TVerdict CCafEmbeddedObjectStep::doTestStepL()
	{
	SetTestStepResult(EPass);

	_LIT(KTestUniqueId, "The UniqueId");
	_LIT(KTestName, "The Name");

	_LIT8(KTestMimeType, "The mime type");

	
	INFO_PRINTF1(_L("CEmbeddedObject test"));
	

	__UHEAP_MARK;
	
	CEmbeddedObject *aObject = CEmbeddedObject::NewL(KTestUniqueId(), KTestName(), KTestMimeType(), EContentObject);
	
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aObject->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CSupplierOutputFile from the stream
	CEmbeddedObject *bObject= CEmbeddedObject::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	
	if(aObject->UniqueId() != bObject->UniqueId())
			{
			SetTestStepResult(EFail);
			}
			
	if(aObject->Name() != bObject->Name())
			{
			SetTestStepResult(EFail);
			}			
	if(aObject->Type() != bObject->Type())
			{
			SetTestStepResult(EFail);
			}			
	if(aObject->MimeType() != bObject->MimeType())
			{
			SetTestStepResult(EFail);
			}			
	
	CleanupStack::PopAndDestroy(buf);
	delete aObject;
	delete bObject;
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

CCafVirtualPathStep::~CCafVirtualPathStep()
	{
	}

CCafVirtualPathStep::CCafVirtualPathStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFVirtualPathStep);
	}


TVerdict CCafVirtualPathStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	INFO_PRINTF1(_L("CVirtualPath test"));
	_LIT(KTestUniqueId, "The UniqueId");
	_LIT(KTestUri, "A URI");
	_LIT(KSection,"virtualpath");
	_LIT(KDefaultUniqueID,"DEFAULT");
	TPtrC uri;
	TPtrC uniqueID;
	TPtrC combined;
	TPtrC expecteduniqueid;
	TPtrC expecteduri;
	__UHEAP_MARK;
	
	/* test the creation of TVirtualPathPtr and CVirtualPath objects and check if they contain
	 * the expected URI and UniqueID values.
	 */
	TInt i;
	for(i = 0;i<6;i++)
		{
		TBuf<13> sectionName(KSection);
		sectionName.AppendNum(i);
		GetStringFromConfig(sectionName,_L("uri"),uri);
		GetStringFromConfig(sectionName,_L("uniqueID"),uniqueID);
		GetStringFromConfig(sectionName,_L("combined"),combined);
		GetStringFromConfig(sectionName,_L("expecteduri"),expecteduri);
		GetStringFromConfig(sectionName,_L("expecteduniqueid"),expecteduniqueid);
		// Test the TVirtualPathPtr API
		TVirtualPathPtr vPath(uri,uniqueID);
		if(vPath.URI()!=expecteduri)
			{
			INFO_PRINTF4(_L("TVirtualPathPtr(uri,uniqueID) failed Uri for %S\nExpected %S\nGot %S"),&combined,&expecteduri,&vPath.URI());
			SetTestStepResult(EFail);
			}
		if(vPath.UniqueId()!=expecteduniqueid)
			{
			INFO_PRINTF4(_L("TVirtualPathPtr(uri,uniqueID) failed uniqueID for %S\nExpected %S\nGot %S"),&combined,&expecteduniqueid,&vPath.UniqueId());
			SetTestStepResult(EFail);
			}
		vPath=TVirtualPathPtr(combined);
		if(vPath.URI()!=expecteduri)
			{
			INFO_PRINTF4(_L("TVirtualPathPtr(uri,uniqueID) failed Uri() for %S\nExpected %S\nGot %S"),&combined,&expecteduri,&vPath.URI());
			SetTestStepResult(EFail);
			}
		if(vPath.UniqueId()!=expecteduniqueid)
			{
			INFO_PRINTF4(_L("TVirtualPathPtr(aCombinedUriUniqueId) failed UniqueID() for %S\nExpected %S\nGot %S"),&combined,&expecteduniqueid,&vPath.UniqueId());
			SetTestStepResult(EFail);
			}
		vPath=combined;
		if(vPath.URI()!=expecteduri)
			{
			INFO_PRINTF4(_L("TVirtualPathPtr operator = failed Uri for %S\nExpected %S\nGot %S"),&combined,&expecteduri,&vPath.URI());
			SetTestStepResult(EFail);
			}
		if(vPath.UniqueId()!=expecteduniqueid)
			{
			INFO_PRINTF4(_L("TVirtualPathPtr operator = UniqueId for %S\nExpected %S\nGot %S"),&combined,&expecteduniqueid,&vPath.UniqueId());
			SetTestStepResult(EFail);
			}
		// Test the CVirtualPath API
		CVirtualPath* heapPath = CVirtualPath::NewL(uri,uniqueID);
		if(heapPath->URI()!=expecteduri)
			{
			INFO_PRINTF4(_L("TCVirtualPath::NewL(uri,uniqueID) failed Uri for %S\nExpected %S\nGot %S"),&combined,&expecteduri,&heapPath->URI());
			SetTestStepResult(EFail);
			}
		if(heapPath->UniqueId()!=expecteduniqueid)
			{
			INFO_PRINTF4(_L("CVirtualPath::NewL(uri,uniqueID) failed UniqueID for %S\nExpected %S\nGot %S"),&combined,&expecteduniqueid,&heapPath->UniqueId());;
			SetTestStepResult(EFail);
			}
		if(heapPath->GetCombinedUriUniqueId()!=combined)
			{
			INFO_PRINTF4(_L("CVirtualPath::NewL(uri,uniqueID) failed GetCombinedUriUniqueId() for %S\nExpected %S\nGot %S"),&combined,&combined,&heapPath->GetCombinedUriUniqueId());
			SetTestStepResult(EFail);
			}
		delete heapPath;
		heapPath=NULL;
		
		heapPath = CVirtualPath::NewL(combined);
		if(heapPath->URI()!=expecteduri)
			{
			INFO_PRINTF4(_L("CVirtualPath::NewL(combined) failed Uri for %S\nExpected %S\nGot %S"),&combined,&expecteduri,&heapPath->URI());
			SetTestStepResult(EFail);
			}
		if(heapPath->UniqueId()!=expecteduniqueid)
			{
			INFO_PRINTF4(_L("CVirtualPath::NewL(combined) failed UniqueID for %S\nExpected %S\nGot %S"),&combined,&expecteduniqueid,&heapPath->UniqueId());
			SetTestStepResult(EFail);
			}
		if(heapPath->GetCombinedUriUniqueId()!=combined)
			{
			INFO_PRINTF4(_L("CVirtualPath::NewL(combined) failed GetCombinedUriUniqueId() for %S\nExpected %S\nGot %S"),&combined,&combined,&heapPath->GetCombinedUriUniqueId());
			SetTestStepResult(EFail);
			}
		delete heapPath;
		heapPath=NULL;
		}


	CVirtualPath *aPath= CVirtualPath::NewL(KTestUri(), KTestUniqueId());
	
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aPath->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CSupplierOutputFile from the stream
	CVirtualPath *bPath= CVirtualPath::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	
	if(aPath->UniqueId() != bPath->UniqueId())
			{
			SetTestStepResult(EFail);
			}
			
	if(aPath->URI() != bPath->URI())
			{
			SetTestStepResult(EFail);
			}			

	// Test that if a uniqueID of length greater than ContentAccess::KMaxCafUniqueId
	// is given then it will not treat it as a valid uniqueID
	HBufC* longUID = HBufC::NewLC(aPath->UniqueId().Length() * 100+aPath->URI().Length()+1);
	longUID->Des().Append(aPath->URI());
	longUID->Des().Append(KCafVirtualPathSeparator);
	// create a very long concatenated URI and UniqueID
	for ( i = 0; i < 100; ++i )
		{
		longUID->Des().Append(aPath->UniqueId());
		}
	// create a TVirtualPathPtr with the concatenated URI and UniqueID
	TVirtualPathPtr longPath(longUID->Des());
	// check if the expected URI and UniqueID are stored
	if(longPath.URI()!=longUID->Des())
		{
		SetTestStepResult(EFail);
		}
	if(longPath.UniqueId()!=KDefaultUniqueID)
		{
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(longUID);
	CleanupStack::PopAndDestroy(buf);
	delete aPath;
	delete bPath;
	
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}



CCafRightsInfoStep::~CCafRightsInfoStep()
	{
	}

CCafRightsInfoStep::CCafRightsInfoStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRightsInfoStep);
	}


TVerdict CCafRightsInfoStep::doTestStepL()
	{
	SetTestStepResult(EPass);

	_LIT(KTestUniqueId, "The UniqueId");
	_LIT(KTestDescription, "A Description");

	INFO_PRINTF1(_L("CRightsInfo test"));
	

	__UHEAP_MARK;
	
	CRightsInfo *aRights= CRightsInfo::NewL(KTestDescription(), KTestUniqueId(), ERightsTypeConsumable, ERightsStatusNone);
	
	
	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aRights->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CSupplierOutputFile from the stream
	CRightsInfo *bRights= CRightsInfo::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	
	if(aRights->UniqueId() != bRights->UniqueId())
			{
			SetTestStepResult(EFail);
			}
			
	if(aRights->Description() != bRights->Description())
			{
			SetTestStepResult(EFail);
			}			

	if(aRights->RightsType() != bRights->RightsType())
			{
			SetTestStepResult(EFail);
			}			

	if(aRights->RightsStatus() != bRights->RightsStatus())
			{
			SetTestStepResult(EFail);
			}			
	
	CleanupStack::PopAndDestroy(buf);
	delete aRights;
	delete bRights;
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}



CCafStreamablePtrArrayStep::~CCafStreamablePtrArrayStep()
	{
	}

CCafStreamablePtrArrayStep::CCafStreamablePtrArrayStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFStreamablePtrArrayStep);
	}


TVerdict CCafStreamablePtrArrayStep::doTestStepL()
	{
	SetTestStepResult(EPass);

	_LIT(KTestUniqueId, "The UniqueId");
	_LIT(KTestName, "The Name");
	_LIT8(KTestMimeType, "The mime type");

	INFO_PRINTF1(_L("StreamablePtrArray test"));
	
	__UHEAP_MARK;
	
	RStreamablePtrArray <CEmbeddedObject> array;
	
	CEmbeddedObject *aObject = CEmbeddedObject::NewL(KTestUniqueId(), KTestName(), KTestMimeType(), EContentObject);
	CEmbeddedObject *bObject = CEmbeddedObject::NewL(KTestUniqueId(), KTestName(), KTestMimeType(), EContentObject);
	CEmbeddedObject *cObject = CEmbeddedObject::NewL(KTestUniqueId(), KTestName(), KTestMimeType(), EContentObject);
	
	array.AppendL(aObject);
	array.AppendL(bObject);
	array.AppendL(cObject);
	

	// Create a buffer
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	array.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// create read stream
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);

	// construct a new CSupplierOutputFile from the stream
	RStreamablePtrArray <CEmbeddedObject> bArray;
	bArray.InternalizeL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	if(array.Count() != bArray.Count())
			{
			SetTestStepResult(EFail);
			}
			
	for(TInt i=0; i < array.Count(); i++)
		{
		if(array[i]->UniqueId() != bArray[i]->UniqueId())
			{
			SetTestStepResult(EFail);
			}
			
		if(array[i]->Name() != bArray[i]->Name())
			{
			SetTestStepResult(EFail);
			}			
		}
	
	CleanupStack::PopAndDestroy(buf);
	array.Close();
	bArray.Close();
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

CCafHelperStep::~CCafHelperStep()
    {     
    }     
  
CCafHelperStep::CCafHelperStep()
    {     
    SetTestStepName(KCAFHelperStep);     
    }     
      
      
TVerdict CCafHelperStep::doTestStepL()     
    {     
    SetTestStepResult(EFail);     
__UHEAP_MARK;     
         
    TInt result;     
         
    TInt error;     
    GetIntFromConfig(ConfigSection(),_L("error"),error);     
         
    TInt expectedValue;     
    GetIntFromConfig(ConfigSection(),_L("expected"),expectedValue);     
         
    // fileheader case.     
    TPtrC uri;     
    if (!GetStringFromConfig(ConfigSection(), _L("uri"), uri))     
        {     
        INFO_PRINTF1(_L("CCafHelper test : HeaderData case."));     
        result = TestHeaderDataCaseL(error);     
        }     
             
    else     
        {     
        TBool isFileName = EFalse;     
        GetBoolFromConfig(ConfigSection(),_L("flag"),isFileName);     
             
        // filehandle case     
        if(isFileName == EFalse)     
            {     
            INFO_PRINTF1(_L("CCafHelper test : FileHandle case."));     
            result = TestFileHandleCaseL(error, uri);     
            }     
             
        // filename case.     
        else     
            {     
            INFO_PRINTF1(_L("CCafHelper test : FileName case."));     
         
            CCAFHelper* helperObj = CCAFHelper::NewL();     
            CleanupStack::PushL(helperObj);     
             
            result = (*helperObj)().HandleCAFErrorL(error, uri);     
            CleanupStack::PopAndDestroy(helperObj);     
            }        
        }     
         
    if(result == expectedValue)     
        {     
        SetTestStepResult(EPass);     
        }     
             
__UHEAP_MARKEND;     
             
    return TestStepResult();     
    }     
         
TInt CCafHelperStep::TestFileHandleCaseL(TInt aError, const TDesC& aFileName)     
    {     
__UHEAP_MARK;     
    RFs fs;     
    RFile fileHandle;     
    User::LeaveIfError(fs.Connect());     
    CleanupClosePushL(fs);     
    User::LeaveIfError(fileHandle.Open(fs, aFileName, EFileRead));     
    CleanupClosePushL(fileHandle);     
         
    CCAFHelper* helperObj = CCAFHelper::NewL();     
    CleanupStack::PushL(helperObj);     
    TInt result = (*helperObj)().HandleCAFErrorL(aError, fileHandle);     
    CleanupStack::PopAndDestroy(3, &fs);     
__UHEAP_MARKEND;     
      
    return result;     
    }     
         
TInt CCafHelperStep::TestHeaderDataCaseL(TInt aError)     
    {     
    _LIT8(KFileHeaderData,"W\0R\0M\0H\0E\0A\0D\0E\0R\0");     
      
__UHEAP_MARK;        
    CCAFHelper* helperObj = CCAFHelper::NewL();     
    CleanupStack::PushL(helperObj);     
         
    TInt result = (*helperObj)().HandleCAFErrorL(aError, KFileHeaderData);     
    CleanupStack::PopAndDestroy(helperObj);     
__UHEAP_MARKEND;     
      
    return result;     
    }     
         
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 


