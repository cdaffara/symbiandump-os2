/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Parser for trace property files
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import java.util.HashMap;

import javax.xml.parsers.DocumentBuilder;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorMessages;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.StringErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.project.ProjectFileParser;

/**
 * Parser for trace property files
 * 
 */
final class PropertyFileParser extends ProjectFileParser {

	/**
	 * DOM document representing the property file
	 */
	private Document document;

	/**
	 * Parsers for document elements
	 */
	private HashMap<String, PropertyFileElementParser> elementParsers = new HashMap<String, PropertyFileElementParser>();

	/**
	 * Document builder
	 */
	private DocumentBuilder builder;

	/**
	 * Constructor
	 * 
	 * @param model
	 *            the trace model
	 * @param fileName
	 *            the property file name
	 * @param builder
	 *            document builder
	 * @throws TraceCompilerException
	 *             if parser cannot be created
	 */
	protected PropertyFileParser(TraceModel model, String fileName,
			DocumentBuilder builder) throws TraceCompilerException {
		super(model, fileName);
		this.builder = builder;
		elementParsers.put(PropertyFileConstants.ENUM_ELEMENT,
				new EnumElementParser(this));
		elementParsers.put(PropertyFileConstants.VALUE_ELEMENT,
				new ValueElementParser());
		elementParsers.put(PropertyFileConstants.FILE_ELEMENT,
				new FileElementParser());

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.ProjectFileParser#createParser()
	 */
	@Override
	protected void createParser() throws TraceCompilerException {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.ProjectFileParser#parse()
	 */
	@Override
	public void parse() throws TraceCompilerException {
		try {
			document = builder.parse(projectFile);
			Element rootElement = PropertyFileUtils.findRoot(document);
			if (rootElement != null) {
				parseChildren(model, rootElement);
			} else {
				throw new TraceCompilerException(
						TraceCompilerErrorCode.INVALID_PROJECT_FILE);
			}
		} catch (TraceCompilerException e) {
			throw e;
		} catch (Exception e) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_PROJECT_FILE, e);
		}
	}

	/**
	 * Parses child elements of given element
	 * 
	 * @param owner
	 *            the owning object
	 * @param element
	 *            the element
	 */
	void parseChildren(Object owner, Element element) {
		NodeList list;
		list = element.getChildNodes();
		for (int i = 0; i < list.getLength(); i++) {
			Node node = list.item(i);
			if (node.getNodeType() == Node.ELEMENT_NODE) {
				parseElement(owner, (Element) node);
			}
		}
	}

	/**
	 * Parses an element
	 * 
	 * @param owner
	 *            the owning trace object
	 * @param element
	 *            the element to be parsed
	 */
	private void parseElement(Object owner, Element element) {
		String name = element.getNodeName();
		PropertyFileElementParser parser = elementParsers.get(name);
		if (parser != null) {
			try {
				parser.parse(owner, element);
			} catch (TraceCompilerException e) {
				String msg = TraceCompilerEngineErrorMessages.getErrorMessage(e);
				TraceCompilerEngineGlobals.getEvents().postWarningMessage(msg,
						e.getErrorSource());
			}
		} else {
			postElementNotSupportedWarning(name);
		}
	}

	/**
	 * Posts element not supported warning
	 * 
	 * @param name
	 *            the element name
	 */
	private void postElementNotSupportedWarning(String name) {
		StringErrorParameters parameter = new StringErrorParameters();
		parameter.string = name;
		String msg = TraceCompilerEngineErrorMessages.getErrorMessage(
				TraceCompilerErrorCode.PROPERTY_FILE_ELEMENT_NOT_SUPPORTED,
				parameter);
		TraceCompilerEngineGlobals.getEvents().postWarningMessage(msg, null);
	}

	/**
	 * Gets the document representing the property file
	 * 
	 * @return the document
	 */
	Document getDocument() {
		return document;
	}
}
