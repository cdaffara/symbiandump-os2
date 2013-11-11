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

public class ErrorLogsTestUnit extends ErrorLogsTestBase {
	
	public static void main(String args[]) {		
	      	org.junit.runner.JUnitCore.main(ErrorLogsTestUnit.class.getName());
	    }
	
	/**
	 * This test calls Trace Compiler directly and invokes it against a simple c++ project with one cpp and one mmp file
	 * The exit code from tracecompiler is checked to ensure an error is indicated and the logs are also checked
	 */
	@Test
	public void UnitTestTraceCompiler (){
		System.out.println ("*********************UnitTestTraceCompiler()****************************");
		//Call Trace Compiler
		InvokeCompiler("_tc");
		
		//Parse the Trace Compiler log files
		ParseLogFile("_tc");
		
		//Validate the Trace Compiler log files
		ValidateLogs("TraceErrorApp","_tc");		
	}
}