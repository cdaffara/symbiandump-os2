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
 * JUnit tests for fixed_id.definitions file
 *
 */

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Writer;

import junit.framework.TestCase;

import org.junit.Test;

import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.source.SourceDocumentFactory;

public class FixedIdDefinitionsTest extends TestCase {

	/**
	 * EMMA output prefix
	 */
	private static final String EMMA_OUTPUT_PREFIX = "EMMA: "; //$NON-NLS-1$
	
	/**
	 * Fixed id definitions test app took note prefix
	 */
	private static final String FIXED_ID_DEFINITIONS_TEST_APP_TOOK_NOTE_PREFIX = "FixedIdDefinitionsTestApp took"; //$NON-NLS-1$

	/**
	 * UID3 of FixedIdDefinitionsTestApp
	 */
	private static final String FIXED_ID_DEFINITIONS_TEST_APP_UID3 = "E8576D92"; //$NON-NLS-1$

	/**
	 * txt extension
	 */
	private static final String TXT_EXTENSION = ".txt"; //$NON-NLS-1$

	/**
	 * Reference fid file name prefix
	 */
	private static final String REFERENCE_FID_FILE_NAME_PREFIX = "reference_fid_file_"; //$NON-NLS-1$

	/**
	 * Reference fid file name prefix
	 */
	private static final String REFERENCE_TC_OUTPUT_NAME_PREFIX = "reference_tc_output_"; //$NON-NLS-1$	

	/**
	 * data directory name
	 */
	private static final String DATA_DIRECTORY = "data"; //$NON-NLS-1$

	/**
	 * traces directory name
	 */
	private static final String TRACES_DIRECTORY = "traces"; //$NON-NLS-1$

	/**
	 * fixed_id.definitions file name
	 */
	private static final String FIXED_ID_DEFINITIONS_FILE_NAME = "fixed_id.definitions"; //$NON-NLS-1$

	/**
	 * Line comment tag
	 */
	private static final String LINE_COMMENT_TAG = "//"; //$NON-NLS-1$

	/**
	 * TRACE_USED_IN_TEST_CASE tag
	 */
	private static final String TRACE_USED_IN_TEST_CASE_TAG = "TRACE_USED_IN_TEST_CASE"; //$NON-NLS-1$

	/**
	 * Error text prefix
	 */
	private static final String ERROR_TEXT_PREFIX = "Error: "; //$NON-NLS-1$

	/**
	 * src directory name
	 */
	private static final String SRC_DIRECTORY = "src"; //$NON-NLS-1$

	/**
	 * .cpp extension
	 */
	private static final String CPP_EXTENSION = ".cpp"; //$NON-NLS-1$

	/**
	 * FixedIdDefinitionsTestApp
	 */
	private static final String FIXED_ID_DEFINITIONS_TEST_APP = "FixedIdDefinitionsTestApp"; //$NON-NLS-1$

	/**
	 * Variable for epocroot
	 */
	String e32;

	/**
	 * Path to FixedIdDefinitionsTestApp application
	 */
	String fixedIdDefinitionsTestAppPath;

	/**
	 * Process builder
	 */
	ProcessBuilder pb = null;

	/**
	 * Compiler path
	 */
	File compilerPath = null;

	/**
	 * Line separator
	 */
	String lineSeparator;

	/**
	 * Source file name with full path
	 */
	String sourceFileNameWithFullPath;

	/**
	 * fixed_id.definitions file name with full path
	 */
	String fixedIdDefinitionsFileNameWithFullPath;

	/**
	 * SourceDocumentFactory
	 */
	SourceDocumentFactory factory = null;

	public static void main(String[] args) {
		org.junit.runner.JUnitCore.main(FixedIdDefinitionsTest.class.getName());
	}

	/**
	 * setup
	 */
	@Test
	public void setUp() {
		System.out.println("Setup class variables"); //$NON-NLS-1$

		e32 = System.getenv("EPOCROOT"); //$NON-NLS-1$

		if (!e32.endsWith(File.separator)) {
			e32 += File.separator;
		}

		fixedIdDefinitionsTestAppPath = e32 + "testdata" + File.separator //$NON-NLS-1$
				+ FIXED_ID_DEFINITIONS_TEST_APP + File.separator;
				
		// These EMMA flags are added, because of EMMA reporting in Hudson
		String emmaOutputFileFlag = "-Demma.coverage.out.file=" + e32 + "testdata" + File.separator + "reports" + File.separator + "emma" + File.separator + "coverage.emma";
		String emmaOutputMergeFlag = "-Demma.coverage.out.merge=true";

		pb = new ProcessBuilder("java", emmaOutputFileFlag, emmaOutputMergeFlag, "-classpath", ".", //$NON-NLS-1$//$NON-NLS-2$ //$NON-NLS-3$
				"com.nokia.tracecompiler.TraceCompiler", FIXED_ID_DEFINITIONS_TEST_APP_UID3, //$NON-NLS-1$
				FIXED_ID_DEFINITIONS_TEST_APP, fixedIdDefinitionsTestAppPath
						+ "group" + File.separator //$NON-NLS-1$
						+ FIXED_ID_DEFINITIONS_TEST_APP + ".mmp", //$NON-NLS-1$
				fixedIdDefinitionsTestAppPath + SRC_DIRECTORY + File.separator
						+ FIXED_ID_DEFINITIONS_TEST_APP + CPP_EXTENSION);

		compilerPath = new File(e32 + "epoc32" + File.separator + "tools" //$NON-NLS-1$//$NON-NLS-2$
				+ File.separator + "tracecompiler" + File.separator); //$NON-NLS-1$

		lineSeparator = System.getProperty("line.separator"); //$NON-NLS-1$

		sourceFileNameWithFullPath = fixedIdDefinitionsTestAppPath
				+ SRC_DIRECTORY + File.separator
				+ FIXED_ID_DEFINITIONS_TEST_APP + CPP_EXTENSION;

		fixedIdDefinitionsFileNameWithFullPath = fixedIdDefinitionsTestAppPath
				+ TRACES_DIRECTORY + File.separator
				+ FIXED_ID_DEFINITIONS_FILE_NAME;
	}

	/**
	 * Check if fixed id file is created correctly when it doesn't already exist
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_1() {
		int testCaseNumber = 1;
		System.out
				.println("*** Check if fixed id file is created correctly when it doesn't already exist ***"); //$NON-NLS-1$
		executeTestCase(testCaseNumber, 0);
	}

	/**
	 * Check new trace points are assigned values correctly (i.e. existing ones
	 * don't change)
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_2() {
		int testCaseNumber = 2;
		System.out
				.println("*** Check new trace points are assigned values correctly (i.e. existing ones don't change) ***"); //$NON-NLS-1$
		executeTestCase(testCaseNumber, 0);
	}

	/**
	 * Check removed trace points / user-defined groups are obsoleted correctly
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_3() {
		int testCaseNumber = 3;
		System.out
				.println("*** Check removed trace points / user-defined groups are obsoleted correctly ***"); //$NON-NLS-1$
		executeTestCase(testCaseNumber, 0);
	}

	/**
	 * Check that adding a trace point with an obsoleted name is given a new
	 * value.
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_4() {
		int testCaseNumber = 4;
		System.out
				.println("*** Check that adding a trace point with an obsoleted name is given a new value. ***"); //$NON-NLS-1$
		executeTestCase(testCaseNumber, 0);
	}

	/**
	 * Check that obsoleting all trace points in a component that refer to a
	 * common group id (e.g. TRACE_FLOW) does NOT mark group as obsolete
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_5() {
		int testCaseNumber = 5;
		System.out
				.println("*** Check that obsoleting all trace points in a component that refer to a common group id (e.g. TRACE_FLOW) does NOT mark group as obsolete ***"); //$NON-NLS-1$
		executeTestCase(testCaseNumber, 0);
	}

	/**
	 * Check that repeatedly obsoleting a trace point does not mess
	 * TraceCompiler.
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_6() {
		int testCaseNumber = 6;
		int subTestNumber = 1;
		System.out
				.println("*** Check that repeatedly obsoleting a trace point does not mess TraceCompiler. ***"); //$NON-NLS-1$
		int numberOfSubTests = 4;
		while (subTestNumber < numberOfSubTests) {
			executeTestCase(testCaseNumber, subTestNumber);
			subTestNumber++;
		}
	}

	/**
	 * Check that repeatedly changing group of the trace point does not mess
	 * TraceCompiler.
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_7() {
		int testCaseNumber = 7;
		int subTestNumber = 1;
		System.out
				.println("*** Check that repeatedly changing group of the trace point does not mess TraceCompiler. ***"); //$NON-NLS-1$
		int numberOfSubTests = 3;
		while (subTestNumber < numberOfSubTests) {
			executeTestCase(testCaseNumber, subTestNumber);
			subTestNumber++;
		}
	}

	/**
	 * Check that a OSTv1 common group id values results in new fixed id file.
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_8() {
		int testCaseNumber = 8;
		System.out
				.println("*** Check that a OSTv1 common group id values results in new fixed id file. ***"); //$NON-NLS-1$
		executeTestCaseWithTraceCompilerOutputCheck(testCaseNumber, 0);
	}

	/**
	 * Check that a OSTv1 user group id values results in new fixed id file.
	 */
	@Test
	public void testFixedIdDefinitionsFunctionalityTestCase_9() {
		int testCaseNumber = 9;
		System.out
				.println("*** Check that a OSTv1 user group id values results in new fixed id file. ***"); //$NON-NLS-1$
		executeTestCaseWithTraceCompilerOutputCheck(testCaseNumber, 0);
	}

	/**
	 * Sub function to execute test case with TraceCompiler output check
	 * 
	 * @param testCaseNumber
	 *            Test case number
	 */
	private void executeTestCaseWithTraceCompilerOutputCheck(
			int testCaseNumber, int subTestNumber) {
		System.out
				.println("Start to execute test case with TraceCompiler output check."); //$NON-NLS-1$
		modifyFixedIdDefinitionsTestApp(testCaseNumber, subTestNumber);
		InputStream traceCompilerOutput = runTraceCompiler();
		verifyTraceCompilerOutput(testCaseNumber, subTestNumber,
				traceCompilerOutput);
		verifyTestResults(testCaseNumber, subTestNumber);
	}

	/**
	 * Sub function to execute test case
	 * 
	 * @param testCaseNumber
	 *            Test case number
	 */
	private void executeTestCase(int testCaseNumber, int subTestNumber) {
		System.out.println("Start to execute test case."); //$NON-NLS-1$
		modifyFixedIdDefinitionsTestApp(testCaseNumber, subTestNumber);
		if (testCaseNumber > 1) {
			initializeFixedIDDefinitionsFile(testCaseNumber, subTestNumber);
		}
		runTraceCompiler();
		verifyTestResults(testCaseNumber, subTestNumber);
	}

	/**
	 * Initialize fixed_id.defitions file
	 * 
	 * @param testCaseNumber
	 *            Test case number
	 */
	private void initializeFixedIDDefinitionsFile(int testCaseNumber,
			int subTestNumber) {
		System.out.println("Initialize fixed_id.defitions file"); //$NON-NLS-1$
		String refesenceFileName;
		if (subTestNumber == 0) {
			refesenceFileName = REFERENCE_FID_FILE_NAME_PREFIX
					+ (testCaseNumber - 1);
		} else {
			refesenceFileName = REFERENCE_FID_FILE_NAME_PREFIX + testCaseNumber
					+ "_" + (subTestNumber - 1); //$NON-NLS-1$
		}
		refesenceFileName = refesenceFileName + TXT_EXTENSION;
		String sourceFile = fixedIdDefinitionsTestAppPath + DATA_DIRECTORY
				+ File.separator + refesenceFileName;
		File source = new File(sourceFile);
		File target = new File(fixedIdDefinitionsFileNameWithFullPath);
		FileUtils.copyFile(source, target);
	}

	/**
	 * Modify FixedIdDefinitionsTestApp application source code
	 * 
	 * @param testCaseNumber
	 *            test case number
	 */
	private void modifyFixedIdDefinitionsTestApp(int testCaseNumber,
			int subTestNumber) {
		System.out
				.println("Modify FixedIdDefinitionsTestApp application source code"); //$NON-NLS-1$
		try {
			StringBuilder modifiedContents = new StringBuilder();
			File sourceFile = new File(sourceFileNameWithFullPath);
			BufferedReader input = new BufferedReader(
					new FileReader(sourceFile));
			String line;
			String currentTestCaseTag = TRACE_USED_IN_TEST_CASE_TAG
					+ "_" + testCaseNumber; //$NON-NLS-1$
			if (subTestNumber != 0) {
				currentTestCaseTag = currentTestCaseTag + "_" + subTestNumber; //$NON-NLS-1$
			}
			while ((line = input.readLine()) != null) {
				if (line.indexOf(TRACE_USED_IN_TEST_CASE_TAG) != -1) {
					if (line.indexOf(currentTestCaseTag) != -1) {
						if (line.startsWith(LINE_COMMENT_TAG)) {
							line = line.substring(LINE_COMMENT_TAG.length());
						}
					} else {
						if (!line.startsWith(LINE_COMMENT_TAG)) {
							line = LINE_COMMENT_TAG + line;
						}
					}
				}
				modifiedContents.append(line);
				modifiedContents.append(lineSeparator);
			}
			input.close();
			Writer sourceFileWriter = new BufferedWriter(new FileWriter(
					sourceFileNameWithFullPath));
			sourceFileWriter.write(modifiedContents.toString());
			sourceFileWriter.close();

			if (testCaseNumber == 8 || testCaseNumber == 9) {
				String fileContent = "#Fixed group and trace id definitions. If this file is removed, the identifiers are rebuilt." + lineSeparator; //$NON-NLS-1$

				if (testCaseNumber == 8) {
					fileContent += "[GROUP]TRACE_FLOW=0x7" + lineSeparator //$NON-NLS-1$
							+ "[TRACE]TRACE_FLOW[0x7]_TRACE1=0x1"; //$NON-NLS-1$
				} else if (testCaseNumber == 9) {
					fileContent += "[GROUP]MY_GROUP=0x70" + lineSeparator //$NON-NLS-1$
							+ "[TRACE]MY_GROUP [0x70]_TRACE1=0x1"; //$NON-NLS-1$
				}
				Writer fixedIDDefinitionsFileWriter = new BufferedWriter(
						new FileWriter(fixedIdDefinitionsFileNameWithFullPath));
				fixedIDDefinitionsFileWriter.write(fileContent);
				fixedIDDefinitionsFileWriter.close();
			}

		} catch (Exception e) {
			System.err.println(ERROR_TEXT_PREFIX + e.getMessage());
			fail();
		}
	}

	/**
	 * Run TraceCompiler
	 */
	private InputStream runTraceCompiler() {
		System.out.println("Run TraceCompiler"); //$NON-NLS-1$
		InputStream inp = null;
		pb.directory(compilerPath);
		pb.redirectErrorStream(true);
		// start the compiler
		try {
			Process p = pb.start();
			p.waitFor();
			inp = p.getInputStream();
		} catch (Exception e) {// Catch exception if any
			System.err.println(ERROR_TEXT_PREFIX + e.getMessage());
			fail();
		}

		return inp;
	}

	/**
	 * Verify TraceCompiler output
	 * 
	 * @param traceCompilerOutput
	 *            TraceCompiler output
	 */
	private void verifyTraceCompilerOutput(int testCaseNumber,
			int subTestNumber, InputStream traceCompilerOutput) {
		System.out.println("Verify TraceCompiler output"); //$NON-NLS-1$

		String refesenceFileName = REFERENCE_TC_OUTPUT_NAME_PREFIX
				+ testCaseNumber;
		if (subTestNumber != 0) {
			refesenceFileName = refesenceFileName + "_" + subTestNumber; //$NON-NLS-1$
		}
		refesenceFileName = refesenceFileName + TXT_EXTENSION;

		try {
			FileInputStream fstreamExpected = new FileInputStream(
					fixedIdDefinitionsTestAppPath + DATA_DIRECTORY
							+ File.separator + refesenceFileName);
			compareExpectedAndActual(fstreamExpected, traceCompilerOutput);
		} catch (FileNotFoundException e) {
			System.err.println(ERROR_TEXT_PREFIX + e.getMessage());
			fail();
		}
	}

	/**
	 * Verify test result
	 * 
	 * @param testCaseNumber
	 *            test case number
	 */
	private void verifyTestResults(int testCaseNumber, int subTestNumber) {
		System.out.println("Verify test result"); //$NON-NLS-1$
		String refesenceFileName = REFERENCE_FID_FILE_NAME_PREFIX
				+ testCaseNumber;

		if (subTestNumber != 0) {
			refesenceFileName = refesenceFileName + "_" + subTestNumber; //$NON-NLS-1$
		}

		refesenceFileName = refesenceFileName + TXT_EXTENSION;
		try {
			FileInputStream fstreamExpected = new FileInputStream(
					fixedIdDefinitionsTestAppPath + DATA_DIRECTORY
							+ File.separator + refesenceFileName);
			FileInputStream fstreamActual = new FileInputStream(
					fixedIdDefinitionsFileNameWithFullPath);
			compareExpectedAndActual(fstreamExpected, fstreamActual);
		} catch (FileNotFoundException e) {
			System.err.println(ERROR_TEXT_PREFIX + e.getMessage());
			fail();
		}
	}

	/**
	 * Compare expected and actual input streams
	 * 
	 * @param streamExpected
	 *            Expected input stream
	 * @param streamActual
	 *            Actual input stream
	 */
	private void compareExpectedAndActual(InputStream streamExpected,
			InputStream streamActual) {
		System.out.println("Compare expected and actual input streams"); //$NON-NLS-1$
		try {
			// Get the objects of DataInputStream
			DataInputStream inExpected = new DataInputStream(streamExpected);
			DataInputStream inActual = new DataInputStream(streamActual);
			BufferedReader brExpected = new BufferedReader(
					new InputStreamReader(inExpected));
			BufferedReader brActual = new BufferedReader(new InputStreamReader(
					inActual));

			String strLineExpected;
			String strLineActual;
			int i = 1;
			// compare the expected and actual input streams
			while (((strLineExpected = brExpected.readLine()) != null)
					&& (((strLineActual = brActual.readLine()) != null))) {

				// Skip EMMA outputs if those exists
				if (strLineActual.trim().startsWith(EMMA_OUTPUT_PREFIX)) {
						strLineActual = brActual.readLine();
						if (strLineActual == null) {
							break;
						}
				}

				// Following check skips different TraceCompiler execution time
				// values
				if (strLineExpected
						.indexOf(FIXED_ID_DEFINITIONS_TEST_APP_TOOK_NOTE_PREFIX) != -1
						&& strLineActual
								.indexOf(FIXED_ID_DEFINITIONS_TEST_APP_TOOK_NOTE_PREFIX) != -1) {
					strLineActual = strLineActual.trim().substring(
							0,
							FIXED_ID_DEFINITIONS_TEST_APP_TOOK_NOTE_PREFIX
									.length());

				}
				assertTrue(ERROR_TEXT_PREFIX + "Line \"" + strLineActual //$NON-NLS-1$
						+ "\" does not match to \"" + strLineExpected + "\"", //$NON-NLS-1$ //$NON-NLS-2$
						strLineExpected.compareTo(strLineActual) == 0);
				i++;
			}
			
			// Check that actual file does not contain any extra lines, it could be that there are some EMMA outputs and those are ok
			while ((strLineActual = brActual.readLine()) != null) {
				assertTrue(
						ERROR_TEXT_PREFIX
								+ "Actual file does contain extra lines", strLineActual.trim().startsWith(EMMA_OUTPUT_PREFIX)); //$NON-NLS-1$				
			}
			
			// never entered loop therefore there actual file was empty or
			// missing
			assertTrue(ERROR_TEXT_PREFIX + "Actual file was empty or missing", //$NON-NLS-1$
					i > 0);

			inExpected.close();
			inActual.close();
		} catch (Exception e) {
			System.err.println(ERROR_TEXT_PREFIX + e.getMessage());
			fail();
		}
	}

	/**
	 * tearDown
	 */
	@Test
	public void tearDown() {
		System.out.println("Comment out all trace points in FixedIdDefinitionsTestApp"); //$NON-NLS-1$
		modifyFixedIdDefinitionsTestApp(0, 0);
	}
}
