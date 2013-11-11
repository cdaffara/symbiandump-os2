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
* Name		: CTe_RegScriptCommandServer.h
* Author	  : Jason Zhou
*
*/



#ifndef CTE_REGSCRIPTCOMMANDSERVER_H
#define CTE_REGSCRIPTCOMMANDSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <test/testexecuteserverbase.h>

// CLASS DECLARATION

/**
 *  CTe_RegScriptCommandServer
 * 
 */
class CTe_RegScriptCommandServer : public CTestServer
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CTe_RegScriptCommandServer();

	/**
	 * Two-phased constructor.
	 */
	static CTe_RegScriptCommandServer* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CTe_RegScriptCommandServer* NewLC();
	
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	TTime& StartTime() { return iStartTime; }

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CTe_RegScriptCommandServer();
	
private:
	TTime iStartTime;
	};

#endif // CTE_REGSCRIPTCOMMANDSERVER_H
