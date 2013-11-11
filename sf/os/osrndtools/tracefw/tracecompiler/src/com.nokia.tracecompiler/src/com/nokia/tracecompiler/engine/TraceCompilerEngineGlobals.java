/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Access point to the exported interfaces of TraceCompiler engine
*
*/
package com.nokia.tracecompiler.engine;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import com.nokia.tracecompiler.TraceCompilerLogger;
import com.nokia.tracecompiler.engine.source.SourceEngine;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.plugin.TraceCompilerPlugin;
import com.nokia.tracecompiler.source.SourceConstants;

/**
 * Access point to the exported interfaces of TraceCompiler engine
 * 
 */
public class TraceCompilerEngineGlobals {

	/**
	 * Max group ID
	 */
	public static final int MAX_GROUP_ID = 65535; // CodForChk_Dis_Magic

	/**
	 * Max trace ID
	 */
	public static final int MAX_TRACE_ID = 65535; // CodForChk_Dis_Magic

	/**
	 * TraceCompiler engine instance
	 */
	private static TraceCompilerEngine instance;
	
	/**
	 * default licence
	 */
	static String defaultLicence;
	
	/**
	 * default licence
	 */
	static String defaultLicenceWithHash;
	
	/**
	 * Constructor is hidden
	 */
	private TraceCompilerEngineGlobals() {
	}

	/**
	 * Starts TraceCompiler engine.
	 * @throws TraceCompilerException 
	 */
	public static void start() throws TraceCompilerException {
		if (instance == null) {
			instance = new TraceCompilerEngine();
			instance.start();
		}
	}

	/**
	 * Shuts down the TraceCompiler instance
	 * @throws TraceCompilerException 
	 */
	public static void shutdown() throws TraceCompilerException {
		if (instance != null) {
			instance.shutdown();
			instance = null;
		}
	}

	/**
	 * Gets the configuration interface. The configuration interface is not
	 * available until view has been registered.
	 * 
	 * @return the configuration
	 */
	public static TraceCompilerEngineConfiguration getConfiguration() {
		return instance.getConfiguration();
	}

	/**
	 * Gets the trace model
	 * 
	 * @return the model
	 */
	public static TraceModel getTraceModel() {
		return instance.getModel();
	}

	/**
	 * Gets the source engine
	 * 
	 * @return the source engine
	 */
	public static SourceEngine getSourceEngine() {
		return instance.getSourceEngine();
	}

	/**
	 * Gets the trace compiler interface
	 * 
	 * @return trace compiler
	 */
	public static TraceCompilerEngineInterface getTraceCompiler() {
		return instance.getTraceCompiler();
	}

	/**
	 * Gets the events interface
	 * 
	 * @return the events interface
	 */
	public static TraceCompilerEngineEvents getEvents() {
		if (instance != null) {
			return instance.getEvents();
		} else {
			return null;
		}
	}

	/**
	 * Called by a plug-in to register itself
	 * 
	 * @param plugin
	 *            the plugin to be registered
	 */
	public static void registerPlugin(TraceCompilerPlugin plugin) {
		instance.registerPlugin(plugin);
	}

	/**
	 * Called by a plug-in to unregister itself
	 * 
	 * @param plugin
	 *            the plugin to be unregistered
	 */
	public static void unregisterPlugin(TraceCompilerPlugin plugin) {
		if (instance != null) {
			instance.unregisterPlugin(plugin);
		}
	}

	/**
	 * Called by the view plug-in to register the view
	 * 
	 * @param view
	 *            the view
	 * @throws TraceCompilerException 
	 */
	public static void setView(TraceCompilerEngineView view) throws TraceCompilerException {
		instance.setView(view);
	}

	/**
	 * Gets the source context manager
	 * 
	 * @return the context manager
	 */
	public static SourceContextManager getSourceContextManager() {
		return instance.getSourceContextManager();
	}

	/**
	 * Get project path
	 * 
	 * @return project path
	 */
	public static String getProjectPath() {
		String projetcPath = instance.getProjectPath();
		return projetcPath;
	}

	/**
	 * Set project path
	 * 
	 * @param path
	 *            the path
	 */
	public static void setProjectPath(String path) {
		instance.setProjectPath(path);
	}
	
	/**
	 * get default licence from com/nokia/tracecompiler/licence.lic
	 * c++ comment format
	 * @param boolean cppCommented
	 *  if true, it returns the licence in cpp comment \/** ... **\/
	 *  else it retuen the licence in a # comment ## ... ##
	 * @return String licence
	 */
	public static String getDefaultLicence(boolean cppCommented) {
		String licenceResourceName;
		if (cppCommented) {
			if (defaultLicence == null) {
				licenceResourceName = "/com/nokia/tracecompiler/licence.lic"; //$NON-NLS-1$
				defaultLicence = readLicence(licenceResourceName);
			}
			return defaultLicence;
		} else {
			if (defaultLicenceWithHash == null) {
				licenceResourceName = "/com/nokia/tracecompiler/licence_hash.lic"; //$NON-NLS-1$
				defaultLicenceWithHash = readLicence(licenceResourceName);
			}
			return defaultLicenceWithHash;
		}
	}

	/**
	 * read the provided resource file and return the text in it.
	 */
	private static String readLicence(String aLicenceResourceName) {
		String licenceText = null;
		InputStream is = instance.getClass().getResourceAsStream(aLicenceResourceName); 
		if (is != null) {
			InputStreamReader isr = new InputStreamReader(is);
			BufferedReader br = new BufferedReader(isr);
			StringBuffer sb = new StringBuffer();
			String line;
			try {
				while ((line = br.readLine()) != null) {
					sb.append(line + SourceConstants.LINE_FEED);
				}
				licenceText = sb.toString();
				br.close();
				isr.close();
				is.close();
			} catch (IOException e) {
				TraceCompilerLogger.printWarning(Messages.getString("TraceCompilerErrorMessages.LicenceFileError") + e.getMessage()); //$NON-NLS-1$
			}
		}
		return licenceText;
	}

}
