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


import static org.junit.Assert.*;
import java.io.*;

import org.junit.Test;

import com.nokia.tracecompiler.TraceCompilerRootException;
import com.nokia.tracecompiler.project.*;

public class GroupNamesTest { 

	
	public static void main(String args[]) {		
	      	org.junit.runner.JUnitCore.main(GroupNamesTest.class.getName());
	    }
			
	@Test
	public void testGetGroupNameVector() {
		
	
		try {
			GroupNames.initialiseGroupName();
		} catch (TraceCompilerRootException e) {
			fail();
		}
					
		// test group id values obtain by name
		assertEquals("TRACE_FATAL gid check", GroupNames.getIdByName("TRACE_FATAL"), 129);
		assertEquals("TRACE_ERROR gid check", GroupNames.getIdByName("TRACE_ERROR"), 130);
		assertEquals("TRACE_WARNING gid check", GroupNames.getIdByName("TRACE_WARNING"), 131);
		assertEquals("TRACE_BORDER gid check", GroupNames.getIdByName("TRACE_BORDER"), 132);	    
		assertEquals("TRACE_API gid check", GroupNames.getIdByName("TRACE_API"), 132);
		assertEquals("TRACE_IMPORTANT gid check", GroupNames.getIdByName("TRACE_IMPORTANT"), 133);
		assertEquals("TRACE_NORMAL gid check", GroupNames.getIdByName("TRACE_NORMAL"), 134);
		assertEquals("TRACE_STATE gid check", GroupNames.getIdByName("TRACE_STATE"), 135);
		assertEquals("TRACE_INTERNALS gid check", GroupNames.getIdByName("TRACE_INTERNALS"), 136);
		assertEquals("TRACE_DETAILED", GroupNames.getIdByName("TRACE_DETAILED"), 136);
		assertEquals("TRACE_DUMP gid check", GroupNames.getIdByName("TRACE_DUMP"), 137);
		assertEquals("TRACE_DEBUG gid check", GroupNames.getIdByName("TRACE_DEBUG"), 137);
		assertEquals("TRACE_FLOW gid check", GroupNames.getIdByName("TRACE_FLOW"), 138);
		assertEquals("TRACE_PERFORMANCE gid check", GroupNames.getIdByName("TRACE_PERFORMANCE"), 139);
		assertEquals("TRACE_ADHOC gid check", GroupNames.getIdByName("TRACE_ADHOC"), 140);
		assertEquals("TRACE_EXTENSION gid check", GroupNames.getIdByName("TRACE_EXTENSION"), 141);

		// test GID strings obtained by id
//		assertEquals("TRACE_FATAL string check", GroupNames.getNameById(129), "TRACE_FATAL");
//		assertEquals("TRACE_ERROR string check", GroupNames.getNameById(130), "TRACE_ERROR");
//		assertEquals("TRACE_WARNING string check", GroupNames.getNameById(131), "TRACE_WARNING");
//		assertEquals("TRACE_BORDER string check", GroupNames.getNameById(132), "TRACE_BORDER");	    
//		assertEquals("TRACE_IMPORTANT string check", GroupNames.getNameById(133), "TRACE_IMPORTANT");
//		assertEquals("TRACE_NORMAL string check", GroupNames.getNameById(134), "TRACE_NORMAL");
//		assertEquals("TRACE_STATE string check", GroupNames.getNameById(135), "TRACE_STATE");
//		assertEquals("TRACE_INTERNALS string check", GroupNames.getNameById(136), "TRACE_INTERNALS");
//		assertEquals("TRACE_DUMP string check", GroupNames.getNameById(137), "TRACE_DUMP");
//		assertEquals("TRACE_FLOW string check", GroupNames.getNameById(138), "TRACE_FLOW");
//		assertEquals("TRACE_PERFORMANCE string check", GroupNames.getNameById(139), "TRACE_PERFORMANCE");
//		assertEquals("TRACE_ADHOC string check", GroupNames.getNameById(140), "TRACE_ADHOC");
//		assertEquals("TRACE_EXTENSION string check", GroupNames.getNameById(141), "TRACE_EXTENSION");	
//		assertEquals("TRACE_TESTING1 string check", GroupNames.getNameById(254), "TRACE_TESTING1");
//		assertEquals("TRACE_TESTING2 string check", GroupNames.getNameById(255), "TRACE_TESTING2");	
	}
	
	
	@Test
	public void testUserDefinedRange(){
		// test that the user defined range starts at the expected value
		assertEquals("user defined range check", GroupNames.USER_GROUP_ID_FIRST, 222);		
		assertEquals("user defined range check", GroupNames.USER_GROUP_ID_LAST, 253);
			
	}
	
	@Test
	public void testModifiedOstHeaderFile(){
		// copy a modfified file to epoc32\platform\include, rename ost_types.h, run tests, and then replace original ost_types
	    
		String e32 = System.getenv("EPOCROOT");
		if(e32 == null || (e32.length()==0)){
			fail();
		}
		
		// need to check that e32 ends in a backslash
		if(!e32.endsWith(File.separator)){ 
			e32 += File.separator; 
		}
		
		e32 = e32 + "epoc32"+File.separator+"include"+File.separator+"platform"+File.separator;
		
		System.out.println(e32);
		
		System.setProperty("user.dir", e32);
		
		String curDir = System.getProperty("user.dir");
		System.out.println(curDir);
		
		// File (or directory) with old name
	    File file = new File(e32 + "opensystemtrace_types.h");
	    
	    // File (or directory) with new name
	    File file2 = new File(e32 + "CBR_opensystemtrace_types.h");
	    
	    // File (or directory) with new name
	    File file3 = new File(e32 + "test_opensystemtrace_types.h");
	    
	    // Rename file (or directory)
	    boolean success = file.renameTo(file2);
	    if (!success) {
	        // File was not successfully renamed
	    	fail();
	    }
	    
	    // Rename file (or directory)
	    success = file3.renameTo(file);
	    if (!success) {
	        // File was not successfully renamed
	    	fail();
	    }
	    
		try {
			GroupNames.initialiseGroupName();
		} catch (TraceCompilerRootException e) {
			fail();
		}
		
		// test group id values obtain by name
		assertEquals("TRACE_FATAL gid check", GroupNames.getIdByName("TRACE_FATAL"), 149);
		assertEquals("TRACE_ERROR gid check", GroupNames.getIdByName("TRACE_ERROR"), 150);
		assertEquals("TRACE_WARNING gid check", GroupNames.getIdByName("TRACE_WARNING"), 151);
		assertEquals("TRACE_BORDER gid check", GroupNames.getIdByName("TRACE_BORDER"), 152);	    
		assertEquals("TRACE_API gid check", GroupNames.getIdByName("TRACE_API"), 152);
		assertEquals("TRACE_IMPORTANT gid check", GroupNames.getIdByName("TRACE_IMPORTANT"), 153);
		assertEquals("TRACE_NORMAL gid check", GroupNames.getIdByName("TRACE_NORMAL"), 154);
		assertEquals("TRACE_STATE gid check", GroupNames.getIdByName("TRACE_STATE"), 155);
		assertEquals("TRACE_INTERNALS gid check", GroupNames.getIdByName("TRACE_INTERNALS"), 156);
		assertEquals("TRACE_DETAILED", GroupNames.getIdByName("TRACE_DETAILED"), 156);
		assertEquals("TRACE_DUMP gid check", GroupNames.getIdByName("TRACE_DUMP"), 157);
		assertEquals("TRACE_DEBUG gid check", GroupNames.getIdByName("TRACE_DEBUG"), 157);
		assertEquals("TRACE_FLOW gid check", GroupNames.getIdByName("TRACE_FLOW"), 158);
		assertEquals("TRACE_PERFORMANCE gid check", GroupNames.getIdByName("TRACE_PERFORMANCE"), 159);
		assertEquals("TRACE_ADHOC gid check", GroupNames.getIdByName("TRACE_ADHOC"), 160);
		assertEquals("TRACE_EXTENSION gid check", GroupNames.getIdByName("TRACE_EXTENSION"), 161);

		// test GID strings obtained by id
//		assertEquals("TRACE_FATAL string check", GroupNames.getNameById(149), "TRACE_FATAL");
//		assertEquals("TRACE_ERROR string check", GroupNames.getNameById(150), "TRACE_ERROR");
//		assertEquals("TRACE_WARNING string check", GroupNames.getNameById(151), "TRACE_WARNING");
//		assertEquals("TRACE_BORDER string check", GroupNames.getNameById(152), "TRACE_BORDER");	    
//		assertEquals("TRACE_IMPORTANT string check", GroupNames.getNameById(153), "TRACE_IMPORTANT");
//		assertEquals("TRACE_NORMAL string check", GroupNames.getNameById(154), "TRACE_NORMAL");
//		assertEquals("TRACE_STATE string check", GroupNames.getNameById(155), "TRACE_STATE");
//		assertEquals("TRACE_INTERNALS string check", GroupNames.getNameById(156), "TRACE_INTERNALS");
//		assertEquals("TRACE_DUMP string check", GroupNames.getNameById(157), "TRACE_DUMP");
//		assertEquals("TRACE_FLOW string check", GroupNames.getNameById(158), "TRACE_FLOW");
//		assertEquals("TRACE_PERFORMANCE string check", GroupNames.getNameById(159), "TRACE_PERFORMANCE");
//		assertEquals("TRACE_ADHOC string check", GroupNames.getNameById(160), "TRACE_ADHOC");
//		assertEquals("TRACE_EXTENSION string check", GroupNames.getNameById(161), "TRACE_EXTENSION");
	} 
   
	@Test
    public void tearDown() {
        // release objects under test here, if necessary
		
		String e32 = System.getenv("EPOCROOT");
		if(e32 == null || (e32.length()==0)){
			fail();
		}
		
		// need to check that e32 ends in a backslash
		if(!e32.endsWith(File.separator)){ 
			e32 += File.separator; 
		}
		
		e32 = e32 + "epoc32"+File.separator+"include"+File.separator+"platform"+File.separator;
		
		System.out.println(e32);
		
		System.setProperty("user.dir", e32);
		
		String curDir = System.getProperty("user.dir");
		System.out.println(curDir);
		
		// File (or directory) with old name
	    File file = new File(e32 + "opensystemtrace_types.h");
	    
	    // File (or directory) with new name
	    File file2 = new File(e32 + "CBR_opensystemtrace_types.h");
	    
	    	    
	    boolean i = file.exists();
	    System.out.println(i);
	    
	    // Rename file (or directory)
	    boolean success = (file.delete());
	    if (!success) {
	        fail();
	    }
	    
	    success = file2.renameTo(file);
	    if (!success) {
	        // File was not successfully renamed
	    	fail();
	    }
	    

		
		// File (or directory) with old name
	    
	    
	}
}
