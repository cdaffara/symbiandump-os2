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
* JUnit tests for OST MacroWrappers
*
*/


import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import org.junit.Assert;
import org.junit.Test;
import com.nokia.tracecompiler.source.*;
import com.nokia.tracecompiler.document.*;

public class MacroWrappersTest {

	public static void main(String[] args) {
		org.junit.runner.JUnitCore.main(MacroWrappersTest.class.getName());
	}
	
	@Test
	public void testMacroWrappers(){
		
		// this string should parse down to the following members in list	
		final String REFERENCE_STR = 
			"TRACE_DEBUG_ONLY ( mystuff,…, OstTraceExt3( TRACE_DEBUG,DUP7__CONSTRUCTL,\"::ConstructL;x=%d;y=%d;z=%d\",x,y,z ) );";
		FileDocumentMonitor monitor = new FileDocumentMonitor();		
		SourceDocumentFactory factory = monitor.getFactory();

		// construct the reference parameter array
		ArrayList<String> expectedTokens = new ArrayList<String>();
		expectedTokens.add("TRACE_DEBUG");
		expectedTokens.add("DUP7__CONSTRUCTL");
		expectedTokens.add("\"::ConstructL;x=%d;y=%d;z=%d\"");
		expectedTokens.add("x");
		expectedTokens.add("y");
		expectedTokens.add("z");

		// construct the trace string from reference parameters
		final String TRACE_STR = "OstTraceExt3( "+ expectedTokens.get(0) + 
								 "," + expectedTokens.get(1) + 
								 "," + expectedTokens.get(2) +
								 "," + expectedTokens.get(3) + 
								 "," + expectedTokens.get(4) +
								 "," + expectedTokens.get(5) + " )";
				
		// first test the parser on an unwrapped string
		SourceParser sourceParser = new SourceParser(factory, factory.createDocument(TRACE_STR));
		ArrayList<String> actualTokens = new ArrayList<String>();
		// parse string
		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}
						
		checkContents(actualTokens, expectedTokens); 
						
		// Create the wrapped OstTrace...  	
		final String WRAPPED_STR = "TRACE_DEBUG_ONLY ( mystuff,…, " + TRACE_STR + " );";
		
		// do a quick check to see that the reference string matches the string generated from list
		// we could use java.util.StringTokenizer to generate the expected list, 
		// but this is test code for tracecompiler after all
		assertTrue(REFERENCE_STR.compareTo(WRAPPED_STR) == 0 );

		sourceParser = new SourceParser(factory, factory.createDocument(WRAPPED_STR));
		

		actualTokens.clear();
		// this string should parse down to the following members in list
		// [0]	"mystuff"	
		// [1]	"…"	
		// [2]	"OstTraceExt3( TRACE_DEBUG, DUP7__CONSTRUCTL, "::ConstructL;x=%d;y=%d;z=%d",x,y,z )"	
		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}

		assertTrue(actualTokens.size() == 3);
		assertTrue(((String)actualTokens.get(0)).compareTo("mystuff") == 0);
		assertTrue(((String)actualTokens.get(1)).compareTo("…") == 0);
		
		// now check the extracted OstTrace part
		String ostStr = (String)actualTokens.get(2);
		assertTrue(ostStr.compareTo(TRACE_STR) == 0);
		
		sourceParser = new SourceParser(factory, factory.createDocument(ostStr));
		actualTokens.clear();

		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}
						
		checkContents(actualTokens, expectedTokens); 
	}
	
	
	@Test
	public void testMultipleMacroWrappers(){
		
		// testing parsing of macro in this format
		//TRACE_DEBUG_ONLY (OstTrace0( TRACE_DETAILED, DUP5__CONSTRUCTL, "::ConstructL" )
		// , OstTraceExt3( TRACE_BORDER, DUP99__CONSTRUCTL, "::ConstructL;x=%x;y=%x;z=%x",x,y, (x+y) ));
		
		final String REFERENCE_STR = 
			"TRACE_DEBUG_ONLY (OstTrace0( TRACE_DETAILED,DUP5__CONSTRUCTL ),OstTraceExt3( TRACE_BORDER,DUP99__CONSTRUCTL,\"::ConstructL;x=%x;y=%x;z=%x\",x,y,(x+y) ));";

		FileDocumentMonitor monitor = new FileDocumentMonitor();		
		SourceDocumentFactory factory = monitor.getFactory();

		
		// construct the reference parameter array
		ArrayList<String> expectedTokens1 = new ArrayList<String>();
		expectedTokens1.add("TRACE_DETAILED");
		expectedTokens1.add("DUP5__CONSTRUCTL");
		
		
		ArrayList<String> expectedTokens2 = new ArrayList<String>();
		expectedTokens2.add("TRACE_BORDER");
		expectedTokens2.add("DUP99__CONSTRUCTL");
		expectedTokens2.add("\"::ConstructL;x=%x;y=%x;z=%x\"");
		expectedTokens2.add("x");
		expectedTokens2.add("y");
		expectedTokens2.add("(x+y)");

		// construct the trace strings from reference parameters
		final String TRACE_STR1 = "OstTrace0( "+ expectedTokens1.get(0) + 
								 "," + expectedTokens1.get(1) + " )";

		final String TRACE_STR2 = "OstTraceExt3( "+ expectedTokens2.get(0) + 
								  "," + expectedTokens2.get(1) + 
								  "," + expectedTokens2.get(2) +
								  "," + expectedTokens2.get(3) + 
								  "," + expectedTokens2.get(4) +
								  "," + expectedTokens2.get(5) + " )";

		// check it works with TRACE_STR1
		SourceParser sourceParser = new SourceParser(factory, factory.createDocument(TRACE_STR1));
		ArrayList<String> actualTokens = new ArrayList<String>();
		// parse string
		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}
						
		checkContents(actualTokens, expectedTokens1); 

		// check it works with TRACE_STR2
		sourceParser = new SourceParser(factory, factory.createDocument(TRACE_STR2));
		actualTokens.clear();
		
		// parse string
		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}
						
		checkContents(actualTokens, expectedTokens2); 

		// create the composite string
		String TRACE_STR3 = "TRACE_DEBUG_ONLY (" + TRACE_STR1 + "," + TRACE_STR2 + ");";

		// do a quick check to see that the reference string matches the string generated from list
		// we could use java.util.StringTokenizer to generate the expected list, 
		// but this is test code for tracecompiler after all
		assertTrue(REFERENCE_STR.compareTo(TRACE_STR3) == 0);
		
		sourceParser = new SourceParser(factory, factory.createDocument(TRACE_STR3));
		actualTokens.clear();

		// this string should parse down to the following members in list
		// [0]	"OstTraceExt0( TRACE_DETAILED,DUP5__CONSTRUCTL )" (id=77)	
		// [1]	"OstTraceExt3( TRACE_BORDER,DUP99__CONSTRUCTL,"::ConstructL;x=%d;y=%d;z=%d",x,y,z )" (id=78)	
		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}
		
		assertTrue(actualTokens.size() == 2);
		final String OST_STR1 = actualTokens.get(0);
		assertTrue(OST_STR1.compareTo(TRACE_STR1) == 0);
		final String OST_STR2 = actualTokens.get(1);
		assertTrue(OST_STR2.compareTo(TRACE_STR2) == 0);
		
		
		// check parsing of first OST macro
		sourceParser = new SourceParser(factory, factory.createDocument(OST_STR1));
		actualTokens.clear();
		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}

		checkContents(actualTokens, expectedTokens1);
		

		// check parsing of second OST macro
		sourceParser = new SourceParser(factory, factory.createDocument(OST_STR2));
		actualTokens.clear();
		try{
			sourceParser.tokenizeParameters(0, actualTokens, true);			
		}catch(SourceParserException e){
			Assert.fail(e.getMessage());
		}

		checkContents(actualTokens, expectedTokens2);
		
	}

	
	private static void checkContents(final ArrayList<String> ACTUAL, final ArrayList<String> EXPECTED ){
		assertTrue(ACTUAL.size() == EXPECTED.size());
		for(int i=0;i < ACTUAL.size();i++){
			assertTrue( ACTUAL.get(i).compareTo(EXPECTED.get(i)) == 0);			
		}		
	}
}
