/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Export plugin manager, which delegates calls to plug-ins
*
*/
package com.nokia.tracecompiler.engine.plugin;

import java.util.ArrayList;
import java.util.Iterator;

import com.nokia.tracecompiler.engine.TraceCompilerEngineBase;
import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorMessages;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceObjectPropertyVerifier;
import com.nokia.tracecompiler.plugin.TraceAPIPlugin;
import com.nokia.tracecompiler.plugin.TraceCompilerExport;
import com.nokia.tracecompiler.plugin.TraceCompilerPlugin;

/**
 * Plugin engine, which delegates calls to plug-ins
 * 
 */
public final class PluginEngine extends TraceCompilerEngineBase {

	/**
	 * List of plug-ins
	 */
	private ArrayList<TraceCompilerPlugin> plugins = new ArrayList<TraceCompilerPlugin>();

	/**
	 * Property verifier
	 */
	private PluginTracePropertyVerifier verifier = new PluginTracePropertyVerifier(
			this);

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Project open flag
	 */
	private boolean projectOpen;

	/**
	 * Sets the trace model. This is not set in constructor, since plug-in
	 * engine is created before the model
	 * 
	 * @param model
	 *            the trace model
	 */
	public void setModel(TraceModel model) {
		this.model = model;
	}

	/**
	 * Gets the started flag
	 * 
	 * @return true if started, false if not
	 */
	public boolean isProjectOpen() {
		return projectOpen;
	}

	/**
	 * Adds a plugin
	 * 
	 * @param plugin
	 *            the plugin to be added
	 */
	public void add(TraceCompilerPlugin plugin) {
		plugins.add(plugin);
		if (plugin instanceof TraceAPIPlugin) {
			TraceAPIPlugin api = (TraceAPIPlugin) plugin;
			TraceAPIPluginManager manager = model
					.getExtension(TraceAPIPluginManager.class);
			manager.addFormatters(api.getFormatters());
			manager.addParsers(api.getParsers());
		}
	}

	/**
	 * Removes a plugin
	 * 
	 * @param plugin
	 *            the plugin to be removed
	 */
	public void remove(TraceCompilerPlugin plugin) {
		// Formatters / parsers are not removed. Currently this is not a
		// problem since plug-in's are removed only on shutdown
		plugins.remove(plugin);
	}

	/**
	 * Gets the property verifier interface
	 * 
	 * @return the verifier
	 */
	public TraceObjectPropertyVerifier getVerifier() {
		return verifier;
	}

	/**
	 * Checks if there are plug-ins
	 * 
	 * @return true if plug-ins exist
	 */
	public boolean hasPlugins() {
		return !plugins.isEmpty();
	}

	/**
	 * Gets the plug-ins
	 * 
	 * @return the plug-ins
	 */
	Iterator<TraceCompilerPlugin> getPlugins() {
		return plugins.iterator();
	}

	/* (non-Javadoc)
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngineBase#exportProject()
	 */
	@Override
	public void exportProject() throws TraceCompilerException {
		if (model.hasTraces()) {
			for (TraceCompilerPlugin plugin : plugins) {
				if (plugin instanceof TraceCompilerExport) {
					try {
						((TraceCompilerExport) plugin).exportTraceProject();
					} catch (TraceCompilerException e) {
						TraceCompilerEngineGlobals.getEvents().postError(e);
							throw e;
					}
				}
			}
		} else {
			TraceCompilerEngineGlobals.getEvents().postInfoMessage(
					TraceCompilerEngineErrorMessages.getErrorMessage(
							TraceCompilerErrorCode.NO_TRACES_TO_EXPORT, null),
					null);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#projectOpened()
	 */
	@Override
	public void projectOpened() {
		if (!projectOpen) {
			for (TraceCompilerPlugin plugin : plugins) {
				plugin.traceProjectOpened(model);
			}
			projectOpen = true;
		} else {
			if (TraceCompilerEngineConfiguration.ASSERTIONS_ENABLED) {
				TraceCompilerEngineGlobals.getEvents().postAssertionFailed(
						"PluginEngine.traceProjectOpened", null); //$NON-NLS-1$
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#projectClosing()
	 */
	@Override
	public void projectClosed() {
		if (projectOpen) {
			for (TraceCompilerPlugin plugin : plugins) {
				plugin.traceProjectClosed();
			}
			projectOpen = false;
		}
	}

}
