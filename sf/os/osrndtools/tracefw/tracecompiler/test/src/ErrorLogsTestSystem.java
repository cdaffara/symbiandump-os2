/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/


import org.junit.Test;

public class ErrorLogsTestSystem extends ErrorLogsTestBase{
	
	public static void main(String args[]) {		
	      	org.junit.runner.JUnitCore.main(ErrorLogsTestSystem.class.getName());
	    }
	
	/**
	 * This test calls Trace Compiler via Raptor and invokes it against a simple c++ project with one mmp file containing one cpp 
	 * The logs from Raptor (written to std::out) are checked to ensure the errors and warnings from traceCompiler are correct.
	 */
	@Test
	public void SystemTestTraceCompilerAndRaptor (){
		System.out.println ("****************SystemTestTraceCompilerAndRaptor()**********************");
		//Call Raptor
		InvokeCompiler("_sbs");
		
		//Parse the Raptor log files (from standard output)
		ParseLogFile("_sbs");
		
		//Validate the Raptor log files
		ValidateLogs("TraceErrorApp","_sbs");		
	}
	
	/**
	 * This test calls Trace Compiler via Raptor and invokes it against a c++ project with one mmp file containing two cpp files.
	 * The logs from Raptor (written to std::out) are checked to ensure the errors and warnings from traceCompiler are correct.
	 * It is expected that all errors from all ccp files are written to the logs.
	 */
	@Test
	public void SystemTestTraceCompilerAndRaptorMultipleCpps (){
		System.out.println ("**********SystemTestTraceCompilerAndRaptorMultipleCpps()****************");
		//Call Raptor
		InvokeCompiler("_mult_cpp");
		
		//Parse the Raptor log files (from standard output)
		ParseLogFile("_mult_cpp");
		
		//Validate the Raptor log files
		ValidateLogs("MultipleCppTraceErrorApp","_mult_cpp");		
	}
	
	/**
	 * This test calls Trace Compiler via Raptor and invokes it against a c++ project with two mmp files each containing two cpp files.
	 * The logs from Raptor (written to std::out) are checked to ensure the erros and warnings from traceCompiler are correct.
	 * It is expected that all errors from all cpp files in the first mmp which is compiled are written to the logs. Any mmp after that
	 * point is not compiled.
	 */
	@Test
	public void SystemTestTraceCompilerAndRaptorMultipleMmps (){
		System.out.println ("**********SystemTestTraceCompilerAndRaptorMultipleMmps()****************");
		//Call Raptor
		InvokeCompiler("_mult_mmp");
		
		//Parse the Raptor log files (from standard output)
		ParseLogFile("_mult_mmp");
		
		//Validate the Raptor log files
		ValidateLogs("MultipleMmpTraceErrorApps","_mult_mmp");	
	}
}