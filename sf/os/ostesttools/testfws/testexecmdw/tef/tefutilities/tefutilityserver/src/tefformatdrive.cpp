/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
* This class implements the formdrive test step to format given drive
*
*/



/**
 @file TEFFormaDrive.cpp
*/

#include "tefformatdrive.h"

//literals

_LIT(KFormatType,		"formattype");
_LIT(KDriveName,		"drivename");

_LIT(KDriveSeperator,	":");
_LIT(KDeviceSeperator,	"\\\\");
_LIT(KDirSeperator,		"\\");


/**
 This is constructor,which sets the test step name
*/
CTEFFormatDrive::CTEFFormatDrive()
	{
	SetTestStepName(KFormatDrive);
	}

/**
 * Run preamble scripts for the test
 * Make the connection with the File Server and creates a session object
 * @return - TVerdict code
 * Override of base class pure virtual
 */
TVerdict CTEFFormatDrive::doTestStepPreambleL()
	{
	User::LeaveIfError(iFs.Connect());
	return TestStepResult();
	}

/**
 * Run postample scripts for the test
 * @return - TVerdict code
 * Override of base class pure virtual
 */
TVerdict CTEFFormatDrive::doTestStepPostambleL()
	{
	iFs.Close();
	return TestStepResult();
	}

/**
 * Overrides base class virtual
 * @return - TVerdict codes
 * Parses buildinfo.txt from \epoc32\... and prints it to TEF log file outout
 */
TVerdict CTEFFormatDrive::doTestStepL()
	{
	INFO_PRINTF1(_L("Formating Disk"));
	
	TDriveUnit	driveUnit;
	ReadDriveNameL(driveUnit);

	//	Get the format type
	TPtrC	formatType;
	if(!GetStringFromConfig(ConfigSection(), KFormatType, formatType))
		{
		WARN_PRINTF1(_L("No format type specified - default is EFullFormat"));
		}

	if(TestStepResult() == EPass)
		{
		TBool		formatFlag=EFalse;
		TDriveName	drivePath(driveUnit.Name());
		//Format the specified driver.
		TInt		errCode= FormatDiskL(drivePath, formatType, formatFlag);

		if(formatFlag)
			{
			INFO_PRINTF1(_L("Disk is fully formatted"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("Error in formatting %D"), errCode);
			SetTestStepResult(EFail);
			SetTestStepError(errCode);
			}
		}

	return TestStepResult();
	}
/**
This functions reads the drive name specified in the ini file
@param aDriveName - specifies the drive name
@panic - system wide error
*/
void CTEFFormatDrive::ReadDriveNameL(TDriveUnit &aDriveName)
{
	TPtrC driveName;
	//read the drive name from ini file
	TInt result = GetStringFromConfig(ConfigSection(),KDriveName,driveName);
	if(result)
		{
		INFO_PRINTF2(_L("Drive name is %S"),&driveName);
		HBufC*	drivePath;
		TInt	parseRet=PreParseFileNameL(driveName, drivePath);
		CleanupStack::PushL(drivePath);

		if ( parseRet==KErrNone )
			{
			aDriveName=drivePath->Des();
			}
		else
			{
			ERR_PRINTF2(_L("Drive %S illegal"), &driveName);
			SetTestStepResult(EFail);
			}
			CleanupStack::PopAndDestroy(drivePath);
		}
		else
		{
		 ERR_PRINTF1(_L("Unable to read the drive name"));
		 SetTestStepResult(EFail);
		}	
}
/**
This method to parse a file name which may include the device name
@param aInputFileName - input file name which may contain the drive name also
@param aOutputFileName - holds the return value.
@panic - system wide error
*/
TInt CTEFFormatDrive::PreParseFileNameL(const TDesC& aInputFileName, HBufC*& aOutputFileName)
	{
	aOutputFileName=HBufC::NewL(aInputFileName.Length());

	TInt	ret=KErrNone;
	const	TDesC&	sep=KDeviceSeperator;

	if ( aInputFileName.FindC(sep) == 0 )
		{
		//	Starts with device name
		TPtrC	strippedFileName=aInputFileName.Right(aInputFileName.Length()-sep.Length());
		INFO_PRINTF2(_L("DeviceSeperator stripped %S"), &strippedFileName);

		TInt	find=strippedFileName.Find(KDirSeperator);
		TPtrC	driveName(find==KErrNotFound
							? strippedFileName
							: strippedFileName.Left(find));

		INFO_PRINTF2(_L("Drive name %S"), &driveName);
		TFileName	driveNameRead;
		TInt		drive;
		TBool		found=EFalse;
		for ( drive=EDriveA; (drive<=EDriveZ) && (!found); )
			{
			if ( iFs.GetDriveName(drive, driveNameRead)==KErrNone )
				{
				if ( driveNameRead.Length()>0 )
					{
					INFO_PRINTF3(_L("Drive %c:='%S'"), TUint(drive+'A'), &driveNameRead);
					}
				found=(driveName.Compare(driveNameRead)==0);
				}
			if ( !found )
				{
				++drive;
				}
			}

		if ( found )
			{
			TPtr	ptrTemp=aOutputFileName->Des();
			ptrTemp.Format(_L("%c"), TUint(drive+'A'));
			ptrTemp.Append(KDriveSeperator);
			ptrTemp.Append(strippedFileName.Right(strippedFileName.Length()-driveName.Length()));
			}
		else
			{
			WARN_PRINTF2(_L("Drive %S not found"), &driveName);
			ret=KErrNotFound;
			SetTestStepResult(EFail);
			}
		}
	else
		{
		*aOutputFileName=aInputFileName;
		}

	return ret;
	}
/**
 This functions reads the type of format that users wants to do and also drive name
 @param aDrivePath - path of the drive to be formated
 @param aFormatType - type of format 
 @param aFormatFlag - flag
 
*/
TInt CTEFFormatDrive::FormatDiskL(TPtrC aDrivePath, TPtrC aFormatType, TBool& aFormatFlag)
{

	TInt	errCode=KErrNone;
	//Format type default to EFullFormat
	TUint	formatType=EFullFormat;
	if(aFormatType == _L("EHighDensity"))
		{
		formatType = EHighDensity;
		}
	else if(aFormatType == _L("ELowDensity"))
		{
		formatType = ELowDensity;
		}
	else if(aFormatType == _L("EQuickFormat"))
		{
		formatType = EQuickFormat;
		}
	else if((aFormatType == _L("EFullFormat")) || (aFormatType.Length() == 0))
		{
		formatType = EFullFormat;
		}
	else
		{
		ERR_PRINTF2(_L("Illegal format type %S."), &aFormatType);
		errCode=KErrArgument;
		}

	aFormatFlag = EFalse;

	if ( errCode == KErrNone )
		{
		//Open the format object for format
		//Format object to format.
		RFormat	formatObj;

		//To hold the number of tracks in the disk
		TInt	trackCount=0;

		//Error code
		errCode = formatObj.Open(iFs, aDrivePath, formatType, trackCount);
		switch ( errCode )
			{
		case KErrNone:
			//Format while track remains for formating
			while(trackCount>0)
				{
				errCode = formatObj.Next(trackCount);
				if(errCode != KErrNone)
					{
					ERR_PRINTF2(_L("Error %d formatObj.Next."), errCode);
					ERR_PRINTF2(_L("Drive Path  %S."), &aDrivePath);
					ERR_PRINTF2(_L("Format Type %d."), formatType);
					ERR_PRINTF2(_L("Track Count %d."), trackCount);
					aFormatFlag = EFalse;
					break;
					}
				else
					{
					aFormatFlag = ETrue;
					}
				}
			formatObj.Close();
			break;
		default:
			ERR_PRINTF2(_L("Error %d formatObj.Open."), errCode);
			ERR_PRINTF2(_L("Drive Path  %S."), &aDrivePath);
			ERR_PRINTF2(_L("Format Type %d."), formatType);
			ERR_PRINTF2(_L("Track Count %d."), trackCount);
			}
		}

	return errCode;
}
