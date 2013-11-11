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
 * A singleton access point to functionality of Trace Compiler
 *
 */
package com.nokia.tracecompiler.engine;

import java.util.ArrayList;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.event.EventEngine;
import com.nokia.tracecompiler.engine.header.HeaderEngine;
import com.nokia.tracecompiler.engine.plugin.PluginEngine;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.engine.propertyfile.PropertyFileEngine;
import com.nokia.tracecompiler.engine.rules.RulesEngine;
import com.nokia.tracecompiler.engine.source.SourceEngine;
import com.nokia.tracecompiler.engine.source.SourceListener;
import com.nokia.tracecompiler.engine.source.SourceProperties;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.plugin.TraceCompilerPlugin;

/**
 * A singleton access point to functionality of Trace Compiler
 * 
 */
final class TraceCompilerEngine implements SourceListener, TraceCompilerEngineInterface {

	/**
	 * TraceCompiler plug-in engine
	 */
	private PluginEngine pluginEngine;

	/**
	 * Event handler interface implementation
	 */
	private TraceCompilerEngineEvents eventEngine;

	/**
	 * Source editor engine
	 */
	private SourceEngine sourceEngine;

	/**
	 * Trace header engine
	 */
	private HeaderEngine headerEngine;

	/**
	 * Trace project engine
	 */
	private ProjectEngine projectEngine;

	/**
	 * Rule engine
	 */
	private RulesEngine rulesEngine;

	/**
	 * Trace property file engine
	 */
	private PropertyFileEngine propertyFileEngine;

	/**
	 * Source context manager
	 */
	private SourceContextManager contextManager;

	/**
	 * Location map
	 */
	private TraceLocationMap locationMap;

	/**
	 * Location converter
	 */
	private TraceLocationConverter locationConverter;

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Interface to the view
	 */
	private TraceCompilerEngineView view = new ViewAdapter();

	/**
	 * Configuration
	 */
	private ConfigurationDelegate configurationDelegate;

	/**
	 * List of engines that need project open / export / close notifications
	 */
	private ArrayList<TraceCompilerEngineBase> engines = new ArrayList<TraceCompilerEngineBase>();

	/**
	 * View has been registered flag
	 */
	private boolean isViewRegistered = false;

	/**
	 * Project path is stored in case the view unregisters and re-registers
	 */
	private String currentProjectPath;

	/**
	 * Gets the configuration of TraceCompiler
	 * 
	 * @return the configuration
	 */
	TraceCompilerEngineConfiguration getConfiguration() {
		return configurationDelegate;
	}

	/**
	 * Gets the trace compiler interface
	 * 
	 * @return this object
	 */
	TraceCompilerEngineInterface getTraceCompiler() {
		return this;
	}

	/**
	 * Gets the trace model
	 * 
	 * @return the trace model
	 */
	TraceModel getModel() {
		return model;
	}

	/**
	 * Gets the source engine
	 * 
	 * @return the source engine
	 */
	SourceEngine getSourceEngine() {
		return sourceEngine;
	}

	/**
	 * Gets the events interface
	 * 
	 * @return the events interface
	 */
	TraceCompilerEngineEvents getEvents() {
		return eventEngine;
	}

	/**
	 * Adds a plug-in
	 * 
	 * @param plugin
	 *            the plugin
	 */
	void registerPlugin(TraceCompilerPlugin plugin) {
		pluginEngine.add(plugin);
	}

	/**
	 * Removes an existing plug-in
	 * 
	 * @param plugin
	 *            the plugin
	 */
	void unregisterPlugin(TraceCompilerPlugin plugin) {
		pluginEngine.remove(plugin);
	}

	/**
	 * Gets the source context manager
	 * 
	 * @return the context manager
	 */
	public SourceContextManager getSourceContextManager() {
		return contextManager;
	}

	/**
	 * Starts TraceCompiler.
	 * @throws TraceCompilerException 
	 */
	void start() throws TraceCompilerException {
		configurationDelegate = new ConfigurationDelegate();
		rulesEngine = new RulesEngine();
		pluginEngine = new PluginEngine();
		model = new TraceModel(rulesEngine, pluginEngine.getVerifier());
		pluginEngine.setModel(model);
		projectEngine = new ProjectEngine(model);
		propertyFileEngine = new PropertyFileEngine(model);
		eventEngine = new EventEngine(model);
		sourceEngine = new SourceEngine(model);
		locationMap = new TraceLocationMap(model);
		locationConverter = new TraceLocationConverter(model);
		sourceEngine.addSourceListener(this);
		contextManager = new SourceContextManagerImpl();
		headerEngine = new HeaderEngine(model);
		// Keep this order -> Exports need to be in correct order
		// Property file engine manages the ID cache so it needs to be run
		// before plug-in's and header
		engines.add(projectEngine);
		engines.add(propertyFileEngine);
		engines.add(headerEngine);
		engines.add(pluginEngine);
	}

	/**
	 * Shuts down TraceCompiler
	 * @throws TraceCompilerException 
	 */
	void shutdown() throws TraceCompilerException {
		closeProject();
	}

	/**
	 * Registers the view
	 * 
	 * @param view
	 *            the view
	 * @throws TraceCompilerException 
	 */
	void setView(TraceCompilerEngineView view) throws TraceCompilerException {
		if (view == null) {
			// Closes the project, but leaves the currentProjectName and
			// currentProjectPath variables. When view re-registers the project
			// is opened back
			internalCloseProject();
			this.view = new ViewAdapter();
			isViewRegistered = false;
		} else {
			this.view = view;
			isViewRegistered = true;
		}
		configurationDelegate.setConfiguration(this.view.getConfiguration());
	}

	/**
	 * Checks that model is valid
	 * 
	 * @throws TraceCompilerException
	 *             if model is not valid
	 */
	private void checkIsModelValid() throws TraceCompilerException {
		if (!model.isValid()) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.MODEL_NOT_READY);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngineInterface#openProject(java.lang.String,
	 *      java.lang.String)
	 */
	public void openProject(String modelName) throws Exception {
		try {
			if (view != null && isViewRegistered && !model.isValid()) {
				projectEngine.openTraceProject(currentProjectPath, modelName);
				for (TraceCompilerEngineBase engine : engines) {
					engine.projectOpened();
				}
				model.setValid(true);
				sourceEngine.start();
			}
		} catch (TraceCompilerException e) {
			closeModel();
			throw e;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngineInterface#exportProject()
	 */
	public void exportProject() throws TraceCompilerException {
		checkIsModelValid();
		if (model.getID() != 0) {
			for (TraceCompilerEngineBase engine : engines) {
					engine.exportProject();
			}
		} else {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_MODEL_PROPERTIES_FOR_EXPORT);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngineInterface#closeProject()
	 */
	public void closeProject() throws TraceCompilerException {
		internalCloseProject();
		currentProjectPath = null;
	}

	/**
	 * Closes the project
	 * @throws TraceCompilerException 
	 */
	private void internalCloseProject() throws TraceCompilerException {
		if (model.isValid()) {
			if (!TraceCompilerEngineGlobals.getConfiguration().getFlag(
					TraceCompilerEngineConfiguration.CONSOLE_UI_MODE)) {
				try {
					// Plug-in's are not exported when project is closed
					// They can do cleanup in traceProjectClosed call
					for (TraceCompilerEngineBase engine : engines) {
						if (engine != pluginEngine) {
							engine.exportProject();
						}
					}
				} catch (TraceCompilerException e) {
					getEvents().postError(e);
						throw e;
				}
			}
			// Engines are closed in reverse order
			for (int i = engines.size() - 1; i >= 0; i--) {
				engines.get(i).projectClosed();
			}
			sourceEngine.shutdown();
			closeModel();
		}
	}

	/**
	 * Closes the trace model
	 */
	private void closeModel() {
		try {
			if (model != null) {
				model.reset();
				model.setValid(false);
			}
		} catch (Exception e) {
			if (TraceCompilerEngineConfiguration.ASSERTIONS_ENABLED) {
				getEvents().postAssertionFailed("Failed to close model", e); //$NON-NLS-1$
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.SourceListener#
	 *      sourceOpened(com.nokia.tracecompiler.engine.SourceProperties)
	 */
	public void sourceOpened(SourceProperties properties) throws TraceCompilerException {
		locationMap.addSource(properties);
		locationConverter.sourceOpened(properties);
	}

	/**
	 * Get project path
	 * 
	 * @return project path
	 */
	public String getProjectPath() {
		return currentProjectPath;
	}

	/**
	 * Set project path
	 * 
	 * @param path
	 *            the path
	 */
	public void setProjectPath(String path) {
		currentProjectPath = path;
	}

}
