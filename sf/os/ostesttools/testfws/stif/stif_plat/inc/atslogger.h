/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This file contains the header file of the CAtsLogger 
* class.
*
*/

#ifndef __ATSLOGGER_H__
#define __ATSLOGGER_H__

//  INCLUDES
#include <f32file.h>

// CONSTANTS
_LIT(KAtsLogger, "AtsLogger");
_LIT(KAtsLoggerNa, "NA");
_LIT(KAtsLoggerPassed, "PASSED");
_LIT(KAtsLoggerFailed, "FAILED");
_LIT(KAtsLoggerSkipped, "SKIPPED");
_LIT(KAtsLoggerEmpty, "");
_LIT(KAtsLoggerSemaphore, "AtsLogger");
_LIT(KAtsLoggerXmlDirectory, "\\SPD_LOGS\\XML\\");
_LIT(KAtsLoggerRawDirectory, "\\SPD_LOGS\\RAW\\");
_LIT(KAtsLoggerXmlSuffix, ".xml");
_LIT(KAtsLoggerRawSuffix, ".log");

_LIT(KAtsLoggerTagOpen, "<");
_LIT(KAtsLoggerSpace, " ");
_LIT(KAtsLoggerAttr, "=\"");
_LIT(KAtsLoggerAttrClose, "\">");
_LIT(KAtsLoggerTagPost, ">");
_LIT(KAtsLoggerTagClose, "</");
_LIT(KAtsLoggerLf, "\n");
_LIT(KAtsLoggerIndent, "  ");
_LIT8(KAtsLoggerIndent8, "  ");
_LIT(KAtsLoggerCommentOpen, "<!--");
_LIT(KAtsLoggerCommentClose, "-->");
_LIT(KAtsLoggerQuot, "&quot;");
_LIT(KAtsLoggerAmp, "&amp;");
_LIT(KAtsLoggerApos, "&apos;");
_LIT(KAtsLoggerLt, "&lt;");
_LIT(KAtsLoggerGt, "&gt;");

_LIT8(KAtsLoggerQuot8, "&quot;");
_LIT8(KAtsLoggerAmp8, "&amp;");
_LIT8(KAtsLoggerApos8, "&apos;");
_LIT8(KAtsLoggerLt8, "&lt;");
_LIT8(KAtsLoggerGt8, "&gt;");
_LIT8(KAtsLoggerEmpty8, "");

_LIT(KAtsLoggerTagTestReport, "test-report");
_LIT(KAtsLoggerTagTestSet, "test-batch");
_LIT(KAtsLoggerTagTestCase, "test-case");
_LIT(KAtsLoggerTagTestSetInit, "batch-init");
_LIT(KAtsLoggerTagDescription, "description");
_LIT(KAtsLoggerTagDate, "date");
_LIT(KAtsLoggerTagFactory, "factory");
_LIT(KAtsLoggerTagComponent, "component");
_LIT(KAtsLoggerTagName, "name");
_LIT(KAtsLoggerTagVersion, "version");
_LIT(KAtsLoggerTagTestSetResult, "batch-result");
_LIT(KAtsLoggerTagId, "id");
_LIT(KAtsLoggerTagTestCaseInit, "case-init");
_LIT(KAtsLoggerTagExpected, "expected-result");
_LIT(KAtsLoggerTagRunTime, "run-time");
_LIT(KAtsLoggerTagTestCaseResult, "case-result");
_LIT(KAtsLoggerTagActual, "actual-result");
_LIT(KAtsLoggerTagDebug, "debug-info");
_LIT(KAtsLoggerTagWarning, "warning");
_LIT(KAtsLoggerTagError, "error");
_LIT(KAtsLoggerTagStatus, "status");
_LIT(KAtsLoggerTimeStamp, "time-stamp");

const TInt KAtsLoggerBufferSegmentGranularity = 32;

// CLASS DECLARATION

// DESCRIPTION
// CAtsLogger defines API for test case result logging.
class CAtsLogger : public CBase
	{
	public:	// Public enumerations
		enum TReportStage
			{
			ENone,
			EUninitialized,
			EBeginTestReport,
			EBeginTestSet,
			EBeginTestCase,
			ESetTestCaseVerdict,
			EEndTestCase,
			EEndTestSet,
			EEndTestReport,
			EFinished
			};
	private:	// Private enumerations
				// None

	public:	// Public constructors/desctructors
		
		/**
		 * Symbian OS constructor
		 */
		IMPORT_C static CAtsLogger* NewL( const TDesC& aName, 
		                                  TBool aAfterReboot = EFalse );

		/**
		 * C++ destructor
		 */
		IMPORT_C ~CAtsLogger();

	public:	// Public new functions

		/**
		 * Returns error message. Also includes
		 * base E32 error codes.
		 */
		IMPORT_C static const TPtrC ErrorMessage(const TInt& aError);

		/**
		 * Closes opened resources. Desctructor calls
		 * this if it is not called before.
		 */
		IMPORT_C void CloseL();

		/**
		 * IsValid() returns ETrue if this logger is valid and
		 * able to write log.
		 */
		IMPORT_C TBool IsValid();
	
		/**
		 * BeginTestReportL() should be called at the beginning of the program,
		 * before any testing begins
		 */
		IMPORT_C void BeginTestReportL();

		/**
		 * EndTestReportL() must be called at the end of the program
		 */
		IMPORT_C void EndTestReportL();
		
		/**
		 * SaveForRebootL() should be called before reboot to save data.
		 */
		IMPORT_C void SaveForRebootL();

		/**
		 * ContinueAfterRebootL() must be called to continue 
		 * after reboot.
		 */
		IMPORT_C void ContinueAfterRebootL();

		/**
		 * BeginTestSetL() must be called when a new test set begins
		 *
		 * factory     = component factory whose component is being tested
		 * component   = name of the software component being tested
		 * version     = version of the software component
		 * description = short, optional description of the batch
		 */
		IMPORT_C void BeginTestSetL(const TDesC& aFactory = KAtsLoggerNa,
								    const TDesC& aComponent = KAtsLoggerNa,
								    const TDesC& aVersion = KAtsLoggerNa,
								    const TDesC& aDescription = KAtsLoggerEmpty);

		/**
		 * EndTestSetL() must be called as soon as a test set ends
		 */
		IMPORT_C void EndTestSetL();

		/**
		 * BeginTestCaseL() is called whenever an individual testcase begins
		 *
		 * id       = unique id of the testcase
		 * expected = expected result of the test (if applicable)
		 * info     = short descritpion of the expected result (if applicable)
		 */
		IMPORT_C void BeginTestCaseL(const TDesC& aId,
									 const TDesC& aExpected = KAtsLoggerNa,
									 const TDesC& aInfo = KAtsLoggerEmpty,
									 const TDesC& aVersion = KAtsLoggerEmpty);

		/**
		 * BeginTestCaseL() is called whenever an individual testcase begins
		 *
		 * id       = unique id of the testcase
		 * expected = expected result code of the test (if applicable)
		 * info     = short descritpion of the expected result (if applicable)
		 */
		IMPORT_C void BeginTestCaseL(const TDesC& aId,
									 const TInt aExpected,
									 const TDesC& aInfo = KAtsLoggerEmpty,
									 const TDesC& aVersion = KAtsLoggerEmpty);
									
		/**
		 * BeginTestCaseReportL() is called whenever an individual testcase report begins
		 *
		 * id       = unique id of the testcase
		 * expected = expected result code of the test (if applicable)
		 * startTime= test case starting time
		 * info     = short descritpion of the expected result (if applicable)
		 */
		 IMPORT_C void BeginTestCaseReportL( const TDesC& aId,
			                                const TDesC& aExpected,
			                                const TTime& aStartTime,
			                                const TDesC& aInfo = KAtsLoggerEmpty,
			                                const TDesC& aVersion = KAtsLoggerEmpty );
			                                
		/**
		 * BeginTestCaseReportL() is called whenever an individual testcase report begins
		 *
		 * id       = unique id of the testcase
		 * expected = expected result code of the test (if applicable)
		 * startTime= test case starting time
		 * info     = short descritpion of the expected result (if applicable)
		 */
		 IMPORT_C void BeginTestCaseReportL( const TDesC& aId,
			                                const TInt aExpected,
			                                const TTime& aStartTime,
			                                const TDesC& aInfo = KAtsLoggerEmpty,
			                                const TDesC& aVersion = KAtsLoggerEmpty );

		/**
		 * EndTestCaseL() is called when an individual testcase ends
		 */
		IMPORT_C void EndTestCaseL();

		/**
		 * TestCasePassed() must be called between calls
		 * to BeginTestCase() and EndTestCase(). This results
		 * test case to be passed.
		 */
		IMPORT_C void TestCasePassed();
		
		
		/**
		 * TestCaseFailed() must be called between calls
		 * to BeginTestCase() and EndTestCase(). This results
		 * test case to be failed.
		 */
		IMPORT_C void TestCaseFailed();

		/**
		 * TestCaseSkipped() must be called between calls
		 * to BeginTestCase() and EndTestCase(). This results
		 * test case to be skipped.
		 */
		IMPORT_C void TestCaseSkipped();
		
		/**
		 * TestCaseNa() must be called between calls
		 * to BeginTestCase() and EndTestCase(). This results
		 * test case to be N/A.
		 */
		IMPORT_C void TestCaseNa();

		/**
		 * SetTestCaseResultL() sets the actual result of the test (if applicable),
		 * this should correspond to the expected result (see BeginTestCase) if
		 * the test succeeds.

		 * Must be called between calls to BeginTestCaseL() and EndTestCaseL().
		 */
		IMPORT_C void SetTestCaseResultL(const TInt aResult);
		IMPORT_C void SetTestCaseResultL(const TDesC& aResult = KAtsLoggerEmpty);
		IMPORT_C void SetTestCaseResultL(TRefByValue<const TDesC> aFmt,...);

		/**
		 * DebugL(), WarningL(), ErrorL() and CommentL() as used to place
		 * informational messages of appropriate debug level into the test report.
         * These functions can be called anywhere between BeginReportL() and EndReportL().
		 */
		IMPORT_C void DebugL(const TDesC& aMsg);
		IMPORT_C void DebugL(TRefByValue<const TDesC> aFmt,...);

		IMPORT_C void WarningL(const TDesC& aMsg);
		IMPORT_C void WarningL(TRefByValue<const TDesC> aFmt,...);

		IMPORT_C void ErrorL(const TDesC& aMsg);
		IMPORT_C void ErrorL(TRefByValue<const TDesC> aFmt,...);

		IMPORT_C void CommentL(const TDesC& aMsg);
		IMPORT_C void CommentL(TRefByValue<const TDesC> aFmt,...);

		/**
		 * RawLogL() writes to a separate log file.
		 */
		IMPORT_C TInt RawLogL(const TDesC& aMsg);
		IMPORT_C TInt RawLogL(TRefByValue<const TDesC> aFmt,...);

		/**
		 * DebugOutput() writes to RDebug
		 */
		IMPORT_C void DebugOutput(const TDesC& aMsg);

		/**
		 * Set this to ETrue if client should use
		 * full set of reporting methods
		 */
		IMPORT_C void SetFullReporting(TBool aFlag);

		/**
		 * Set this to ETrue if XML log should be
		 * written to the debug port also.
		 */
		IMPORT_C void SetDebugOutput(TBool aFlag);

		/**
		 * Set this to ETrue if XML log should have
		 * indentation. This only adds human readability.
		 */
		IMPORT_C void SetIndentation(TBool aFlag);


    public: // Public functions from base classes
			// None
	
    protected:	// Protected new functions
		/**
		 * Outputs message closed by the tags.
		 */
		void TagL(const TDesC& aTag,
			      const TDesC& aMsg);

		/**
		 * Adds attribute with value to the message and
		 * outputs message closed by the tags.
		 */
		void TagL(const TDesC& aTag,
			      const TDesC& aMsg,
				  const TDesC& aAttribute,
				  const TDesC& aValue);

		/**
		 * Outputs message closed by the tags. This is used with
		 * DebugL(), WarningL(), ErrorL() and CommentL(). Time stamp is
		 * added to the message.
		 */
		void TimeStampedMessageL(const TDesC& aTag,
			                     const TDesC& aMsg);

		/**
		 * Opens a tag. 
		 */
		void OpenTagL(const TDesC& aTag);	

		
		/**
		 * Opens a tag with attribute and value.
		 */
		void OpenTagL(const TDesC& aTag,
					  const TDesC& aAttribute,
					  const TDesC& aValue);
		
		/**
		 * Opens a tag and increases indent level.
		 */
		void OpenTagIndentL(const TDesC& aTag);


		/**
		 * Opens a tag with attribute having value and increases indent level.
		 */
		void OpenTagIndentL(const TDesC& aTag,
			                const TDesC& aAttribute,
						    const TDesC& aValue);

		/**
		 * Closes a tag;
		 */
		void CloseTagL(const TDesC& aTag);

		/**
		 * Closes a tag and descreases indent level.
		 */
		void CloseTagIndentL(const TDesC& aTag);

		/**
		 * Outputs line feed.
		 */
		void LineFeedL();

		/**
		 * Writes data to the specified file.
		 */
		void WriteL(const TDesC& aData, RFile& aFile);

		/**
		 * Writes data to the XML file.
		 */
		void WriteL(const TDesC& aData);

		/**
		 * Encodes XML special chararacters from message
		 */
		HBufC* EncodeLC(const TDesC& aMsg);

		/**
		 * Checks whether report stage transition is legal.
		 */
		TBool IsTransitionLegal(const TReportStage& aNewStage);

	protected:  // Protected functions from base classes
				// None

	protected:	// Protected data
				// None
	
	private:	// Private functions
        /**
        * By default Symbian OS two-phase constructor is private.
        */
		void ConstructL( TBool aAfterReboot );

		/**
		 * C++ constructor
		 */
		CAtsLogger(const TDesC& aName);

		/**
		 * Checks whether directory exist or not
		 */
		TBool CheckDirectoryL(const TDesC& aDirName, TChar& aDrive);
		
		/**
		 * Replaces file if already exist
		 */
		TInt ReplaceFileL(const TDesC& aFileName);

		/**
		 * Open existing file if already exist
		 */
		TInt OpenExistingFileL(const TDesC& aFileName);

		/**
		 * Formats given date to a format of yyyy-mm-dd hh:mm:ss
		 */
		HBufC* FormatDateLC(TDateTime aDate);

		/**
		 * Formats given time to a format of hh:mm:ss
		 */
		HBufC* FormatTimeLC(TTimeIntervalMicroSeconds aTime);

		/**
		 * Expands buffer and then writes aBuf into it.
		 */
		TInt BufferL(CBufBase* aBufBase, TInt& aLength,const TDesC& aBuf);
		
		/**
		 * Writes whole buffer to a file. Does not reset buffer.
		 */
		TInt WriteBufferToFileL(CBufBase* aCBufBase, const TDesC& aFileName);

		/**
		 * Writes whole buffer to the debug port. Does not reset buffer.
		 */
		TInt WriteBufferToDebugPortL(CBufBase* aCBufBase);

	protected:	// Protected data
		// TTestSet data holder class definition and implementation
		class TTestSet
			{
			public:
				TTime iStartTime;
			};
		
		// TCase data holder class definition and implementation
		class TTestCase
			{
			public:
				TTestCase() {  iResult = NULL; }
				~TTestCase()
					{
					if (iResult)
						{
						delete iResult;
						iResult = NULL;
						}
					}
			public:
				HBufC* iResult;
				TPtrC iStatus;
				TTime iStartTime;
			
			};

		// Data holder for batch information
		CAtsLogger::TTestSet iTestSet;

		// Data holder for test case information
		CAtsLogger::TTestCase iTestCase;

		// Holds current report stage
		CAtsLogger::TReportStage iReportStage;

		// Test case and logger ID
		const TFileName iName;

	private: // Private data
		// Dynamic buffer for XML data
		CBufBase* iXmlBuffer;

		// Current legth of iXmlBuffer
		TInt iXmlBufferLength;
		
		// Indentation level of outputted XML
		int iIndentLevel;

		// Handle to file server
		RFs iRfs;

		// The name of XML file
		TFileName iXmlLogFileName;

		// The name of raw log file
		TFileName iRawLogFileName;

		// Flag to indicate that raw log should be done
		TBool iRawLog;

		// Flag to indicate that XML log should be done
		TBool iXmlLog;

		// Flag to indicate that logger is valid
		TBool iValid;

		// Flag to indicate that a line feed has been done
		TBool iLineFeedDone;

		// Flag to indicate that BeginTestCase() has been called
		// and EndTestCase() hasn't yet been called.
		TBool iInTestCase;

		// Flag to indicate that logger is closed
		TBool iClosed;

		// Flag to indicate if client should use
		// full set of logger methods:
		//		BeginTestReportL()
		//		BeginTestSetL();
		//		BeginTestCaseL();
		//		TestCasePassed(); // or other verdict method
		//		EndTestCase();
		//		EndTestSetL();
		//		EndTestReportL();
		TBool iFullReportingMode;

		// Flag to indicate that XML log should be
		// written to the debug port also
		TBool iDoDebugOutput;

		// Flag to indicate that logger does intendation
		// to the XML file
		TBool iDoIndentation;

	public:	// Friend classes
			// None
        
    protected:	// Friend classes
				// None
        
    private:	// Friend classes
				// None
};

#endif// End of File

// End of File
