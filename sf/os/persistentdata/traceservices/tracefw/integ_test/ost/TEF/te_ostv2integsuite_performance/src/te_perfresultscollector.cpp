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
// Ost Performance Tests Results Collector.
//



/**
 @file te_perfresultscollector.cpp
 @internalTechnology
 @prototype
*/

#include "te_perfresultscollector.h"
#include "te_perfcsvgenerator.h"
#include "te_lightlogger.h"
#include "uloggerclient.h"
using namespace Ulogger; //CA:added so can use old ulogger api
CUptResults::CUptResults()
	{
	/*Constructor*/
	}

CUptResults::~CUptResults()
	{
	/*Destructor*/
	iPluginTest.Reset();
	iApiTest.Reset();
	}

/** This user-side method uses take an API test object as an argument and stores it as an element in the
private data member iApiTest, an array of API test objects.
@param aApiTest is a reference to a CUptApiTest object
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */
void CUptResults::StoreApiResultsL(const TApiTestResult& aApiTest)
	{
	//store the apitest result object at the end of the list
	iApiTest.InsertL(&aApiTest, iApiTest.Count());
	__TEST_LOG("API Store");
	__TEST_LOGNUM(iApiTest.Count())
	}
	
/** This user-side method uses take an OutputPlugin test object as an argument and stores it as an element in the
private data member iOutputPluginTest, an array of output plugin test objects.
@param aOutputPluginTest is a reference to a CUptOutputPluginTest object
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */
void CUptResults::StorePluginResultsL(const CPluginTestResult& aPluginTest)
	{
	//store the plugin test result object at the end of the list
	iPluginTest.InsertL(&aPluginTest, iPluginTest.Count());
	__TEST_LOG("Plugin Store");
	__TEST_LOGNUM(iPluginTest.Count())
	}
	
/** This method user-side method uses takes a filepath, whether the relevant file should be appended or overwritten and the testtype as arguments
uses these to store the contents of results of the relevant test type (exposed by accessing the relevant private data member)
to the specified file.
@param aFileName is the file path where the results are to be stored, passed as a TDesC
@param aAppend is a TBool specifying whether to append or overwrite the file
				ETrue=Append, EFalse=EFalse
@param aTestType is an enum specifying which test type the user wants to write to the file.
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */
void CUptResults::WriteUptResultsL(const TDesC& aFileName, const TBool& aAppend, const TInt& aTestType)
	{	
	//create an RArray object for writing the results to csv

	RArray <TInt64> testdataarray;
	CleanupClosePushL(testdataarray); 
	RArray <TPtrC8> initialpluginarray;
	CleanupClosePushL(initialpluginarray);
	
	//create a csvgenerator class
	CUptCsvGenerator csvgenerator;
	csvgenerator.OpenL(aFileName,aAppend); 
	
	//call writeheader to write the relevent test column header for the csv file, 
	csvgenerator.WriteHeaderL(aTestType);
	
	if(aTestType==0)
		{
		//loop over each element of the RPointerapitestobject (i.e. each of the test runs carried out)
		__TEST_LOG("about to insert array");
		
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iApiId), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iApiSize), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iSizeOfUptData), testdataarray.Count()); //dont really need this as we already have the trace point size - check!
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iHasContext), testdataarray.Count());
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iHasProgramCounter), testdataarray.Count()); 
		//test run config
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iDoTestClassificationFiltering), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iDoTestModuleIdFiltering), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iDoTestClassificationFilterMatch), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iRunConfigs.iDoTestModuleIdFilterMatch), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iRunResults.iTraceEnabled), testdataarray.Count()); 
		//test results
		testdataarray.InsertL((iApiTest[0]->iMinimumExecutionTime), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iTypicalExecutionTime), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iRunResults.iApiStackUsage), testdataarray.Count()); 
		testdataarray.InsertL((iApiTest[0]->iRunResults.iApiHeapUsage), testdataarray.Count()); 
		__TEST_LOG("array inserted, about to write array");
		
		
		//write the API Name to the file
		csvgenerator.WriteApiNameL(iApiTest[0]->iRunConfigs.iApiId);
		//call writetocsv routine to write this test run to a csv file with the name aFileName
		csvgenerator.WriteL(testdataarray);
		__TEST_LOG("array written");
		
		//	__TEST_LOG("Write");
		///	__TEST_LOGNUM(i);
		//	__TEST_LOGNUM(iApiTest.Count());

		//call write newline to write a new line in preparation for the next test object
		csvgenerator.WriteNewLineL();
		
		//clear out the RArray object in preparation for the next test result object
		testdataarray.Reset(); 
		}
	
	if(aTestType==1||aTestType==2)
		{
		//loop over each element of the RPointeroutputplugintestobject (i.e. each of the test runs carried out)
		__TEST_LOG("about to insert plugin text array");
		
		initialpluginarray.InsertL((iPluginTest[0]->iPluginName), initialpluginarray.Count());
		initialpluginarray.InsertL(iPluginTest[0]->iPluginKey, initialpluginarray.Count());
		initialpluginarray.InsertL(iPluginTest[0]->iPluginValue, initialpluginarray.Count());
		initialpluginarray.InsertL(iPluginTest[0]->iTestType, initialpluginarray.Count());
		
		__TEST_LOG("array inserted, about to write plugin text array");
		
		csvgenerator.WriteL(initialpluginarray);
		__TEST_LOG("output plugin text array written");
		
		__TEST_LOG("about to insert plugin number array");
		testdataarray.InsertL((iPluginTest[0]->iDataSize), testdataarray.Count());
		testdataarray.InsertL((iPluginTest[0]->iIteration), testdataarray.Count());
		//testdataarray.InsertL((iOutputPluginTest[0]->iThroughput), testdataarray.Count());
		testdataarray.InsertL((iPluginTest[0]->iLatency), testdataarray.Count()); //dont really need this as we already have the trace point size - check!
	
		__TEST_LOG("array inserted, about to write plugin number array");
		
		//call writetocsv routine to write this test run to a csv file with the name aFileName
		csvgenerator.WriteL(testdataarray);
		__TEST_LOG("plugin number array written");
		
		

		//call write newline to write a new line in preparation for the next test object
		csvgenerator.WriteNewLineL();
		
		//clear out the RArray objects in preparation for the next test result object
		initialpluginarray.Reset();
		testdataarray.Reset(); 
		}

	//close the csv file and close and cleanup any heap ojects
	csvgenerator.Close();
	
//	testdataarray.Close();
//	initialpluginarray.Close();
	CleanupStack::PopAndDestroy(&initialpluginarray);
	CleanupStack::PopAndDestroy(&testdataarray);	


	}

/** This sanity test method is executed at the start of the test run to verify that UPT methods in
this class are stable before any of the performance tests are carried out 
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */
TInt CUptResults::Test()
	{
	return KErrNone;
	}

