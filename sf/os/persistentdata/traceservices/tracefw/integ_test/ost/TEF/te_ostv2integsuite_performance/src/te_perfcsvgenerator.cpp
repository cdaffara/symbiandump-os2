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
// UTrace Performance Tests CSV output generator.
//



/**
 @file te_perfcsvgenerator.cpp
 @internalTechnology
 @prototype
*/

#include "te_perfcsvgenerator.h"
#include "te_ostv2integsuite_defs.h"
#include "te_perfapinames.h"


//Punctuation symbols required for generating csv files.


CUptCsvGenerator::CUptCsvGenerator()
	{
	/*Constructor*/
	}

CUptCsvGenerator::~CUptCsvGenerator()
	{
	/*Destructor*/
	}

/** This user-side method uses the RFs and RFile Session methods to open a file specified by the user, 
if the file already exists, it will be appended or overwritten as per the user's specification.
The file information is stored in the private data member iCSVFile.
@param aFileName the constant descriptor containing the filepath as specified by the user.
@param aAppend is the TBool controlling whether files should be appended to or overwritten
				ETrue=Append, EFalse=Overwrite
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUptCsvGenerator::OpenL(const TDesC& aFileName, const TBool& aAppend)
	{

	iCsvFileSession.Connect();
	//if the user has set for the file to be overwritten if it already exists
	//then this part with ensure this is carried out with write permissions
	if(aAppend==EFalse)
		{
		User::LeaveIfError(iCsvFile.Replace(iCsvFileSession, aFileName, EFileWrite));	
		}
	//otherwise, the file is opened and appended to if it already exists
	//and if it doesnt exist, the file is created with write perimission with the user defined filename.
	else
		if(iCsvFile.Open(iCsvFileSession, aFileName ,EFileWrite)!=KErrNone)
			{
			User::LeaveIfError(iCsvFile.Create(iCsvFileSession, aFileName, EFileWrite));			
			}
			
	//NB!!!! may need to put in check if file is already open if this is going to be run on emulator 
	//(not that that matters for performance tests). Not an issue on device.
	return KErrNone;

	}

/** This user-side method uses the RFs and RFile Session methods to close the file specified by the user in the OpenL()
method above and described by the private data member iCSVFile.
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUptCsvGenerator::Close()
	{
	//close the csv file
	iCsvFile.Close();
	iCsvFileSession.Close();	
	
	return KErrNone;
	}
	
/** This user-side method uses the RFile Session methods to find the end of the file described by iCSVFile and 
appends a newline character.
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUptCsvGenerator::WriteNewLineL() 
	{
	TInt filesize;
	
	// find end of this file, and append the newline data field from here.
	iCsvFile.Size(filesize);
	iCsvFile.Seek(ESeekStart, filesize);
	User::LeaveIfError(iCsvFile.Write(KCsvNewLine));
	
	return KErrNone;				
	}

/** This user-side method uses the RFile Session methods to find the end of the file described by iCSVFile.
It then appends performance data metrics by element from the passed array, with each element separated by a comma character.
@param aPerformanceData is the constant array of performance data stored as TInt32
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
void CUptCsvGenerator::WriteL(const RArray<TInt64>& aPerformanceData)
	{
	
	// find end of this file, and append the passed data from here.
	TInt filesize;
	iCsvFile.Size(filesize);
	iCsvFile.Seek(ESeekStart, filesize);
	
	RBuf8 buf; 
	CleanupClosePushL(buf);
	//create a buf large enough to contain the passed data and comma separators
	TInt numbytes = 2*aPerformanceData.Count()*sizeof(TInt64);
	buf.CreateL(numbytes);
	
	//for the number of elements in the passed array:- append each element, separated by a comma, to the buffer
	for(TInt i=0; i!=aPerformanceData.Count(); i++) //could replace aPerformance.Count() with structure paramter Parameter.Count
		{
		buf.AppendNum(aPerformanceData[i]);
		//may reimplement this
		if(i!=aPerformanceData.Count()) 
			buf.Append(KCsvComma);  

		}
	
	//write the buffer to the given file	
	User::LeaveIfError(iCsvFile.Write(buf));
	
	//close and cleanup the heap objects
	buf.Close();
	CleanupStack::PopAndDestroy(&buf);

	}

/** This user-side method uses the RFile Session methods to find the end of the file described by iCSVFile.
It then appends performance data metrics by element from the passed array, with each element separated by a comma character.
@param aPerformanceData is the constant array of performance data stored as TDesC
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
void CUptCsvGenerator::WriteL(const RArray<TPtrC8>& aPerformanceConfig)
	{
	// find end of this file, and append the passed data from here.
	TInt filesize;
	iCsvFile.Size(filesize);
	iCsvFile.Seek(ESeekStart, filesize);
	
	RBuf8 buf; 
	CleanupClosePushL(buf);
	//create a buf large enough to contain the passed data and comma separators
	TInt numbytes = 5*aPerformanceConfig.Count()*sizeof(TPtrC8);
	buf.CreateL(numbytes);
		
	//for the number of elements in the passed array:- append each element, separated by a comma, to the buffer
	for(TInt i=0; i!=aPerformanceConfig.Count(); i++) //could replace aPerformance.Count() with structure paramter Parameter.Count
		{
		buf.Append(aPerformanceConfig[i]);
		//may reimplement this
		if(i!=aPerformanceConfig.Count()) 	
			buf.Append(KCsvComma);  

		}
	//write the buffer to the given file	
	User::LeaveIfError(iCsvFile.Write(buf));
	
	
	//close and cleanup the heap objects
	buf.Close();
	CleanupStack::PopAndDestroy(&buf);

	}


void CUptCsvGenerator::WriteApiNameL(const TInt aApiEnum)
	{
	// find end of this file, and append the passed data from here.
	TInt filesize;
	iCsvFile.Size(filesize);
	iCsvFile.Seek(ESeekStart, filesize);
		
	RBuf8 buf; 
	CleanupClosePushL(buf);
	//create a buf large enough to contain the passed data and comma separators
	TInt numbytes = 1024;//2*sizeof(TPtrC8);
	buf.CreateL(numbytes);
		
	//read the APIenum (which is the same as the first element of aPerformanceData)
	//use this enum value to write the name of the API being tested
	TApiNames getApiName;
	buf.Append(getApiName.GetApiIdString(aApiEnum));
	buf.Append(KCsvComma); 
	
	//write the buffer to the given file	
	User::LeaveIfError(iCsvFile.Write(buf));
		
		
	//close and cleanup the heap objects
	buf.Close();
	CleanupStack::PopAndDestroy(&buf);
		
	}

/** This user-side method writes the column titles (from the KLit string defined in the relevent test.cpp
    to the user-specified csv file, according to the parameters for aTestType.
@param aTestType is the enum identifier for the test in question
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */	
TInt CUptCsvGenerator::WriteHeaderL(const TInt& aTestType)
	{
	RBuf8 buf; 
	CleanupClosePushL(buf);
	
	TInt numbytes = 0;

	//test which testtype is to be written and
	//append the header string to the buffer and write it to the csv file
	//create general KGeneralHeader? 
	//first identify which test
	switch(aTestType)
	{
	case 0:
		//KGeneralHeader=KApiCallHeader;
		//now create a buf large enough to contain the passed data and comma separators
		numbytes = sizeof(KApiCallHeader); 
		buf.CreateL(numbytes);
	
		//append the relevant header to the file
		buf.Append(KApiCallHeader);
	break;
	case 1:
	//now create a buf large enough to contain the passed data and comma separators
		numbytes = sizeof(KOutputPluginHeader); 
		buf.CreateL(numbytes);

		//append the relevant header to the file
		buf.Append(KOutputPluginHeader);
	break;
	case 2:
	//now create a buf large enough to contain the passed data and comma separators
		numbytes = sizeof(KOutputPluginHeader); 
		buf.CreateL(numbytes);

		//append the relevant header to the file
		buf.Append(KOutputPluginHeader);
	break;
	//case 3://to be implemented on next iteration
	//break;
	//case 4://to be implemented on next iteration
	//break;
	default:
		// NB: change this to an empty string
		//now create a buf large enough to contain the passed data and comma separators
		numbytes = sizeof(KApiCallHeader); 
		buf.CreateL(numbytes);

		//append the relevant header to the file
		buf.Append(KApiCallHeader);
	break;
	
	}
	
	//now create a buf large enough to contain the passed data and comma separators
	//numbytes = sizeof(KGeneralHeader); 
	//buf.CreateL(numbytes);
	
	//append the relevant header to the file
	//buf.Append(KGeneralHeader);		

	
	//append a newline to the buffer in preparation for the data
	buf.Append(KCsvNewLine);
	TInt startpos = 0;
	//go to the start of the file and write the buffer 
	iCsvFile.Seek(ESeekStart, startpos); //nb: put in check incase this overwrites data when appending to a file??
	User::LeaveIfError(iCsvFile.Write(buf));

	//close and cleanup the heap objects
	buf.Close();
	CleanupStack::PopAndDestroy(&buf);
	
	return KErrNone;
	}

/** This sanity test method is executed at the start of the test run to verify that UPT methods in
this class are stable before any of the performance tests are carried out

// what tests does it do?
 
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */
TInt CUptCsvGenerator::TestL()
	{
	//define filepaths for the test csv files according to the test platform.
#ifdef __WINSCW__
	_LIT(KTestFileAppend, "c:\\te_CSVoutputfileAppend.csv");
	_LIT(KTestFileOverwrite, "c:\\te_CSVoutputfileOverwrite.csv");
#else
	_LIT(KTestFileAppend, "e:\\te_CSVoutputfileAppend.csv");
	_LIT(KTestFileOverwrite, "e:\\te_CSVoutputfileOverwrite.csv");
#endif	

	//initialise some generic data to write to csv
	RArray<TInt64> atestdata1;
	RArray<TInt64> atestdata2;
	CleanupClosePushL(atestdata1); 
	CleanupClosePushL(atestdata2); 
		
	//data of the form:
	//0	1	2	3	4	5	6	7	8	9
	TInt data1element=10;
	for(TInt i=0; i!=data1element;i++)
		{
		atestdata1.Append((TInt64) i);
		}		
	//data of the form:
	//0	1000	2000	3000	4000	5000	6000	7000	8000	9000	10000	11000
	TInt data2element=12;
	for(TInt i=0; i!=data2element;i++)
		{
		atestdata2.Append((TInt64) i*1000);
		}
	
	
	//now test the CSV Generator functions
	//test the append data option - outputfile should contain an extra 6 lines of data of the form:
	// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	// 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000
	// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	// 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000
	// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	// 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000
	TInt appendcount=3;
	for(TInt i=0; i!=appendcount;i++)
		{ 
		OpenL(KTestFileAppend, ETrue);
		WriteL(atestdata1);
		WriteNewLineL();
		WriteL(atestdata2);
		WriteNewLineL();
		Close();	
		}

		
	//test the overwrite data option - outputfile should contain only 2 lines of data of the form:
	// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	// 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000
	for(TInt i=0; i!=appendcount;i++)
		{
		OpenL(KTestFileOverwrite, EFalse);
		WriteL(atestdata1);
		WriteNewLineL();
		WriteL(atestdata2);
		WriteNewLineL();
		Close();	
		}

	// read the files back to check they are valid, as an automated check
	
	RFs fsSession;
	RFile appendfile;
	RFile overwritefile;
	TInt filesize;
	User::LeaveIfError(fsSession.Connect());
	
	//sizes in bytes of generated output csv data
	TInt appendsize=appendcount*(sizeof(atestdata1)+sizeof(atestdata2)+2*sizeof(KCsvNewLine)+(data1element+data2element)*sizeof(KCsvComma));
	TInt overwritesize=(sizeof(atestdata1)+sizeof(atestdata2)+2*sizeof(KCsvNewLine)+(data1element+data2element)*sizeof(KCsvComma));
	
	// fill buffers used for data read from the outputted file
	RBuf8 readappendfile;
	readappendfile.CreateL(appendsize);
	RBuf8 readoverwritefile;
	readoverwritefile.CreateL(overwritesize);
	CleanupClosePushL(readappendfile); 
	CleanupClosePushL(readoverwritefile); 

	// comparison data buffers used to contain the expected data
	RBuf8 acompareappend;
	acompareappend.CreateL(appendsize);	
	RBuf8 acompareoverwrite;
	acompareoverwrite.CreateL(overwritesize);
	CleanupClosePushL(acompareappend); 
	CleanupClosePushL(acompareoverwrite); 
		
	//fill comparison array for appended data
	TInt err=FillReferenceBuffer(acompareappend, appendcount, atestdata1, data1element, atestdata2, data2element);

	//first check the appended file by reading back the last 6 lines in the file and comparing with acompareappend
	if(err==KErrNone)
		{
		if(appendfile.Open(fsSession, KTestFileAppend, EFileRead))
			{
			if(appendfile.Size(filesize))	
				{
				if(appendfile.Read((filesize-sizeof(acompareappend)),readappendfile))
					{
					if(!readappendfile.Compare(acompareappend))
						err=KErrGeneral;
					}
				}
			}
		}
	// close test output csv file
	appendfile.Close();
	
	
	//given the above has passed,
	//fill comparison array for overwritten data			
	if(err==KErrNone)
		err=FillReferenceBuffer(acompareoverwrite, (TInt) 0, atestdata1, data1element, atestdata2, data2element);
	
	//check the overwritten file by reading back the only 2 lines in the file and comparing with acompareoverwrite
	//Note: as a thorough check- read from a zero offset
	if(err==KErrNone)
		{
		if(overwritefile.Open(fsSession, KTestFileOverwrite, EFileRead))
			{
			if(overwritefile.Size(filesize))	
				{
				if(overwritefile.Read(0,readoverwritefile))
					{
					if(!readoverwritefile.Compare(acompareoverwrite))
						err=KErrGeneral;
					}
				}
			}
		}
	// close test output csv file
	overwritefile.Close();
	CleanupStack::PopAndDestroy(&atestdata1); 
	CleanupStack::PopAndDestroy(&atestdata2); 
	CleanupStack::PopAndDestroy(&readappendfile); 
	CleanupStack::PopAndDestroy(&readoverwritefile); 
	CleanupStack::PopAndDestroy(&acompareappend); 
	CleanupStack::PopAndDestroy(&acompareoverwrite); 
	return err;
	}
	
	
TInt CUptCsvGenerator::FillReferenceBuffer(RBuf8& aBufferName, const TInt& aAppendCount, const RArray<TInt64>& aData1Name, const TInt& aData1Count, const RArray<TInt64>& aData2Name, const TInt& aData2Count)
	{
	for(TInt i=0; i!=aAppendCount;i++)
		{
		// first line of data
		for(TInt j=0;j!=aData1Count;j++)
			{
			aBufferName.AppendNum(aData1Name[j]);
			//if(j!=aData1Count-1)
			if(j!=aData1Count)
				aBufferName.Append(KCsvComma);
			}	
			
		// newline
		aBufferName.Append(KCsvNewLine);
		
		// second line of data with a potentially different number of elements
		for(TInt j=0;j!=aData2Count;j++)
			{
			aBufferName.AppendNum(aData2Name[j]);
			//if(j!=aData2Count-1)
			if(j!=aData2Count)
				aBufferName.Append(KCsvComma);
			}	
		aBufferName.Append(KCsvNewLine);	
		}
	return 0;
	}

// eof


