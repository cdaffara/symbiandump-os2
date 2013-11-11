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
 * Description:
 *
 * JUnit tests for function parameters parsing
 *
 */

import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import org.junit.Assert;
import org.junit.Test;
import com.nokia.tracecompiler.source.*;
import com.nokia.tracecompiler.document.*;

public class ParseFunctionParametersTest {

	/**
	 * SourceDocumentFactory
	 */
	SourceDocumentFactory iFactory = null;

	public static void main(String[] args) {
		org.junit.runner.JUnitCore.main(ParseFunctionParametersTest.class
				.getName());
	}

	/**
	 * jUnit test to test function parameter parsing
	 */
	@Test
	public void testFunctionParameterParsing() {

		FileDocumentMonitor monitor = new FileDocumentMonitor();
		iFactory = monitor.getFactory();

		ArrayList<String> expectedParameterNames = new ArrayList<String>();
		ArrayList<String> expectedParameterTypes = new ArrayList<String>();
		String functionHeader;

		// Function header 1

		// First expected parameter
		expectedParameterNames.add("aLenght"); //$NON-NLS-1$
		expectedParameterTypes.add("TUint32"); //$NON-NLS-1$

		// Second expected parameter
		expectedParameterNames.add("aWidth"); //$NON-NLS-1$
		expectedParameterTypes.add("TUint16"); //$NON-NLS-1$

		// Third expected parameter
		expectedParameterNames.add("aDelay"); //$NON-NLS-1$
		expectedParameterTypes.add("TUint32"); //$NON-NLS-1$

		// construct the function header
		functionHeader = "DHelloWorld::DHelloWorld(TUint32 aLenght, TUint16 aWidth, TUint32 aDelay)\n" //$NON-NLS-1$
				+ "{"; //$NON-NLS-1$

		System.out.println("Execute test to function header 1"); //$NON-NLS-1$
		executeTest(expectedParameterNames, expectedParameterTypes,
				functionHeader);

		// Function header 2

		// Clear expected names and types arrays
		expectedParameterNames.clear();
		expectedParameterTypes.clear();

		// construct the function header
		functionHeader = "EXPORT_C DMessageHandler::DMessageHandler()\n" //$NON-NLS-1$
				+ ": iWriter( NULL )\n" //$NON-NLS-1$
				+ ", iSettings( NULL )\n" //$NON-NLS-1$				
				+ "{"; //$NON-NLS-1$

		System.out.println("Execute test to function header 2"); //$NON-NLS-1$
		executeTest(expectedParameterNames, expectedParameterTypes,
				functionHeader);

		// Test headers those caused defects in TraceCompiler version 2.1.0 and
		// 2.1.1
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
		// x

		System.out
				.println("Execute tests to function headers those caused defects in TraceCompiler version 2.1.0 and 2.1.1"); //$NON-NLS-1$

		// Clear expected names and types arrays
		expectedParameterNames.clear();
		expectedParameterTypes.clear();

		// First expected parameter
		expectedParameterNames.add("aObserver"); //$NON-NLS-1$
		expectedParameterTypes.add("MModemLcsServerObserver"); //$NON-NLS-1$

		// Second expected parameter
		expectedParameterNames.add("aIsiMsgObserver"); //$NON-NLS-1$
		expectedParameterTypes.add("MModemLcsIsiMsgObserver"); //$NON-NLS-1$

		// construct the function header
		functionHeader = "Cmodemlcsserverrrc::Cmodemlcsserverrrc(MModemLcsServerObserver *aObserver, MModemLcsIsiMsgObserver* aIsiMsgObserver):\n" //$NON-NLS-1$
				+ " CActive(EPriorityStandard) // Standard priority\n" //$NON-NLS-1$
				+ "{"; //$NON-NLS-1$

		executeTest(expectedParameterNames, expectedParameterTypes,
				functionHeader);

		// Clear expected names and types arrays
		expectedParameterNames.clear();
		expectedParameterTypes.clear();

		// First expected parameter
		expectedParameterNames.add("aDriver"); //$NON-NLS-1$
		expectedParameterTypes.add("RMeDriver"); //$NON-NLS-1$

		// Second expected parameter
		expectedParameterNames.add("aMaxMsgLength"); //$NON-NLS-1$
		expectedParameterTypes.add("TUint16"); //$NON-NLS-1$

		// construct the function header
		functionHeader = "CNpeSendData::CNpeSendData(RMeDriver* aDriver, TUint16 aMaxMsgLength): CActive(EPriorityStandard),\n" //$NON-NLS-1$
				+ "iDriver(aDriver),\n" //$NON-NLS-1$
				+ "iMaxMsgLength(aMaxMsgLength)\n" //$NON-NLS-1$
				+ "{"; //$NON-NLS-1$

		executeTest(expectedParameterNames, expectedParameterTypes,
				functionHeader);

		// Same expected parameter names and types are used as previous case

		// construct the function header
		functionHeader = "CNpeReceiveData::CNpeReceiveData(RMeDriver* aDriver, TUint16 aMaxMsgLength): CActive(EPriorityStandard),\n" //$NON-NLS-1$
				+ "iDriver(aDriver),\n" //$NON-NLS-1$
				+ "iMaxMsgLength(aMaxMsgLength)\n" //$NON-NLS-1$
				+ "{"; //$NON-NLS-1$

		executeTest(expectedParameterNames, expectedParameterTypes,
				functionHeader);
		// x
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	}

	/**
	 * Execute test for function header
	 * 
	 * @param expectedParameterNames
	 *            expected parameter names
	 * @param expectedParameterTypes
	 *            expected parameter types
	 * @param functionHeader
	 *            function header to be parsed
	 */
	private void executeTest(ArrayList<String> expectedParameterNames,
			ArrayList<String> expectedParameterTypes,
			final String functionHeader) {

		// first test the parser on an unwrapped string
		SourceParser sourceParser = new SourceParser(iFactory, iFactory
				.createDocument(functionHeader));

		// ArrayList<String> actualTokens = new ArrayList<String>();
		ArrayList<SourceParameter> actualParameters = new ArrayList<SourceParameter>();

		// parse string
		try {
			sourceParser.parseFunctionParameters(0, actualParameters);

		} catch (SourceParserException e) {
			Assert.fail(e.getMessage());
		}

		checkContents(actualParameters, expectedParameterNames,
				expectedParameterTypes);
	}

	/**
	 * Check contents
	 * 
	 * @param actualParameters
	 *            actual parsed parameters
	 * @param expectedParameterNames
	 *            expected parameter names
	 * @param expectedParameterTypes
	 *            expected parameter types
	 */
	private static void checkContents(
			final ArrayList<SourceParameter> actualParameters,
			final ArrayList<String> expectedParameterNames,
			final ArrayList<String> expectedParameterTypes) {

		// Confirm count of parsed parameters
		
		System.out.println("Confirm count of parsed parameters:"); //$NON-NLS-1$
		System.out.println("actualParameters.size() = " + actualParameters.size()); //$NON-NLS-1$
		System.out.println("expectedParameterNames() = " + expectedParameterNames.size()); //$NON-NLS-1$
		assertTrue(actualParameters.size() == expectedParameterNames.size());
		for (int i = 0; i < actualParameters.size(); i++) {

			// Confirm parsed parameter names
			
			System.out.println("Confirm parsed parameter names:"); //$NON-NLS-1$
			System.out.println("actualParameters name = " + actualParameters.get(i).getName()); //$NON-NLS-1$
			System.out.println("expectedParameter name = " + expectedParameterNames.get(i)); //$NON-NLS-1$
			assertTrue(actualParameters.get(i).getName().compareTo(
					expectedParameterNames.get(i)) == 0);

			// Confirm parsed parameter types
			
			System.out.println("Confirm parsed parameter types:"); //$NON-NLS-1$
			System.out.println("actualParameters type = " + actualParameters.get(i).getType()); //$NON-NLS-1$
			System.out.println("expectedParameter type = " + expectedParameterTypes.get(i)); //$NON-NLS-1$
			assertTrue(actualParameters.get(i).getType().compareTo(
					expectedParameterTypes.get(i)) == 0);
		}
	}
}
