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
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.junit.BeforeClass;
import org.junit.Test;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.model.TraceCompilerException;


public class MultipleMmpTest {
	
	public static void main(String args[]) {		
	      	org.junit.runner.JUnitCore.main(MultipleMmpTest.class.getName());
	    }
	
	/****************************************************UTILITY FUNCTIONS FOR TESTS************************************************************/
	
	private static String epocroot = null; 
	private static String projectdir = "testdata\\MultipleMmpTestCases\\"; //$NON-NLS-1$
	private static String dictpath= "epoc32\\ost_dictionaries\\"; //$NON-NLS-1$
	private static String autogenpath= "epoc32\\include\\platform\\symbiantraces\\autogen\\"; //$NON-NLS-1$
	private static Pattern versionPattern = Pattern.compile("^.*(\\d+\\.\\d+\\.\\d+).*$"); //$NON-NLS-1$
	private static Pattern oldversionPat = Pattern.compile("^(1\\..*)|(2\\.1.*)"); //$NON-NLS-1$
	//old TC version should be up to 2.12.5 as new functionality was submitted to 2.12.6 (we hope)
	private static Pattern sbsoldversionPat = Pattern.compile("^(1\\..*)|(2\\.[01]\\..*)|(2\\.1[0-2]\\.[0-3].*)"); //$NON-NLS-1$
	private static String TCversion = ""; //$NON-NLS-1$
	private static String SBSversion = ""; //$NON-NLS-1$
	private static File compilerpath;
	private static boolean oldTC = false;
	private static boolean oldBuilder = false; 
	private static HashMap<String, List<File>> headers = new HashMap<String, List<File>>();
	private static HashMap<String, List<File>> dicts = new HashMap<String, List<File>>();
	private static HashMap<String, List<File>> sources = new HashMap<String, List<File>>();
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

		compilerpath = new File (epocroot + "epoc32" + File.separator + "tools" + File.separator +"tracecompiler" + File.separator); //default value to remove warnings. //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$ //$NON-NLS-4$

		ProcessBuilder tc = new ProcessBuilder("java", "-classpath", compilerpath.getPath(), //$NON-NLS-1$//$NON-NLS-2$
				"com.nokia.tracecompiler.TraceCompiler", "-v"); //$NON-NLS-1$//$NON-NLS-2$
		System.out.println("compilerPath= " + compilerpath); //$NON-NLS-1$
		tc.directory(compilerpath);
		tc.redirectErrorStream(true);
		try {
		Process p = tc.start();
		p.waitFor();	

		
		String str = readProcessOutput(p);
		System.out.println("TC version = " + str); //$NON-NLS-1$
		Matcher m = versionPattern.matcher(str.trim());
		if (m.matches()) {
			TCversion = m.group(1);
			System.out.println("TC Version = " + TCversion); //$NON-NLS-1$
		}
		
		m = oldversionPat.matcher(TCversion);
		if (m.matches()){
			oldTC=true;
		}

		System.out.println("TC version = " + TCversion); //$NON-NLS-1$ 
		System.out.println("OLD TC version = " + oldTC); //$NON-NLS-1$

		ProcessBuilder sbs = new ProcessBuilder("sbs.bat","-v"); //$NON-NLS-1$ //$NON-NLS-2$
		sbs.directory(compilerpath);
		sbs.redirectErrorStream(true);
			
		//start the compiler
		p = sbs.start();
		p.waitFor();
			
		InputStream inp = p.getInputStream();
			
	        str = ""; //$NON-NLS-1$
	        int c;
	        //read the output from the compiler into the input stream
	        while ((c = inp.read()) != -1) {
	           	 str= str +((char)c);
	        }

		System.out.println("SBS version = " + str); //$NON-NLS-1$
		m = versionPattern.matcher(str.trim());
		if (m.matches()) {
			SBSversion = m.group(1);
		}
		
		m = sbsoldversionPat.matcher(SBSversion);
		if (m.matches()){
			oldBuilder=true;
		}
		
		} catch (Exception e) {// Catch exception if any
			System.err.println(e.getMessage());
		}
	}
	
	
	public void createListHeadersDicts(String builder)
	{
		//The whole logic of what is expected is built here whether we build with sbs 
		//new TC or old, The structure is re-built for each case.
		
		
		Matcher m = sbsoldversionPat.matcher(SBSversion);
		if (m.matches()){
			oldBuilder=true;
		} else {
			oldBuilder=false;
		}
		
		
		System.out.println("OLD Builder :" + builder + ": = " + oldBuilder); //$NON-NLS-1$
		
		File tracesHeader1;
		File tracesHeader2;
		File tracesHeader3;
		File tracesHeader4;
		File source1;
		File source2;
		File source3;
		File source4;
		File ostTraceDefinitions;
		File fixedidDefinitions;
		File ostDict1;
		File ostDict2;
		File autogenDict1;
		File autogenDict2;
		String loc = "traces\\"; //$NON-NLS-1$
		
		source1 = new File (epocroot+projectdir+"mmp_traces\\src\\MultipleMmpApp1.cpp"); //$NON-NLS-1$
		source2 = new File (epocroot+projectdir+"mmp_traces\\src\\ExtraCppFile1.cpp"); //$NON-NLS-1$
		source3 = new File (epocroot+projectdir+"mmp_traces\\src\\MultipleMmpApp2.cpp"); //$NON-NLS-1$
		source4 = new File (epocroot+projectdir+"mmp_traces\\src\\ExtraCppFile2.cpp"); //$NON-NLS-1$
		sources.put("_traces", Arrays.asList(source1, source2, source3, source4));
		
		tracesHeader1 = new File (epocroot+projectdir+"mmp_traces\\traces\\MultipleMmpApp1Traces.h"); //$NON-NLS-1$
		tracesHeader2 = new File (epocroot+projectdir+"mmp_traces\\traces\\ExtraCppFile1Traces.h"); //$NON-NLS-1$
		tracesHeader3 = new File (epocroot+projectdir+"mmp_traces\\traces\\MultipleMmpApp2Traces.h"); //$NON-NLS-1$
		tracesHeader4 = new File (epocroot+projectdir+"mmp_traces\\traces\\ExtraCppFile2Traces.h"); //$NON-NLS-1$
		
		ostTraceDefinitions = new File(epocroot+projectdir+"mmp_traces\\traces\\OstTraceDefinitions.h"); //$NON-NLS-1$
		fixedidDefinitions = new File(epocroot+projectdir+"mmp_traces\\traces\\fixed_id.definitions"); //$NON-NLS-1$)
		headers.put("_traces", Arrays.asList(tracesHeader1,tracesHeader2,tracesHeader3,tracesHeader4,ostTraceDefinitions,fixedidDefinitions)); //$NON-NLS-1$
		
		ostDict1     = new File(epocroot+dictpath+"mmp_traces1_0xe8576d96_Dictionary.xml"); //$NON-NLS-1$
		ostDict2     = new File(epocroot+dictpath+"mmp_traces2_0xe8576d95_Dictionary.xml"); //$NON-NLS-1$	
		autogenDict1 = new File(epocroot+autogenpath+"mmp_traces1_0xe8576d96_TraceDefinitions.h"); //$NON-NLS-1$	
		autogenDict2 = new File(epocroot+autogenpath+"mmp_traces2_0xe8576d95_TraceDefinitions.h"); //$NON-NLS-1$	
		dicts.put("_traces", Arrays.asList(ostDict1, ostDict2, autogenDict1, autogenDict2)); //$NON-NLS-1$
		
		//=============================
		
		if (!oldBuilder && !oldTC) {
			loc = "traces_mmp_traces_mmpname1\\"; //$NON-NLS-1$
		}
		tracesHeader1 = new File (epocroot+projectdir+"mmp_traces_mmpname\\"+ loc + "MultipleMmpApp1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader2 = new File (epocroot+projectdir+"mmp_traces_mmpname\\" + loc + "ExtraCppFile1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		if (!oldBuilder && !oldTC) {
			loc = "traces_mmp_traces_mmpname2\\"; //$NON-NLS-1$
		}
		tracesHeader3 = new File (epocroot+projectdir+"mmp_traces_mmpname\\" + loc + "MultipleMmpApp2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader4 = new File (epocroot+projectdir+"mmp_traces_mmpname\\" + loc + "ExtraCppFile2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		ostTraceDefinitions = new File(epocroot+projectdir+"mmp_traces_mmpname\\" + loc + "OstTraceDefinitions.h"); //$NON-NLS-1$
		fixedidDefinitions = new File(epocroot+projectdir+"mmp_traces_mmpname\\" + loc + "fixed_id.definitions"); //$NON-NLS-1$)

		headers.put("_traces_mmpname", Arrays.asList(tracesHeader1,tracesHeader2,tracesHeader3,tracesHeader4,ostTraceDefinitions,fixedidDefinitions)); //$NON-NLS-1$
		
		source1 = new File (epocroot+projectdir+"mmp_traces_mmpname\\src\\MultipleMmpApp1.cpp"); //$NON-NLS-1$
		source2 = new File (epocroot+projectdir+"mmp_traces_mmpname\\src\\ExtraCppFile1.cpp"); //$NON-NLS-1$
		source3 = new File (epocroot+projectdir+"mmp_traces_mmpname\\src\\MultipleMmpApp2.cpp"); //$NON-NLS-1$
		source4 = new File (epocroot+projectdir+"mmp_traces_mmpname\\src\\ExtraCppFile2.cpp"); //$NON-NLS-1$
		sources.put("_traces_mmpname", Arrays.asList(source1, source2, source3, source4));
		
		ostDict1     = new File(epocroot+dictpath+"mmp_traces_mmpname1_0xe8576d96_Dictionary.xml"); //$NON-NLS-1$
		ostDict2     = new File(epocroot+dictpath+"mmp_traces_mmpname2_0xe8576d95_Dictionary.xml"); //$NON-NLS-1$	
		autogenDict1 = new File(epocroot+autogenpath+"mmp_traces_mmpname1_0xe8576d96_TraceDefinitions.h"); //$NON-NLS-1$	
		autogenDict2 = new File(epocroot+autogenpath+"mmp_traces_mmpname2_0xe8576d95_TraceDefinitions.h"); //$NON-NLS-1$	
		dicts.put("_traces_mmpname", Arrays.asList(ostDict1, ostDict2, autogenDict1, autogenDict2)); //$NON-NLS-1$

	    //=============================
	    
	    loc = "traces\\"; //$NON-NLS-1$
		if (!oldTC) {
			loc = "traces\\target7_dll\\"; //$NON-NLS-1$
		}
		tracesHeader1 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\" + loc + "MultipleMmpApp1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader2 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\" + loc + "ExtraCppFile1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		if (!oldTC) {
			loc = "traces\\target8_dll\\"; //$NON-NLS-1$
		}
		tracesHeader3 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\" + loc + "MultipleMmpApp2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader4 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\" + loc + "ExtraCppFile2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		ostTraceDefinitions = new File(epocroot+projectdir+"mmp_traces_slash_target_ext\\" + loc + "OstTraceDefinitions.h"); //$NON-NLS-1$
		fixedidDefinitions = new File(epocroot+projectdir+"mmp_traces_slash_target_ext\\" + loc + "fixed_id.definitions"); //$NON-NLS-1$)

		headers.put("_traces_slash_target_ext", Arrays.asList(tracesHeader1,tracesHeader2,tracesHeader3,tracesHeader4,ostTraceDefinitions,fixedidDefinitions)); //$NON-NLS-1$
		
		source1 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\src\\MultipleMmpApp1.cpp"); //$NON-NLS-1$
		source2 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\src\\ExtraCppFile1.cpp"); //$NON-NLS-1$
		source3 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\src\\MultipleMmpApp2.cpp"); //$NON-NLS-1$
		source4 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\src\\ExtraCppFile2.cpp"); //$NON-NLS-1$
		sources.put("_traces_slash_target_ext", Arrays.asList(source1, source2, source3, source4));

		
		String name1 = "";
		String name2 = "";
		if (oldTC) {
			name1 = "mmp_traces_slash_target_ext1";
			name2 = "mmp_traces_slash_target_ext2";
		} else {
			name1 = "target7_dll";
			name2 = "target8_dll";
		}
		
		ostDict1     = new File(epocroot+dictpath + name1 + "_0xe8576d96_Dictionary.xml"); //$NON-NLS-1$
		ostDict2     = new File(epocroot+dictpath + name2 + "_0xe8576d95_Dictionary.xml"); //$NON-NLS-1$	
		autogenDict1 = new File(epocroot+autogenpath + name1 + "_0xe8576d96_TraceDefinitions.h"); //$NON-NLS-1$	
		autogenDict2 = new File(epocroot+autogenpath + name2 + "_0xe8576d95_TraceDefinitions.h"); //$NON-NLS-1$	
		dicts.put("_traces_slash_target_ext", Arrays.asList(ostDict1, ostDict2, autogenDict1, autogenDict2)); //$NON-NLS-1$
		
	    //================================
	    loc = "traces\\"; //$NON-NLS-1$
		if (!oldTC) {
			loc = "traces\\target9_dll\\"; //$NON-NLS-1$
		}
		tracesHeader1 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\" + loc + "MultipleMmpApp1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader2 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\" + loc + "ExtraCppFile1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		if (!oldTC) {
			loc = "traces\\target10_dll\\"; //$NON-NLS-1$
		}
		tracesHeader3 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\" + loc + "MultipleMmpApp1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader4 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\" + loc + "ExtraCppFile1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		ostTraceDefinitions = new File(epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\" + loc + "OstTraceDefinitions.h"); //$NON-NLS-1$
		fixedidDefinitions = new File(epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\" + loc + "fixed_id.definitions"); //$NON-NLS-1$)

		headers.put("_traces_slash_target_ext_commonsource", Arrays.asList(tracesHeader1,tracesHeader2,tracesHeader3,tracesHeader4,ostTraceDefinitions,fixedidDefinitions)); //$NON-NLS-1$
		
		source1 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\src\\MultipleMmpApp1.cpp"); //$NON-NLS-1$
		source2 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\src\\ExtraCppFile1.cpp"); //$NON-NLS-1$
		source3 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\src\\MultipleMmpApp1.cpp"); //$NON-NLS-1$
		source4 = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\src\\ExtraCppFile1.cpp"); //$NON-NLS-1$
		sources.put("_traces_slash_target_ext_commonsource", Arrays.asList(source1, source2, source3, source4));

		
		if (oldTC) {
			name1 = "mmp_traces_slash_target_ext1_cs";
			name2 = "mmp_traces_slash_target_ext2_cs";
		} else {
			name1 = "target9_dll";
			name2 = "target10_dll";
		}
		ostDict1     = new File(epocroot+dictpath + name1 + "_0xe8576d96_Dictionary.xml"); //$NON-NLS-1$
		ostDict2     = new File(epocroot+dictpath + name2 + "_0xe8576d95_Dictionary.xml"); //$NON-NLS-1$	
		autogenDict1 = new File(epocroot+autogenpath + name1 + "_0xe8576d96_TraceDefinitions.h"); //$NON-NLS-1$	
		autogenDict2 = new File(epocroot+autogenpath + name2 + "_0xe8576d95_TraceDefinitions.h"); //$NON-NLS-1$	
		dicts.put("_traces_slash_target_ext_commonsource", Arrays.asList(ostDict1, ostDict2, autogenDict1, autogenDict2)); //$NON-NLS-1$
		
		//==================================
		loc = "traces\\"; //$NON-NLS-1$
		if (!oldTC && !oldBuilder) {
			loc = "traces_target11_exe\\"; //$NON-NLS-1$
		}
		tracesHeader1 = new File (epocroot+projectdir+"mmp_traces_target_type\\" + loc +"MultipleMmpApp1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader2 = new File (epocroot+projectdir+"mmp_traces_target_type\\" + loc +"ExtraCppFile1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		if (!oldTC && !oldBuilder) {
			loc = "traces_target12_exe\\"; //$NON-NLS-1$
		}
		tracesHeader3 = new File (epocroot+projectdir+"mmp_traces_target_type\\" + loc + "MultipleMmpApp2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader4 = new File (epocroot+projectdir+"mmp_traces_target_type\\" + loc + "ExtraCppFile2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		ostTraceDefinitions = new File(epocroot+projectdir+"mmp_traces_target_type\\" + loc + "OstTraceDefinitions.h"); //$NON-NLS-1$
		fixedidDefinitions = new File(epocroot+projectdir+"mmp_traces_target_type\\" + loc + "fixed_id.definitions"); //$NON-NLS-1$)

		headers.put("_traces_target_type", Arrays.asList(tracesHeader1,tracesHeader2,tracesHeader3,tracesHeader4,ostTraceDefinitions,fixedidDefinitions)); //$NON-NLS-1$
		
		
		source1 = new File (epocroot+projectdir+"mmp_traces_target_type\\src\\MultipleMmpApp1.cpp"); //$NON-NLS-1$
		source2 = new File (epocroot+projectdir+"mmp_traces_target_type\\src\\ExtraCppFile1.cpp"); //$NON-NLS-1$
		source3 = new File (epocroot+projectdir+"mmp_traces_target_type\\src\\MultipleMmpApp2.cpp"); //$NON-NLS-1$
		source4 = new File (epocroot+projectdir+"mmp_traces_target_type\\src\\ExtraCppFile2.cpp"); //$NON-NLS-1$
		sources.put("_traces_target_type", Arrays.asList(source1, source2, source3, source4));

		ostDict1     = new File(epocroot+dictpath+"target11_exe_0xe8576d96_Dictionary.xml"); //$NON-NLS-1$
		ostDict2     = new File(epocroot+dictpath+"target12_exe_0xe8576d95_Dictionary.xml"); //$NON-NLS-1$	
		autogenDict1 = new File(epocroot+autogenpath+"target11_exe_0xe8576d96_TraceDefinitions.h"); //$NON-NLS-1$	
		autogenDict2 = new File(epocroot+autogenpath+"target12_exe_0xe8576d95_TraceDefinitions.h"); //$NON-NLS-1$	
		dicts.put("_traces_target_type", Arrays.asList(ostDict1, ostDict2, autogenDict1, autogenDict2)); //$NON-NLS-1$	
		
		//==================================
		
		loc = "traces\\"; //$NON-NLS-1$
		if (!oldTC && !oldBuilder) {
			loc = "traces\\target3_dll\\"; //$NON-NLS-1$
		}
		tracesHeader1 = new File (epocroot+projectdir+"mmp_traces_mixed\\" + loc + "MultipleMmpApp1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader2 = new File (epocroot+projectdir+"mmp_traces_mixed\\" + loc + "ExtraCppFile1Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		if (!oldTC && !oldBuilder) {
			loc = "traces_target4_kext\\"; //$NON-NLS-1$ 
		}
		tracesHeader3 = new File (epocroot+projectdir+"mmp_traces_mixed\\" + loc + "MultipleMmpApp2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$
		tracesHeader4 = new File (epocroot+projectdir+"mmp_traces_mixed\\" + loc + "ExtraCppFile2Traces.h"); //$NON-NLS-1$ //$NON-NLS-2$		
		ostTraceDefinitions = new File(epocroot+projectdir+"mmp_traces_mixed\\" + loc + "OstTraceDefinitions.h"); //$NON-NLS-1$
		fixedidDefinitions = new File(epocroot+projectdir+"mmp_traces_mixed\\" + loc + "fixed_id.definitions"); //$NON-NLS-1$)

		headers.put("_traces_mixed", Arrays.asList(tracesHeader1,tracesHeader2,tracesHeader3,tracesHeader4,ostTraceDefinitions,fixedidDefinitions)); //$NON-NLS-1$		
		
		source1 = new File (epocroot+projectdir+"mmp_traces_mixed\\src\\MultipleMmpApp1.cpp"); //$NON-NLS-1$
		source2 = new File (epocroot+projectdir+"mmp_traces_mixed\\src\\ExtraCppFile1.cpp"); //$NON-NLS-1$
		source3 = new File (epocroot+projectdir+"mmp_traces_mixed\\src\\MultipleMmpApp2.cpp"); //$NON-NLS-1$
		source4 = new File (epocroot+projectdir+"mmp_traces_mixed\\src\\ExtraCppFile2.cpp"); //$NON-NLS-1$
		sources.put("_traces_mixed", Arrays.asList(source1, source2, source3, source4));

		String suffix = ""; //$NON-NLS-1$
		String name = "";
		if (!oldBuilder) {
			suffix = "_dll"; //$NON-NLS-1$
			name = "target3";
		} else {
		        name = "mmp_traces_mixed1";
		}

		ostDict1     = new File(epocroot+dictpath+name + suffix + "_0xe8576d96_Dictionary.xml"); //$NON-NLS-1$
		autogenDict1 = new File(epocroot+autogenpath+name + suffix + "_0xe8576d96_TraceDefinitions.h"); //$NON-NLS-1$
		if (!oldBuilder) {
			suffix = "_kext"; //$NON-NLS-1$
			name = "target4";
		} else {
			name = "mmp_traces_mixed2";
		}
		ostDict2     = new File(epocroot+dictpath+name + suffix + "_0xe8576d95_Dictionary.xml"); //$NON-NLS-1$
		autogenDict2 = new File(epocroot+autogenpath+name + suffix + "_0xe8576d95_TraceDefinitions.h"); //$NON-NLS-1$	
		dicts.put("_traces_mixed", Arrays.asList(ostDict1, ostDict2, autogenDict1, autogenDict2)); //$NON-NLS-1$	
	}
	
	
	/**
	 * This function invokes raptor on the desired test c++ project
	 * 
	 * @param path String specifying the path the compiler needs
	 * 				  to run from
	 */
	public void InvokeCompiler(String path, String builder) {
		System.out.println ("InvokeCompiler() for : " + builder);
		
		List<File> headersList;
		List<File> dictsList;
		try{
			
			//set up the directory from which the process will be called
			if (path.compareTo("_traces")==0){ //$NON-NLS-1$
				headersList = headers.get("_traces"); //$NON-NLS-1$
				System.out.println("deleting file " + headersList.toString()); //$NON-NLS-1$
				for (File header : headersList) {
					if (header.exists()) header.delete();
				}
				
				dictsList = dicts.get("_traces"); //$NON-NLS-1$
				System.out.println("deleting file " + dictsList.toString()); //$NON-NLS-1$
				for (File dict : dictsList) {
					if (dict.exists()) dict.delete();
				}
				compilerpath = new File (epocroot+projectdir+"mmp_traces\\group\\"); //$NON-NLS-1$
			}
			else if (path.compareTo("_traces_mmpname")==0){ //$NON-NLS-1$
				headersList = headers.get("_traces_mmpname"); //$NON-NLS-1$
				System.out.println("deleting file " + headersList.toString()); //$NON-NLS-1$
				for (File header : headersList) {
					if (header.exists()) {
						header.delete();
					}
				}
				
				dictsList = dicts.get("_traces_mmpname");
				System.out.println("deleting file " + dictsList.toString());
				for (File dict : dictsList) {
					if (dict.exists()) {
						dict.delete();
					}
				}
				compilerpath = new File (epocroot+projectdir+"mmp_traces_mmpname\\group\\");
			}
			else if (path.compareTo("_traces_slash_target_ext")==0){
				headersList = headers.get("_traces_slash_target_ext");
				System.out.println("deleting file " + headersList.toString());
				for (File header : headersList) {
					if (header.exists()) header.delete();
				}
				
				dictsList = dicts.get("_traces_slash_target_ext");
				System.out.println("deleting file " + dictsList.toString());
				for (File dict : dictsList) {
					if (dict.exists()) dict.delete();
				}
				compilerpath = new File (epocroot+projectdir+"mmp_traces_slash_target_ext\\group\\");
			}
			else if (path.compareTo("_traces_slash_target_ext_commonsource")==0){
				headersList = headers.get("_traces_slash_target_ext_commonsource");
				System.out.println("deleting file " + headersList.toString());
				for (File header : headersList) {
					if (header.exists()) header.delete();
				}
				
				dictsList = dicts.get("_traces_slash_target_ext_commonsource");
				System.out.println("deleting file " + dictsList.toString());
				for (File dict : dictsList) {
					if (dict.exists()) dict.delete();
				}
				compilerpath = new File (epocroot+projectdir+"mmp_traces_slash_target_ext_commonsource\\group\\");
			}
			else if (path.compareTo("_traces_target_type")==0){
				headersList = headers.get("_traces_target_type");
				System.out.println("deleting file " + headersList.toString());
				for (File header : headersList) {
					if (header.exists()) header.delete();
				}
				
				dictsList = dicts.get("_traces_target_type");
				System.out.println("deleting file " + dictsList.toString());
				for (File dict : dictsList) {
					if (dict.exists()) dict.delete();
				}
				compilerpath = new File (epocroot+projectdir+"mmp_traces_target_type\\group\\");
			}
			else if (path.compareTo("_traces_mixed")==0){
				headersList = headers.get("_traces_mixed");
				System.out.println("deleting file " + headersList.toString());
				for (File header : headersList) {
					if (header.exists()) header.delete();
				}
				
				dictsList = dicts.get("_traces_mixed");
				System.out.println("deleting file " + dictsList.toString());
				for (File dict : dictsList) {
					if (dict.exists()) dict.delete();
				}
				compilerpath = new File (epocroot+projectdir+"mmp_traces_mixed\\group\\");
			}
			else{
			
				System.out.println("Error: Unrecognised test case.");
				fail();
			}
			
			//set up the process builder object
			sbs_build.directory(compilerpath);
			sbs_reallyclean.directory(compilerpath);
			sbs_build.redirectErrorStream(true);
			
			
			
			Process p;
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
				System.out.println("build process failed:"+str);
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
	
	/**
	 * This function checks that the traces header files have been generated in the expected locations
	 * 
	 * @param path  String specifying where to look for the traces header file.  		   
	 */
	public void CheckForTracesHeaders(String path) {
		System.out.println ("CheckForTracesHeaders()");
		System.out.println("Old Builder : " + oldBuilder + "......Old TC : " + oldTC);
		List<File> headersList = headers.get(path);
		List<File> sourceList =  sources.get(path);
		System.out.println("checking files " + headersList.toString());
		boolean error = false;
		if ((path.compareTo("_traces_slash_target_ext")==0 || path.compareTo("_traces_slash_target_ext_commonsource")==0) && oldBuilder && !oldTC){
			System.out.println("No need to check for header files as old sbs cannot call TC");
		} else {
			try {
				TraceCompilerEngineGlobals.start();
			} catch (TraceCompilerException e) {
				fail();
			}
			for (File header : headersList) {
				if (!header.exists()) {
					error = true;
					System.out.println("Error: "+header+" does not exist/hasn't been generated by TraceCompiler");
				} else {
					String licenceInSource = null;
					String licenceInHeader = null;
					File source = null;
					// read the licence from the header file
					if (header.getName().trim().equalsIgnoreCase("OstTraceDefinitions.h")) {
						licenceInSource = TraceCompilerEngineGlobals.getDefaultLicence(true);
						System.out.println("Reading default licence for OstTraceDefinitions.h" + licenceInSource);
					} else {
						if (header.getName().trim().equalsIgnoreCase("fixed_id.definitions")) {
							licenceInSource = TraceCompilerEngineGlobals.getDefaultLicence(false);
							System.out.println("Reading default licence for fixed_id.definitions" + licenceInSource);
						} else {
							licenceInHeader = readFirstMultiLineComment(header.getAbsolutePath());
							System.out.println("Reading licence from " + header.getAbsolutePath() + " == " + licenceInHeader);
							source = sourceList.get(headersList.indexOf(header));

							licenceInSource = readFirstMultiLineComment(source.getAbsolutePath());
							System.out.println("Reading licence from " + source.getAbsolutePath() + " == " + licenceInSource);
							if (licenceInSource == null) {
								//licence should be EPL
								licenceInSource = TraceCompilerEngineGlobals.getDefaultLicence(true);
								System.out.println("Reading default licence for " + header.getAbsolutePath() + " == " + licenceInSource);
							}
						}
					}
					if (licenceInHeader != licenceInHeader) {
						System.out.println("Error: licence in header " + header + " is not the same in source." + source);
						fail();
					}
				}
			}
		
			if(!error){
				System.out.println("All Traces Header files have been generated as expected");
			}
			else {
				fail();
				}
		}
	}


	/**
	 * @param path
	 */
	private String readFirstMultiLineComment(String path) {
		StringBuffer sb = new StringBuffer();
		try {
			FileReader reader = new FileReader(new File(path));
			BufferedReader br = new BufferedReader(reader);
			String line;
			boolean inComment = false;
			
			while ((line = br.readLine()) != null) {
				if (line.trim().startsWith("/*")) {
					inComment = true;
				}
				if (inComment) {
					sb.append(line + System.getProperty("line.separator"));
				}
				if (line.trim().endsWith("*/") && inComment) {
					break;
				}
			}
			br.close();
		} catch (Exception e) {
			System.out.println("Failed to open/read file " + path + "  " + e.getMessage());
			fail();
		}
		String licence = null;
		if (sb.length()> 0) {
			licence = sb.toString();
			if (!licence.contains("Copyright")) {
				licence = null;
			}
		}
			
		return licence;
	}
	
	/**
	 * This function checks that the dictionary files have been generated in the expected locations with the right name
	 * 
	 * @param path.  		   
	 */
	public void CheckForDictionary(String path) {
		System.out.println ("CheckForDictionaries()");

		List<File> dictsList = dicts.get(path);
		System.out.println("checking files " + dictsList.toString());
		boolean error = false;

		if ((path.compareTo("_traces_slash_target_ext")==0 || path.compareTo("_traces_slash_target_ext_commonsource")==0) && oldBuilder && !oldTC){
			System.out.println("No need to check for dictionary as old sbs cannot call TC");
		} else {
			for (File dict : dictsList) {
				if(!dict.exists()) {
					error = true;
					System.out.println("Error: "+dict+" does not exist/hasn't been generated by TraceCompiler");
				}
			}

			if (!error) {
				System.out.println("Dictionary files have been generated as expected");
			}
			else {
				fail();
			}
		}
	}
	

	/****************************************************ACTUAL TESTS************************************************************/
	@Test
	public void MultipleMmpsTest1 (){
		System.out.println ("*********************traces****************************");
		createListHeadersDicts("sbs");
		//Call Raptor
		InvokeCompiler("_traces", "sbs");		
		//Check for header files
		CheckForTracesHeaders("_traces");
		//Check dictionary
		CheckForDictionary("_traces");	
		
	
	}
	

	@Test
	public void MultipleMmpsTest2 (){
		System.out.println ("****************traces_mmpname**********************");
		createListHeadersDicts("sbs");
		//Call Raptor
		InvokeCompiler("_traces_mmpname", "sbs");
		//Check for header files
		CheckForTracesHeaders("_traces_mmpname");
		//Check dictionary
		CheckForDictionary("_traces_mmpname");
		
		
	}
	

	@Test
	public void MultipleMmpsTest3 (){
		System.out.println ("**********traces_slash_target_ext****************");
		createListHeadersDicts("sbs");
		//Call Raptor
		InvokeCompiler("_traces_slash_target_ext", "sbs");
		//Check for header files
		CheckForTracesHeaders("_traces_slash_target_ext");
		//Check dictionary
		CheckForDictionary("_traces_slash_target_ext");	
		
		
	}

	@Test
	public void MultipleMmpsTest4 (){
		System.out.println ("**********traces_slash_target_ext_commonsource****************");
		createListHeadersDicts("sbs");
		//Call Raptor
		InvokeCompiler("_traces_slash_target_ext_commonsource", "sbs");
		//Check for header files
		CheckForTracesHeaders("_traces_slash_target_ext_commonsource");
		//Check dictionary
		CheckForDictionary("_traces_slash_target_ext_commonsource");
		
		
				
	}

	@Test
	public void MultipleMmpsTest5 (){
		System.out.println ("**********traces_target_type****************");
		createListHeadersDicts("sbs");
		//Call Raptor
		InvokeCompiler("_traces_target_type", "sbs");
		//Check for header files
		CheckForTracesHeaders("_traces_target_type");
		//Check dictionary
		CheckForDictionary("_traces_target_type");
		
		
	}

	@Test
	public void MultipleMmpsTest6 (){
		System.out.println ("**********traces_mixed****************");
		createListHeadersDicts("sbs");
		//Call Raptor
		InvokeCompiler("_traces_mixed", "sbs");
		//Check for header files
		CheckForTracesHeaders("_traces_mixed");
		//Check dictionary
		CheckForDictionary("_traces_mixed");	
	}
}
