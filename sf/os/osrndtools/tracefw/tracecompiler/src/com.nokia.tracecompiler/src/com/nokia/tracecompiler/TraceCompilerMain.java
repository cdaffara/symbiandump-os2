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
 * TraceCompiler command-line main class
 *
 */
package com.nokia.tracecompiler;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.nokia.tracecompiler.document.FileDocumentMonitor;
import com.nokia.tracecompiler.document.StringDocumentFactory;
import com.nokia.tracecompiler.engine.TraceCompilerEngineEvents;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineInterface;
import com.nokia.tracecompiler.engine.TraceLocationList;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.plugin.TraceCompilerPlugin;
import com.nokia.tracecompiler.project.GroupNames;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.utils.DocumentFactory;
import com.nokia.tracecompiler.utils.TraceCompilerVersion;

/**
 * TraceCompiler command-line main class
 * 
 */
public class TraceCompilerMain {
	
	/** line separator */
	private static String LINE_SEPARATOR =  System.getProperty("line.separator"); //$NON-NLS-1$
	
	/**
	 * Index of third character
	 */
	private static final int INDEX_OF_THIRD_CHARACTER = 2;

	/**
	 * Index of first character
	 */
	private static final int INDEX_OF_FIRST_CHARACTER = 0;
	
	/**
	 * TraceCompiler options
	 */
	
	/**
	 * UID : it is used to create the dictionary file
	 */
	private static final String UID_SWITCH = Messages.getString("TraceCompiler.UidStwich"); //$NON-NLS-1$
	private static final String UID_SWITCH_TEXT = Messages.getString("TraceCompiler.UidText"); //$NON-NLS-1$
	/**
	 * project name : it is used to create the dictionary file
	 */
	private static final String PRJ_NAME_SWITCH = Messages.getString("TraceCompiler.ProjectSwitch");//$NON-NLS-1$
	private static final String PRJ_NAME_SWITCH_TEXT = Messages.getString("TraceCompiler.ProjectText"); //$NON-NLS-1$
	/**
	 * mmp file path:  may be used to compute the traces folder
	 */
	private static final String MMP_PATH_SWITCH = Messages.getString("TraceCompiler.MmpSwitch");//$NON-NLS-1$
	private static final String MMP_PATH_SWITCH_TEXT = Messages.getString("TraceCompiler.MmpText"); //$NON-NLS-1$
	/**
	 * traces folder: absolute or relative to the mmp folder
	 */
	private static final String TRACES_PATH_SWITCH = Messages.getString("TraceCompiler.TracesSwitch");//$NON-NLS-1$
	private static final String TRACES_PATH_SWITCH_TEXT = Messages.getString("TraceCompiler.TracesText"); //$NON-NLS-1$
	

	/**
	 * Version option
	 */
	private static final String VERSION_OPTION = Messages.getString("TraceCompiler.VersionSwitchLong"); //$NON-NLS-1$
	private static final String VERSION_OPTION_SF = Messages.getString("TraceCompiler.VersionSwitchShort"); //$NON-NLS-1$
	private static final String VERSION_OPTION_INSTRUCTION_TEXT = Messages.getString("TraceCompiler.VersionText"); //$NON-NLS-1$

	
	/**
	 * help option
	 */
	private static final String HELP_OPTION = Messages.getString("TraceCompiler.HelpSwicthLong"); //$NON-NLS-1$
	private static final String HELP_OPTION_SF = Messages.getString("TraceCompiler.HelpSwitchShort"); //$NON-NLS-1$
	private static final String HELP_OPTION_INSTRUCTION_TEXT = Messages.getString("TraceCompiler.HelpText"); //$NON-NLS-1$
	
	/**
	 * Verbose option
	 */
	private static final String VERBOSE_OPTION = Messages.getString("TraceCompiler.VerboseSwitchLong"); //$NON-NLS-1$
	private static final String VERBOSE_OPTION_SF = Messages.getString("TraceCompiler.VerboseSwitchShort"); //$NON-NLS-1$
	private static final String VERBOSE_OPTION_INSTRUCTION_TEXT = Messages.getString("TraceCompiler.VerboseText"); //$NON-NLS-1$

	/**
	 * keep going option
	 */
	private static final String STOP_ON_ERROR_OPTION = Messages.getString("TraceCompiler.StopSwitchLong"); //$NON-NLS-1$
	private static final String STOP_ON_ERROR_OPTION_SF = Messages.getString("TraceCompiler.StopSwitchShort"); //$NON-NLS-1$
	private static final String STOP_ON_ERROR_OPTION_INSTRUCTION_TEXT = Messages.getString("TraceCompiler.StopText"); //$NON-NLS-1$

	

	/**
	 * Version text
	 */
	private static final String VERSION_TEXT = Messages.getString("TraceCompiler.DisplayVersionText"); //$NON-NLS-1$

	/**
	 * Option instruction text
	 */
	private static final String OPTION_INSTRUCTION_TEXT = Messages.getString("TraceCompiler.Options"); //$NON-NLS-1$
	private static final String VALUE=Messages.getString("TraceCompiler.Value"); //$NON-NLS-1$
	

	
	private static final String USAGE = Messages.getString("TraceCompiler.Usage") + LINE_SEPARATOR + //$NON-NLS-1$
	Messages.getString("TraceCompiler.UsageText") + LINE_SEPARATOR + //$NON-NLS-1$
										"\t" + OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR + //$NON-NLS-1$
										"\t" + HELP_OPTION_SF  + ", " + HELP_OPTION + ", " + HELP_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR +   //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$
										"\t" + VERSION_OPTION_SF + ", " + VERSION_OPTION + "\t" +  VERSION_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR +   //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$
										"\t" + VERBOSE_OPTION_SF + ", " + VERBOSE_OPTION + "\t\t" + VERBOSE_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR +  //$NON-NLS-1$//$NON-NLS-2$ //$NON-NLS-3$
										"\t" + STOP_ON_ERROR_OPTION_SF + ", " + STOP_ON_ERROR_OPTION + "\t" + STOP_ON_ERROR_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR +  //$NON-NLS-1$//$NON-NLS-2$ //$NON-NLS-3$
										"\t" + UID_SWITCH + "=" + VALUE + "\t" + UID_SWITCH_TEXT +  LINE_SEPARATOR + //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$
										"\t" + PRJ_NAME_SWITCH + "=" + VALUE + "\t" + PRJ_NAME_SWITCH_TEXT +  LINE_SEPARATOR + //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$
										"\t" + MMP_PATH_SWITCH + "=" + VALUE + "\t" + MMP_PATH_SWITCH_TEXT +  LINE_SEPARATOR + //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$
										"\t" + TRACES_PATH_SWITCH + "=" + VALUE + "\t" + TRACES_PATH_SWITCH_TEXT; //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$



	/**
	 * End of source files tag
	 */
	private static final String ENDOFSOURCEFILES = Messages.getString("TraceCompiler.EndOfList"); //$NON-NLS-1$

	/**
	 * MMP file extension
	 */
	private static final String MMP_FILE_TYPE = Messages.getString("TraceCompiler.MmpExtension"); //$NON-NLS-1$

	/**
	 * Decode plugins path
	 */
	private String DECODE_PLUGINS_PATH = Messages.getString("TraceCompiler.PluginPath"); //$NON-NLS-1$

	/**
	 * Decode plugin name tag
	 */
	private String DECODE_PLUGIN_NAME_TAG = Messages.getString("TraceCompiler.DecodeText1"); //$NON-NLS-1$

	/**
	 * Decode plugin class name tag
	 */
	private String DECODE_PLUGIN_CLASS_NAME_TAG = Messages.getString("TraceCompiler.DecodeText2"); //$NON-NLS-1$

	/**
	 * Decode plugin engine class name template
	 */
	private String ENGINE_CLASS_NAME_TEMPLATE = DECODE_PLUGIN_NAME_TAG
			+ "Engine"; //$NON-NLS-1$

	/**
	 * Decode plugin engine file name template
	 */
	private String ENGINE_FILE_NAME_TEMPLATE = DECODE_PLUGIN_CLASS_NAME_TAG
			+ ".class"; //$NON-NLS-1$

	/**
	 * Decode plugins class template
	 */
	private String CLASS_TEMPLATE = Messages.getString("TraceCompiler.DecodePluginsNameSpace") + DECODE_PLUGIN_NAME_TAG + "." + DECODE_PLUGIN_CLASS_NAME_TAG; //$NON-NLS-1$ //$NON-NLS-2$	
	
	//switch with value such as --uid=value
	private Pattern valueSwitchPattern = Pattern.compile("(--\\S+)=(\\S+)"); //$NON-NLS-1$
	//switches with no values such -v, --version
	private Pattern singleSwitchPattern = Pattern.compile("-{1,2}([^=]+)"); //$NON-NLS-1$
	
	/**
	 * list of source files
	 */
	private ArrayList<String> sources = new ArrayList<String>();
	/**
	 * traces path
	 */
	private String traces_path = null;
	
	
	
	
	/**
	 * Main function
	 * 
	 * @param args
	 *            the command line arguments
	 */
	public static void main(String[] args) {
		if (args.length == 0) {
			printUsage();
			return;
		}
		
		ArrayList<String> list = null;
		
		//support spaces in switches, build a long string, clean it and convert it back to array - a bit expensive
		String tmpString = "";  //$NON-NLS-1$
		for (int i = 0; i < args.length; i++) {
			tmpString = tmpString + " " + args[i];  //$NON-NLS-1$
		}
		tmpString = tmpString.replaceAll("\\s*=\\s*", "="); //$NON-NLS-1$ //$NON-NLS-2$
		tmpString = tmpString.replaceAll("\\s+", " "); //$NON-NLS-1$ //$NON-NLS-2$
		String[] split = tmpString.trim().split(" "); //$NON-NLS-1$
		tmpString = null; // not needed anymore
		
		//rebuild array of arguments
		if (split.length > 0) {
			list = new ArrayList<String>(Arrays.asList(split));
		}
				
		long startTime = System.currentTimeMillis();
		//create a new session of TraceCompiler
		TraceCompilerMain console = new TraceCompilerMain();
		
		try {
			console.parseCommandLine(list);
		} catch (Exception e) { //should cover IOException and TraceCompilerIllegalArgumentsException
			//There is no point to continue if there are problems with the arguments.
			TraceCompilerLogger.printError(e.getMessage());
			printUsage();
			System.exit(1);
		}
		
		boolean error = false;
		try {
			if(list.size() != 0) {
				console.createPlugins();
				console.start();
				console.buildTraceFiles();				
			}
		} catch (Exception e) {
			if (e instanceof TraceCompilerRootException) {
				TraceCompilerLogger.printError(e.getMessage()); 
			} //else the error should have been reported earlier
			error = true;
		} finally {
			try {
				if (!error) { //check if errors have been logged by EventEngine
					TraceCompilerEngineEvents events = TraceCompilerEngineGlobals.getEvents();
					if (events != null && events.hasErrorHappened()) {
						error = true;
					}
				}
				console.shutdown();
			} catch (TraceCompilerException e) {
				error = true;
			}
		}

		if (console.projectName != null) {
			TraceCompilerLogger.printMessage(console.projectName + Messages.getString("TraceCompiler.Took") //$NON-NLS-1$
					+ (System.currentTimeMillis() - startTime) + Messages.getString("TraceCompiler.MS")); //$NON-NLS-1$
		}
		if (error) {
			System.exit(1);
		} else {
			System.exit(0);
		}	
	}

	/**
	 * With Eclipse, the plug-ins are loaded by Eclipse framework. Here they
	 * must be manually created and started
	 */
	private ArrayList<TraceCompilerPlugin> plugIns = new ArrayList<TraceCompilerPlugin>();

	/**
	 * Model listener
	 */
	private TraceCompilerModelListener modelListener;

	/**
	 * Name of the component
	 */
	private String projectName = null;
	

	/**
	 * UID of the component
	 */
	private long componentUID = 0L;

	/**
	 * Component path
	 */
	private String componentPath = null;

	/**
	 * MMP file path
	 */
	private File mmpPath = null;

	/**
	 * Constructor
	 */
	TraceCompilerMain() {

		// Creates listeners and preferences
		modelListener = new TraceCompilerModelListener();
	}

	/**
	 * Creates the plug-ins to be registered with TraceCompiler
	 * @throws TraceCompilerRootException if fail to create a valid plugins
	 */
	private void createPlugins() throws TraceCompilerRootException {
		
		// Get location of the TraceCompiler
		URL path = getClass().getProtectionDomain().getCodeSource()
				.getLocation();
		String decodePluginsPath = path.getPath();

		// If first character is forward slash and it is located before drive
		// letter remove it
		if (decodePluginsPath.charAt(INDEX_OF_FIRST_CHARACTER) == SourceConstants.FORWARD_SLASH_CHAR
				&& decodePluginsPath.charAt(INDEX_OF_THIRD_CHARACTER) == SourceConstants.COLON_CHAR) {
			decodePluginsPath = decodePluginsPath.substring(1);
		}
		
		// Concatenate decode plugins path
		decodePluginsPath = decodePluginsPath.concat(DECODE_PLUGINS_PATH);
		
		// Replace slashes with correct separator character
		decodePluginsPath = decodePluginsPath.replace(
				SourceConstants.FORWARD_SLASH_CHAR, File.separatorChar);
		decodePluginsPath = decodePluginsPath.replace(
				SourceConstants.BACKSLASH_CHAR, File.separatorChar);
		File decodePluginsDir = new File(decodePluginsPath);
		String[] decodePlugins = decodePluginsDir.list();
		if (decodePlugins != null) {
			for (int i = 0; i < decodePlugins.length; i++) {

				// Get decode plugin name
				String decodePluginName = decodePlugins[i];

				// Get decode plugin path
				String decodePluginPath = decodePluginsPath
						+ File.separatorChar + decodePluginName;

				// Decode plugin must be in own directory
				Boolean isDirectory = (new File(decodePluginPath))
						.isDirectory();
				if (isDirectory) {

					// Construct decode plugin engine class name
					String engineClassName = ENGINE_CLASS_NAME_TEMPLATE
							.replaceFirst(DECODE_PLUGIN_NAME_TAG,
									decodePluginName.substring(0, 1)
											.toUpperCase()
											+ decodePluginName.substring(1));

					// Construct decode plugin engine file name
					String engineFileName = ENGINE_FILE_NAME_TEMPLATE
							.replaceFirst(DECODE_PLUGIN_CLASS_NAME_TAG,
									engineClassName);
					String engineFileFullName = decodePluginPath
							+ File.separatorChar + engineFileName;

					// Check does engine file exist
					Boolean exists = (new File(engineFileFullName)).exists();
					if (exists) {
						String engineClassFullName = CLASS_TEMPLATE
								.replaceFirst(DECODE_PLUGIN_NAME_TAG,
										decodePluginName).replaceFirst(
										DECODE_PLUGIN_CLASS_NAME_TAG,
										engineClassName);
						try {
							Class<?> engineClass = Class
									.forName(engineClassFullName);
							TraceCompilerPlugin engine = (TraceCompilerPlugin) engineClass
									.newInstance();
							plugIns.add(engine);
							TraceCompilerLogger.printInfo(Messages.getString(Messages.getString("TraceCompiler.DecodePlugin") + engineClassFullName + Messages.getString("TraceCompiler.Added"))); //$NON-NLS-1$ //$NON-NLS-2$
						} catch (Exception e) {
							String msg = Messages.getString("TraceCompiler.DecodePlugin" + engineClassFullName + Messages.getString("TraceCompiler.AddingFailed")); //$NON-NLS-1$ //$NON-NLS-2$
							throw new TraceCompilerRootException(msg, e); 
						}
					} else {
						String msg = Messages.getString("TraceCompiler.DecodePluginFile") + Messages.getString("TraceCompiler.EngineFullName") + engineFileFullName + Messages.getString("TraceCompiler.DoesNotExist"); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
						throw new TraceCompilerRootException(msg, null);
					}
				}
			}
		}
	}

	/**
	 * Parses the command line
	 * 
	 * @param args
	 *            the arguments
	 * @throws TraceCompilerRootException if arguments are invalid
	 * @throws IOException 
	 */
	private void parseCommandLine(ArrayList<String> list) throws TraceCompilerIllegalArgumentsException, IOException {
		TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.BuildingTracesMess")); //$NON-NLS-1$
		Iterator<String> argIterator = list.iterator();
		
		if (list.size() == 0) {
			printUsage();
			System.exit(0);
		}
		while (argIterator.hasNext()) {
			String element = argIterator.next().trim();
			Matcher m = singleSwitchPattern.matcher(element);

			if (m.matches()) { //it's one of the single switches 
				if (element.equalsIgnoreCase(HELP_OPTION) || element.equalsIgnoreCase(HELP_OPTION_SF)) {
					printUsage();
					System.exit(0);
				}
				if (element.equalsIgnoreCase(VERBOSE_OPTION) || element.equalsIgnoreCase(VERBOSE_OPTION_SF)) {
					TraceCompilerGlobals.setVerbose(true);
					TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.veboseEnabled")); //$NON-NLS-1$
					continue;
				}
				if ( element.equalsIgnoreCase(VERSION_OPTION) || element.equalsIgnoreCase(VERSION_OPTION_SF)) {
					TraceCompilerLogger.printMessage(VERSION_TEXT + TraceCompilerVersion.getVersion());
					System.exit(0);
				}
				if (element.equalsIgnoreCase(STOP_ON_ERROR_OPTION) || element.equalsIgnoreCase(STOP_ON_ERROR_OPTION_SF)) {
					TraceCompilerGlobals.setKeepGoing(false);
					TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.StopOnErrorEnabled")); //$NON-NLS-1$
					continue;
				}
				TraceCompilerLogger.printMessage(Messages.getString("TraceCompiler.UnsupportedSwitch")  + element); //$NON-NLS-1$
			} else {
				m = valueSwitchPattern.matcher(element.trim());
				if (m.matches()) { //it's one of the swithes with values
					if (m.group(1).equalsIgnoreCase(UID_SWITCH)) {
						// UID
						try {
							componentUID = Long.parseLong(m.group(2),TraceCompilerConstants.HEX_RADIX);	
							TraceCompilerLogger.printInfo("Component UID: 0x" + Long.toHexString(componentUID)); //$NON-NLS-1$
							if (componentUID <= 0L) {
								String msg = Messages.getString("TraceCompiler.componentUidIsNotValidExceptionText") + componentUID; //$NON-NLS-1$
								throw new TraceCompilerIllegalArgumentsException(msg, null);
							}
						} catch (NumberFormatException e) {
							String msg = Messages.getString("TraceCompiler.componentUidIsNotValidExceptionText") + componentUID; //$NON-NLS-1$
							throw new TraceCompilerIllegalArgumentsException(msg, null);
						}
					} else {
						if (m.group(1).equalsIgnoreCase(PRJ_NAME_SWITCH)) {
							// project name
							projectName = m.group(2);
							TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.CompNameSet") + projectName); //$NON-NLS-1$
						} else {
							if (m.group(1).equalsIgnoreCase(MMP_PATH_SWITCH)) {
								//mmp path. for the moment only the mmp folder is used.
								mmpPath = new File(m.group(2));
								TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.MMPPathSet") + mmpPath); //$NON-NLS-1$
							} else {
								if (m.group(1).equalsIgnoreCase(TRACES_PATH_SWITCH)) {
									//traces path
									traces_path = m.group(2);
								} else {
									//unsupported switch
									TraceCompilerLogger.printMessage(Messages.getString("TraceCompiler.UnsupportedSwitch") + element); //$NON-NLS-1$
								}
							}
						}
					}

				} else {
					//it must be a file name
					//it's a good time to stop TC here if the file does not exist
					if (!(new File(element)).exists()) {
						throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.sourceFileDoesNotExist") + element, null); //$NON-NLS-1$
					}
					sources.add(element);				
				}	
			}
		}
		
		// by now, if the user wanted just help or version they would have got it and TC stopped
		
		if (componentUID <= 0L) {
			String msg = Messages.getString("TraceCompiler.componentUidIsNotValidExceptionText") + componentUID; //$NON-NLS-1$
			throw new TraceCompilerIllegalArgumentsException(msg, null);
		}
		
		if (projectName == null) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.projectNameMissing"), null); //$NON-NLS-1$
		}
		
		//if files have not been provided , get them from stdin
		if (sources.size() == 0) {
			//get them from the stdin
			readFromStdin();
		}
		
		if (mmpPath == null ) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.mmpPathMissing"), null); //$NON-NLS-1$
		} else if (!isMmpValid()) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.invalidMmpExceptionText") + mmpPath, null); //$NON-NLS-1$
		}
		
		if (traces_path == null ) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.tracesPathMissing"), null); //$NON-NLS-1$
		} else {
			computeTracesPath(traces_path);
		}
		
		if (sources.size() == 0 ) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.fileListMissing"), null); //$NON-NLS-1$
		}
		
		//we have all parameters input and validated, register files.
		registerSourceFiles(sources);		
	}
	
	
	/**
	 * If traces path is relative, work out the full path relative to the location of the mmp file
	 * @param path
	 * @throws TraceCompilerIllegalArgumentsException
	 */
	private void computeTracesPath(String path) throws TraceCompilerIllegalArgumentsException {
		String traces_pathString = path;
		traces_pathString = traces_pathString.replace('/', File.separatorChar);
		traces_pathString = traces_pathString.replace('\\', File.separatorChar);

		File traces_path = new File(traces_pathString);

		Pattern p = Pattern.compile("(([a-zA-Z]:[\\\\/])|([\\\\/])).*"); //$NON-NLS-1$
		Matcher m = p.matcher(traces_pathString);

		if (!m.matches() /*  workaround for isAbsolute in java */) {
			//traces path is relative to mmp location
			traces_pathString = mmpPath.getAbsoluteFile().getParent() + File.separator + traces_pathString;
		}

		traces_path = new File(traces_pathString);

		if (traces_path.isDirectory() && !traces_path.canWrite()) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.TracesPathWriteProtected") + traces_path, null); //$NON-NLS-1$
		}

		boolean dirExists = true;
		if (!traces_path.exists()) {
			dirExists = FileUtils.createDirectories(traces_path);
		}
		if (!dirExists) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.mkdirFailed") + traces_path, null); //$NON-NLS-1$
		}

		//set component path and trace folder
		componentPath = traces_path.getParent();
		ProjectEngine.traceFolderName = traces_path.getName();
		TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.settingComponentPath") + componentPath); //$NON-NLS-1$
		TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.settingTracesPath") + ProjectEngine.traceFolderName); //$NON-NLS-1$
	}

	private static void printUsage() {
		TraceCompilerLogger.printMessage(VERSION_TEXT + TraceCompilerVersion.getVersion());
		TraceCompilerLogger.printMessage(USAGE);
	}

	/**
	 * Read information from STDIN
	 * @throws IOException if fails to read the input
	 * @throws TraceCompilerRootException if the list of files is empty
	 */
	private void readFromStdin() throws IOException, TraceCompilerIllegalArgumentsException {
		TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.ReadingFilesMess")  + ENDOFSOURCEFILES); //$NON-NLS-1$
		// Create reader
		BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));

			String line = stdin.readLine();
						
			while (line != null) {
				line = line.trim();
				if (line.length() > 0) {
					// End of source files received
					if (line.equals(ENDOFSOURCEFILES)) {
						break;
					}
					line = line.replaceAll("\\s+", ""); //$NON-NLS-1$ //$NON-NLS-2$
					Matcher m = valueSwitchPattern.matcher(line.trim());
					//because mmp path and traces path can be very long, we should allow them to be input'ed through stdin too.
					if (m.matches()) { //it's one of the swithes with values
						if (m.group(1).equalsIgnoreCase(MMP_PATH_SWITCH)) {
							//mmp path. for the moment only the mmp folder is used.
							mmpPath = new File(m.group(2));
							TraceCompilerLogger.printInfo(Messages.getString("TraceCompiler.MMPPathSet") + mmpPath); //$NON-NLS-1$
						} else {
							if (m.group(1).equalsIgnoreCase(TRACES_PATH_SWITCH)) {
								//traces path
								traces_path = m.group(2);
							} else {
								//unsupported switch
								TraceCompilerLogger.printMessage(Messages.getString("TraceCompiler.UnsupportedSwitch") + line); //$NON-NLS-1$
							}
						}
					} else {

						//it must be a file name
						//it's a good time to stop TC here if the file does not exist
						if (!(new File(line)).exists()) {
							throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.sourceFileDoesNotExist") + line, null); //$NON-NLS-1$
						}
						sources.add(line);
					}
				}
				// Read new line from STDIN
				line = stdin.readLine();				
			}
		stdin.close();
	}

	/**
	 * Registers source files
	 * 
	 * @param files
	 */
	private void registerSourceFiles(ArrayList<String> files) throws TraceCompilerIllegalArgumentsException {
		if (sources.size() == 0) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.noSourceFilesExceptionText"), null); //$NON-NLS-1$
		}
		if (files.size() > 0) {
			String[] fileArr = new String[files.size()];
			files.toArray(fileArr);

			// Sets the source files to the TraceCompiler document
			// factory. It will create a document from each source in the array
			FileDocumentMonitor.setFiles(fileArr);
			DocumentFactory.registerDocumentFramework(
					new FileDocumentMonitor(), StringDocumentFactory.class);
		}
	}

	/**
	 * Initializes TraceCompiler
	 * @throws TraceCompilerRootException if fail to initialize the plugins
	 * @throws TraceCompilerException 
	 */
	private void start() throws TraceCompilerRootException, TraceCompilerException {

		// Starts TraceCompiler. This is normally called from the Eclipse
		// plug-in
		// activator, but in console case that does not exist
		TraceCompilerEngineGlobals.start();
						
		//Reads the GroupId values from opensystemtrace_types.h
		//If this fails a message is logged and trace compiler stops
		GroupNames.initialiseGroupName();

		
		// Registers a view to TraceCompiler
		TraceCompilerEngineGlobals
				.setView(new TraceCompilerView(componentPath));

		// Registers all plug-in components
		for (TraceCompilerPlugin plugin : plugIns) {
			TraceCompilerEngineGlobals.registerPlugin(plugin);
		}
		// Adds a model event listener
		TraceCompilerEngineGlobals.getTraceModel().addModelListener(
				modelListener);
		TraceCompilerEngineGlobals.getTraceModel().addExtensionListener(
				modelListener);
		TraceCompilerEngineGlobals.getTraceModel().getExtension(
				TraceLocationList.class).addLocationListListener(modelListener);
		TraceCompilerEngineGlobals.getTraceModel().addProcessingListener(modelListener);
	}

	/**
	 * Parses the sources and generates trace files
	 * @throws Exception 
	 */
	private void buildTraceFiles() throws Exception {
		TraceCompilerEngineInterface tbi = TraceCompilerEngineGlobals
				.getTraceCompiler();
		try {
			// Opens a trace project

			// Set project path before opening project
			TraceCompilerEngineGlobals.setProjectPath(componentPath);
			tbi.openProject(projectName);
			TraceModel model = TraceCompilerEngineGlobals.getTraceModel();
			if (model.isValid()) {
				model.setID((int) componentUID);

				// Location errors are printed after a file changes, but the
				// last file is not detected by the listener
				if (modelListener.getErrors().size() > 0) {
					modelListener.printLocationErrors();
					tbi.closeProject();
					throw new TraceCompilerRootException(null, null);
				}
				tbi.exportProject();
				tbi.closeProject();
			} else {
				String msg = Messages.getString("TraceCompiler.ProjectCancelledMess"); //$NON-NLS-1$
				throw new TraceCompilerRootException(msg, null);
			}
		} catch (TraceCompilerException e) {
			TraceCompilerEngineGlobals.getEvents().postError(e);
			throw new TraceCompilerRootException(Messages.getString("TraceCompiler.BuildFailed"), e); //$NON-NLS-1$
		}
	}

	/**
	 * Shuts down TraceCompiler
	 * @throws TraceCompilerException 
	 */
	private void shutdown() throws TraceCompilerException {
		for (TraceCompilerPlugin plugin : plugIns) {
			TraceCompilerEngineGlobals.unregisterPlugin(plugin);
		}
		TraceCompilerEngineGlobals.shutdown();
	}

	/**
	 * Checks if the MMP file is valid
	 * 
	 * @return true if MMP file is valid
	 */
	private boolean isMmpValid() {
		boolean valid = false;
		String pathStr = mmpPath.getAbsolutePath();

		if (mmpPath.exists() && pathStr.length() > MMP_FILE_TYPE.length()) {
			String end = pathStr.substring(pathStr.length() - MMP_FILE_TYPE.length());
			if (end.equalsIgnoreCase(MMP_FILE_TYPE)) {
				valid = true;
			} else {
				TraceCompilerLogger.printError(Messages.getString("TraceCompiler.invalidMmpExceptionText") + mmpPath.getAbsolutePath()); //$NON-NLS-1$
			}
		} else {
			TraceCompilerLogger.printError(Messages.getString("TraceCompiler.InvalidMMP2") + mmpPath.getAbsolutePath()); //$NON-NLS-1$
		}
		return valid;
		
	}

}
