/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <iostream>
#include <tools/certapp-api.h>
#include "badconfigwriter.h"
#include "tcertapp_good.h"
#include "tcertapp_bad.h"
#include "goodconfigwriter.h"
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include "logger.h"
#include "utils.h"

using namespace std;

// Write a pass or fail result for a test case to the TMS log file
void writeTMSResult(ofstream &aLogFile, stringstream &aTestCaseType, int aTestCaseIndex, bool aResult, stringstream &aLogging);

//Funtions to generate Good/Valid config files for tcertapp
void GoodFileCaCertTests(const stringstream &aFilename, int aArray_value);
void GoodFileCertClientsTests(const stringstream &aFilename);
void GoodSwiCertTests(const stringstream &aFilename, int aArray_value);

//Functions to generate Good/Valid script files for emulator tests
void GenerateGoodCaCertsTestScripts(const stringstream &aFilename, const stringstream &aTestCaseType, int &aTestIndex);
void GenerateGoodCertClientTestScripts(const stringstream &aFilename, const stringstream &aTestCaseType, int &aTestIndex);
void GenerateGoodSwiCertTestScripts(const stringstream &aFilename, const stringstream &aTestCaseType, int &aTestIndex);

//Funtions to generate Bad/illegal config files for tcertapp
void BadFileCaCertTests(const stringstream &aFilename, int aArray_value, stringstream &aTestCaseType, int &aTestIndex);
void BadFileCertClientsTests(const stringstream &aFilename, int aIndex, stringstream &aTestCaseType, int &aTestIndex);
void BadSwiCertTests(const stringstream &aFilename, int aArray_value, stringstream &aTestCaseType, int &aTestIndex);

//Function call for RunCertApp api
void RunCertAppTool(int argc, char **argv, stringstream &aStringval, stringstream &testCaseType, int testCaseIndex);
void RunCertAppToolForStoreDump(int aArgc, char **aArgv, stringstream &aStringval, stringstream &testCaseType, int testCaseIndex); 
void RunCertAppToolForBadParams(int aArgc, char **aArgv, stringstream &aStringval, stringstream &testCaseType, int testCaseIndex); 

void TestsWithEmptyConfigFile();
void CreateFileToAugmentStore();
void TestsWithEncodedFileFormats();
void TestToDecodeAndEncodeStore();
void TestToReviewingAggregateStore();
void TestBadConfigFiles(const stringstream &aFilename, bool aBool, stringstream &aTestCaseType, int aTestCaseIndex);
void CertClientBadParamsFuncCall(stringstream &aFileName, stringstream &aTestCaseType, int aTestCaseIndex);
void decodeEncodeCertAppCall(const char *aArray[]);
int CompareFiles(stringstream &aFileName1, stringstream &aFileName2);
int Compare(istream &aIstream1, istream &aIstream2);
void TestWithUnicodFileName();

ofstream file; //global file handle to write to output file
const char *progress = "progress.txt";  // logs the progress report
const char *errors = "error.txt";       // logs the error report

//some constants used
const int KFileArrayIndex = 8;
const int KSwicertIndex = 10;
const int KSwiBadcertIndex = 11;
const int KFileBadcertIndex = 9;
const int KAppUidIndex = 4; 
const int KIndex = 4;

// to get the count of passed and failed test cases
static int failCount = 0, passCount = 0;

//pointers required for creation of good filecerts config files
const char **filecert_array[KFileArrayIndex]=
{
	goodCert_array,
	goodDeletables,
	goodFormats,
	goodCertOwnerTypes,
	goodSubjectKeyIds,
	goodIssuerKeyIds,
	goodApplications,
	goodTrusteds
};

//pointers required for creation of bad filecerts config files
const char **badfilecert_array[KFileBadcertIndex]=
{
	badLabels,
	badDeletables,
	badFormats,
	badCertOwnerTypes,
	badSubjectKeyIds,
	badIssuerKeyIds,
	badApplications,
	badTrusteds,
	badDatafilePath
};


//pointers required for creation of good swicerts config files
const char **swicert_array[KSwicertIndex]=
{
	goodLabels,
	goodFormats,
	goodCertOwnerTypes,
	goodSubjectKeyIds,
	goodIssuerKeyIds,
	goodAppNames,
	goodTrusteds,
	goodCapabilitySets,
	goodMandatory,
	goodSystemUpgrade
};

//pointers required for creation of bad swicerts config files
const char **badswicert_array[KSwiBadcertIndex]=
{
	badLabels,
	badFormats,
	badCertOwnerTypes,
	badSubjectKeyIds,
	badIssuerKeyIds,
	badApplications,
	badTrusteds,
	badDatafilePath,
	badCapabilitySets,
	badMandatory,
	badSystemUpgrade
};


//encoded file names for input to the tool
const char *fileEncodeType[4]=
{
"--hfilecertstore=utf8.txt",
"--hfilecertstore=utf8_without_bom.txt",
"--hfilecertstore=ucs2-bigendian.txt",
"--hfilecertstore=ucs2-littleendian.txt"
};

// Write a pass or fail result for a test case to the TMS log file
void writeTMSResult(ofstream &aLogFile, stringstream &aTestCaseType, int aTestCaseIndex, bool aResult, stringstream &aLogging)
	{
	// get the current time
	time_t rawtime;
	struct tm * timeinfo;
	char standardTimeBuffer[10];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(standardTimeBuffer, 10, "%H:%M:%S", timeinfo);
	
	char millisecondTimeBuffer[6];

	#ifdef __TOOLS2_LINUX__
		struct timeb tstruct;		
		ftime(&tstruct);
	#else
		struct _timeb tstruct;		
		_ftime(&tstruct);
	#endif

	sprintf(millisecondTimeBuffer, ":%03u ", tstruct.millitm);
	
	stringstream timeBuffer;
	timeBuffer << standardTimeBuffer << millisecondTimeBuffer;
	
	// set result string
	stringstream resultBuffer;
	
	if (aResult)
		{
		resultBuffer << "PASS";
		}
	else
		{
		resultBuffer << "FAIL";
		}
	
	// set test case ID string
	char testCaseIndexBuffer[6];
	sprintf(testCaseIndexBuffer, "-%04d", aTestCaseIndex);
	
	// write result in the correct format
	aLogFile << "<html><body><pre>\r\n";
	aLogFile << timeBuffer.str();
	aLogFile << "START_TESTCASE ";
	aLogFile << aTestCaseType.str() << testCaseIndexBuffer;
	aLogFile << "\r\n";
	aLogFile << timeBuffer.str();
	aLogFile << "Line = 1 Command = START_TESTCASE  ";
	aLogFile << aTestCaseType.str() << testCaseIndexBuffer;
	aLogFile << "\r\n";
	
	if (aLogging.str() != "")
		{
		aLogFile << timeBuffer.str();
		aLogFile << aLogging.str();
		aLogFile << "\r\n";		
		}
	
	aLogFile << timeBuffer.str();
	aLogFile << "END_TESTCASE ";
	aLogFile << aTestCaseType.str() << testCaseIndexBuffer;
	aLogFile << "\r\n";
	aLogFile << timeBuffer.str();
	aLogFile << "Line = 1 Command = END_TESTCASE ";
	aLogFile << aTestCaseType.str() << testCaseIndexBuffer;
	aLogFile << " ***TestCaseResult = ";
	aLogFile << resultBuffer.str();
	aLogFile << "\r\n</pre></body></html>\r\n";
	}

//RunCertApp call to create store from text input
void RunCertAppTool( int argc, char **argv, stringstream &stringval, stringstream &testCaseType, int testCaseIndex)
{
	for(int i =0; i<argc; i++)
		{
		cout << "Arguments Being passed to the RunCertApp tool is " << argv[i] << endl;
		}
	int ret = RunCertApp(progress, errors, argc, argv); 
	cout << "Return Value from CertApp Tool is " << ret << endl;
	stringstream logging;	
	if(ret!= 0)
		{
		failCount++;
		logging << "Error in creating store for " << stringval.str() << " and result is " << ret << " Fail" ;
		}
	else
		{
		passCount++;
		logging << "No Error in creating store for " << stringval.str() << " and result is " << ret << " Pass" ;
		}
	
	writeTMSResult(file, testCaseType, testCaseIndex, ret==0, logging);
}

//RunCertApp call to dump the store to human readable format
void RunCertAppToolForStoreDump(int argc, char **argv, stringstream &stringval, stringstream &testCaseType, int testCaseIndex)
{
	int ret = RunCertApp(progress, errors, argc, argv); 
	cout << "Return Value from CertApp Tool is " << ret << endl;
	stringstream logging;	
	if(ret!= 0)
		{
		failCount++;
		logging << "Error in Dumping the store file for " << stringval.str() << " and result is " << ret << " Fail";
		}
	else
		{
		passCount++;
		logging << "No Error in Dumping store for " << stringval.str() << " and result is " << ret << " Pass";
		}
	
	writeTMSResult(file, testCaseType, testCaseIndex, ret==0, logging);
}

//RunCertApp call to create store for bad params
void RunCertAppToolForBadParams(int argc, char **argv, stringstream &stringval, stringstream &testCaseType, int testCaseIndex)
{
	int ret = RunCertApp(progress, errors, argc, argv); 
	stringstream logging;	
	if(ret!= 0)
		{
		passCount++;
		cout << "We have passsed bad paramerters! Expected Return value is -1 And the RunCertApp has returned " << ret << " as Expected"<< endl;
		logging << "Bad Params! So Cannot create the store for " << stringval.str() << " and result is " << ret << " Pass";
		}
	else
		{
		failCount++;
		logging << "We are using bad parameters! So the tools should fail to create store! " << stringval.str() << " and result is " << ret << " Fail";
		cout << "We have passsed bad paramerters! The Tool has failed- It should return -1, But returned" << ret << endl;
		}
	
	writeTMSResult(file, testCaseType, testCaseIndex, ret==0, logging);	
}


/**
main entry to the program
*/
int main(int /*argc*/, char ** /*argv*/)
{
	stringstream testCaseType;
	int testCaseIndex = 0;

	dbg.SetStream(&std::cout);
	prog.SetStream(&std::cout);
	std::cout << std::dec;

	int num_tests = 0; 
	int argCount3 = 3;
	int argCount4 = 4;
	// File to log the result of all tests
	if(file)
		{
		file.open("tCertapp_Output.htm", ios_base::trunc | ios_base::out);
		}

	//create a file to augment the content to existing store
	CreateFileToAugmentStore();

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCDAT_CREATE-0000
  @SYMTestCaseDesc	Create store
*/	

	//create certclient config files 
	//Format used for the files name is SDP-Security_Tools-CertApp-XXX-000X  to match with the TMS test case id
	stringstream certclientfileName;
	certclientfileName << "SDP-Security_Tools-CertApp-CCDAT_000"<<num_tests<<".txt";
	GoodFileCertClientsTests(certclientfileName);
	
	//create command line argument for creation of certclientx.dat through certapp tool
	stringstream strArg1, strArg2, augFileName;
	strArg1 <<"--hcertclients=" << certclientfileName.str();
	cout << "test" << strArg1.str().c_str() <<endl;

	strArg2<< "--bcertclients=SDP-Security_Tools-CertApp-CCDAT_000" <<num_tests <<".dat";
	cout <<"string value is " << strArg2.str().c_str() << endl;		
	
	string argument1, argument2;
	const char *argv_certclient[4];
	
	argument1 = strArg1.str().c_str();
	argv_certclient[0] = argument1.c_str();
			
	argv_certclient[1] = "--out";
	argument2 = strArg2.str().c_str();
	argv_certclient[2] = argument2.c_str();

	char **argv, **argv1;
	char *argument_ptr[3]; //[Format:: certapp --hcertclients=t_certclients_0.txt --out --bcertclients=certclients5.dat]

	for(int n1 = 0; n1<argCount3; n1++)
		{
		argument_ptr[n1] = const_cast<char*>(argv_certclient[n1]);
		argv = argument_ptr;
		}
	
	testCaseType.str("SEC-TOOLS-CERTAPP-CCDAT_CREATE");
	RunCertAppTool(argCount3, argv, certclientfileName, testCaseType, 0); // creating dat file
	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCAUG_AUGMENT-0000
  @SYMTestCaseDesc	Augment the Store with Extra entries
*/	

	//Augment the Store with Extra entries
	augFileName<<"SDP-Security_Tools-CertApp-CCAUG_0000.dat";
	argv_certclient[0] = "--hcertclients=tCertClient_Augment.txt";
	argv_certclient[1] =  argv_certclient[2];  
	argv_certclient[2] = "--out";
	argv_certclient[3] = "--bcertclients=SDP-Security_Tools-CertApp-CCAUG_0000.dat";
	for(int x = 0; x<argCount4; x++)
			{
			argument_ptr[x] = const_cast<char*>(argv_certclient[x]);
			argv1 = argument_ptr;   
			}
	
	testCaseType.str("SEC-TOOLS-CERTAPP-CCAUG_AUGMENT");
	RunCertAppTool(argCount4, argv1, augFileName, testCaseType, 0);

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCDAT_DUMP-0000
  @SYMTestCaseDesc	Dump the dat file to human readable text format
*/	

	//Dump the dat file to human readable text format
	argv_certclient[0] = argv_certclient[3];
	argv_certclient[1] = "--out";
	argv_certclient[2] = "--hcertclients=SDP-Security_Tools-CertApp-CCDUMP_0000.txt";
			
	for(int k = 0; k<argCount3; k++)
		{
		argument_ptr[k] = const_cast<char*>(argv_certclient[k]);
		argv1 = argument_ptr;
		}

	testCaseType.str("SEC-TOOLS-CERTAPP-CCDAT_DUMP");
	RunCertAppToolForStoreDump(argCount3, argv1, certclientfileName, testCaseType, 0); 
	//End of t_certclients
	
	//Creates good config files to augment store and dump store to text format for file certstore - cacerts.dat
	//Test for all positive cases
	for(num_tests = 0; num_tests<=KFileArrayIndex; num_tests++)
		{
		stringstream caCertFileName;
		caCertFileName << "SDP-Security_Tools-CertApp-FILEDAT_000"<<num_tests<<".txt";
		GoodFileCaCertTests(caCertFileName,num_tests);

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0000
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0001
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0002
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0003
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0004
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0005
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0006
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0007
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDAT_CREATE-0008
  @SYMTestCaseDesc	CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
*/	

		// CREATE CACERTS STORE FROM HUMAN READABLE TEXT INPUT
		//ex:certapp --hcertclients=t_certclients_0.txt --hfilecertstore=t_cacerts_0.txt --out --bfilecertstore=cacerts_x.dat
		stringstream strArg1, strArg2, strArg3, strArg4, strArg5;
		strArg1 <<"--hfilecertstore=SDP-Security_Tools-CertApp-FILEDAT_000" <<num_tests << ".txt" ; 
		
		strArg2<< "--bfilecertstore=SDP-Security_Tools-CertApp-FILEDAT_000"<<num_tests <<".dat";
		cout <<"string value is " << strArg2.str().c_str() << endl;	
		
		strArg3<< "--hcertclients=SDP-Security_Tools-CertApp-CCDAT_0000.txt";
		cout <<"string value is " << strArg3.str().c_str() << endl;

		string argument1, argument2, argument3, argument4, argument5;
		const char *argv_certclient[4];
		
		argument1 = strArg1.str().c_str();
		argument3 = strArg3.str().c_str();

		argv_certclient[0] = argument3.c_str(); // --hcertclients=t_certclients_0.txt
		argv_certclient[1] = argument1.c_str(); // --hfilecertstore=t_cacerts_x.txt
		argv_certclient[2] = "--out";			//--out
		argument2 = strArg2.str().c_str();
		argv_certclient[3] = argument2.c_str(); //--bfilecertstore=cacerts_x.dat

		char *argument_ptr[4]; 
		char **argv, **argv1, **argv2 ;
		for(int n = 0; n<argCount4; n++)
			{
			argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
			argv = argument_ptr;  
			}

		testCaseType.str("SEC-TOOLS-CERTAPP-FILEDAT_CREATE");
		RunCertAppTool(argCount4, argv, caCertFileName, testCaseType, num_tests); 
		
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0000
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0001
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0002
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0003
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0004
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0005
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0006
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0007
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT-0008
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	

		//AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
		//ex:certapp  --hfilecertstore=tAugment_cacerts_x.txt  --bfilecertstore=cacerts.dat --out --bfilecertstore=new_extra_filecertstore.dat
		stringstream augFileName, textDumpFileName;
		augFileName<<"SDP-Security_Tools-CertApp-FILEAUG_000" <<num_tests << ".dat";
		strArg4<<"--bfilecertstore=SDP-Security_Tools-CertApp-FILEAUG_000" << num_tests << ".dat";
		argument4 = strArg4.str();
		argv_certclient[0] =  "--hfilecertstore=tfile_Augment.txt";  //--hfilecertstore=tAugment_cacerts_x.txt
		argv_certclient[1] =  argv_certclient[3]; //--bfilecertstore=cacerts_x.dat
		argv_certclient[2] = "--out";			  //--out
		argv_certclient[3] = argument4.c_str();  //--bfilecertstore=aug_cacerts_x.dat
		
		for(int x = 0; x<argCount4; x++)
			{
			argument_ptr[x] = const_cast<char*>(argv_certclient[x]);
			argv1 = argument_ptr;   
			}

		testCaseType.str("SEC-TOOLS-CERTAPP-FILEAUG_AUGMENT");
		RunCertAppTool(argCount4, argv1, augFileName, testCaseType, num_tests);

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0000
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0001
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0002
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0003
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0004
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0005
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0006
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0007
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEDUMP_DUMP-0008
  @SYMTestCaseDesc	DUMP THE STORE TO HUMAN READABLE FORMAT
*/	
		
		//DUMP THE STORE TO HUMAN READABLE FORMAT
		//ex:certapp --bfilecertstore=test.dat --out --hfilecertstore=t_cacerts.txt 
		textDumpFileName <<"SDP-Security_Tools-CertApp-FILEDUMP_000"<< num_tests << ".txt";
		argv_certclient[0] =  argv_certclient[3];  //--bfilecertstore=aug_cacerts_x.dat
		argv_certclient[1] = "--out";
		strArg5<< "--hfilecertstore=SDP-Security_Tools-CertApp-FILEDUMP_000" <<num_tests << ".txt";
		argument5 = strArg5.str().c_str();
		argv_certclient[2] = argument5.c_str();
						
		for(int k = 0; k<argCount3; k++)
			{
			argument_ptr[k] = const_cast<char*>(argv_certclient[k]);
			argv2 = argument_ptr;
			}
		//Dumps the augmented store

		testCaseType.str("SEC-TOOLS-CERTAPP-FILEDUMP_DUMP");
		RunCertAppToolForStoreDump(argCount3, argv2, textDumpFileName, testCaseType, num_tests); 
		}//End of cacerts


	//create config files with legal values for swi certstore 
	for(num_tests = 0; num_tests<KSwicertIndex; num_tests++)
		{
		stringstream swiCertStoreFileName;
		swiCertStoreFileName << "SDP-Security_Tools-CertApp-SWIDAT_000"<<num_tests<<".txt";
		GoodSwiCertTests(swiCertStoreFileName,num_tests);

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0000
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0001
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0002
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0003
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0004
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0005
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0006
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0007
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0008
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDAT_CREATE-0009
  @SYMTestCaseDesc	CREATE THE SWICERTSTORE FROM THE HUMAN READABLE TEXT INPUT
*/	

		//CREATE THE SWICERTSTORE  FROM THE HUMAN READABLE TEXT INPUT
		//Ex: certapp --hcertclients=t_certclients_0.txt --hswicertstore=t_swicertstore_0.txt --out --bswicertstore=swicertstore_x.dat
		stringstream strArg1, strArg2, strArg3, strArg4, strArg5;
		strArg1 <<"--hswicertstore=SDP-Security_Tools-CertApp-SWIDAT_000" <<num_tests << ".txt" ; //--hswicertstore=t_cacerts_x.txt
		
		strArg2<< "--bswicertstore=SDP-Security_Tools-CertApp-SWIDAT_000" <<num_tests <<".dat"; //--bswicertstore=swicertstore.dat
		cout <<"string value is " << strArg2.str().c_str() << endl;	
		
		strArg3<< "--hcertclients=SDP-Security_Tools-CertApp-CCDAT_0000.txt";
		cout <<"string value is " << strArg3.str().c_str() << endl;

		string argument1, argument2, argument3, argument4, argument5;
		const char *argv_certclient[4];
		
		argument1 = strArg1.str().c_str();
		argument3 = strArg3.str().c_str();

		argv_certclient[0] = argument3.c_str(); //--hcertclients=t_certclients_0.txt
		argv_certclient[1] = argument1.c_str(); //--hswicertstore=t_swicertstore_
		argv_certclient[2] = "--out";
		argument2 = strArg2.str().c_str();
		argv_certclient[3] = argument2.c_str();  //--bswicertstore=swicertstore_x.dat

	    char *argument_ptr[4]; 
	
		for(int j=0; j<argCount4; j++)
			{
			cout << "Value in argv_certclient " << j << "is " <<argv_certclient[j] <<endl;
			}

		char **argv;
		for(int n = 0; n<argCount4; n++)
			{
			argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
			argv = argument_ptr;
			}

		testCaseType.str("SEC-TOOLS-CERTAPP-SWIDAT_CREATE");
		RunCertAppTool(argCount4, argv,swiCertStoreFileName, testCaseType, num_tests); 

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0000
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0001
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0002
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0003
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0004
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0005
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0006
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0007
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0008
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT-0009
  @SYMTestCaseDesc	AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
*/	

		//AUGMENT THE CACERT STORE WITH EXTRA INFORMATION
		//certapp  --hswicertstore=tAugment_swicertstore_x.txt  --bswicertstore=swicertstore_x.dat --out --bswicertstore=aug_swicertstore_x.dat
		stringstream augFileName, textDumpFileName;
		augFileName << "SDP-Security_Tools-CertApp-SWIAUG_000" << num_tests <<".dat";
		strArg4<<"--bswicertstore=SDP-Security_Tools-CertApp-SWIAUG_000" << num_tests << ".dat";
		argument4 = strArg4.str();
		argv_certclient[0] =  "--hswicertstore=tswi_Augment.txt";  //--hswicertstore=tAugment_swicertstore_x.txt
		argv_certclient[1] =  argv_certclient[3]; //--bswicertstore=swicertstore_x.dat
		argv_certclient[2] = "--out";
		argv_certclient[3] = argument4.c_str();  //--bswicertstore=aug_swicertstore_x.dat
		char **argv1;
		for(int x = 0; x<argCount4; x++)
			{
			argument_ptr[x] = const_cast<char*>(argv_certclient[x]);
			argv1 = argument_ptr;   
			}

		testCaseType.str("SEC-TOOLS-CERTAPP-SWIAUG_AUGMENT");
		RunCertAppTool(argCount4, argv1, augFileName, testCaseType, num_tests);

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0000
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0001
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0002
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0003
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0004
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0005
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0006
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0007
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0008
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIDUMP_DUMP-0009
  @SYMTestCaseDesc	DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
*/	
		
		//DUMP THE SWI CERTSTORE TO HUMAN READABLE FORMAT
		textDumpFileName << "SDP-Security_Tools-CertApp-SWIDUMP_000" <<num_tests << ".txt";
		argv_certclient[0] =  argv_certclient[3];  
		argv_certclient[1] = "--out";
		strArg5<< "--hswicertstore=SDP-Security_Tools-CertApp-SWIDUMP_000" <<num_tests << ".txt";
		argument5= strArg5.str().c_str();
		argv_certclient[2] = argument5.c_str();
			
		char ** argv2;
		for(int k = 0; k<argCount3; k++)
			{
			argument_ptr[k] = const_cast<char*>(argv_certclient[k]);
			argv2 = argument_ptr;
			}

		testCaseType.str("SEC-TOOLS-CERTAPP-SWIDUMP_DUMP");
		RunCertAppToolForStoreDump(argCount3, argv2, textDumpFileName, testCaseType, num_tests); 
		}
	
	//Test with unicode filename
	TestWithUnicodFileName();

	//create empty config file
	TestsWithEmptyConfigFile();

	//Test case to test for different file formats
	TestsWithEncodedFileFormats();

	//Test case for REQ10313 to check decode/encode of stores
	TestToDecodeAndEncodeStore();

	//Test case for REQ10314 - reviewing aggregate certificate database
	TestToReviewingAggregateStore();

	//Test cases for BAD Params
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0000
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0001
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATEE-0002
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0003
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0004
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0005
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0006
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0007
  @SYMTestCaseDesc	Create bad certclient config files
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE-0008
  @SYMTestCaseDesc	Create bad certclient config files
*/	
	//create certclient config files 
	testCaseType.str("SEC-TOOLS-CERTAPP-CCBADPARAMS_CREATE");
	testCaseIndex = 0;
	for(num_tests =0; num_tests<3; num_tests++) 
		{
		stringstream badCertClientsFileName;
		badCertClientsFileName << "SDP-Security_Tools-CertApp-CCBadParams-000"<<num_tests;
		BadFileCertClientsTests(badCertClientsFileName,num_tests,testCaseType,testCaseIndex);
		} //End of bad params for cerclients

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0000
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0001
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0002
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0003
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0004
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0005
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0006
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0007
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0008
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0009
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0010
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0011
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0012
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0013
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0014
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0015
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0016
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0017
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0018
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0019
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0020
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0021
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0022
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0023
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0024
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0025
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE-0026
  @SYMTestCaseDesc	Create bad config files for file certstore
*/	
	//create bad config files for file certstore - cacerts.dat
	testCaseType.str("SEC-TOOLS-CERTAPP-FILEBADPARAMS_CREATE");
	testCaseIndex = 0;
	for(num_tests = 0; num_tests<KFileArrayIndex+1; num_tests++)
		{
		stringstream badCaCertsFileName;
		badCaCertsFileName << "SDP-Security_Tools-CertApp-FileBadParams-000"<<num_tests; 
		BadFileCaCertTests(badCaCertsFileName,num_tests,testCaseType,testCaseIndex);
		}
	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0000
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0001
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0002
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0003
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0004
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0005
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0006
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0007
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0008
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0009
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0010
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0011
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0012
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0013
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0014
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0015
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0016
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0017
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0018
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0019
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0020
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0021
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0022
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0023
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0024
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0025
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0026
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0027
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0028
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0029
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE-0030
  @SYMTestCaseDesc	Create config files with invalid values swi certstore 
*/	
	//create config files with invalid values swi certstore 
	testCaseType.str("SEC-TOOLS-CERTAPP-SWIBADPARAMS_CREATE");
	testCaseIndex = 0;
	for(num_tests = 0; num_tests<KSwiBadcertIndex; num_tests++)
		{
		stringstream badSwiCertsFileName;
		badSwiCertsFileName << "SDP-Security_Tools-CertApp-SwiBadParams-000"<<num_tests;
		BadSwiCertTests(badSwiCertsFileName,num_tests,testCaseType,testCaseIndex);
		}

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0001
  @SYMTestCaseDesc	Initializing a CUnifiedCertStore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0002
  @SYMTestCaseDesc	Get the list of certificates
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0003
  @SYMTestCaseDesc	Get certificate details
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0004
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0005
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0006
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0007
  @SYMTestCaseDesc	Get certificate details
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0008
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0009
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0010
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0011
  @SYMTestCaseDesc	Get certificate details
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0012
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0013
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0014
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0015
  @SYMTestCaseDesc	Get certificate details
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0016
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0017
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0018
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0019
  @SYMTestCaseDesc	Get certificate details
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0020
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0021
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0022
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0023
  @SYMTestCaseDesc	Get certificate details
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0024
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0025
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCACERT-0026
  @SYMTestCaseDesc	Retrieve Certificate
*/	
	//Code to create Script files for the file/sw/certclients to run validation tests on emulator
	//create script file for the symbian test harness-  for cacerts.dat
	testCaseType.str("SEC-TOOLS-CERTAPP-GOODCACERT");
	testCaseIndex = 0;
	for(num_tests = 0; num_tests<1; num_tests++)
		{
		stringstream emu_CacertsFileName;
		emu_CacertsFileName << "SDP-Security_Tools-CertApp-FILEDAT-EMU0"<<num_tests<<".script";
		GenerateGoodCaCertsTestScripts(emu_CacertsFileName,testCaseType,testCaseIndex);
		}

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0001
  @SYMTestCaseDesc	Initialise a CertClientStore
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0002
  @SYMTestCaseDesc	Get Count of Applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0003
  @SYMTestCaseDesc	Getting the application list
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0004
  @SYMTestCaseDesc	Get application with given id
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0005
  @SYMTestCaseDesc	Get application with given id
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0006
  @SYMTestCaseDesc	Get application with given id
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0007
  @SYMTestCaseDesc	Get application with given id
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODCERT-0008
  @SYMTestCaseDesc	Destroy the manager
*/	
	//create script file for certclients.dat
	testCaseType.str("SEC-TOOLS-CERTAPP-GOODCERT");
	testCaseIndex = 0;
	for(num_tests = 0; num_tests<1; num_tests++)
		{
		stringstream emu_CertClientsFileName;
		emu_CertClientsFileName << "SDP-Security_Tools-CertApp-CCDAT-EMU0"<<num_tests<<".script";
		GenerateGoodCertClientTestScripts(emu_CertClientsFileName,testCaseType,testCaseIndex);
		}

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0001
  @SYMTestCaseDesc	Initialise a SWICertStore
*/
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0002
  @SYMTestCaseDesc	Get the list of certificates
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0003
  @SYMTestCaseDesc	Get the systemupgrade flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0004
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0005
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0006
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0007
  @SYMTestCaseDesc	Get the capabilities
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0008
  @SYMTestCaseDesc	Get the mandatory flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0009
  @SYMTestCaseDesc	Get the systemupgrade flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0010
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0011
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0012
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0013
  @SYMTestCaseDesc	Get the capabilities
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0014
  @SYMTestCaseDesc	Get the mandatory flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0015
  @SYMTestCaseDesc	Get the systemupgrade flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0016
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0017
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0018
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0019
  @SYMTestCaseDesc	Get the capabilities
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0020
  @SYMTestCaseDesc	Get the mandatory flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0021
  @SYMTestCaseDesc	Get the systemupgrade flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0022
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0023
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0024
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0025
  @SYMTestCaseDesc	Get the capabilities
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0026
  @SYMTestCaseDesc	Get the mandatory flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0027
  @SYMTestCaseDesc	Get the systemupgrade flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0028
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0029
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0030
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0031
  @SYMTestCaseDesc	Get the capabilities
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0032
  @SYMTestCaseDesc	Get the mandatory flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0033
  @SYMTestCaseDesc	Get the systemupgrade flag
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0034
  @SYMTestCaseDesc	Retrieve Certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0035
  @SYMTestCaseDesc	Get applications
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0036
  @SYMTestCaseDesc	Get Trust certificate
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0037
  @SYMTestCaseDesc	Get the capabilities
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-GOODSWICERT-0038
  @SYMTestCaseDesc	Get the mandatory flag
*/	
	//create script file for the symbian test harness-  for swicerts.dat
	testCaseType.str("SEC-TOOLS-CERTAPP-GOODSWICERT");
	testCaseIndex = 0;
	for(num_tests = 0; num_tests<1; num_tests++)
		{
		stringstream emu_SwiCertsFileName;
		emu_SwiCertsFileName << "SDP-Security_Tools-CertApp-SWIDAT-EMU0"<<num_tests<<".script";
		GenerateGoodSwiCertTestScripts(emu_SwiCertsFileName,testCaseType,testCaseIndex);
		}

	int testCaseCount = failCount+passCount;
	cout  << "\nTestrun result is :: " << failCount <<" tests failed out of " << testCaseCount <<endl;
	file << "\n\nTotal number of test cases run is -> " << testCaseCount << endl;
	file << failCount << " tests failed out of " << testCaseCount <<endl;
	file.close();
	return 0;
}



/**
Function to create input file for augmenting the existing store
*/
void CreateFileToAugmentStore()
{
	stringstream swifilename, cacertsfilename, certclientName;
	swifilename <<"tswi_Augment.txt";
	cacertsfilename<< "tfile_Augment.txt";
	certclientName << "tCertClient_Augment.txt";

	SwiCertStoreConfigWriter swiwriter(swifilename);
	swiwriter.WriteExtraSwiEntry();
	
	FileCertStoreConfigWriter filewriter(cacertsfilename);
	filewriter.WriteExtraFileEntry();

	FileCertClientConfigWriter certclientwriter(certclientName);
	certclientwriter.WriteExtraCertClientEntry();
}


/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-UNICODEFILE_CREATE-0000
  @SYMTestCaseDesc	Check the certapp tool for unicode named config file
*/	
/**
Test case to check the certapp tool for unicode named config file
Testing with only one unicode named config file for now
*/
void TestWithUnicodFileName()
{
	//unicode file name in utf8 format
	const char *filename= "\xe4\xb8\x95\xe4\xb8\x8d\xe4\xb8\x8e\xe4\xb8\x90\xe4\xb8\x91\xe4\xb8\x96\xe4\xb8\x99\x2e\x74\x78\x74";
	fstream fileHandle;

	OpenUtf8FStreamForWrite(fileHandle,filename);

	char * buffer;
	long size;

	ifstream infile ("SDP-Security_Tools-CertApp-CCDAT_0000.txt",ifstream::binary);
	// get size of file
	infile.seekg(0,ifstream::end);
	size=infile.tellg();
	infile.seekg(0);
	// allocate memory for file content
	buffer = new char [size];
	// read content of infile
	infile.read (buffer,size);
	// write to outfile
	fileHandle.write (buffer,size);
  
	// release dynamically-allocated memory
	delete[] buffer;
	fileHandle.close();
	infile.close();

	//create argument list for certapp
	const char *argv_certclient[3];
	stringstream str1;
	str1 <<"--hcertclients=" << filename;
	string string1;
	string1 = str1.str();

	argv_certclient[0] =  string1.c_str();
	argv_certclient[1] = "--out";
	argv_certclient[2] = "--bcertclients=SDP-Security_Tools-CertApp-UNI00.dat";
	
	char **argv;
	char *argument_ptr[3];
	int argCount=3;
	for(int n = 0; n<argCount; n++)
		{
		argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
		argv = argument_ptr;
		}

	for(int i =0; i<argCount; i++)
		{
		cout << "Arguments Being passed to the RunCertApp tool is " << argv[i] << endl;
		}

	int ret = RunCertApp(progress, errors, argCount, argv); 
	cout << "Return value from certapp is " << ret <<endl;
	stringstream logging;
	if(ret!= 0)
		{
		failCount++;
		logging << "Error in creating store using the unicode file name config file " << filename << " and result is " << ret << " Fail";
		}
	else
		{
		passCount++;
		logging << "No Error in creating store using the unicode file named config file " << filename << " and result is " << ret << " Pass";
		}
	
	stringstream testCaseType("SEC-TOOLS-CERTAPP-UNICODEFILE_CREATE");
	writeTMSResult(file, testCaseType, 0, ret==0, logging);
}

/**
Generate Certclient config files
*/
void GoodFileCertClientsTests(const stringstream &aFilename)
{
	FileCertClientConfigWriter filewriter(aFilename);
	const char *certclientPtr1; 
	const char *certclientPtr2;
	for(int i = 0; i<KAppUidIndex ; i++)
		{
		certclientPtr1 = goodAppNames[i];
		certclientPtr2 = gooduid_array[i];
		filewriter.WriteCertClientUid(certclientPtr2);
		filewriter.WriteCertClientName(certclientPtr1);
		}
}


/**
Generate good config files for cacerts
*/
void GoodFileCaCertTests(const stringstream &aFilename ,int aValue)
{
	FileCertStoreConfigWriter fileCertWriter(aFilename);
	const char **cacertPtr; 
	const char **label = filecert_array[0];
	switch(aValue)
	{
	case 0:
		{
		// Tests for GoodLabels
		for(label = filecert_array[aValue]; *label; ++label)
			{
		
			fileCertWriter.WriteFileEntry(*label);
			}
		break;
		}
	case 1:
		{
		// Tests for GoodDeletables
		for(cacertPtr = filecert_array[aValue]; *cacertPtr; ++cacertPtr)
			{
			fileCertWriter.WriteFileEntry(*label,*cacertPtr);
			label++;
			}
		break;
		}
	case 2:
		{
		//Tests for GoodFormats
		for(cacertPtr = filecert_array[aValue]; *cacertPtr; ++cacertPtr)
			{
			fileCertWriter.WriteFileEntry(*label,0,*cacertPtr);
			label++;
			}
		break;
		}
	case 3:
		{
		//Tests for GoodCertOwnerTypes
		for(cacertPtr = filecert_array[aValue]; *cacertPtr; ++cacertPtr)
			{
			fileCertWriter.WriteFileEntry(*label,0,0,*cacertPtr);
			label++;
			}
		break;
		}
	case 4:
		{
		//Tests for GoodSubjectKeyIds
		for(cacertPtr = filecert_array[aValue]; *cacertPtr; ++cacertPtr)
			{
			fileCertWriter.WriteFileEntry(*label,0,0,0,*cacertPtr);
			label++;
			}
		break;
		}
	case 5:
		{
		//Tests for GoodIssuerKeyIds
		for(cacertPtr = filecert_array[aValue]; *cacertPtr; ++cacertPtr)
			{
			fileCertWriter.WriteFileEntry(*label,0,0,0,0,*cacertPtr);
			label++;
			}
		break;
		}
	case 6:
		{
		//Tests for GoodApplications
		for(cacertPtr = filecert_array[aValue]; *cacertPtr; ++cacertPtr)
			{
			fileCertWriter.WriteFileEntry(*label,0,0,0,0,0,*cacertPtr);
			label++;
			}
		break;
		}
	case 7:
		{
		//Tests for GoodTrusteds
		for(cacertPtr = filecert_array[aValue]; *cacertPtr; ++cacertPtr)
			{
			fileCertWriter.WriteFileEntry(*label,0,0,0,0,0,0,*cacertPtr);
			label++;
			}
		break;
		}
	
	case 8:
		{	
		//Test for PEM support
		const char* datafilename = "\"root5ca.pem\"";
		fileCertWriter.WriteFileEntry(*label,0,0,0,0,0,0,0,datafilename);
		}
		break;
	
	default:
		break;
	}
}

/**
Generate  config file for swicert
*/
void GoodSwiCertTests(const stringstream &aFilename, int aIndex)
{
	SwiCertStoreConfigWriter swiWriter(aFilename);
	const char **swiPtr1, **swiPtr2; 
	swiPtr2 = swicert_array[0]; // labels for all scripts
	switch(aIndex)
	{
	case 0:
		{
		// Tests for GoodLabels
		for(swiPtr2=swicert_array[aIndex] ; *swiPtr2; ++swiPtr2)
			{
			swiWriter.WriteSwiEntry(*swiPtr2);
			}
		break;
		}
	case 1:
		{
		//Tests for GoodFormats
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	case 2:
		{
		//Tests for GoodCertOwnerTypes
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,0,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	case 3:
		{
		//Tests for GoodSubjectKeyIds
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,0,0,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	case 4:
		{
		//Tests for GoodIssuerKeyIds
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,0,0,0,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	case 5:
		{
		//Tests for GoodAswiPtr1lications
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,0,0,0,0,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	case 6:
		{
		//Tests for GoodTrusteds
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,0,0,0,0,0,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	case 7:
		{
		//Tests for GoodCapabilities
		swiPtr1 = swicert_array[aIndex]; 
		swiWriter.WriteSwiEntry(*swiPtr2,0,0,0,0,0,0,*swiPtr1);
		break;
		}
	case 8:
		{
		//Tests for sGoodMandatory
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,0,0,0,0,0,0,0,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	case 9:
		{
		//Tests for sGoodSystemUpgrade
		for(swiPtr1 = swicert_array[aIndex]; *swiPtr1; ++swiPtr1)
			{
			swiWriter.WriteSwiEntry(*swiPtr2,0,0,0,0,0,0,0,0,*swiPtr1);
			++swiPtr2;
			}
		break;
		}
	default:
		break;
	}
}

/**
Generate config  files with invalid values for the store
*/
void TestBadConfigFiles(const stringstream &aFilename, bool aBool, stringstream &aTestCaseType, int aTestCaseIndex)
{
	// create command line input for cacerts
	//Ex:certapp --hcertclients=t_certclients_0.txt --hfilecertstore=t_cacerts_0.txt --out --bfilecertstore=cacerts_x.dat
	stringstream strArg1, strArg2, strArg3;
	stringstream configFileName;
	configFileName <<aFilename.str() << ".txt";
	cout << aFilename.str() <<endl;
	if(aBool == 0) // file store
		{
		strArg1 <<"--hfilecertstore="<<aFilename.str() <<".txt";;//--hfilecertstore=t_cacerts_x.txt
		
		strArg2<< "--bfilecertstore=" << aFilename.str() <<".dat"; //--bfilecertstore=cacerts_x.dat
		cout <<"string value is " << strArg2.str().c_str() << endl;	
		}
	else  //swi store
		{
		strArg1 <<"--hswicertstore="<<aFilename.str() <<".txt";;//--hswicertstore=t_cacerts_x.txt
		
		strArg2<< "--bswicertstore=" << aFilename.str() <<".dat"; //--bswicertstore=cacerts_x.dat
		cout <<"string value is " << strArg2.str().c_str() << endl;	
		}

	strArg3<< "--hcertclients=SDP-Security_Tools-CertApp-CCDAT_0000.txt";
	cout <<"string value is " << strArg3.str().c_str() << endl;

	string argument1, argument2, argument3;
	const char *argv_certclient[4];
	
	argument1 = strArg1.str().c_str();
	argument3 = strArg3.str().c_str();

	argv_certclient[0] = argument3.c_str(); // --hcertclients=t_certclients_0.txt
	argv_certclient[1] = argument1.c_str(); // --hfilecertstore=t_cacerts_x.txt
	argv_certclient[2] = "--out";			//--out
	argument2 = strArg2.str().c_str();
	argv_certclient[3] = argument2.c_str(); //--bfilecertstore=cacerts_x.dat

	char *argument_ptr[4]; 
	int argCount4 = 4;
	char **argv;
	for(int n = 0; n<argCount4; n++)
		{
		argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
		argv = argument_ptr;   
		}
	RunCertAppToolForBadParams(argCount4, argv, configFileName, aTestCaseType, aTestCaseIndex); 
}

/**
Generate bad config files for cacerts
*/
void BadFileCaCertTests(const stringstream &aFilename ,int aValue, stringstream &aTestCaseType, int &aTestIndex)
{
	const char **caCertPtr; 
	int count = 0;
	switch(aValue)
	{
	case 0:
		{
		// Tests for BadLabels
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr ; ++caCertPtr )
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(*caCertPtr);
			TestBadConfigFiles(file1, 0, aTestCaseType, aTestIndex++);
			count++;
			}
		break;
		}
	case 1:
		{
		// Tests for BadDeletables
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr; ++caCertPtr)
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(0,*caCertPtr);
			TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 2:
		{
		//Tests for BadFormats
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr; ++caCertPtr)
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(0,0,*caCertPtr);
			TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 3:
		{
		//Tests for BadCertOwnerTypes
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr; ++caCertPtr)
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(0,0,0,*caCertPtr);
			TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 4:
		{
		//Tests for BadSubjectKeyIds
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr; ++caCertPtr)
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(0,0,0,0,*caCertPtr);
			TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 5:
		{
		//Tests for BadIssuerKeyIds
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr; ++caCertPtr)
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(0,0,0,0,0,*caCertPtr);
			TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 6:
		{
		//Tests for BadApplications
		caCertPtr = badfilecert_array[aValue];
		stringstream file1, file2;
		file1 << aFilename.str() <<"_"<<count;
		file2 << aFilename.str() <<"_"<<count <<".txt";
		FileBadCertStoreConfigWriter badcertfileWriter(file2);
		badcertfileWriter.WriteFileEntry(0,0,0,0,0,0,*caCertPtr);
		TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
		break;
		}
	case 7:
		{
		//Tests for BadTrusteds
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr; ++caCertPtr)
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(0,0,0,0,0,0,0,*caCertPtr);
			TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 8:
		{
		//Tests for BadDataFileName
		for(caCertPtr = badfilecert_array[aValue]; *caCertPtr; ++caCertPtr)
			{
			stringstream file1, file2;
			file1 << aFilename.str() <<"_"<<count;
			file2 << aFilename.str() <<"_"<<count <<".txt";
			FileBadCertStoreConfigWriter badcertfileWriter(file2);
			badcertfileWriter.WriteFileEntry(0,0,0,0,0,0,0,0,*caCertPtr);
			TestBadConfigFiles(file1,0,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	default:
		break;
	}
}


/**
Generate bad Certclient config files
*/
void BadFileCertClientsTests(const stringstream &aFilename, int aIndex, stringstream &aTestCaseType, int &aTestCaseIndex)
{
	FileBadCertClientConfigWriter certfileWriter(aFilename);
	const char *certfilePtr1, *certfilePtr2; 
	
	switch(aIndex)
	{
	case 0:
		{
		for(int i=0; i<KAppUidIndex; i++) // 4 badUids[] length
			{
			stringstream file1, file2;
			file2 << aFilename.str() << "_" << i ;
			file1 << aFilename.str() <<"_"<<i <<".txt";
			FileBadCertClientConfigWriter certfileWriter(file1);
			certfilePtr1 = goodcertclient_array[0]; // good app name and 
			certfilePtr2 = badUids[i];
			certfileWriter.WriteCertClientUid(certfilePtr2); // bad uid entry
			certfileWriter.WriteCertClientName(certfilePtr1);
			CertClientBadParamsFuncCall(file2, aTestCaseType, aTestCaseIndex++);
			}
		break;
		}

	case 1:
		{
		for(int i = 0; i<4 ; i++) // length of badcertclient_array
			{
			stringstream file1, file2;
			file2 << aFilename.str() << "_" << i ;
			file1 << aFilename.str() <<"_"<<i <<".txt";
			FileBadCertClientConfigWriter certfileWriter(file1);
			certfilePtr1 = badcertclient_array[i];
			certfilePtr2 = goodUids[0];
			certfileWriter.WriteCertClientUid(certfilePtr2);
			certfileWriter.WriteCertClientName(certfilePtr1);
			CertClientBadParamsFuncCall(file2, aTestCaseType, aTestCaseIndex++);
			}
		break;
		}
	
	// multiple names mapping to single uid
	case 2:
		{
		stringstream file1;
		file1 << aFilename.str() <<".txt";
		FileBadCertClientConfigWriter certfileWriter(file1);
		for(int i= 0; i<KAppUidIndex; i++)
			{
			certfilePtr1 = goodAppNames[i]; //app names - SW Install,SW install Ocsp Signing etc.
			certfilePtr2 = goodUids[0];
			certfileWriter.WriteCertClientName(certfilePtr1);
			certfileWriter.WriteCertClientUid(certfilePtr2);
			}
		CertClientBadParamsFuncCall(file1, aTestCaseType, aTestCaseIndex++);
		break;
		}
	
	default:
		break;
	}
}


void CertClientBadParamsFuncCall(stringstream &aFileName, stringstream &aTestCaseType, int aTestCaseIndex)
{
	//Ex:certapp --hcertclients=t_certclients_0.txt --out --bcertclients=t_certclients_0.dat 
	stringstream strArg1, strArg2, strArg3;
	strArg1 <<"--hcertclients=" << aFileName.str() << ".txt";   
	
	strArg2<< "--bcertclients=" << aFileName.str() << ".dat"; 
	cout <<"string value is " << strArg2.str().c_str() << endl;	
	
	string argument1, argument2;
	const char *argv_certclient[3];
	
	argument1= strArg1.str().c_str();
	argument2= strArg2.str().c_str();

	argv_certclient[0] = argument1.c_str(); // --hcertclients=tbad_certclients_x.txt
	argv_certclient[1] = "--out";			//--out
	argv_certclient[2] = argument2.c_str(); //--bcertclients=t_certclients_x.dat

	char *argument_ptr[3]; 
	int argCount3 = 3;
	char **argv;
	for(int n = 0; n<argCount3; n++)
		{
		argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
		argv = argument_ptr;   
		}
	RunCertAppToolForBadParams(argCount3, argv, aFileName, aTestCaseType, aTestCaseIndex); 
}


/**
Generate  config file for swicert
*/
void BadSwiCertTests(const stringstream &aFileName, int aIndex, stringstream &aTestCaseType, int &aTestIndex)
{
	const char **swicertPtr1; 
	int count = 0 ;
	switch(aIndex)
	{
	case 0:
		{
		// Tests for BadLabels
		for(swicertPtr1= badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}

	case 1:
		{
		//Tests for BadFormats
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 2:
		{
		//Tests for BadCertOwnerTypes
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 3:
		{
		//Tests for BadSubjectKeyIds
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;	
			}
		break;
		}
	case 4:
		{
		//Tests for BadIssuerKeyIds
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 5:
		{
		//Tests for BadApplications
		swicertPtr1 = badswicert_array[aIndex];
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 6:
		{
		//Tests for BadTrusteds
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 7:
		{
		//Tests for BadDataFileName
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,0,0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 8:
		{
		//Tests for BadCapabilities
		swicertPtr1 = badswicert_array[aIndex];
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,0,0,0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 9:
		{
		//Tests for BadMandatory
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,0,0,0,0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	case 10:
		{
		//Tests for BadSystemUpgrade
		for(swicertPtr1 = badswicert_array[aIndex]; *swicertPtr1; ++swicertPtr1)
			{
			stringstream file1, file2;
			file1 << aFileName.str() <<"_"<<count;
			file2 << aFileName.str() <<"_"<<count <<".txt";
			SwiBadCertStoreConfigWriter badswiWriter(file2);
			badswiWriter.WriteSwiEntry(0,0,0,0,0,0,0,0,0,0,*swicertPtr1);
			TestBadConfigFiles(file1,1,aTestCaseType,aTestIndex++);
			count++;
			}
		break;
		}
	default:
		break;
	}
}

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-EMPTYFILE_CREATE-0000
  @SYMTestCaseDesc	Test the tool with empty configuration file as input
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-EMPTYFILE_CREATE-0001
  @SYMTestCaseDesc	Test the tool with empty configuration file as input
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-EMPTYFILE_CREATE-0002
  @SYMTestCaseDesc	Test the tool with empty configuration file as input
*/	
/**
Test cases to test the tool with empty configuration files as input for all the stores
*/
void TestsWithEmptyConfigFile()
{
	stringstream testCaseType("SEC-TOOLS-CERTAPP-EMPTYFILE_CREATE");
	for(int i = 0 ; i<3 ; i++)
		{
		stringstream emptyConfigFileName, configFileName1, configFileName2;
		emptyConfigFileName<< "SDP-Security_Tools-CertApp-EmptyFile_000"<< i <<".txt";
		switch(i)
			{
			case 0:
				{
				configFileName1 << "--hcertclients=" << emptyConfigFileName.str();
				configFileName2 << "--bcertclients=SDP-Security_Tools-CertApp-EmptyFile_0000"<<".dat";
				FileCertClientConfigWriter certclient(emptyConfigFileName);
				}
			break;

			case 1:
				{
				configFileName1 << "--hfilecertstore=" << emptyConfigFileName.str();
				configFileName2 << "--bfilecertstore=SDP-Security_Tools-CertApp-EmptyFile_0001"<<".dat";
				FileCertStoreConfigWriter fileWriter(emptyConfigFileName);
				}
			break;

			case 2:
				{
				configFileName1 << "--hswicertstore=" << emptyConfigFileName.str();
				configFileName2 << "--bswicertstore=SDP-Security_Tools-CertApp-EmptyFile_0002"<<".dat";
				SwiCertStoreConfigWriter swiFileWriter(emptyConfigFileName);
				}
			break;

			default:
				break;
			}

		string argument1, argument2;
		argument1  = configFileName1.str().c_str();
		argument2  = configFileName2.str().c_str();
		const char *argv_certclient[4];
		argv_certclient[0] = "--hcertclients=SDP-Security_Tools-CertApp-CCDAT_0000.txt";
		argv_certclient[1] = argument1.c_str();  //"--hfilecertstore=t_emptyConfig.txt";
		argv_certclient[2] = "--out";
		argv_certclient[3] = argument2.c_str();

		int argcount = 4;
		char *argument_ptr[4]; 
		char **argv;
		for(int n = 0; n<argcount; n++)
			{
			argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
			argv = argument_ptr;
			}
		for(int z=0; z<argcount; z++)
			{
			cout << "Testing with empty configuration file" << argv[z] <<endl;
			}

		RunCertAppTool(argcount, argv, emptyConfigFileName, testCaseType, i);
		}
}

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEFORMATS_CREATE-0000
  @SYMTestCaseDesc	Test the certapp tool for different file encode formats
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEFORMATS_CREATE-0001
  @SYMTestCaseDesc	Test the certapp tool for different file encode formats
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEFORMATS_CREATE-0002
  @SYMTestCaseDesc	Test the certapp tool for different file encode formats
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-FILEFORMATS_CREATE-0003
  @SYMTestCaseDesc	Test the certapp tool for different file encode formats
*/	
/**
Code to test the certapp tool for different file encode formats
*/
void TestsWithEncodedFileFormats()
{
	stringstream testCaseType("SEC-TOOLS-CERTAPP-FILEFORMATS_CREATE");
	bool hasResult;

	//Provide files with different encoded formats
	for(int i = 0; i<4; i++)
		{
		stringstream unicodeName, outputFileName, strArg3;
		const char *argv_certclient[KIndex];
		unicodeName<< fileEncodeType[i];
		outputFileName <<"SDP-Security_Tools-CertApp-FileFormats-000" <<i;
		strArg3 << "--bfilecertstore="<< "SDP-Security_Tools-CertApp-FileFormats-000" <<i<<".dat";
		
		string argument;
		argument = strArg3.str().c_str();
		argv_certclient[0] = "--hcertclients=SDP-Security_Tools-CertApp-CCDAT_0000.txt";
		argv_certclient[1] = fileEncodeType[i];   // manually created unicode content files
		argv_certclient[2] = "--out";
		argv_certclient[3] = argument.c_str();

		for(int m=0; m<KIndex; m++)
			{
			cout << "Value in argv_certclient " << m << "is " <<argv_certclient[m] <<endl;
			}
		 char *argument_ptr[4]; 
		char **argv;
		for(int n = 0; n<KIndex; n++)
			{
			argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
			argv = argument_ptr;
			}
		
		for(int z=0; z<KIndex; z++)
			{
			cout << "Argument array contains : " << argv[z] <<endl;
			}
		
		int ret = RunCertApp(progress, errors, KIndex, argv); 
		cout << "Return value from RunCertApp tool is "  << ret << endl;
		
		hasResult = false;
		stringstream logging;
		if(ret!= 0 && ((strcmp(unicodeName.str().c_str(),"--hfilecertstore=ucs2-bigendian.txt")==0) || (strcmp(unicodeName.str().c_str(),"--hfilecertstore=ucs2-littleendian.txt")==0)))
			{
			passCount++;
			logging << "Tool doesnt support for UCS encoded file-"<< outputFileName.str() <<" so the result is " << ret << " Pass";
			hasResult = true;
			}
		else if(ret== 0 && ((strcmp(unicodeName.str().c_str(),"--hfilecertstore=utf8.txt") ==0) || (strcmp(unicodeName.str().c_str(),"--hfilecertstore=utf8_without_bom.txt")==0)))
			{
			passCount++;
			logging << "No Error in creating store for UTF8 encoded file-"<<outputFileName.str()<< " and result is " << ret << " Pass";
			hasResult = true;
			}
		
		if (hasResult)
			{
			writeTMSResult(file, testCaseType, i, true, logging);
			}
		} //End of encoded file formats testing
}

/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-AGGRCERTSTORE_CREATE-0000
  @SYMTestCaseDesc	Test case for REQ-10314 (Reviewing Aggregate Certificate DataBase)
*/	
/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-AGGRCERTSTORE_CREATE-0001
  @SYMTestCaseDesc	Test case for REQ-10314 (Reviewing Aggregate Certificate DataBase)
*/	
/**
Test case for REQ-10314 (Reviewing Aggregate Certificate DataBase)
*/
void TestToReviewingAggregateStore()
{
	stringstream testCaseType("SEC-TOOLS-CERTAPP-AGGRCERTSTORE_CREATE");

	for(int index = 0; index<2; index++)
		{
		stringstream file1, file2;
		const char *argv_certclient[4];
		argv_certclient[2] = "--out";
		switch(index)
			{
			case 0: // different certificate labels in two stores
				{
				argv_certclient[0] = "--bfilecertstore=teststore1.dat";   // these are 2 binary stores saved in perforce
				argv_certclient[1] = "--bfilecertstore=teststore3.dat"; 
				argv_certclient[3] = "--hfilecertstore=SDP-Security_Tools-CertApp-AggrCertStore-0000.txt";
				file1 <<"SDP-Security_Tools-CertApp-AggrCertStore-0000.txt";
				file2 <<"aggregatestorecontents1_3.txt";
				}
				break;

			case 1: //duplicate certificate labels in two stores
				{
				argv_certclient[0] = "--bfilecertstore=teststore2.dat";   // these are 2 binary stores saved in perforce
				argv_certclient[1] = "--bfilecertstore=teststore3.dat"; 
				argv_certclient[3] = "--hfilecertstore=SDP-Security_Tools-CertApp-AggrCertStore-0001.txt";
				file1 <<"SDP-Security_Tools-CertApp-AggrCertStore-0001.txt";
				file2 <<"aggregatestorecontents2_3.txt";
				}
				break;

			default:
				break;
			}

		for(int m=0; m<KIndex; m++)
			{
			cout << "Value in argv_certclient " << m << "is " <<argv_certclient[m] <<endl;
			}
		char *argument_ptr[KIndex]; 
		char **argv;
		for(int n = 0; n<KIndex; n++)
			{
			argument_ptr[n] = const_cast<char*>(argv_certclient[n]);
			argv = argument_ptr;
			}
		
		int ret = RunCertApp(progress, errors, KIndex, argv); 
		stringstream logging;
		if(!ret)
			{
			passCount++;
			int compareValue  = CompareFiles(file1,file2); // compare with the reference files in perforce
			if (compareValue)
				{
				logging << "Failed to successfully create Aggregate Database for " << file1.str() << " is Fail";
				
				}
			else
				{
				logging << "Successfully created Aggregate Database(text form) for " <<file1.str() << " result is Pass";
				}
			}
		else
			{
			failCount++;
			logging << "Failed to dump the stores to single text format for " <<file1.str();
			}
		
		writeTMSResult(file, testCaseType, index, !ret, logging);		
		}
}


/**
  @SYMTestCaseID	SEC-TOOLS-CERTAPP-DECODEENCODE_COMPARE-0000
  @SYMTestCaseDesc	Encode-decode test for certclient store
*/	
/**
TestCase to test the req of 10313- decode/encode certstore
Encode-decode test for certclient store
*/
void TestToDecodeAndEncodeStore()
{
	stringstream testCaseType("SEC-TOOLS-CERTAPP-DECODEENCODE_COMPARE");

	stringstream outputFileName, strArg1, strArg2;
	outputFileName <<"SDP-Security_Tools-CertApp-DecodeEncode-0000";
	strArg1 << "decodeoutput1.txt";
	strArg2 << "decodeoutput2.txt";
	
	const char *argv_certclient[3]; //dump store to text
	argv_certclient[0] = "--bcertclients=certclients_0.dat";
	argv_certclient[1] = "--out";
	argv_certclient[2] = "--hcertclients=decodeoutput1.txt";   
	decodeEncodeCertAppCall(argv_certclient);

	//create store with the decoded text as input
	argv_certclient[0] = "--hcertclients=decodeoutput1.txt";
	argv_certclient[2] = "--bcertclients=decodeoutput.dat";   
	decodeEncodeCertAppCall(argv_certclient);

	//take the encoded store and dump it back to text form
	argv_certclient[0] = "--bcertclients=decodeoutput.dat";
	argv_certclient[2] = "--hcertclients=decodeoutput2.txt"; 
	decodeEncodeCertAppCall(argv_certclient);

	int compVal = CompareFiles(strArg1, strArg2);
	stringstream logging;
	cout << "Return value of comparision is " << compVal <<endl;
	if (compVal)
		{
		failCount++;
		logging << "Encode-Decode fail for " <<outputFileName.str() << " result is Fail";
		}
	else
		{
		passCount++;
		logging << "Encode-Decode Pass for " <<outputFileName.str() << " result is Pass";
		}
	
	writeTMSResult(file, testCaseType, 0, compVal==0, logging);			
}

void decodeEncodeCertAppCall(const char *aArray[])
{
	char *argument_ptr[3]; 
	char **argv;
	for(int m=0; m<3; m++)
		{
		cout << "Value in argv_certclient " << m << "is " <<aArray[m] <<endl;
		}
	for(int n = 0; n<3; n++)
		{
		argument_ptr[n] = const_cast<char*>(aArray[n]);
		argv = argument_ptr;
		}
	RunCertApp(progress, errors ,3, argv);
}


int CompareFiles(stringstream &aFile1, stringstream &aFile2)
{
	ifstream inFile1, inFile2;
	string name1 = aFile1.str();
	string name2 = aFile2.str();
	inFile1.open(name1.c_str(),  ifstream::in);
	if (inFile1.fail())
		{
		cout << "Failed to open the file! May be not existing" << endl;
		return -1;
		}
	inFile2.open(name1.c_str(),  ifstream::in);
	if (inFile2.fail()) 
		{
	    cout << "Failed to open the file! May be not existing" << endl;
	    return -1;
		}
	return Compare(inFile1, inFile2);
}


int Compare(istream &aIstream1, istream &aIstream2)
{
  char ch1, ch2;
  while (aIstream1.get(ch1)) 
  {
    if (!aIstream2.get(ch2) || (ch1 != ch2))
	{
      cout << "files are not equal";
	  return -1;
    }
  }
  if (!aIstream2.get(ch2))  // no more chars to read fom file
	{
	  cout << "files are equal\n";
	  return 0;
	}
  else 
	{
	  cout << "files are not equal";
	  return -1;
	}
}


/**
Generate script files for the filecertstore emulator tests
*/
void GenerateGoodCertClientTestScripts(const stringstream &aFilename, const stringstream &aTestCaseType, int &aTestIndex)
{
	CertClientsStoreScriptGeneration cert_clients(aFilename);

	cert_clients.WriteInitialiseCertClient(aTestCaseType, aTestIndex);
	cert_clients.WriteGetCount(KAppUidIndex, aTestCaseType, aTestIndex);
	cert_clients.WriteGetApplicationsList(aTestCaseType, aTestIndex);
	
	for(int i = 0; i<KAppUidIndex;i++)
		{
		const char *goodlabel = goodcertclient_array[i]; 
		const char *uid = gooddecimalUid_array[i];
		cert_clients.WriteGetAppWithUid(goodlabel, uid, aTestCaseType, aTestIndex);
		}
	cert_clients.WriteDestroyManager(aTestCaseType, aTestIndex);
}

/**
Generates config files for cacerts
*/
void GenerateGoodCaCertsTestScripts(const stringstream &aFilename, const stringstream &aTestCaseType, int &aTestIndex)
{
	FileStoreScriptGeneration fileStore(aFilename);

	const char *mode = "write"; 
	const char *ownertype = goodOwnerType[0]; //CA ownerType

	fileStore.WriteInitialiseCert(mode, aTestCaseType, aTestIndex);
	fileStore.WriteListcert(ownertype, aTestCaseType, aTestIndex);
	
	for(int i = 0; i<6;i++)
		{
		const char *trust = goodTrust[0]; // true
		const char *goodlabel = goodEmuCert_array[i]; // list of label - root5ca , symbiandtestcsa, symbiantestrcsa

		fileStore.WriteGetCertificateDetails(goodlabel, aTestCaseType, aTestIndex);
		fileStore.WriteGetTrust(goodlabel, trust, aTestCaseType, aTestIndex);
		fileStore.WriteGetApplications(goodlabel, aTestCaseType, aTestIndex);
		fileStore.WriteRetrieveCerts(goodlabel, aTestCaseType, aTestIndex);
		}
}

/**
Generate script files for the swicertstore emulator tests
*/
void GenerateGoodSwiCertTestScripts(const stringstream &aFilename, const stringstream &aTestCaseType, int &aTestIndex)
{
	SWIStoreScriptGeneration swiStore(aFilename);
	
	const char *ownertype = goodOwnerType[0]; //CA ownerType
	
	//initilaise the store and get list all the certificate ones
	swiStore.WriteInitialiseCert(aTestCaseType, aTestIndex);
	swiStore.WriteListcert(ownertype, aTestCaseType, aTestIndex);

	for(int i = 0; i<6;i++)
		{
		const char *trust = goodTrust[0]; // true
		const char *goodlabel = goodSwiCert_array[i]; // list of label - root5ca , symbiandtestcsa, symbiantestrcsa
		const char *mandatoryVal = mandatory[0];
		const char *systemupgrade = systemUpgrade[0];

		swiStore.WriteGetSystemUpgrade(goodlabel, systemupgrade, aTestCaseType, aTestIndex);
		swiStore.WriteRetrieveCerts(goodlabel, aTestCaseType, aTestIndex);
		swiStore.WriteGetApplications(goodlabel, aTestCaseType, aTestIndex);
		swiStore.WriteGetTrust(goodlabel, trust, aTestCaseType, aTestIndex);
		swiStore.WriteGetCapabilities(goodlabel, aTestCaseType, aTestIndex);
		swiStore.WriteGetMandatoryFlag(goodlabel, mandatoryVal, aTestCaseType, aTestIndex);
		}
}

//End of file


