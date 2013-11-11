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
* Description: This module contains the implementation of 
* CAtsLogger class member functions.
*
*/

// INCLUDE FILES
#include "atslogger.h"

CAtsLogger::CAtsLogger(const TDesC& aName) :
	iReportStage(EUninitialized),
	iName(aName),
	iIndentLevel(0),
	iRawLog(EFalse),
	iXmlLog(EFalse),
	iValid(EFalse),
	iLineFeedDone(EFalse),
	iInTestCase(EFalse),
	iClosed(EFalse),
	iFullReportingMode(ETrue),
	iDoDebugOutput(EFalse),
	iDoIndentation(ETrue)
	{
	}

void CAtsLogger::ConstructL( TBool aAfterReboot )
	{
	TChar xmlDrive;
	iXmlLog = CheckDirectoryL(KAtsLoggerXmlDirectory, xmlDrive);
	TChar rawDrive;
	iRawLog = CheckDirectoryL(KAtsLoggerRawDirectory, rawDrive);
	if (iXmlLog)
		{
		iXmlLogFileName.Copy(_L("?:"));		
		iXmlLogFileName.Append(KAtsLoggerXmlDirectory);
		iXmlLogFileName.Append(iName);
		iXmlLogFileName.Append(KAtsLoggerXmlSuffix);
		iXmlLogFileName[0] = (TUint8)xmlDrive;
		if( aAfterReboot )
		    {
		    User::LeaveIfError(OpenExistingFileL(iXmlLogFileName));
		    }
		else
		    {
		    User::LeaveIfError(ReplaceFileL(iXmlLogFileName));
		    }
		iValid = ETrue;
		}
	else
		{
		iValid = EFalse;
		}
	if (iRawLog)
		{
		iRawLogFileName.Copy(_L("?:"));		
		iRawLogFileName.Append(KAtsLoggerRawDirectory);
		iRawLogFileName.Append(iName);
		iRawLogFileName.Append(KAtsLoggerRawSuffix);
		iRawLogFileName[0] = (TUint8)rawDrive;
		if( aAfterReboot )
		    {
		    User::LeaveIfError(OpenExistingFileL(iRawLogFileName));
		    }
		else
		    {
		    User::LeaveIfError(ReplaceFileL(iRawLogFileName));
		    }
		}
	iXmlBuffer = CBufSeg::NewL(KAtsLoggerBufferSegmentGranularity);
	iXmlBufferLength = 0;
	}


TBool CAtsLogger::CheckDirectoryL(const TDesC& aDirName, TChar& aDrive)
	{
	User::LeaveIfError(iRfs.Connect());
	TDriveList driveList;
	iRfs.DriveList(driveList);
	TBool dirFound = EFalse;
	TFileName dir;
	for (TInt i = 0; i < driveList.Length() && !dirFound; i++)
		{
		TChar drive;
		if (driveList[i] != 0)
			{
			iRfs.DriveToChar(i, drive);
			dir.Copy(_L("?:"));
			dir.Append(aDirName);
			dir[0] = (TUint8)drive;
			RDir rDir;
			TInt error = rDir.Open(iRfs, dir, KEntryAttNormal);
			if (error == KErrNone)
				{
				dirFound = ETrue;
				aDrive = drive;
				}
			else 
				{
				dirFound = EFalse;
				aDrive = '?';
				}
			rDir.Close();
			}
		}
	iRfs.Close();
	return dirFound;
	}

TInt CAtsLogger::ReplaceFileL(const TDesC& aFileName)
	{
	User::LeaveIfError(iRfs.Connect());
	RFile file;
	TInt err = file.Replace(iRfs, aFileName, EFileWrite | EFileShareAny);
	file.Close();
	iRfs.Close();
	return err;
	}
	
TInt CAtsLogger::OpenExistingFileL(const TDesC& aFileName)
	{
	User::LeaveIfError(iRfs.Connect());
	RFile file;
	TInt err = file.Open(iRfs, aFileName, EFileWrite | EFileShareAny);
	file.Close();
	iRfs.Close();
	return err;
	}
	
EXPORT_C CAtsLogger::~CAtsLogger()
	{
	if (!iClosed)
		{
		CloseL();
		}
	if (iXmlBuffer)
		{
		delete iXmlBuffer;
		iXmlBuffer = NULL;
		}
	}

EXPORT_C void CAtsLogger::CloseL()
	{
	if (iClosed)
		{
		return;
		}
	User::LeaveIfError(iRfs.Connect());
	if (iXmlLog)
		{
		RFile file;
		TInt err = file.Open(iRfs, iXmlLogFileName, EFileWrite | EFileShareAny);
		if (err == KErrNone)
			{
			TInt fileSize = 0;
			file.Size(fileSize);
			file.Close();
			if (fileSize == 0)
				{
				iRfs.Delete(iXmlLogFileName);
				}
			}
		}
	if (iRawLog)
		{
		RFile file;
		TInt err = file.Open(iRfs, iRawLogFileName, EFileWrite | EFileShareAny);
		if (err == KErrNone)
			{
			TInt fileSize = 0;
			file.Size(fileSize);
			file.Close();
			if (fileSize == 0)
				{
				iRfs.Delete(iRawLogFileName);
				}
			}
		}
	iRfs.Close();
	iClosed = ETrue;
	}


EXPORT_C CAtsLogger* CAtsLogger::NewL( const TDesC& aName,
                                       TBool aAfterReboot )
	{
	__ASSERT_ALWAYS(aName.Length() > 0, User::Leave(KErrBadName));
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	__ASSERT_ALWAYS(rfs.IsValidName(aName), User::Leave(KErrBadName));
	rfs.Close();
	CAtsLogger* self = new (ELeave) CAtsLogger(aName);
	CleanupStack::PushL(self);
	self->ConstructL( aAfterReboot );
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C TBool CAtsLogger::IsValid()
	{
	return iValid;
	}

EXPORT_C void CAtsLogger::BeginTestReportL()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(EBeginTestReport))
			{
			User::Leave(EBeginTestReport);
			}
		iReportStage = EBeginTestReport;
		OpenTagIndentL(KAtsLoggerTagTestReport());
		}
	else
		{
		//iTestSet.iStartTime = TTime();
		iTestSet.iStartTime.HomeTime();
		}
	}

EXPORT_C void CAtsLogger::EndTestReportL()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(EEndTestReport))
			{
			User::Leave(EEndTestReport);
			}
		iReportStage = EEndTestReport;
		CloseTagIndentL(KAtsLoggerTagTestReport());
		}
	if (iDoDebugOutput)
		{
		WriteBufferToDebugPortL(iXmlBuffer);
		}
	if (iXmlLog && !iInTestCase && iXmlBufferLength > 0)
		{
		WriteBufferToFileL(iXmlBuffer, iXmlLogFileName);
		}
	iXmlBuffer->Reset();
	iXmlBufferLength = 0;
	}

EXPORT_C void CAtsLogger::BeginTestSetL(const TDesC& aFactory,
									    const TDesC& aComponent,
									    const TDesC& aVersion,
									    const TDesC& aDescription)
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(EBeginTestSet))
			{
			User::Leave(EBeginTestSet);
			}
		iReportStage = EBeginTestSet;
		//iTestSet.iStartTime = TTime();
		iTestSet.iStartTime.HomeTime();
		TDateTime date(iTestSet.iStartTime.DateTime());
		OpenTagIndentL(KAtsLoggerTagTestSet());
		OpenTagIndentL(KAtsLoggerTagTestSetInit());
		TagL(KAtsLoggerTagDescription(), aDescription);
		HBufC* stamp;
		stamp = FormatDateLC(date);
		TagL(KAtsLoggerTagDate(), *stamp);
		CleanupStack::PopAndDestroy(stamp);
		TagL(KAtsLoggerTagFactory(), aFactory);
		OpenTagIndentL(KAtsLoggerTagComponent());
		TagL(KAtsLoggerTagName(), aComponent);
		TagL(KAtsLoggerTagVersion(), aVersion);
		CloseTagIndentL(KAtsLoggerTagComponent());
		CloseTagIndentL(KAtsLoggerTagTestSetInit());
		}
	}

EXPORT_C void CAtsLogger::SaveForRebootL()
	{
	if (iFullReportingMode)
		{
		iReportStage = EEndTestReport;
		}
	if (iDoDebugOutput)
		{
		WriteBufferToDebugPortL(iXmlBuffer);
		}
	if (iXmlLog && !iInTestCase && iXmlBufferLength > 0)
		{
		WriteBufferToFileL(iXmlBuffer, iXmlLogFileName);
		}
	iXmlBuffer->Reset();
	iXmlBufferLength = 0;
	}

EXPORT_C void CAtsLogger::ContinueAfterRebootL()
	{
	if (iFullReportingMode)
		{
		iIndentLevel += 2;
		iLineFeedDone = ETrue;
		iReportStage = EBeginTestSet;
		//iTestSet.iStartTime = TTime();
		iTestSet.iStartTime.HomeTime();
		}
	}


EXPORT_C void CAtsLogger::EndTestSetL()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(EEndTestSet))
			{
			User::Leave(EEndTestSet);
			}
		iReportStage = EEndTestSet;
		OpenTagIndentL(KAtsLoggerTagTestSetResult());
		TTime current;
		current.HomeTime();
		HBufC* time;
		time = FormatTimeLC(current.MicroSecondsFrom(iTestSet.iStartTime));
		TagL(KAtsLoggerTagRunTime(), *time);
		CleanupStack::PopAndDestroy(time);
		CloseTagIndentL(KAtsLoggerTagTestSetResult());
		CloseTagIndentL(KAtsLoggerTagTestSet());
		}
	}

EXPORT_C void CAtsLogger::BeginTestCaseL(const TDesC& aId,
									     const TDesC& aExpected,
								    	 const TDesC& aInfo,
										 const TDesC& aVersion)
	{
	TTime current;
	current.HomeTime();	
	BeginTestCaseReportL( aId, aExpected, current, aInfo, aVersion );
	}

EXPORT_C void CAtsLogger::BeginTestCaseL(const TDesC& aId,
										 const TInt aExpected,
										 const TDesC& aInfo,
										 const TDesC& aVersion)
	{
		TBuf<16> res;
		res.Format(_L("%d"), aExpected);
		BeginTestCaseL(aId, res, aInfo, aVersion);
	}
	
EXPORT_C void CAtsLogger::BeginTestCaseReportL( const TDesC& aId,
										        const TDesC& aExpected,
										        const TTime& aStartTime,
										        const TDesC& aInfo,
										        const TDesC& aVersion )
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(EBeginTestCase))
			{
			User::Leave(EBeginTestCase);
			}
		iReportStage = EBeginTestCase;
	}		
	
	iTestCase.iStartTime = aStartTime;
	
	HBufC* time;
	time = FormatTimeLC(aStartTime.MicroSecondsFrom(iTestSet.iStartTime));
	OpenTagIndentL(KAtsLoggerTagTestCase(), KAtsLoggerTimeStamp(), *time);
	CleanupStack::PopAndDestroy(time);
	OpenTagIndentL(KAtsLoggerTagTestCaseInit());
	TagL(KAtsLoggerTagVersion, aVersion);
	TagL(KAtsLoggerTagId(), aId);
	TagL(KAtsLoggerTagExpected(), aExpected, KAtsLoggerTagDescription(), aInfo);
	CloseTagIndentL(KAtsLoggerTagTestCaseInit());
	iTestCase.iStatus.Set(KAtsLoggerNa());
	iInTestCase = ETrue;
	}
	
EXPORT_C void CAtsLogger::BeginTestCaseReportL( const TDesC& aId,
										        const TInt aExpected,
										        const TTime& aStartTime,
										        const TDesC& aInfo,
										        const TDesC& aVersion )
	{
	TBuf<16> res;
	res.Format(_L("%d"), aExpected);
	BeginTestCaseReportL( aId, res, aStartTime, aInfo, aVersion );
    }
    
EXPORT_C void CAtsLogger::EndTestCaseL()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(EEndTestCase))
			{
			User::Leave(EEndTestCase);
			}
		iReportStage = EEndTestCase;
	}
	TTime current;
	current.HomeTime();
	OpenTagIndentL(KAtsLoggerTagTestCaseResult(), KAtsLoggerTagStatus(), iTestCase.iStatus);
	if (iTestCase.iResult)
		{
		TagL(KAtsLoggerTagActual(), *iTestCase.iResult);
		delete iTestCase.iResult;
		iTestCase.iResult = NULL;
		}
	else
		{
		TagL(KAtsLoggerTagActual(), KAtsLoggerEmpty());
		}
	HBufC* time;
	time = FormatTimeLC(current.MicroSecondsFrom(iTestCase.iStartTime));
	TagL(KAtsLoggerTagRunTime(), *time);
	CleanupStack::PopAndDestroy(time);
	CloseTagIndentL(KAtsLoggerTagTestCaseResult());	
	CloseTagIndentL(KAtsLoggerTagTestCase());
	iInTestCase = EFalse;
	if (iDoDebugOutput)
		{
		WriteBufferToDebugPortL(iXmlBuffer);
		}
	if (iXmlLog)
		{
		WriteBufferToFileL(iXmlBuffer, iXmlLogFileName);
		}
	iXmlBuffer->Reset();
	iXmlBufferLength = 0;
	}

EXPORT_C void CAtsLogger::TestCasePassed()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(ESetTestCaseVerdict))
			{
			User::Leave(ESetTestCaseVerdict);
			}
		iReportStage = ESetTestCaseVerdict;
	}
	iTestCase.iStatus.Set(KAtsLoggerPassed());
	}

EXPORT_C void CAtsLogger::TestCaseFailed()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(ESetTestCaseVerdict))
			{
			User::Leave(ESetTestCaseVerdict);
			}
		iReportStage = ESetTestCaseVerdict;
	}
	iTestCase.iStatus.Set(KAtsLoggerFailed());
	}

EXPORT_C void CAtsLogger::TestCaseNa()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(ESetTestCaseVerdict))
			{
			User::Leave(ESetTestCaseVerdict);
			}
		iReportStage = ESetTestCaseVerdict;
	}
	iTestCase.iStatus.Set(KAtsLoggerNa());
	}

EXPORT_C void CAtsLogger::TestCaseSkipped()
	{
	if (iFullReportingMode)
		{
		if (!IsTransitionLegal(ESetTestCaseVerdict))
			{
			User::Leave(ESetTestCaseVerdict);
			}
		iReportStage = ESetTestCaseVerdict;
	}
	iTestCase.iStatus.Set(KAtsLoggerSkipped());
	}

EXPORT_C void CAtsLogger::SetTestCaseResultL(const TDesC& aResult)
	{
	if (iTestCase.iResult)
		{
		delete iTestCase.iResult;
		iTestCase.iResult = NULL;
		}
	HBufC* newResult = HBufC::NewL(aResult.Length());
	
	CleanupStack::PushL(newResult);
	TPtr ptr(newResult->Des());
	ptr.Copy(aResult);
	CleanupStack::Pop(newResult);
	
	iTestCase.iResult = newResult;
	}

EXPORT_C void CAtsLogger::SetTestCaseResultL(const TInt aResult)
	{
	if (iTestCase.iResult)
		{
		delete iTestCase.iResult;
		iTestCase.iResult = NULL;
		}
	HBufC* newResult = HBufC::NewL(16);
		
	CleanupStack::PushL(newResult);
	TPtr ptr(newResult->Des());
	ptr.Format(_L("%d"), aResult);
	CleanupStack::Pop(newResult);
	
	iTestCase.iResult = newResult;
	}

EXPORT_C void CAtsLogger::DebugL(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list, NULL);
	DebugL(buf);
	}

EXPORT_C void CAtsLogger::DebugL(const TDesC& aMsg)
	{
	if (iInTestCase)
		{
		TimeStampedMessageL(KAtsLoggerTagDebug(), aMsg);
		}
	else
		{
		CommentL(aMsg);
		}
	}

EXPORT_C void CAtsLogger::WarningL(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list, NULL);
	WarningL(buf);
	}

EXPORT_C void CAtsLogger::WarningL(const TDesC& aMsg)
	{
	if (iInTestCase)
		{
		TimeStampedMessageL(KAtsLoggerTagWarning(), aMsg);
		}
	else
		{
		CommentL(aMsg);
		}
	}

EXPORT_C void CAtsLogger::ErrorL(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list, NULL);
	ErrorL(buf);
	}

EXPORT_C void CAtsLogger::ErrorL(const TDesC& aMsg)
	{
	if (iInTestCase)
		{
		TimeStampedMessageL(KAtsLoggerTagError(), aMsg);
		}
	else
		{
		CommentL(aMsg);
		}
	}

EXPORT_C void CAtsLogger::CommentL(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list, NULL);
	CommentL(buf);
	}

EXPORT_C void CAtsLogger::CommentL(const TDesC& aMsg)
	{
	WriteL(KAtsLoggerCommentOpen());
	WriteL(aMsg);
	WriteL(KAtsLoggerCommentClose());
	LineFeedL();
	}

EXPORT_C TInt CAtsLogger::RawLogL(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list, NULL);
	RawLogL(buf);
	return KErrNone;
	}

EXPORT_C TInt CAtsLogger::RawLogL(const TDesC& aMsg)
	{
	if (iRawLog)
		{
		TBool lfStatus = iLineFeedDone;
		iLineFeedDone = EFalse;
		User::LeaveIfError(iRfs.Connect());
		RFile file;
		TInt err = file.Open(iRfs, iRawLogFileName, EFileWrite | EFileShareAny);
		if (err == KErrNotFound)
			{
			User::LeaveIfError(file.Create(iRfs, iRawLogFileName, EFileWrite | EFileShareAny));
			}
		else
			{
			TInt pos = 0;
			file.Seek(ESeekEnd, pos);
			}
		WriteL(aMsg, file);
		file.Close();
		iRfs.Close();
		iLineFeedDone = lfStatus;
		}
	return KErrNone;
	}

EXPORT_C void CAtsLogger::DebugOutput(const TDesC& aMsg)
	{
	RDebug::Print(_L("%S"), &aMsg);
	}

EXPORT_C void CAtsLogger::SetFullReporting(TBool aFlag)
	{
	iFullReportingMode = aFlag;
	}

EXPORT_C void CAtsLogger::SetDebugOutput(TBool aFlag)
	{
	iDoDebugOutput = aFlag;
	}

EXPORT_C void CAtsLogger::SetIndentation(TBool aFlag)
	{
	iDoIndentation = aFlag;
	}

HBufC* CAtsLogger::FormatDateLC(TDateTime aDate)
	{
	HBufC* buf = HBufC::NewL(19);
	CleanupStack::PushL(buf);
	TPtr ptr(buf->Des());
	ptr.Format(_L("%d-%02d-%02d %02d:%02d:%02d"), aDate.Year(),
		                                          aDate.Month(),
												  aDate.Day(),
												  aDate.Hour(),
												  aDate.Minute(),
												  aDate.Second());	
	return buf;
	}

HBufC* CAtsLogger::FormatTimeLC(TTimeIntervalMicroSeconds aTime)
	{	
    //@js<--remove--> TInt seconds1 = (aTime.Int64() / TInt64(1000000)).GetTInt();
    TInt seconds = (I64INT(aTime.Int64()) / I64INT(TInt64(1000000)));
	
	TInt hours = seconds / (60*60);
	TInt minutes = (seconds / 60) % 60;
	seconds = (seconds) % 60;
	HBufC* buf = HBufC::NewL(10);
	CleanupStack::PushL(buf);
	TPtr ptr(buf->Des());
	ptr.Format(_L("%02d:%02d:%02d"), hours, minutes, seconds);
	return buf;
	}

	
void CAtsLogger::TimeStampedMessageL(const TDesC& aTag, const TDesC& aMsg)
	{
	TTime current;
	current.HomeTime();
	HBufC* time;
	time = FormatTimeLC(current.MicroSecondsFrom(iTestSet.iStartTime));
	TagL(aTag, aMsg, KAtsLoggerTimeStamp(), *time);
	CleanupStack::PopAndDestroy(time);
	}

void CAtsLogger::OpenTagL(const TDesC& aTag)
	{
	WriteL(KAtsLoggerTagOpen());
	WriteL(aTag);
	WriteL(KAtsLoggerTagPost());
	}

void CAtsLogger::OpenTagL(const TDesC& aTag,
						  const TDesC& aAttribute,
						  const TDesC& aValue)
	{
	WriteL(KAtsLoggerTagOpen());
	WriteL(aTag);
	WriteL(KAtsLoggerSpace());
	WriteL(aAttribute);
	WriteL(KAtsLoggerAttr());
	HBufC* encoded;
	encoded = EncodeLC(aValue);
	WriteL(encoded->Des());
	CleanupStack::PopAndDestroy(encoded);
	WriteL(KAtsLoggerAttrClose());
	}

void CAtsLogger::OpenTagIndentL(const TDesC& aTag)
	{
	OpenTagL(aTag);
	LineFeedL();
	iIndentLevel++;
	}

void CAtsLogger::OpenTagIndentL(const TDesC& aTag,
							    const TDesC& aAttribute,
							    const TDesC& aValue)
	{
	OpenTagL(aTag, aAttribute, aValue);
	LineFeedL();
	iIndentLevel++;
	}

void CAtsLogger::CloseTagL(const TDesC& aTag)
	{
	WriteL(KAtsLoggerTagClose());
	WriteL(aTag);
	WriteL(KAtsLoggerTagPost());
	}

void CAtsLogger::CloseTagIndentL(const TDesC& aTag)
	{
	iIndentLevel--;
	CloseTagL(aTag);
	LineFeedL();
	}

void CAtsLogger::TagL(const TDesC& aTag, const TDesC& aMsg)
	{
	OpenTagL(aTag);
	HBufC* encoded;
	encoded = EncodeLC(aMsg);
	WriteL(encoded->Des());
	CleanupStack::PopAndDestroy(encoded);
	CloseTagL(aTag);
	LineFeedL();
	}

void CAtsLogger::TagL(const TDesC& aTag,
					  const TDesC& aMsg,
					  const TDesC& aAttribute,
					  const TDesC& aValue)
	{
	OpenTagL(aTag, aAttribute, aValue);
	HBufC* encoded;
	encoded = EncodeLC(aMsg);
	WriteL(encoded->Des());
	CleanupStack::PopAndDestroy(encoded);
	CloseTagL(aTag);
	LineFeedL();
	}

void CAtsLogger::LineFeedL()
	{
	WriteL(KAtsLoggerLf());
	iLineFeedDone = ETrue;
	}

HBufC* CAtsLogger::EncodeLC(const TDesC& aMsg)
	{
	TInt length = aMsg.Length();
	HBufC* buf = HBufC::NewL(length);
	*buf = aMsg;
	TPtr ptr(buf->Des());
	TInt index = 0;
	TInt offset = 0;
	while (index < length)
		{
		TPtrC16 p;
		switch (ptr[index + offset])
			{
			case '"':
				p.Set(KAtsLoggerQuot().Ptr());
				break;
			case '\'':
				p.Set(KAtsLoggerApos().Ptr());
				break;
			case '<':
				p.Set(KAtsLoggerLt().Ptr());
				break;
			case '>':
				p.Set(KAtsLoggerGt().Ptr());
				break;
			case '&':
				p.Set(KAtsLoggerAmp().Ptr());
				break;
			default:
				p.Set(KAtsLoggerEmpty().Ptr());
				break;
			}
		if (p.Length() > 0)
			{
			TInt len = ptr.Length() + p.Length();
			HBufC* tmp = buf;
			CleanupStack::PushL(tmp);
			buf = buf->ReAllocL(len);
			CleanupStack::Pop(tmp);
			tmp = NULL;
			ptr.Set(buf->Des());
			ptr.Replace(index + offset, 1, p);

			// offset has to be decrement by one because
			// we have replaced a char and index remains the
			// same.
			offset += p.Length() - 1;
			}
		else
			{
			index++;
			}
		}
	CleanupStack::PushL(buf);
	return buf;
	}

TInt CAtsLogger::BufferL(CBufBase* aBufBase, TInt& aLength, const TDesC& aBuf)
	{
	TInt length = aBuf.Length();
	if (iDoIndentation && iLineFeedDone)
		{
		iLineFeedDone = EFalse;
		TInt maxIndentLength = iIndentLevel * KAtsLoggerIndent8().Length();
		TInt indentLength = KAtsLoggerIndent8().Length();
		aBufBase->ExpandL(aLength, length + maxIndentLength);
		for (TInt i = 0; i < iIndentLevel; i++)
			{
			aBufBase->Write(aLength + (i * indentLength), KAtsLoggerIndent8());
			}
		aLength += maxIndentLength;
		}
	else
		{
		aBufBase->ExpandL(aLength, length);
		}
	HBufC8* hBufC8;
	hBufC8 = HBufC8::NewL(aBuf.Length());
	CleanupStack::PushL(hBufC8);
	TPtr8 ptr8(hBufC8->Des());
	for (TInt i = 0; i < length; i++)
		{
		if (aBuf[i] != '\n' && aBuf[i] != '\r')
			{
			ptr8.Append((TInt8)(aBuf[i] & 0xff));
			}
		else
			{
			ptr8.Append((TInt8)('\n'));
			}
		}
	aBufBase->Write(aLength, *hBufC8);
	CleanupStack::PopAndDestroy(hBufC8);
	aLength += length;
	return KErrNone;
	}

TInt CAtsLogger::WriteBufferToFileL(CBufBase* aCBufBase, const TDesC& aFileName)
	{
	User::LeaveIfError(iRfs.Connect());
	RFile file;
	TInt err = file.Open(iRfs, aFileName, EFileWrite | EFileShareAny);
	if (err == KErrNotFound)
		{
		User::LeaveIfError(file.Create(iRfs, aFileName, EFileWrite | EFileShareAny));
		}
	else
		{
		TInt pos = 0;
		file.Seek(ESeekEnd, pos);
		}
	TInt i = 0;
	TPtrC8 ptr = aCBufBase->Ptr(i);
	while (ptr.Length() > 0)
		{
		file.Write(ptr, ptr.Length());
		i += ptr.Length();
		ptr.Set(aCBufBase->Ptr(i));
		}
	file.Close();
	iRfs.Close();
	return KErrNone;
	}	

TInt CAtsLogger::WriteBufferToDebugPortL(CBufBase* aCBufBase)
	{
	TInt i = 0;
	TPtrC8 ptr = aCBufBase->Ptr(i);
	while (ptr.Length() > 0)
		{
		TBuf<KAtsLoggerBufferSegmentGranularity> debug;
		for (TInt j = 0; j < ptr.Length(); j++)
			{
			debug.AppendFormat(_L("%c"), ptr[j]);
			}
		DebugOutput(debug);
		i += ptr.Length();
		ptr.Set(aCBufBase->Ptr(i));
		}
	return KErrNone;
	}	

void CAtsLogger::WriteL(const TDesC& aMsg)
	{
	if (iXmlLog || iDoDebugOutput)
		{
		BufferL(iXmlBuffer, iXmlBufferLength, aMsg);
		iXmlBuffer->Compress();
		}
	}

void CAtsLogger::WriteL(const TDesC& aMsg, RFile& aFile)
	{
	if (iLineFeedDone)
		{
		iLineFeedDone = EFalse;
		for (TInt i = 0; i < iIndentLevel; i++)
			{
			WriteL(KAtsLoggerIndent(), aFile);
			}
		}
	HBufC8* hBufC8 = HBufC8::NewL(aMsg.Length());
	CleanupStack::PushL(hBufC8);
	TPtr8 ptr8(hBufC8->Des());
	TInt length = aMsg.Length();
	for (TInt i = 0; i < length; i++)
		{
		if (aMsg[i] != '\n' && aMsg[i] != '\r')
			{
			ptr8.Append((TInt8)(aMsg[i] & 0xff));
			}
		else
			{
			ptr8.Append((TInt8)('\n'));
			}
		}
	TRequestStatus status;
	aFile.Write(*hBufC8, length, status);
	User::WaitForRequest(status);
	CleanupStack::PopAndDestroy(hBufC8);
	}

TBool CAtsLogger::IsTransitionLegal(const TReportStage& aNewStage)
	{
	switch (iReportStage)
		{
		case EUninitialized:
			return (aNewStage == EUninitialized || aNewStage == EBeginTestReport);
		case EBeginTestReport:
			return (aNewStage == EBeginTestSet || aNewStage == EEndTestReport);
		case EBeginTestSet:
			return (aNewStage == EBeginTestCase || aNewStage == EEndTestSet);
		case EBeginTestCase:
			return (aNewStage == ESetTestCaseVerdict);
		case ESetTestCaseVerdict:
			return (aNewStage == EEndTestCase);
		case EEndTestCase:
			return (aNewStage == EEndTestSet || aNewStage == EBeginTestCase);
		case EEndTestSet:
			return (aNewStage == EEndTestReport || aNewStage == EBeginTestSet);
		case EEndTestReport:
			return (aNewStage == EFinished);
		case EFinished:
		default:
			break;
		}
	return EFalse;
	}

EXPORT_C const TPtrC CAtsLogger::ErrorMessage(const TInt& aError)
	{
	switch (aError)
		{
		case EUninitialized:
			return _L("Invalid report stage transition to EUninitialized");
		case EBeginTestReport:
			return _L("Invalid report stage transition to EBeginTestReport");
		case EBeginTestSet:
			return _L("Invalid report stage transition to EBeginTestSet");
		case EBeginTestCase:
			return _L("Invalid report stage transition to EBeginTestCase");
		case ESetTestCaseVerdict:
			return _L("Invalid report stage transition to ESetTestCaseVerdict");
		case EEndTestCase:
			return _L("Invalid report stage transition to EEndTestCase");
		case EEndTestSet:
			return _L("Invalid report stage transition to EEndTestSet");
		case EEndTestReport:
			return _L("Invalid report stage transition to EEndTestReport");
		case EFinished:
			return _L("Invalid report stage transition to EFinished");
		case KErrNone:
			return _L("KErrNone");
		case KErrNotFound:
			return _L("KErrNotFound");
		case KErrGeneral:
			return _L("KErrGeneral");
		case KErrCancel:
			return _L("KErrCancel");
		case KErrNoMemory:
			return _L("KErrNoMemory");
		case KErrNotSupported:
			return _L("KErrNotSupported");
		case KErrArgument:
			return _L("KErrArgument");
		case KErrTotalLossOfPrecision:
			return _L("KErrTotalLossOfPrecision");
		case KErrBadHandle:
			return _L("KErrBadHandle");
		case KErrOverflow:
			return _L("KErrOverflow");
		case KErrUnderflow:
			return _L("KErrUnderflow");
		case KErrAlreadyExists:
			return _L("KErrAlreadyExists");
		case KErrPathNotFound:
			return _L("KErrPathNotFound");
		case KErrDied:
			return _L("KErrDied");
		case KErrInUse:
			return _L("KErrInUse");
		case KErrServerTerminated:
			return _L("KErrServerTerminated");
		case KErrServerBusy:
			return _L("KErrServerBusy");
		case KErrCompletion:
			return _L("KErrCompletion");
		case KErrNotReady:
			return _L("KErrNotReady");
		case KErrUnknown:
			return _L("KErrUnknown");
		case KErrCorrupt:
			return _L("KErrCorrupt");
		case KErrAccessDenied:
			return _L("KErrAccessDenied");
		case KErrLocked:
			return _L("KErrLocked");
		case KErrWrite:
			return _L("KErrWrite");
		case KErrDisMounted:
			return _L("KErrDisMounted");
		case KErrEof:
			return _L("KErrEof");
		case KErrDiskFull:
			return _L("KErrDiskFull");
		case KErrBadDriver:
			return _L("KErrBadDriver");
		case KErrBadName:
			return _L("KErrBadName");
		case KErrCommsLineFail:
			return _L("KErrCommsLineFail");
		case KErrCommsFrame:
			return _L("KErrCommsFrame");
		case KErrCommsOverrun:
			return _L("KErrCommsOverrun");
		case KErrCommsParity:
			return _L("KErrCommsParity");
		case KErrTimedOut:
			return _L("KErrTimedOut");
		case KErrCouldNotConnect:
			return _L("KErrCouldNotConnect");
		case KErrCouldNotDisconnect:
			return _L("KErrCouldNotDisconnect");
		case KErrDisconnected:
			return _L("KErrDisconnected");
		case KErrBadLibraryEntryPoint:
			return _L("KErrBadLibraryEntryPoint");
		case KErrBadDescriptor:
			return _L("KErrBadDescriptor");
		case KErrAbort:
			return _L("KErrAbort");
		case KErrTooBig:
			return _L("KErrTooBig");
		case KErrDivideByZero:
			return _L("KErrDivideByZero");
		case KErrBadPower:
			return _L("KErrBadPower");
		case KErrDirFull:
			return _L("KErrDirFull");
		case KErrHardwareNotAvailable:
			return _L("KErrHardwareNotAvailable");
		default:
			return _L("Unknown error to ATS logger.");
		}
	}

// End of File
