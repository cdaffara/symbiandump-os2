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

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import org.junit.BeforeClass;
import org.junit.Test;


public class TracesInHeadersTest {
	
		public static void main(String args[]) {		
	      	org.junit.runner.JUnitCore.main(TracesInHeadersTest.class.getName());
	    }
	/****************************************************UTILITY FUNCTIONS FOR TESTS************************************************************/
	
	private static String epocroot = null; 
	private static String projectdir = "testdata\\TracesInHeadersApps\\"; //$NON-NLS-1$
	private static File compilerpath;
	private static ProcessBuilder sbs_build = new ProcessBuilder("sbs.bat","-k","-c","winscw_udeb.tracecompiler");
	private static ProcessBuilder sbs_reallyclean = new ProcessBuilder("sbs.bat","-k","-c","winscw_udeb.tracecompiler", "reallyclean");

	@BeforeClass
	static public void setEnvVariables() {
		epocroot = System.getenv("EPOCROOT"); //$NON-NLS-1$
		if(epocroot == null || (epocroot.length()==0)){
			fail();
		}
		
		// need to check that the path ends in a backslash
		if(!epocroot.endsWith("\\")){ 
			epocroot += "\\"; 
		}
	}
	
	
	/**
	 * This function invokes raptor on the desired test c++ project
	 * 
	 * @param path String specifying the path the compiler needs
	 * 				  to run from
	 */
	public void InvokeCompiler(String path, String builder) {
		System.out.println ("InvokeCompiler() for : " + builder);
		
		try{
			
			//set up the directory from which the process will be called
			if (path.compareTo("_diffnames")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"singlemmp_diffnames\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_samenames")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"singlemmp_samenames\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_diffnames_tracesplus")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"singlemmp_diffnames_tracesplus\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_samenames_tracesplus")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"singlemmp_samenames_tracesplus\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_samenames")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"singlemmp_samenames\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_separate_source")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"multmmp_separate_source\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_comm_source")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"multmmp_comm_source\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_comm_source_tracesplus")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"multmmp_comm_source_tracesplus\\group\\"); //$NON-NLS-1$
			} else if (path.compareTo("_import")==0){ //$NON-NLS-1$
				
				compilerpath = new File (epocroot+projectdir+"singlemmp_import\\group\\"); //$NON-NLS-1$
			} 
			else{
			
				System.out.println("Error: Unrecognised test case.");
				fail();
			}
			
			//set up the process builder object
			sbs_build.directory(compilerpath);
			sbs_reallyclean.directory(compilerpath);
			sbs_build.redirectErrorStream(true);
						
			Process p = null;
			String str = "";
			//start the compiler
			System.out.println("Starting build process ....");

				System.out.println("Running sbs reallyclean on : " + compilerpath + " command: " + sbs_reallyclean.command().toString());
				p = sbs_reallyclean.start();
				readProcessOutput(p);
				System.out.println("Running sbs on : " + compilerpath + " command: " + sbs_build.command().toString());
				p = sbs_build.start();
				str = readProcessOutput(p);
			
			int ret = p.exitValue();
			System.out.println("build process ended....");
			if (ret!=0){
				if (path.compareTo("_import")==0){ //$NON-NLS-1$
					System.out.println("build process expected to fail: "+str);
				} else {
					System.out.println("build process failed:"+str);
					fail();
				}
			}
		}
		
    	catch (Exception e){//Catch exception if any
    		System.err.println("Error: " + e.getMessage());
    		fail();
        }
		
	}


	static private String readProcessOutput(Process p) throws IOException {
		InputStream inp = p.getInputStream();
		
			int c;
			String str = "";	        
			//read the output from the compiler into the input stream
			while ((c = inp.read()) != -1) {
		   	 str= str +((char)c);
			}
		return str;
	}

	

	/****************************************************ACTUAL TESTS************************************************************/
	@Test
	public void TracesInHeradersTest1 (){
		System.out.println ("*********************single mmp with diff names****************************");
		//Call Raptor
		InvokeCompiler("_diffnames", "sbs");
	}
	
	@Test
	public void TracesInHeradersTest2 (){
		System.out.println ("*********************single mmp with same names****************************");
		//Call Raptor
		InvokeCompiler("_samenames", "sbs");
	}
	
	@Test
	public void TracesInHeradersTest3 (){
		System.out.println ("*********************single mmp with diff names and using tarce/<taget>_<ext>****************************");
		//Call Raptor
		InvokeCompiler("_diffnames_tracesplus", "sbs");
	}
	
	@Test
	public void TracesInHeradersTest4 (){
		System.out.println ("*********************single mmp with same names and using tarce/<taget>_<ext>****************************");
		//Call Raptor
		InvokeCompiler("_samenames_tracesplus", "sbs");
	}
	
	@Test
	public void TracesInHeradersTest5 (){
		System.out.println ("*********************multiple mmp with separate source ****************************");
		//Call Raptor
		InvokeCompiler("_separate_source", "sbs");
	}
	
	@Test
	public void TracesInHeradersTest6 (){
		System.out.println ("*********************multiple mmp with common source ****************************");
		//Call Raptor
		InvokeCompiler("_comm_source", "sbs");
	}
	
	@Test
	public void TracesInHeradersTest7 (){
		System.out.println ("*********************multiple mmp with common source and traces/<target>_<ext> ****************************");
		//Call Raptor
		InvokeCompiler("_comm_source_tracesplus", "sbs");
	}
	
	@Test
	public void TracesInHeradersTest8 (){
		System.out.println ("*********************Traces in header exported to epoc32 by another mmp****************************");
		//Call Raptor
		InvokeCompiler("_import", "sbs");
	}
	
}

