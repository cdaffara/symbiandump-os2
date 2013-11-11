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
* Description: This file contains STIFUnit implementation.
*
*/

/**
 * STIF_UNIT_INCLUDE SECTION - put all #includes between STIF_UNIT_INCLUDE_SECTION
 *                             and STIF_UNIT_INCLUDE_SECTION_END
 */
#ifdef STIF_UNIT_INCLUDE_SECTION
 

#endif //STIF_UNIT_INCLUDE_SECTION_END

/**
 * GLOBAL VARIABLES SECTION
 */
#ifdef TEST_VAR_DECLARATIONS
	/**
	 * Example of variable common for some test cases
	 */
	 RBuf buffer;
	 
#endif
/**
 * END OF GLOBAL VARIABLES SECTION
 */


/**
 * TEST CASES SECTION
 */
#ifdef TEST_CASES
/**
 * STIF_SETUP defines activities needed before every test case.
 */
STIF_SETUP
{
	/** Example of use of STIF_SETUP - a variable common for some test cases is initialized
	 */
	buffer.CreateL(5);
	buffer.CleanupClosePushL();
}

/**
 * STIF_TEARDOWN defines activities needed after every test case
 */
STIF_TEARDOWN
{
	/** Example of use of STIF_TEARDOWN - a variable common for some test cases is destroyed
	 */
	CleanupStack::PopAndDestroy();	
}

/**
 * STIF_TESTDEFINE defines a test case
 *
 *  Example test case - length of string is checked.
 *  The only argument of macro is a name of test case.
 */
STIF_TESTDEFINE(test1)
{
	_LIT(KHello, "Hello");
	buffer.Copy(KHello());
	buffer.ReAllocL(11);

	_LIT(KWorld, " World");
	buffer.Append(KWorld);
	
	STIF_ASSERT_EQUALS(buffer.Length(), 11);
}

#endif
/**
 * END OF TEST CASES SECTION
 */

// End of File
