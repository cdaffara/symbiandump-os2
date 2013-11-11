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
import java.util.List;

import com.nokia.tracecompiler.document.FileDocumentMonitor;
import com.nokia.tracecompiler.document.StringDocumentFactory;
import com.nokia.tracecompiler.engine.TraceCompilerEngineEvents;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineInterface;
import com.nokia.tracecompiler.engine.TraceLocationList;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.engine.utils.TraceUtils;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.plugin.TraceCompilerPlugin;
import com.nokia.tracecompiler.project.GroupNames;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SymbianConstants;
import com.nokia.tracecompiler.utils.DocumentFactory;
import com.nokia.tracecompiler.utils.TraceCompilerVersion;

/**
 * TraceCompiler command-line main class
 * 
 */
public class TraceCompiler {
	
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
	 * Version option
	 */
	private static final String VERSION_OPTION = "--version"; //$NON-NLS-1$
	private static final String LEGACY_VERSION_OPTION = "-version"; //$NON-NLS-1$
	private static final String VERSION_OPTION_SF = "-v"; //$NON-NLS-1$
	/**
	 * Version option instruction text
	 */
	private static final String VERSION_OPTION_INSTRUCTION_TEXT = "print TraceCompiler version"; //$NON-NLS-1$

	
	/**
	 * help option
	 */
	private static final String HELP_OPTION = "--help"; //$NON-NLS-1$
	private static final String HELP_OPTION_SF = "-h"; //$NON-NLS-1$
	private static final String HELP_OPTION_INSTRUCTION_TEXT = "print help"; //$NON-NLS-1$
	
	/**
	 * Verbose option
	 */
	private static final String VERBOSE_OPTION = "--verbose"; //$NON-NLS-1$
	private static final String VERBOSE_OPTION_SF = "-vb"; //$NON-NLS-1$
	/**
	 * Verbose option instruction text
	 */
	private static final String VERBOSE_OPTION_INSTRUCTION_TEXT = "print info messages."; //$NON-NLS-1$

	/**
	 * keep going option
	 */
	private static final String STOP_ON_ERROR_OPTION = "--stopOnError"; //$NON-NLS-1$
	private static final String STOP_ON_ERROR_OPTION_SF = "-soe"; //$NON-NLS-1$
	/**
	 * keep going option instruction text
	 */
	private static final String STOP_ON_ERROR_OPTION_INSTRUCTION_TEXT = "On error, stop at the end of the compilation unit."; //$NON-NLS-1$

	

	/**
	 * Version text
	 */
	private static final String VERSION_TEXT = "TraceCompiler version "; //$NON-NLS-1$

	/**
	 * Option instruction text
	 */
	private static final String OPTION_INSTRUCTION_TEXT = "Options:"; //$NON-NLS-1$

	
	private static final String USAGE = "Usage: " + LINE_SEPARATOR + //$NON-NLS-1$
										"tracecompiler [options] Component_UID [Component_name MMP_path source_file...]" + LINE_SEPARATOR + //$NON-NLS-1$
										"\t" + OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR + //$NON-NLS-1$
										"\t" + HELP_OPTION_SF  + ", " + HELP_OPTION + ", " + HELP_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR +   //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$
										"\t" + VERSION_OPTION_SF + ", " + VERSION_OPTION + ", " + LEGACY_VERSION_OPTION + "\t" +  VERSION_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR +   //$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$ //$NON-NLS-4$
										"\t" + VERBOSE_OPTION_SF + ", " + VERBOSE_OPTION + "\t\t" + VERBOSE_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR +  //$NON-NLS-1$//$NON-NLS-2$ //$NON-NLS-3$
										"\t" + STOP_ON_ERROR_OPTION_SF + ", " + STOP_ON_ERROR_OPTION + "\t" + STOP_ON_ERROR_OPTION_INSTRUCTION_TEXT + LINE_SEPARATOR;  //$NON-NLS-1$//$NON-NLS-2$ //$NON-NLS-3$



	/**
	 * End of source files tag
	 */
	private static final String ENDOFSOURCEFILES = "*ENDOFSOURCEFILES*"; //$NON-NLS-1$
	

	/**
	 * Offset to UID
	 */
	private static final int COMPONENT_UID_ARG = 0;

	/**
	 * Offset to component name
	 */
	private static final int COMPONENT_NAME_ARG = 1;

	/**
	 * Offset to MMP path
	 */
	private static final int COMPONENT_MMP_PATH_ARG = 2; // CodForChk_Dis_Magic

	/**
	 * Offset to source files
	 */
	private static final int SOURCE_FILE_START_OFFSET = 3; // CodForChk_Dis_Magic

	/**
	 * Number of arguments
	 */
	private static final int MANDATORY_ARGUMENT_COUNT = 1;

	/**
	 * MMP file extension
	 */
	private static final String MMP = ".mmp"; //$NON-NLS-1$

	/**
	 * Underscore character
	 */
	private static final String UNDERSCORE = "_"; //$NON-NLS-1$

	/**
	 * Name of the trace folder that include component name
	 */
	private String traceFolderName;

	/**
	 * Decode plugins path
	 */
	private String DECODE_PLUGINS_PATH = "com/nokia/tracecompiler/decodeplugins"; //$NON-NLS-1$

	/**
	 * Decode plugin name tag
	 */
	private String DECODE_PLUGIN_NAME_TAG = "<DECODE_PLUGIN_NAME>"; //$NON-NLS-1$

	/**
	 * Decode plugin class name tag
	 */
	private String DECODE_PLUGIN_CLASS_NAME_TAG = "<DECODE_PLUGIN_CLASS_NAME>"; //$NON-NLS-1$

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
	private String CLASS_TEMPLATE = "com.nokia.tracecompiler.decodeplugins." + DECODE_PLUGIN_NAME_TAG + "." + DECODE_PLUGIN_CLASS_NAME_TAG; //$NON-NLS-1$ //$NON-NLS-2$	

	/**
	 * Main function
	 * 
	 * @param args
	 *            the command line arguments
	 */
	public static void main(String[] args) {
		ArrayList<String> list = new ArrayList<String>(Arrays.asList(args));
		long startTime = System.currentTimeMillis();
		//create a new session of TraceCompiler
		TraceCompiler console = new TraceCompiler();
		
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

		if (console.componentName != null) {
			TraceCompilerLogger.printMessage(console.componentName + " took " //$NON-NLS-1$
					+ (System.currentTimeMillis() - startTime) + " ms"); //$NON-NLS-1$
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
	private String componentName;

	/**
	 * UID of the component
	 */
	private long componentUID;

	/**
	 * Component path
	 */
	private String componentPath;

	/**
	 * MMP file path
	 */
	private File mmpPath;

	/**
	 * Constructor
	 */
	TraceCompiler() {

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
							TraceCompilerLogger.printInfo("Decode plugin " + engineClassFullName + " added"); //$NON-NLS-1$ //$NON-NLS-2$
						} catch (Exception e) {
							String msg = "Decode plugin " + engineClassFullName + " adding failed"; //$NON-NLS-1$ //$NON-NLS-2$
							throw new TraceCompilerRootException(msg, e); 
						}
					} else {
						String msg = "Decode plugin file " + engineFileFullName + " does not exist"; //$NON-NLS-1$ //$NON-NLS-2$
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
		TraceCompilerLogger.printInfo("Building traces..."); //$NON-NLS-1$
		if (list.size() == 0) {
			printUsage();
			System.exit(0);
		}
		List<String> switches = new ArrayList<String>();

		// version, verbose, keepgoing if available could be anywhere, so process them and remove them from the list
		for (String element : list) {
			if (element.equalsIgnoreCase(HELP_OPTION) || element.equalsIgnoreCase(HELP_OPTION_SF)) {
				printUsage();
				System.exit(0);
			}
			if (element.equalsIgnoreCase(VERBOSE_OPTION) || element.equalsIgnoreCase(VERBOSE_OPTION_SF)) {
				TraceCompilerGlobals.setVerbose(true);
				switches.add(element);
			}
			if (element.equalsIgnoreCase(LEGACY_VERSION_OPTION) || element.equalsIgnoreCase(VERSION_OPTION) || element.equalsIgnoreCase(VERSION_OPTION_SF)) {
				TraceCompilerLogger.printMessage(VERSION_TEXT + TraceCompilerVersion.getVersion());
				System.exit(0);
			}
			if (element.equalsIgnoreCase(STOP_ON_ERROR_OPTION) || element.equalsIgnoreCase(STOP_ON_ERROR_OPTION_SF)) {
				TraceCompilerGlobals.setKeepGoing(false);
				switches.add(element);
			}
		}
		
		//remove switches from the list
		for (String string : switches) {
			list.remove(string);
		}
		
		switches = null;
		
		if (list.size() < MANDATORY_ARGUMENT_COUNT) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.invalidTraceCompilerArgumetsExceptionText"), null); //$NON-NLS-1$
		}
		
		
		//the rest of elements must be in the order COMPONENT_UID, COMPONENT_NAME, COMPONENT_MMP_PATH, source...
		//COMPONENT_UID must be on the command line, the rest can either be on the command line or stdin

		try {
			componentUID = Long.parseLong(list.get(COMPONENT_UID_ARG),
					TraceCompilerConstants.HEX_RADIX);
			TraceCompilerLogger.printInfo("Component UID: 0x" + Long.toHexString(componentUID)); //$NON-NLS-1$

		} catch (NumberFormatException e) {
			String msg = Messages.getString("TraceCompiler.componentUidIsNotValidExceptionText") + componentUID; //$NON-NLS-1$
			throw new TraceCompilerIllegalArgumentsException(msg, null);
		}
		if (componentUID > 0) {

			// Arguments are given as a parameter
			if (list.size() > MANDATORY_ARGUMENT_COUNT + 1) {
				parseParameters(list);

				// Otherwise, read arguments from STDIN
			} else {
				readFromStdin();
			}
		} else {
			String msg = Messages.getString("TraceCompiler.componentUidIsNotValidExceptionText") + componentUID; //$NON-NLS-1$
			throw new TraceCompilerIllegalArgumentsException(msg, null);
		}
	}


	private static void printUsage() {
		TraceCompilerLogger.printMessage(VERSION_TEXT + TraceCompilerVersion.getVersion());
		TraceCompilerLogger.printMessage(USAGE);
	}

	/**
	 * Parses parameters
	 * 
	 * @param args
	 *            the parameters
	 * @throws TraceCompilerRootException if arguments are not valid
	 */
	private void parseParameters(List<String> args) throws TraceCompilerIllegalArgumentsException {
		//index is safe, already checked by the caller
		componentName = args.get(COMPONENT_NAME_ARG);
		traceFolderName = 	TraceCompilerConstants.TRACES_DIRECTORY + UNDERSCORE
							+ componentName;
		
		mmpPath = new File(args.get(COMPONENT_MMP_PATH_ARG));

		if (isMmpValid()) {

			// Move the files from args array to source file list
				ArrayList<String> sources = new ArrayList<String>();
				for (int i = 0; i < args.size() - SOURCE_FILE_START_OFFSET; i++) {
					sources.add(args.get(SOURCE_FILE_START_OFFSET + i));
				}

				registerFiles(sources);
		} else {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.invalidMmpExceptionText") + mmpPath, null); //$NON-NLS-1$
		}
		
	}

	/**
	 * Read information from STDIN
	 * @throws IOException if fails to read the input
	 * @throws TraceCompilerRootException if the list of files is empty
	 */
	private void readFromStdin() throws IOException, TraceCompilerIllegalArgumentsException {
		ArrayList<String> files = new ArrayList<String>();

		// Create reader
		BufferedReader stdin = new BufferedReader(
				new InputStreamReader(System.in));
		
			int numberOfReceivedLines = 1;

			String line = stdin.readLine();
						
			while (line != null) {

				// End of source files received
				if (line.equals(ENDOFSOURCEFILES)) {
					break;
				}

				// Component name
				if (numberOfReceivedLines == COMPONENT_NAME_ARG) {
					componentName = line;
					traceFolderName = TraceCompilerConstants.TRACES_DIRECTORY
							+ UNDERSCORE + componentName;

					// MMP path
				} else if (numberOfReceivedLines == COMPONENT_MMP_PATH_ARG) {
					mmpPath = new File(line);
					if (!isMmpValid()) {
						stdin.close();
						throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.invalidMmpExceptionText") + mmpPath, null); //$NON-NLS-1$
					}

					// Source files
				} else {
					// Add to the files list
					File file = new File(line);
					files.add(file.getAbsolutePath());
				}

				numberOfReceivedLines++;

				// Read new line from STDIN
				line = stdin.readLine();				
			}
		stdin.close();
		registerFiles(files);
	}

	/**
	 * Registers files to document monitor.
	 * 
	 * @param sources
	 *            sources
	 * @throws TraceCompilerRootException 
	 */
	private void registerFiles(ArrayList<String> sources) throws TraceCompilerIllegalArgumentsException {

		if (sources.size() == 0) {
			throw new TraceCompilerIllegalArgumentsException(Messages.getString("TraceCompiler.noSourceFilesExceptionText"), null); //$NON-NLS-1$
		}
		File parent = mmpPath.getParentFile();
		boolean found = false;

		componentPath = parent.getAbsolutePath();

		ProjectEngine.traceFolderName = TraceCompilerConstants.TRACES_DIRECTORY;

		// Find location of "traces" or "traces_<component name>" -folder. If
		// "traces" or "traces_<component name>" -folder does not
		// exist, "traces" -folder will be add to same level than "group" or
		// "mmpfiles" -folder. If "group" or "mmpfiles" -folder does not exist
		// then "traces" -folder will be added to same level than source file.
		while (!found && parent != null) {
			File[] children = parent.listFiles();
			if (children != null) {
				for (int i = 0; i < children.length; i++) {
					File child = children[i];
					String childName = child.getName();
					if (child.isDirectory() && isProjectRoot(childName)) {
						componentPath = parent.getAbsolutePath();
						found = true;

						// Check that does subdirectory that name is
						// "traces_<component name>" exist in this directory. If
						// it exist use that as traces directory name.
						for (i = 0; i < children.length; i++) {
							child = children[i];
							childName = child.getName();
							if (child.isDirectory()
									&& childName
											.equalsIgnoreCase(traceFolderName)) {
								ProjectEngine.traceFolderName = traceFolderName;
								break;
							}
						}
						break;
					}
				}
			}

			if (found == false) {
				parent = parent.getParentFile();
			}
		}

		registerSourceFiles(sources);
	}

	/**
	 * Checks if this folder is the project root
	 * 
	 * @param name
	 *            name of the folder
	 * @return true if this folder is the project root
	 */
	private boolean isProjectRoot(String name) {
		boolean retval = false;
		if (name.equalsIgnoreCase(traceFolderName)
				|| name.equalsIgnoreCase(SymbianConstants.GROUP_DIRECTORY)
				|| name.equalsIgnoreCase(SymbianConstants.MMPFILES_DIRECTORY)
				|| name
						.equalsIgnoreCase(TraceCompilerConstants.TRACES_DIRECTORY)) {
			retval = true;
		}
		return retval;
	}

	/**
	 * Registers source files
	 * 
	 * @param files
	 */
	private void registerSourceFiles(ArrayList<String> files) {
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
			componentName = TraceUtils.convertName(componentName);

			// Set project path before opening project
			TraceCompilerEngineGlobals.setProjectPath(componentPath);
			tbi.openProject(componentName);
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
				String msg = "Project creation was cancelled"; //$NON-NLS-1$
				throw new TraceCompilerRootException(msg, null);
			}
		} catch (TraceCompilerException e) {
			TraceCompilerEngineGlobals.getEvents().postError(e);
			throw new TraceCompilerRootException("Build trace files failed.", e); //$NON-NLS-1$
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

		if (mmpPath.exists() && pathStr.length() > MMP.length()) {
			String end = pathStr.substring(pathStr.length() - MMP.length());
			if (end.equalsIgnoreCase(MMP)) {
				valid = true;
			} else {
				TraceCompilerLogger.printError("Invalid MMP file: " + mmpPath.getAbsolutePath()); //$NON-NLS-1$
			}
		} else {
			TraceCompilerLogger.printError("Missing or can not access MMP path: " + mmpPath.getAbsolutePath()); //$NON-NLS-1$
		}
		return valid;
		
	}

}
