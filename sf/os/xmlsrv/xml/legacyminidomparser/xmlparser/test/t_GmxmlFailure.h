// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32base.h>
#include <f32file.h>
#include <gmxmlparser.h>

/** @file
 * This file contains the declaration of the generic CGmxmlFailure class
 * which is responsible for testing the memory leak in the XMLParser.
 */

class CGmxmlFailure : public CActive, public MMDXMLParserObserver
	{
public:
	/** Allocates and constructs a new CGmxmlFailure test.
	@return New CGmxmlFailure 
	*/
	static CGmxmlFailure* NewLC();

	// Second constructor
	void ConstructL();

	/** Constructor. */
	CGmxmlFailure();

	/** Destructor. */
	~CGmxmlFailure();
	
	/**
	 * Functions from MMDXMLParserObserver class
	 */
	void ParseFileCompleteL();

	/**
	 * Function from MMDXMLComposerObserver
	 */
	void ComposeFileCompleteL(); 
	
	void LoadXmlFile(const TDesC &aFilePath);
	void LockXmlFile();
	
public: // from CActive
	void DoCancel();
	/*
	 * RunL function inherited from CActive base class - carries out the actual 
	 * parser leak test.
	 * @leave can Leave due to OOM
	 */
	void RunL();
	
private:
	CMDXMLParser* iCMDXMLParser;
	RFs iSession;						// File session for ParseFile.
	TInt iState;
	enum TStates
		{
		EEndState = 0x00,
		ENonExistFileState,
		ELockFileState,
		};
	};
	

