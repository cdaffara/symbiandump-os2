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


import com.nokia.tracecompiler.TraceCompilerRootException;
import com.nokia.tracecompiler.engine.*;
import static org.junit.Assert.*;
import java.io.*;

import org.junit.Test;

import com.nokia.tracecompiler.project.*;


public class TraceLocationConverterTest { 

	
	public static void main(String args[]) {		
	      	org.junit.runner.JUnitCore.main(TraceLocationConverterTest.class.getName());
	    }
	
	@Test
    public void searchForOldGroupIdsTest() {  
		
		
		try {
			GroupNames.initialiseGroupName();
		} catch (TraceCompilerRootException e) {
			fail();
		}
		
		
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds(""));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("                              "));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("123456789"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_FATAL=0x1"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_IMPORTANT=0x2"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_NORMAL=0x77"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_PERFORMANCE=0x4"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_STATE=0x5"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_STATE=5"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_STATE=5555"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_STATE=565nh"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_API=0x6"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_FLOW=0x7"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_DETAILED=0x8"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_DEBUG=0x9"));
		assertFalse("Test case 1",TraceLocationConverter.searchForOldGroupIds("USER_DEFINED_TRACE=0xA1"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("USER_DEFINED_TRACE=0xEA"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_FATAL=0x81"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_ERROR=0x82"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_WARNING=0x83"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_BORDER=0x84"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_API=0x84"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_IMPORTANT=0x85"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_NORMAL=0x86"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_STATE=0x87"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_INTERNALS=0x88"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_DUMP=0x89"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_DEBUG=0x89"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_FLOW=0x8a"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_PERFORMANCE=0x8b"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_ADHOC=0x8c"));
		assertTrue("Test case 1",TraceLocationConverter.searchForOldGroupIds("TRACE_EXTENSION=0x8d"));
	}
	
}

