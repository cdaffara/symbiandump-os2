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
* This file is used for the verification function of the pkcs12 file
*
*/


/**
 @file 
 @internalTechnology
*/
 
// System Include
#include <test/testexecutelog.h>

//User Include 
#include "tpkcs12libteststep.h"
#include "tpkcs12data.h"
#include "tpkcs12common.h"

using namespace PKCS12;

const TInt KBagDataLength = 7;
const TInt KContentInfoLength = 11;
/**
Description:constructor:used for initialization of the variables declared      
@test
@internalTechnology
*/
CPKCS12LibTestStep::CPKCS12LibTestStep()
	{
	iActualResult=ETrue;
	iUnitTest=EFalse;
	iUnitTestContinueFlag = ETrue;
	iExpectedError = ETrue;
	iExpectedResult=KPositiveTest;
	}
/**
Description:Destructor
@test
@internalTechnology
*/
CPKCS12LibTestStep::~CPKCS12LibTestStep()
	{
	iFileSession.Close();
	delete iParser;	
	iPrivacyPassword.ResetAndDestroy();	
	}

/**
Function is intended to parse the pkcs12 file, compare the results and 
log the same
@internalTechnology:
@return:TVerdict -returns EFail if there is a failure else EPass
@test
@leave:- if memory allocation fails
*/
TVerdict CPKCS12LibTestStep::doTestStepL()
	{
	//__UHEAP_MARK;
	TPtrC integrityMode(KNullDesC);
	TPtrC integrityPassword(KNullDesC);
	TPtrC privacyPassword(KNullDesC);
	TPtrC dataPath(KNullDesC);
	TBool flagReturn=ETrue;
	
	
	if(iFileSession.Connect())
		{
		INFO_PRINTF1(_L("File server session failed"));
		flagReturn=EFalse;
		}
	
	if(flagReturn)	
		{
		if((!GetStringFromConfig(KMainSec,KFilePath,dataPath)) ||
		   !GetStringFromConfig(KMainSec,KIntegrityMode,integrityMode))
			{
			SetTestStepResult(EAbort);
			ERR_PRINTF1(_L("Input Data not found in ini file"));
			flagReturn=EFalse;
			}	
		}
	//Check the input
	if(flagReturn)
		{
		if(!GetBoolFromConfig(KMainSec,KUnitTest,iUnitTest))
			{
			iUnitTest=EFalse;
			}
		//Password Integrity Mode	
		if(integrityMode.CompareF(KPassword)==0)
			{
			if(!GetStringFromConfig(KMainSec,KIntegrityPassword,integrityPassword))
				{
				ERR_PRINTF1(_L("Problem in the Main section of the ini file"));
				SetTestStepResult(EFail);
				flagReturn=EFalse;
				}
			else
				{
				
				iParser	= CPkcs12Parser::NewL();
				// set the integrity password 
				iParser->SetIntegrityPasswordL(integrityPassword);
				// set the datapath
				iParser->SetDataL(dataPath);
				//set the privacy password
				ParsePrivacyPwdL();
				//parse the pkcs12 file
				iParser->ParseL();
			
				if(!GetBoolFromConfig(KMainSec,KIntegrityValid,iExpectedResult))
					{
					WARN_PRINTF1(_L("Integrity valid not found in ini file"));
					iExpectedResult=KPositiveTest;
					}
				if(!VerifyMainSecL())
					{
					ERR_PRINTF1(_L("Main section failed"));
					flagReturn=EFalse;
					}
				else if(!iUnitTestContinueFlag)
					{
					SetTestStepResult(EPass);
					delete iParser;
					iParser = NULL;
					flagReturn = EFalse;
					}
				}
			}
		else if(integrityMode.CompareF(KPublicIntegrity)==0)
			{
			//public Integrity Mode	
			}
		else
			{
			ERR_PRINTF1(_L("Integrity pwd not known in ini file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}
		}
	if(flagReturn)
		{
		//Check the Authenticated safe section	
		if(!VerifyAuthSafe())
			{
			ERR_PRINTF1(_L("Authenticated safe section failed"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}
		else if(!iUnitTestContinueFlag )
			{
			SetTestStepResult(EPass);
			delete iParser;
			iParser = NULL;
			flagReturn = EFalse;
			}
		}
	if(flagReturn)	
		{
		//calculate the totalsafebags in the pkcs12 file	
		TInt totalsafebags = iParser->PfxHeader().TotalSafeBagCount();
						 
		//calculate the totalbags(including the safecontentsbag) in the pkcs12 file						
		TInt totalbags = totalsafebags + iParser->PfxHeader().SafecontentBagCount();
	
		//Check the BagData section	
		if(!ChkBagDataL(totalsafebags,totalbags))
			{
			ERR_PRINTF1(_L("Bag Data section failed"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}
		else if(!iUnitTestContinueFlag)
			{
			delete iParser;
			iParser = NULL;
			flagReturn = EFalse;
			}
		}
	if(flagReturn)
		{
		SetTestStepResult(EPass);		
		}
	
	//__UHEAP_MARKEND; 
	return TestStepResult();
	}
/**
Function is intended to verify the expected error in each section of the ini file
@internalTechnology:
@return:TBool:returns EFalse if there is a failure else ETrue
@test:
*/
TBool CPKCS12LibTestStep::VerifyExpectedError(const TDesC& aSecName)
	{
	TBool flagReturn = ETrue;
	if ( iUnitTest ) 
		{
		if(GetIntFromConfig(aSecName,KExpectedError,iExpectedError)==EFalse)
			{
			INFO_PRINTF1(_L("ExpectedError not found in INI File"));
			}	
		// Checks the error returned by the library with the expected error read 
		// from the INI file.
		TInt actualError = 0;
		
		if ( (!aSecName.Compare(KMainSec) || !aSecName.Compare(KAuthSafeSec) ||
		     !aSecName.Compare(_L("MacData")) || (!aSecName.Left(KBagDataLength).Compare(KBagDataSec)) ||
		     (!aSecName.Left(KContentInfoLength).CompareF(KContentInfoKey))) && !iExpectedResult )
			{
			actualError = iParser->PfxHeader().Pkcs12ActualError();
			}	
		if (actualError == iExpectedError)
			{
			INFO_PRINTF1(_L("ExpectedError matchs"));
			iUnitTestContinueFlag = EFalse;
			iExpectedResult = ETrue;
			}
		}
		
	if(iExpectedResult!=iActualResult)
		{
		SetTestStepResult(EFail);
		flagReturn= EFalse;
		}
	iActualResult = ETrue;
	return flagReturn;	
	}
/**
Function is intended to set the privacy passwords of the contentinfotype
@internalTechnology:
@test:
@leave: if leave occurs if memory allocation fails
*/		
void CPKCS12LibTestStep::ParsePrivacyPwdL()
	{
	TPtrC pass(KNullDesC);
	//to append the number to get the actual key value
	TInt count=0;
	//return value
	TBool privacyPwdPresent=EFalse;
	
	//Put all the content info privacy passwords in an array 
	do
		{
		TBuf<KPrivacyPwdStrLen> privacyPwdBuf(KPrivacyPassword);
		pass.Set(KNullDesC);
		count++;
		//appending the count to the Keyvalue
		privacyPwdBuf.AppendNum(count);
		privacyPwdPresent=GetStringFromConfig(KMainSec,privacyPwdBuf,pass);
		if(privacyPwdPresent)
			{
			HBufC *temp=pass.AllocLC();
			iPrivacyPassword.AppendL(temp);
			CleanupStack::Pop(temp);
			}
		}while(privacyPwdPresent);
	iParser->SetPrivacyPassword(iPrivacyPassword);	
	}
/**
Function is intended to parse the main section of the ini file
@internalTechnology:
@return:returns EFalse if there is a failure else ETrue
@test:
*/			
TBool CPKCS12LibTestStep::VerifyMainSecL()
	{
	TBool mac=EFalse;
	TBool flagReturn=ETrue;
	
	//retrieve the expected result:PositiveTest/NegativeTest	
	if(!GetBoolFromConfig(KMainSec,KExpectedResult,iExpectedResult))
		{
		WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
		iExpectedResult=KPositiveTest;
		}
	if(!GetBoolFromConfig(KMainSec,KMacPresent,mac))
		{
		WARN_PRINTF1(_L("Mac present not found in ini file"));
		//default only integration test
		mac=EFalse;
		}
	else if(mac && iUnitTest)
		{
		//call the mac data function and check for all the macdata section values
		iActualResult=ChkMacValueL();
		if (!iUnitTestContinueFlag)	
			{
			return flagReturn;
			}
		}
	if(!VerifyExpectedError(KMainSec))
		{
		SetTestStepResult(EFail);
		flagReturn = EFalse;
		}
	return flagReturn;	
	}
/**
Function is intended to parse the AuthSafe section of the ini file
@internalTechnology:
@return:returns EFalse if there is a failure else ETrue
@test:
*/			
TBool CPKCS12LibTestStep::VerifyAuthSafe()
	{
	TInt contentInfoCount=0;
	TPtrC contentSection(KNullDesC);
	TInt contentIndex=0;
	TBool flagReturn=ETrue;
	
	if(!GetIntFromConfig(KAuthSafeSec,KContentInfoNum,contentInfoCount))
		{
		ERR_PRINTF1(_L("Count not available in the inifile"));
		SetTestStepResult(EFail);
		flagReturn=EFalse;
		}
	//retrieve the expected result:PositiveTest/NegativeTest	
	if(GetIntFromConfig(KAuthSafeSec,KExpectedResult,iExpectedResult)==EFalse)
		{
		INFO_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
		}
	//verify positive or negative test
	if(!VerifyExpectedError(KAuthSafeSec()))
		{
		INFO_PRINTF1(_L("Content Info Failure"));
		SetTestStepResult(EFail);
		flagReturn = EFalse;
		}
	else if(!iUnitTestContinueFlag)
		{
		return flagReturn;
		}
	for(TInt i=1;i<=contentInfoCount;i++)
		{
		TBuf<KContentInfoBufLen> contentBuf(KContentInfoKey);
		//Append the counter to the keyname		
		contentBuf.AppendNum(i);
		if(!GetStringFromConfig(KAuthSafeSec,contentBuf,contentSection))
			{
			ERR_PRINTF1(_L("ContentSection not available in the inifile"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			break;
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(GetIntFromConfig(contentSection,KExpectedResult,iExpectedResult)==EFalse)
			{
			INFO_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}
		// Verify before calling array to access the data.
		if(!VerifyExpectedError(contentSection))
			{
			INFO_PRINTF1(_L("Content Info Failure"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			break;
			}
		else if(!iUnitTestContinueFlag)
			{
			flagReturn = ETrue;
			break;
			}
		//Call the function for checking the contentinfo,pass the contntsction as parameter
		iBagData=iParser->ArraySafeBagData()[contentIndex];
		contentIndex++;
		//Check the contentinfo type
		iActualResult=ChkCIType(contentSection,*iBagData);
		}
	return flagReturn;
	}
/**
Function is intended to parse the BagData section of the ini file
@param-aTotalsafebags:total number of safebags
@param-aTotalbags:total number of bags including the safecontentbags
@internalTechnology:
@return:returns EFalse if  there is a failure else return ETrue
@test:
@leave: if leave occurs at the ChkBagValL function
*/			
TBool CPKCS12LibTestStep::ChkBagDataL(TInt aTotalsafebags,TInt aTotalbags)	
	{
	TInt bagType=0;
	TInt safeContentIndex=0;
	TInt safeBagIndex=0;
	CSafeContentBag* safeContentBag;
	TBool flagReturn=ETrue;
	
	TInt totalSafeContentsBag = iParser->PfxHeader().SafecontentBagCount();
	

	//loop for the totalbags
	for(TInt bags=0;bags<aTotalbags;bags++)
		{
		TBuf<KBagDataSecBufLen> bagSection(KBagDataSec);
		//Append the bags to the sectionname :BagData1
		bagSection.AppendNum(bags+1);
		if(!GetIntFromConfig(bagSection,KBagType,bagType))
			{
			ERR_PRINTF1(_L("Not able to get the BagType from the ini file"));
			flagReturn=EFalse;
			break;
			}
		if(bagType == CDecPkcs12SafeBag::ESafeContentsBag && flagReturn)
			{
			//loop for the safecontents bags
			while(safeContentIndex < totalSafeContentsBag && flagReturn)
				{
				safeContentBag=iParser->ArraySafeContentBag()[safeContentIndex];
				if(!ChkSafeContsBagData(bagSection,*safeContentBag))
					{
					ERR_PRINTF1(_L("SafeContent Bag Failed"));
					flagReturn=EFalse;
					}
				safeContentIndex++;
				break;
				} 
			}
		else
			{
			while(safeBagIndex < aTotalsafebags && flagReturn)
				{
				//retrieve the safebag sructure pointer
				iBagData=iParser->ArraySafeBagData()[safeBagIndex];
				//Chk the BagType
				if(!ChkBagType(bagSection,*iBagData,bagType))
					{
					ERR_PRINTF1(_L("Bag Type does not match"));
					flagReturn=EFalse;
					break;
					}
				if(!iUnitTestContinueFlag)
					{
					break;
					}
				iBagAttrNumVal=0;	
				if(!ChkBagAttbutesL(bagSection,*iBagData))
					{
					ERR_PRINTF1(_L("Failure in Attributes"));
					flagReturn=EFalse;
					break;
					}	
				switch(bagType)
					{
					case CDecPkcs12SafeBag::ECertBag:
						{
						//check the bagvalue
						if(!ChkBagValL(bagSection,*iBagData))
							{
							flagReturn=EFalse;
							ERR_PRINTF1(_L("Certificate Bag Value comparison Failed"));
							}
						//check the certificate type
						if(flagReturn)	
							{
							if(!ChkCertId(bagSection,*iBagData))
								{
								flagReturn=EFalse;
								ERR_PRINTF1(_L("Certificate Bag certID comparison Failed"));
								}
							}	
						break;	
						}
					case  CDecPkcs12SafeBag::EKeyBag:
						{
						if(!ChkBagValL(bagSection,*iBagData))
							{
							flagReturn = EFalse;
							ERR_PRINTF1(_L("Key Bag Failed"));
							}	
						break;		
						}
					case CDecPkcs12SafeBag::EShroudedKeyBag:
						{
							if(!ChkEncodedBagValL(bagSection,*iBagData))
							{
							flagReturn = EFalse;
							ERR_PRINTF1(_L("Shrouded Bag Failed"));
							}	
						break;	
						}
					case CDecPkcs12SafeBag::ECrlBag:
						{
						break;		
						}
					case CDecPkcs12SafeBag::ESecretBag:
						{
						break;	
						}
					}
					if(flagReturn)
						{
						safeBagIndex++;	
						break;			
						}
				}
			}
		}
		
	return flagReturn;
	}
/**
Function is intended to check the safecontents bag
@param-aSec:BagData Section
@param-aSfContBag:safecontent class pointer
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/		
TBool CPKCS12LibTestStep::ChkSafeContsBagData(TDesC& aSec,CSafeContentBag &aSfContBag)
	{
	TInt safeContentsCount=0;
	TBool flagReturn=ETrue;
	//retrieve the safecontentsbag count from ini file
	if(!GetIntFromConfig(aSec,KSafeBagsCount,safeContentsCount)) 		
		{
		ERR_PRINTF1(_L("SafeContent Bags count not found in INI file"));
		SetTestStepResult(EFail);
		flagReturn=EFalse;
		}
	//compare the data with the ini file data	
	if(safeContentsCount != aSfContBag.SafeBagCount() && flagReturn)
		{
		ERR_PRINTF1(_L("Number of Bags in the safecontent bag is different"));
		iActualResult=EFalse;
		}
	//retrieve the expected result:PositiveTest/NegativeTest	
	if(!GetBoolFromConfig(aSec,KExpectedResult,iExpectedResult))
		{
		INFO_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
		iExpectedResult=KPositiveTest;
		}	
	//check whether its a positive or a negative test	
	if(flagReturn)
		{
		//check whether its a positive or a negative test	
		if(!VerifyExpectedError(aSec))
			{
			INFO_PRINTF1(_L("SafeContentsBag Failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}	
		}
	return flagReturn;
	}
/**
Function is intended to check the content info type
@param-aSecName:BagData Section
@param-aBagData:safebag class pointer
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/		
TBool CPKCS12LibTestStep::ChkCIType(TDesC &aSecName,CSafeBagData &aBagData)
	{
	TInt contentType=0;
	TBool flagReturn=ETrue;
	//retrieve the contentype from the ini file
	if(!GetIntFromConfig(aSecName,KContentInfoType,contentType))
		{
		ERR_PRINTF1(_L("ContentInfoType value not found in INI file"));
		SetTestStepResult(EFail);
		flagReturn=EFalse;
		}
	//compare the contentype with the ini file data
	if(flagReturn)
		{
		if(aBagData.ContentInfoType() != contentType)
			{
			iActualResult=EFalse;
			INFO_PRINTF1(_L("ContentType Comparison Failed"));
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSecName,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}
		//check whether its a positive or a negative test	
		if(!VerifyExpectedError(aSecName))
			{
			INFO_PRINTF1(_L("ContentInfo Failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}				
		}
	return flagReturn;
	}
/**
Function is intended to check the bag type
@param-aSec:BagData Section
@param-aBgDta:safebag class pointer
@param-aId:Bag ID
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/	
TBool CPKCS12LibTestStep::ChkBagType(TDesC &aSec,CSafeBagData &aBagData,TInt aId)
	{
	TBool flagReturn=ETrue;
	
	//check for the bagid	
	if(aId != aBagData.BagType())
		{
		iActualResult=EFalse;
	   	INFO_PRINTF1(_L("BagType Comparision Failed"));
		}
		
	//retrieve the expected result:PositiveTest/NegativeTest	
	if(!GetBoolFromConfig(aSec,KExpectedResult,iExpectedResult))
		{
		WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
		iExpectedResult=KPositiveTest;
		}	
	
	//check whether its a positive or a negative test		
	if(!VerifyExpectedError(aSec))
		{
		INFO_PRINTF1(_L("BagType Failed"));
		SetTestStepResult(EFail);
		flagReturn = EFalse;
		}		
	return flagReturn;
	}
/**
Function is intended to check the bag attributes
@param-aSec:BagData Section
@param-aBagData:safebag class pointer
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/
TBool CPKCS12LibTestStep::ChkBagAttbutesL(TDesC &aSec,CSafeBagData &aBagData)
	{
	TInt numAttr=0;
	TInt count=0;
	TPtrC attrSec(KNullDesC);
	TBool flagReturn=ETrue;
	
	//retrieve the number of attributes
	if(!GetIntFromConfig(aSec,KBagAttributesNum,numAttr))
		{
		WARN_PRINTF1(_L("Number of attributes not found in INI file"));
		}
		
	for(TInt attrindex=0;attrindex < numAttr;attrindex++)	
		{
		TBuf<KAttributeBufLen> attrBuf(KBagAttributesKey);
		//increment the attr counter
		count++;
		//appending to get the attr key from the ini file
		attrBuf.AppendNum(count);
		
		if(!GetStringFromConfig(aSec,attrBuf,attrSec))
			{
			ERR_PRINTF1(_L("Number of attributes not found in INI file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			break;
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSec,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}
		//check whether its a positive or a negative test		
		if(!VerifyExpectedError(aSec))
			{
			INFO_PRINTF1(_L("BagAttributes Failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}		
		else if(!iUnitTestContinueFlag)
			{
			flagReturn = ETrue;
			break;
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSec,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}
		//check whether its a positive or a negative test		
		if(!VerifyExpectedError(aSec))
			{
			INFO_PRINTF1(_L("BagAttributes Failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}		
		else if(!iUnitTestContinueFlag)
			{
			flagReturn = ETrue;
			break;
			}
		//Check the BagAttribute section
		if(!ChkAtrSecL(attrSec,aBagData,attrindex))
			{
			ERR_PRINTF1(_L("Attributes comparison failed"));
			flagReturn=EFalse;
			break;
			}
		}
	return flagReturn;
	}
/**
Function is intended to check the attribute section
@param-aSec:BagData Section
@param-aBgDta:safebag class pointer
@param-aIndx:attribute index
@param-aAttrindex:attribute index
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
@leave: if leave occurs if ChkAtrValL fails
*/
TBool CPKCS12LibTestStep::ChkAtrSecL(TDesC &aSec,CSafeBagData &aBagData,TInt aIndex)
	{
	TPtrC attrId(KNullDesC);
	TInt numVal=0;
	CSafeBagAttribute *bagAttr;
	TPtrC friendlyName(KFriendlyNameOid);
	TPtrC localKey(KLocalKeyOid);
	TBool flagReturn=ETrue;
	
	//retrieve the attribute id
	if(!GetStringFromConfig(aSec,KBagAttributesID,attrId))
		{
		ERR_PRINTF1(_L("attribute id not found in INI file"));
		SetTestStepResult(EFail);
		flagReturn=EFalse;
		}
	if(flagReturn)
		{
		//retrieve the number of values	
		if(!GetIntFromConfig(aSec,KBagAttributeValuesNum,numVal))
			{
			ERR_PRINTF1(_L("total number of attribute values not found in INI file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}	
		}
	if(flagReturn)
		{
		bagAttr = aBagData.iAttributeIDs[aIndex];
		
		//increment the Number of BagAttrValues
		iBagAttrNumVal+=numVal;
		 
		 //compare the attrval count and the attrid
		if((bagAttr->AttributeValueCount()!=numVal) || 
		 (attrId.CompareF((bagAttr->AttributeId()))!=0))
		 	{
		 	iActualResult=EFalse;
			}
		else if((friendlyName.CompareF((bagAttr->AttributeId()))==0) ||
				(localKey.CompareF((bagAttr->AttributeId()))==0))
			{
			//Check for BagAttribute values 
			if(!ChkAtrValL(aSec,aBagData,numVal))
				{
				iActualResult=EFalse;
				}
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSec,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file"));
			iExpectedResult=KPositiveTest;
			}	
		//check for positive or negative test	
		if(!VerifyExpectedError(aSec))
			{
			INFO_PRINTF1(_L("Attributes Comparison Test Case failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}	
	
		}
	//retrieve the bag attribute structure pointer
	return flagReturn;
	}
/**
Function is intended to check the attribute values
@param-aSec:BagData Section
@param-aBgDta:safebag class pointer
@param-aNum:Bagattribute value count
@param-aAtr:Bagattribute ID
@param-aNumVal:Bagattribute value count
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
@leave: if memory allocation of buffer fails
*/
TBool CPKCS12LibTestStep::ChkAtrValL(TDesC &aSec,CSafeBagData &aBagData,TInt aNum)
	{
	TInt start=iBagAttrNumVal - aNum;
	TInt end=iBagAttrNumVal + aNum;
	
	TBool flagReturn=ETrue;
	TInt index=0;
	TPtrC value(KNullDesC);
	
	HBufC8 *bufferVal;
	
	//loop till the total number of values
	do	
		{
		TBuf<KBagValBufLen> bagVal(KBagAttributeValue);
		//increment the index to append the count
		index++;
		//append the value
		bagVal.AppendNum(index);
		//retrieve the value from the ini file
		if(!GetStringFromConfig(aSec,bagVal,value))
			{
			ERR_PRINTF1(_L("Bag Value not found in the ini file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			break;
			}
		for(TInt startIndex=start;startIndex<end;startIndex++)
			{
			bufferVal=aBagData.iAttributeValues[startIndex];
			
			//check the local key value
			if(!CompareFileDataL(value,*bufferVal))
				{
				ERR_PRINTF1(_L("Bag Attribute Value comparison failed"));
				iActualResult=EFalse;
				break;	
				}
			else
				{
				iActualResult=ETrue;
				break;
				}
			}
			//retrieve the expected result:PositiveTest/NegativeTest	
			if(!GetBoolFromConfig(aSec,KExpectedResult,iExpectedResult))
				{
				WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
				iExpectedResult=KPositiveTest;
				}	
			//check for positive or negative test
			if(!VerifyExpectedError(aSec))
				{
				INFO_PRINTF1(_L("Bag Value Test failed"));
				SetTestStepResult(EFail);
				flagReturn = EFalse;
				break;
				}
			else if(!iUnitTestContinueFlag)
				{
				break;
				}		
		}while(index<aNum);
	return flagReturn;
	}
/**
Function is intended to check the value.
@param-aPValue:filepath containing the path of the file 
			   retrieved from the ini file
@param-aBufferVal:Value retrieved after parsing the pkcs12 file
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
@leave: if leave occurs if openfile fails or allocation to buffer fails
*/	
TBool CPKCS12LibTestStep::CompareFileDataL(TDesC &aPValue,TDesC8 &aBufferVal)
	{
	RFile file;
	TInt fileSize=0;
	TBool flagReturn=ETrue;
	// Negative Test: If mac/ macSalt is not present.
	if (&aBufferVal)
		{
		//open the file
		User::LeaveIfError(file.Open(iFileSession,aPValue,EFileRead));
		User::LeaveIfError(file.Size(fileSize));
		//raw data for reading the bagvalue
		HBufC8 *rawData=HBufC8::NewMaxLC(fileSize);
		//converting the HBufC8 to TPtr8
		TPtr8 data(rawData->Des());
		//read the contents of the file
		User::LeaveIfError(file.Read(data));
		//compare the value
		if(rawData->CompareF(aBufferVal)!=0)
			{
			ERR_PRINTF1(_L("comparison failed"));
			flagReturn=EFalse;
			}
		//closes the data file
		file.Close();

		CleanupStack::PopAndDestroy(rawData); 	
		}
	return flagReturn;
	}
/**
Function is intended to check the algorithm id
@param-aSecName:BagData Section
@param-aBagData:safebagdata class pointer
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/
TBool CPKCS12LibTestStep::ChkAlgId(TDesC &aSecName,CSafeBagData &aBagData)
	{
	TPtrC algorithm(KNullDesC);
	TBool flagReturn=ETrue;
	//retrieve the algorithm section name
	if(!GetStringFromConfig(aSecName,_L("Algorithm"),algorithm))
		{
		ERR_PRINTF1(_L("AlgorithmID value not found in INI file"));
		SetTestStepResult(EFail);
		flagReturn=EFalse;
		}
	if(flagReturn)	
		{
		if(algorithm.CompareF((aBagData.AlgorithmId()))!=0)
			{
			iActualResult=EFalse;
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSecName,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}	
		//check for positive oor negative test	
		if(!VerifyExpectedError(aSecName))
			{
			INFO_PRINTF1(_L("Algorithm Comparison Test Case failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}			
		}
	return flagReturn;	
	}
/**
Function is intended to check the certificate type
@param-aSecName:BagData Section
@param-aBagData:safebagdata class pointer
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/
TBool CPKCS12LibTestStep::ChkCertId(TDesC &aSecName,CSafeBagData &aBagData)
	{
	TPtrC certid(KNullDesC);
	TBool flagReturn=ETrue;
	//retrieve the cert name from ini file
	if(!GetStringFromConfig(aSecName,KCertType,certid))
		{
		ERR_PRINTF1(_L("certificateID value not found in INI file"));
		SetTestStepResult(EFail);
		flagReturn= EFalse;
		}
	if(flagReturn)
		{
		if(certid.CompareF((aBagData.CertificateId()))!=0)
			{
			INFO_PRINTF1(_L("certificateID comparision Failed"));
			iActualResult=EFalse;
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSecName,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}	
		//check for positive or negative test	
		if(!VerifyExpectedError(aSecName))
			{
			INFO_PRINTF1(_L("Certificate Comparison Test Case failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}				
		}
	return flagReturn;
	}
/**
Function is intended to check for the bag value
@param-aSecName:BagData Section
@param-aBagData:safebag class pointer
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/
TBool CPKCS12LibTestStep::ChkBagValL(TDesC &aSecName,CSafeBagData &aBagData)
	{
	TPtrC certBagPath(KNullDesC);
	TBool flagReturn=ETrue;
	
	//retrieve the certificate bag value
	if(!GetStringFromConfig(aSecName,KBagValue,certBagPath))
		{
		ERR_PRINTF1(_L("Not able to get the CertBagValue from the ini file"));
		SetTestStepResult(EFail);
	  	flagReturn=EFalse;
	  	}
	TInt fileReturn=0; 	
	RFile file;
	
	if(flagReturn)
		{
		fileReturn=file.Open(iFileSession,certBagPath,EFileRead);
		
		if(fileReturn)
			{
			ERR_PRINTF1(_L("CertBagValue:Not able to open the file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}	
		}
	//raw data for reading the bagvalue
	HBufC8 *rawData=NULL;
	TInt filesize=0;	
	if(flagReturn)
		{
		User::LeaveIfError(file.Size(filesize));
		rawData=HBufC8::NewMaxLC(filesize);
		TPtr8 data(rawData->Des());
		if(file.Read(data))
			{
			ERR_PRINTF1(_L("CertBagValue:Not able to Read the file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}
		}
	if(flagReturn)
		{
	  	//compare the data
		if(rawData->CompareF(aBagData.BagValue())!=0)
			{
			iActualResult=EFalse;
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSecName,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}			
		//check for positive or negative test	
		if(!VerifyExpectedError(aSecName))
			{
			INFO_PRINTF1(_L("BagValue Failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}			
		}
	if(rawData!=NULL)	
		{
		CleanupStack::PopAndDestroy(rawData); 	
		}
	if(!fileReturn)
		{
		file.Close();	
		}
	return flagReturn;
	}
/**
Function is intended to check for the mac value
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
@leave: if leave occurs at the CompareFileDataL function
*/	
TBool CPKCS12LibTestStep::ChkMacValueL()	
	{
	TPtrC mac(KNullDesC);
	TPtrC macSalt(KNullDesC);
	TPtrC secName(_S("MacData"));
	TPtrC expectedError(KNullDesC);
	TInt itCount=0;
	TBool flagReturn=ETrue;
	
	//retrieve the macid,macsalt and iteration count 
	if((!GetStringFromConfig(secName,KMac,mac)) || 
	   (!GetStringFromConfig(secName,KMacSalt,macSalt)) ||
	   (!GetIntFromConfig(secName,KIterationCount,itCount)))
		{
		ERR_PRINTF1(_L("Not able to access the mac values from ini file"));
		SetTestStepResult(EFail);
		flagReturn=EFalse;
		}
	//comapre the data with the ini file
	if((CompareFileDataL(mac,iParser->PfxHeader().MacId()) ==0)||
	   (CompareFileDataL(macSalt,iParser->PfxHeader().MacSalt()) == 0)||
	   (itCount!=iParser->PfxHeader().IterationCount()))
		{
		WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
		flagReturn=EFalse;
		}
	//retrieve the expected result:PositiveTest/NegativeTest	
	if(!GetBoolFromConfig(secName,KExpectedResult,iExpectedResult))
		{
		WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
		iExpectedResult=KPositiveTest;
		}	
	if(!VerifyExpectedError(secName))
		{
		SetTestStepResult(EFail);
		flagReturn = EFalse;
		}	
	return flagReturn;
	}

CTPKCS12OOMStep::CTPKCS12OOMStep()
	{
	
	}
CTPKCS12OOMStep::~CTPKCS12OOMStep()
	{
	delete iRawData;
	delete iAuthSafeData;
	iFs.Close();
	}
/**
Function is intended to do the OOM test.
@internalTechnology:
@return:TVerdict -returns EFail if there is a failure else EPass
@leave:- if memory allocation fails
@test
*/	
TVerdict CTPKCS12OOMStep::doTestStepL()
	{
	TInt OOMTest;
	GetIntFromConfig(ConfigSection(),_L("OOMTest"),OOMTest);
		
	__UHEAP_MARK;
			
	switch(OOMTest)
		{
		case 1:
			{
			CDecPkcs12* theTest = NULL;
			theTest = CDecPkcs12::NewL(iRawData->Des());
			delete theTest;
			}
			break;
		// File Stream
		case 2:	
			{
			TPtrC fileName;
			GetStringFromConfig(ConfigSection(), _L("File"), fileName);
			
			RFile file;
			User::LeaveIfError(file.Open(iFs, fileName, EFileRead) );
				
			TInt size;
			file.Size(size);
			file.Close();

			HBufC8* res = HBufC8::NewLC(size);
			TPtr8 p(res->Des());
			p.SetLength(size);

			RFileReadStream stream;
			User::LeaveIfError(stream.Open(iFs, fileName, EFileStream));
			CleanupClosePushL(stream);
				
			stream.ReadL(p, size);
		
			CDecPkcs12* theTest = NULL;
			theTest = CDecPkcs12::NewL(stream);
			delete theTest;			
			CleanupStack::PopAndDestroy(2,res); // stream
			}
			break;
		// MacData
		case 3:
			{	
			CDecPkcs12MacData* theTest = NULL;
			theTest = CDecPkcs12MacData::NewL(iRawData->Des(),iAuthSafeData->Des());
			delete theTest;
			}
			break;
		// Attribute
		case 4:
			{
			CDecPkcs12Attribute* theTest = NULL;
			theTest = CDecPkcs12Attribute::NewL(iRawData->Des());
			delete theTest;
			}
			break;
		// ShroudedBag
		case 5:
			{
			CDecPkcs12ShroudedKeyBag* theTest = NULL;
			theTest = CDecPkcs12ShroudedKeyBag::NewL(iRawData->Des());
			delete theTest;
			}
			break;
		// CertBag
		case 6:
			{
			CDecPkcs12CertBag* theTest = NULL;
			theTest = CDecPkcs12CertBag::NewL(iRawData->Des());
			delete theTest;
			}
			break;
		// KeyBag
		case 7:
			{
			CDecPkcs12KeyBag* theTest = NULL;
			theTest = CDecPkcs12KeyBag::NewL(iRawData->Des());
			delete theTest;
			}
			break;
		// SafeContentsBag
		case 8:
			{
			CDecPkcs12SafeContentsBag* theTest = NULL;
			theTest = CDecPkcs12SafeContentsBag::NewL(iRawData->Des());
			delete theTest;
			}	
			break;
		// Safebag/CRL bag
		case 9:
			{
			CDecPkcs12SafeBag* theTest = NULL;
			theTest = CDecPkcs12SafeBag::NewL(iRawData->Des());
			delete theTest;
			}
			break;
		// SafeContentsData / SafeContentsEncryptedData
		case 10:
			{
			CDecPkcs12* p12 = NULL;
			p12 = CDecPkcs12::NewL(iRawData->Des());
			CleanupStack::PushL(p12);
			
			const RPointerArray<CPKCS7ContentInfo>& contentInfos = p12->AuthenticatedSafeContents();
				
			TInt contentInfoCount = contentInfos.Count();
			for ( TInt index = 0; index < contentInfoCount; index++ )
				{
				TInt contentType = contentInfos[index]->ContentType();
				CDecPkcs12SafeContents* pkcs12SafeContents ;	
				TInt contentTypeData;
				GetIntFromConfig(ConfigSection(),_L("ContentTypeData"),contentTypeData);
				
				TInt contentTypeEncryptedData;
				GetIntFromConfig(ConfigSection(),_L("ContentTypeEncryptedData"),contentTypeEncryptedData);
				
				if ( contentType == contentTypeEncryptedData )
					{
					TPtrC expectedPassword(KNullDesC());
					GetStringFromConfig(ConfigSection(), _L("Password"), expectedPassword);
					pkcs12SafeContents = CDecPkcs12SafeContents::NewL(*contentInfos[index],expectedPassword);
					delete pkcs12SafeContents;
					}
				else if ( contentType == contentTypeData )
					{
					pkcs12SafeContents = CDecPkcs12SafeContents::NewL(*contentInfos[index]);
					delete pkcs12SafeContents;
					}
				}
				CleanupStack::PopAndDestroy(p12);
			break;
			}
		}
 	__UHEAP_MARKEND;
 	return TestStepResult();
	}
	
/**
Function is intended to read the data from the specified path.
@param-aPValue:filepath containing the path of the file 
			   retrieved from the ini file.
@internalTechnology:
@return:returns data retrieved after reading from the file.
@test:
*/	
HBufC8* CTPKCS12OOMStep::ReadFileDataL(TPtrC tag)
	{
	TPtrC fileName;
	if (!GetStringFromConfig(ConfigSection(), tag, fileName))
		{
		return NULL;
		}

	RFile file;
	if (file.Open(iFs, fileName, EFileRead))
		{
		INFO_PRINTF2(_L("Cannot open file %S for reading"), &fileName);
		return NULL;
		}
	CleanupClosePushL(file);

	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));

	HBufC8* result = HBufC8::NewL(fileSize);
	result->Des().SetLength(fileSize);
	
	TPtr8 rawDataPtr(result->Des());
	rawDataPtr.SetLength(fileSize);
	file.Read (rawDataPtr);

	CleanupStack::PopAndDestroy (&file);

	INFO_PRINTF3(_L("Read %d octets from %S"), result->Size(), &fileName);

	return result;
	}

TVerdict CTPKCS12OOMStep::doTestStepPreambleL()
	{
	User::LeaveIfError (iFs.Connect());

	SetTestStepResult(EPass);
	
	iRawData = ReadFileDataL(_L("File"));
	if (iRawData == NULL)
		{
		INFO_PRINTF1(_L("Failed to read 'File' section of script"));
		SetTestStepResult(ETestSuiteError);
		}
	iAuthSafeData = ReadFileDataL(_L("AuthSafe"));
	if (iAuthSafeData == NULL)
		{
		INFO_PRINTF1(_L("authsafedata file is not required"));
		}
	return TestStepResult();
	}
/**
Function is intended to check for the re encoded shrouded key
@param-aSecName:BagData Section
@param-aBagData:safebag class pointer
@internalTechnology:
@return:returns EFalse if there is a failure else return ETrue
@test:
*/
TBool CPKCS12LibTestStep::ChkEncodedBagValL(TDesC &aSecName,CSafeBagData &aBagData)
	{
	TPtrC bagPath(KNullDesC);
	TBool flagReturn=ETrue;
	
	//retrieve the shrouded bag value
	if(!GetStringFromConfig(aSecName,KEncBagValue,bagPath))
		{
		ERR_PRINTF1(_L("Not able to get the EncBagValue from the ini file"));
		SetTestStepResult(EFail);
	  	flagReturn=EFalse;
	  	}
	TInt fileReturn=0; 	
	RFile file;
	
	if(flagReturn)
		{
		fileReturn=file.Open(iFileSession,bagPath,EFileRead);
		
		if(fileReturn)
			{
			ERR_PRINTF1(_L("BagValue:Not able to open the file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}	
		}
	CleanupClosePushL(file);
	//raw data for reading the bagvalue
	HBufC8 *rawData=NULL;
	TInt filesize=0;	
	if(flagReturn)
		{
		User::LeaveIfError(file.Size(filesize));
		rawData=HBufC8::NewMaxLC(filesize);
		TPtr8 data(rawData->Des());
		if(file.Read(data))
			{
			ERR_PRINTF1(_L("BagValue:Not able to Read the file"));
			SetTestStepResult(EFail);
			flagReturn=EFalse;
			}
		}
	if(flagReturn)
		{
	  	//compare the data
	  	if(rawData->CompareC(aBagData.EncodedShroudedKey())!=0)
			{
			iActualResult=EFalse;
			}
		//retrieve the expected result:PositiveTest/NegativeTest	
		if(!GetBoolFromConfig(aSecName,KExpectedResult,iExpectedResult))
			{
			WARN_PRINTF1(_L("ExpectedResult not found in INI file:Default:PositiveTest"));
			iExpectedResult=KPositiveTest;
			}			
		//check for positive or negative test	
		if(!VerifyExpectedError(aSecName))
			{
			INFO_PRINTF1(_L("BagValue Failed"));
			SetTestStepResult(EFail);
			flagReturn = EFalse;
			}			
		}
	if(rawData!=NULL)	
		{
		CleanupStack::PopAndDestroy(rawData); 	
		}
	if(!fileReturn)
		{
		file.Close();	
		}
	CleanupStack::PopAndDestroy(&file);
	return flagReturn;
	}


