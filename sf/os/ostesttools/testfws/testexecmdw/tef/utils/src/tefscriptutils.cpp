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
* Implementation for utitity class used for parsing script files
* Collect details on counts of test steps and test cases in a script file
*
*/



/**
 @file TEFScriptUtils.cpp
*/

#include "tefscriptutils.h"

/**
 * CScriptPreProcess(const TDesC aScriptFilePath, TCommandForCounting* aCommandsAndCounts, TInt aStructSize) - Constructor
 * Overloaded constructor used for collecting the script file path along with the structure array and size for storing count values
 * @param aScriptFilePath - Path of the script file to be loaded for reading
 * @param aCommandsAndCounts - Pointer to array of TCommandForCounting structures for collecting counts of commands
 * @param aStructSize - Integer value representing the size of the structure array passed in
 */
CScriptPreProcess::CScriptPreProcess(const TDesC& aScriptFilePath, TPtrC aCommandList[KTEFCommandCountsStructSize])
: iScriptFilePath(aScriptFilePath), iStructSize(KTEFCommandCountsStructSize), iScriptData(NULL)
	{
	for (TInt index=0; index < iStructSize; index++)
		{
		iCommandsAndCounts[index].iCommandName.Copy(aCommandList[index]);
		iCommandsAndCounts[index].iCommandCount = KTEFZeroValue;
		}
	}

/**
 * ~CScriptPreProcess() - Destructor
 * Deallocates heap allocation if done
 */
CScriptPreProcess::~CScriptPreProcess()
	{
	if (iScriptData != 0)
		{
		delete iScriptData;
		}
	}

/**
 * CountNoOfOccurences()
 * Member function used for parsing each of the script line
 * Updates the structure array for any command of interest found during parsing
 * @return TInt - Return system-wide error codes
 */
TInt CScriptPreProcess::CountNoOfOccurences()
	{
	// Construct the buffer with contents of script file
	TRAPD(err,CreateScriptDataFromScriptFileL());
	if (err != KErrNone)
		{
		// Return the error value, if the we fail to retieve the script file contents
		return err;
		}
	
	TBool statusOfWhile=ETrue;
	while (statusOfWhile)
		{
		// Get the individual script line contents until EOF
		statusOfWhile=GetNextScriptLine();

		// If the script line is available, parse it and update the command counts in the structure array, if any
		if (statusOfWhile)
			{
			UpdateCommandCounts();
			}
		}
	// Return KErrNone if the process is completed safely
	return KErrNone;
	}

/**
 * CreateScriptDataFromScriptFileL()
 * Funtion used to copy the contents of the script file to a buffer for reading
 */
void CScriptPreProcess::CreateScriptDataFromScriptFileL()
	{
	// Create a file session and connect
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);

	// Create a file object and try to open the script file for reading
	RFile scriptFile;
	User::LeaveIfError(scriptFile.Open(fS,iScriptFilePath,EFileRead | EFileShareAny));
	CleanupClosePushL(scriptFile);

	// Extract the size of the script file and create a heap buffer to the particular size
	TInt fileSize;
	User::LeaveIfError(scriptFile.Size(fileSize));

	// Create a 16bit heap buffer
	iScriptData = HBufC::NewL(fileSize);

	// Create a 8 bit heap buffer for temporily copying the contents of the script file from the file object
	HBufC8* narrowData = HBufC8::NewL(fileSize);
	CleanupStack::PushL(narrowData);
	TPtr8 narrowPtr=narrowData->Des();
	// Read the file into an 8bit heap buffer
	User::LeaveIfError(scriptFile.Read(narrowPtr));

	TPtr widePtr(iScriptData->Des());
	// Copy it to the 16bit buffer
	widePtr.Copy(narrowData->Des());

	// Cleanup all heap allocations
	CleanupStack::PopAndDestroy(narrowData);
	CleanupStack::Pop(2);

	// Close the file object and file session
	scriptFile.Close();
	fS.Close();

	// Set up the instance token parser
	iScriptLex = iScriptData->Des();
	}

/**
 * GetNextScriptLine()
 * Funtion used to retrieve individual script line and store them to a member buffer for parsing
 * @return TBool - Returns EFalse if the cursor position reaches end of file. ETrue otherwise
 */
TBool CScriptPreProcess::GetNextScriptLine()
	{
	// Place the lex marker for the next read
	while(!iScriptLex.Eos())
		{
		// Peek and place the cursor position to the end of line
		TChar peek = iScriptLex.Peek();
		if(peek == '\n')
			{
			iScriptLex.Inc();
			break;
			}
		else
			{
			iScriptLex.Inc();
			}
		}
	
	// Store the current line to a member variable
	iCurrentScriptLine.Set(iScriptLex.MarkedToken());
	iScriptLex.Mark();
	// Replace the remainder of the script contents into the original buffer
	// This is done to enable retrieving of next line during successive calls to the same function
	iScriptLex.Assign(iScriptLex.RemainderFromMark());

	// Check for end of file and return status accordingly
	if(iCurrentScriptLine.Length() || !iScriptLex.Eos())
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
 * UpdateCommandCounts()
 * Funtion used to parse the script line and update the the command struture with incrementing counts within the structure array
 */
void CScriptPreProcess::UpdateCommandCounts()
	{
	// Copy the current script line contents to a TLex for parsing
	TLex scriptLine(iCurrentScriptLine);

	// Extract the first token from the current script line lex
	TPtrC commandName(scriptLine.NextToken());

	// Run through a loop and check if the first token of the current script line
	// matches the command names provided in the structure array
	// If it matches any of the commands in array, increment the counter correspondingly
	for (TInt index=0;index<iStructSize;index++)
		{
		if (iCommandsAndCounts[index].iCommandName.CompareF(commandName) == 0)
			{
			iCommandsAndCounts[index].iCommandCount++;
			}
		}
	}

/**
 * RetrieveCount(TInt& aIndex, TDes& aCommandName, TInt& aCommandCount)
 * Funtion used to retrieve individual command counts from the structure array and store them to reference parameters
 * @param aIndex - TInt value representing the aray index whose details needs to be fetched
 * @param aCommandName - Reference to a buffer used to retrieve the command whose count is being updated
 * @param aCount - Reference to TInt descriptor used to retrieve the counts of the command from script file
 */
void CScriptPreProcess::RetrieveValues(TInt& aIndex, TDes& aCommandName, TInt& aCommandCount)
	{
	aCommandName.Copy(iCommandsAndCounts[aIndex].iCommandName);
	aCommandCount = iCommandsAndCounts[aIndex].iCommandCount;
	}
