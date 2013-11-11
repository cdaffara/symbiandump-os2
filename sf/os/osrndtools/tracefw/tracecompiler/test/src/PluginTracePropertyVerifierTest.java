/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * JUnit tests for PluginTracePropertyVerifier
 *
 */
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

public class PluginTracePropertyVerifierTest extends TestCase {

	/**
	 * Class to PluginTracePropertyVerifier
	 */
	static Class<?> pluginTracePropertyVerifier;

	/**
	 * PluginTracePropertyVerifier object
	 */
	static Object ptpvObject;

	/**
	 * Empty string
	 */
	private static final String EMPTY_STRING = ""; //$NON-NLS-1$

	/**
	 * White space
	 */
	private static final String WHITE_SPACE = " "; //$NON-NLS-1$

	/**
	 * Enum for different functions to test
	 * 
	 */
	private enum FunctionToTest {
		ISVALIDDATA
	}

	/**
	 * TestData class
	 * 
	 */
	private class TestData {
		String testString;
		boolean expectedIsValidDataResult;

		/**
		 * 
		 * Constructor
		 * 
		 * @param testStringParam
		 *            the test string
		 * @param expectedIsValidDataResultParam
		 *            the expected return value in case of isValidData
		 *            method
		 */
		TestData(String testStringParam, boolean expectedIsValidDataResultParam) {
			this.testString = testStringParam;
			this.expectedIsValidDataResult = expectedIsValidDataResultParam;
		}
	}

	/**
	 * List of test data
	 */
	List<TestData> testDataList = new ArrayList<TestData>();

	/**
	 * main
	 * 
	 * @param args
	 */
	public static void main(String[] args) {
		org.junit.runner.JUnitCore
				.main(PluginTracePropertyVerifierTest.class.getName());
	}

	/**
	 * setUp
	 */
	@Before
	public void setUp() {

		if (testDataList.size() == 0) {
			initializeTestDataList();
		}

		if (pluginTracePropertyVerifier == null && ptpvObject == null) {
			try {
				pluginTracePropertyVerifier = Class
						.forName("com.nokia.tracecompiler.engine.plugin.PluginTracePropertyVerifier"); //$NON-NLS-1$
				ptpvObject = pluginTracePropertyVerifier.newInstance();
			} catch (Exception e) {
				Assert.fail(e.toString());
			}
		}

	}

	/**
	 * Initialize test data list
	 */
	private void initializeTestDataList() {
		char VTAB = 0xB; //vertical TAB
		testDataList.add(new TestData("Some valid text", true)); //valid text //$NON-NLS-1$
		testDataList.add(new TestData("Some text with non-valid character - vertical TAB - : " + String.valueOf(VTAB) , false)); //$NON-NLS-1$
		testDataList.add(new TestData("Some valid text with TAB	: " , true)); //$NON-NLS-1$
		testDataList.add(new TestData("Some valid text with TABs				: " , true)); //$NON-NLS-1$
	}

	/**
	 * Execute tests
	 * 
	 * @param nameOfFunctionUnderTest
	 *            the name of the function that should be tested
	 * @param testType
	 *            the number of expected result column
	 */
	private void executeTests(String nameOfFunctionUnderTest,
			FunctionToTest testType) {
		for (int i = 0; i < testDataList.size(); i++) {
			String testString = testDataList.get(i).testString;
			boolean expectedResult = false;

			switch (testType) {
			case ISVALIDDATA:
				expectedResult = testDataList.get(i).expectedIsValidDataResult;
				break;
			}

			executeTest(nameOfFunctionUnderTest, testString, expectedResult);
		}
	}

	/**
	 * Execute test
	 * 
	 * @param nameOfFunctionUnderTest
	 *            the name of the function that should be tested
	 * @param testString
	 *            the test string that is used in test
	 * @param columNumberOfExpectedResult
	 *            the number of expected result column
	 */
	private void executeTest(String nameOfFunctionUnderTest, String testString,
			boolean expectedResult) {
		Method functionUnderTest = null;
		try {
			functionUnderTest = pluginTracePropertyVerifier.getDeclaredMethod(
					nameOfFunctionUnderTest, String.class);
			// change access of the function under test because otherwise we can
			// test to private functions
			functionUnderTest.setAccessible(true);
			// invoke the function and get result
			Object retObj = null;

			// remove white spaces from test string, because those those does
			// not exist in real life either
			retObj = functionUnderTest.invoke(ptpvObject, testString.replace(
					WHITE_SPACE, EMPTY_STRING));

			// cast the result to the expected return type.
			Boolean res = (Boolean) retObj;

			String msg = functionUnderTest.getName() + "  invoked on \"" //$NON-NLS-1$
					+ testString + "\" returned " + res.toString(); //$NON-NLS-1$
			Assert.assertEquals(msg, expectedResult, res.booleanValue());
		} catch (Exception e) {
			Assert.fail(e.getMessage());
		}
	}

	/**
	 * Test isValidData method
	 */
	@Test
	public void testIsValidDataMethod() {
		executeTests("isValidData", FunctionToTest.ISVALIDDATA); //$NON-NLS-1$
	}

	//add future methods tests after this point.
	
	/**
	 * tearDown
	 */
	@After
	public void tearDown() {
		// nothing to do
	}

}
