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


import static org.junit.Assert.fail;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;

public class ErrorLogsTestBase {
	
	/****************************************************UTILITY FUNCTIONS FOR TESTS************************************************************/
	
	/**
	 * This function gets an environment variable value given the name.
	 * @param variablename The name of the variable for which the value is required.
	 * @return The variable value.
	 */
	public String GetEnvironmentVariablePath(String variablename) {
		String variablevalue = System.getenv(variablename);
		if(variablevalue == null || (variablevalue.length()==0)){
			fail();
		}
		
		// need to check that the path ends in a backslash
		if(!variablevalue.endsWith(File.separator)){ 
			variablevalue += File.separator; 
		}
		return variablevalue;
	}
	
	/**
	 * This function invokes the desired compiler for a particular test case depending
	 * on the string passed
	 * @param compiler String specifying the compiler to run, and also specifying where
	 * 		  		   the logs should be stored.
	 */
	public void InvokeCompiler(String compiler) {
		System.out.println ("InvokeCompiler()");
		//get epocroot location
		String e32=GetEnvironmentVariablePath("EPOCROOT");
		
		String TraceErrorAppPath = e32 + "testdata" + File.separator + "TraceErrorApp" + File.separator;
		try{
			File compilerpath = new File (e32 + "epoc32" + File.separator + "tools" + File.separator + "tracecompiler" + File.separator);
			
			if (compiler.compareTo("_sbs")==0){//raptor
				compilerpath = new File (e32 + "testdata" + File.separator + "TraceErrorApp" + File.separator +"group" + File.separator);
			}
			else if (compiler.compareTo("_mult_cpp")==0){//raptor compiling project with multiple source files
				compilerpath = new File (e32 + "testdata" + File.separator + "MultipleCppTraceErrorApp" + File.separator + "group" + File.separator);
			}
			else if (compiler.compareTo("_mult_mmp")==0){//raptor compiling project with multiple source files
				compilerpath = new File (e32 + "testdata" + File.separator + "MultipleMmpTraceErrorApps" + File.separator + "group" + File.separator);
			}
			else if (compiler.compareTo("_tc")!=0){
				System.out.println("Error: Unrecognised test case. Use either _tc, _sbs, _mult_cpp or _mult_mmp");
				fail();
			}
			
			//set up the process builder object
	 		ProcessBuilder pb; 
	 		//commands to be used to invoke the compiler
	 		if (compiler.compareTo("_tc")==0){//tracecompiler
	 			// These EMMA flags are added, because of EMMA reporting in Hudson
				// EMMA lfags are commented out, because it does not work in Hudson at the moment
//	 			String emmaOutputFileFlag = "-Demma.coverage.out.file=" + e32 + "testdata" + File.separator + "reports" + File.separator + "emma" + File.separator + "coverage.emma";
//	 			String emmaOutputMergeFlag = "-Demma.coverage.out.merge=true";
	 			String emmaOutputFileFlag = "";
	 			String emmaOutputMergeFlag = "";
	 			pb = new ProcessBuilder("java", emmaOutputFileFlag, emmaOutputMergeFlag, "-classpath",".", "com.nokia.tracecompiler.TraceCompiler", "E8576D92", "TraceErrorApp",TraceErrorAppPath+"group"+File.separator+"TraceErrorApp.mmp",TraceErrorAppPath+"src"+File.separator+"TraceErrorApp.cpp");
	 		}
	 		else /*if (compiler.compareTo("_sbs")==0)*/{//for all other raptor cases
	 			pb = new ProcessBuilder("sbs.bat","-c","armv5.tracecompiler");
	 		}
	 		
			pb.directory(compilerpath);
			pb.redirectErrorStream(true);
			
			//start the compiler
			Process p = pb.start();
			InputStream inp = p.getInputStream();
	        int c = -1;
	        
	        
	        String str = "";
	        
	        /*
	        //read the output from the compiler into the input stream
	        while ((c = inp.read()) != -1) {
	            str= str +((char)c);
	        }
	        */
	        
	        StringBuilder sb = new StringBuilder();
	        //read the output from the compiler into the input stream
	        while ((c = inp.read()) != -1) {
	        	sb.append(((char)c));
	        }
	        str = sb.toString();
	        
	        
	    	//Declare output file to write logs to
	    	FileOutputStream outstream; 
			PrintStream ps; 
			System.out.println ("creating output logs in buildlog"+compiler+".txt");
			outstream = new FileOutputStream(e32+"epoc32"+File.separator+"build"+File.separator+"buildlog"+compiler+".txt");
			ps = new PrintStream( outstream );
			ps.println (str);
			
			//check return code if tracecompiler.. we don't check error code for raptor:
			int ret = p.exitValue();
			if ((ret==0)&&(compiler.compareTo("_tc")==0)){
				System.out.println("FAIL: TraceCompiler does not exit with error");
				fail();
			}
			inp.close();
			outstream.close();
		}
		
    	catch (IOException e){//Catch IOException
    		System.err.println("Error: " + e.getMessage());
    		fail();
        }
		
	}
	
	/**
	 * This function parses the log files (as specified by the passed string) for errors and warnings
	 *  and stores them in a separate file (also specified by the same passed string)
	 * 
	 * @param pathext  String specifying the part of the name of the build log file to parse which is 
	 * 				   also part of the name of the file where parsed logs are written to.	  		   
	 */
	public void ParseLogFile(String pathext) {
		System.out.println ("ParseLogFile()");
		//get epocroot location
		String e32=GetEnvironmentVariablePath("EPOCROOT");
		
		
	    try{
	    	// Open the file
	    	System.out.println ("opening buildlog"+pathext+".txt");
	    	FileInputStream fstream = new FileInputStream(e32+"epoc32"+File.separator+"build"+File.separator+"buildlog"+pathext+".txt");

	    	// Get the object of DataInputStream
	    	//System.out.println ("creating input stream");
	    	DataInputStream in = new DataInputStream(fstream);
	        BufferedReader br = new BufferedReader(new InputStreamReader(in));
	    	String strLine;
	    	
	    	//Declare output file to write parsed logs to
	    	FileOutputStream out; 
			PrintStream p; 
			//System.out.println ("creating output stream");
			out = new FileOutputStream(e32+"epoc32"+File.separator+"build"+File.separator+"parsedlogs"+pathext+".txt");
			p = new PrintStream( out );
	    	
	    	//Read File Line By Line and write to parsedlogs.txt
	    	System.out.println ("Writing to parsedlogs"+pathext+".txt");
	    	while ((strLine = br.readLine()) != null&&(strLine.length()) >= 9)   {
	    		//remove any leading white spaces because there are spaces when printed to standard output.
	    		strLine=strLine.trim();
		    	if((strLine.startsWith("error:")==true)||(strLine.startsWith("warning:")==true)){
			    	// Write to a separate file
			    	p.println (strLine);
	    	}
      	
    	}
    	
    	//Close the input and output streams
    	in.close();
    	out.close();
    	}
    	
    	catch (IOException e){//Catch IOException
    		System.err.println("Error: " + e.getMessage());
    		fail();
    	}
	
	}
	
	/**
	 * This function validates the parsed log file by comparing it to a reference file containg
	 * the knows warnings and errors 
	 * @param project String specifying the name of the project folder that was compiled.
	 * @param pathext String specifying the part of the name of the parsed log file to validate.	   
	 */
	public void ValidateLogs(String project, String pathext) {
		System.out.println ("ValidateLogs()");
		//get epocroot location
		String e32=GetEnvironmentVariablePath("EPOCROOT");
		
	try{
		// Open the files
		System.out.println ("opening referencelog.txt and parsedlogs"+pathext+".txt");
		FileInputStream fstream1 = new FileInputStream(e32+"testdata"+File.separator+project+File.separator+"data"+File.separator+"referencelog.txt");
		FileInputStream fstream2 = new FileInputStream(e32+"epoc32"+File.separator+"build"+File.separator+"parsedlogs"+pathext+".txt");
	
		// Get the objects of DataInputStream
		//System.out.println ("creating input streams");
		DataInputStream in1 = new DataInputStream(fstream1);
		DataInputStream in2 = new DataInputStream(fstream2);
		//System.out.println ("creating buffered readers");
	    BufferedReader br1 = new BufferedReader(new InputStreamReader(in1));
	    BufferedReader br2 = new BufferedReader(new InputStreamReader(in2));
		String strLine1;
		String strLine2;
		int i=1;
		
		//compare the two files
		while (((strLine1 = br1.readLine()) != null)&&(((strLine2 = br2.readLine()) != null)))   {
			if (pathext.compareTo("_mult_mmp")!=0){//if this is not the multiple mmp test, then compare the whole error string
				if(strLine1.compareTo(strLine2)==0){
					System.out.println ("Validated line "+i+": "+strLine1);
				}
				else{
					System.out.println ("Line "+i+" ("+strLine2+") is not correct.");
					fail();
				}
			}
			else{//else if this is the multiple mmp test, compare only the last 40 characters of the string because
				//raptor does not have a specified order in which it compiles mmps using trace compiler so we can't validate exactly
				if(strLine1.substring(strLine1.length()-40, strLine1.length()).compareTo(strLine2.substring(strLine1.length()-40, strLine2.length()))==0){
					System.out.println ("Validated line "+i+": "+strLine1);
				}
				else{
					System.out.println ("Line "+i+" ("+strLine2+") is not correct.");
					fail();
				}			
			}
		i++;
		}
		
		if (i==1) {//never entered loop therefore there were no errors or warnings in the build logs
			System.out.println("FAIL: Compiler did not output any correct errors to logs");
			fail();
		}
		in1.close();
		in2.close();
	}
	
	catch (IOException e){//Catch IOException
		System.err.println("Error: " + e.getMessage());
		fail();
    }
	}
}