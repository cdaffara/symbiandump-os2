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
* Value element parser
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import org.w3c.dom.Element;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.StringErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceConstantTable;
import com.nokia.tracecompiler.model.TraceModelExtension;

/**
 * Value element parser
 * 
 */
final class ValueElementParser implements PropertyFileElementParser {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.propertyfile.PropertyFileElementParser#
	 *      parse(java.lang.Object, org.w3c.dom.Element)
	 */
	public void parse(Object owner, Element element)
			throws TraceCompilerException {
		// Value element must be parsed within context of TraceConstantTable
		// Struct has not yet been implemented
		if (owner instanceof TraceConstantTable) {
			parseConstantTableEntry((TraceConstantTable) owner, element);
		} else {
			StringErrorParameters parameter = new StringErrorParameters();
			parameter.string = PropertyFileConstants.VALUE_ELEMENT;
			throw new TraceCompilerException(
					TraceCompilerErrorCode.PROPERTY_FILE_ELEMENT_MISPLACED,
					parameter, null);
		}
	}

	/**
	 * Parses a constant table entry
	 * 
	 * @param table
	 *            the constant table
	 * @param element
	 *            the table entry
	 * @throws TraceCompilerException
	 *             if entry is not valid
	 */
	private void parseConstantTableEntry(TraceConstantTable table,
			Element element) throws TraceCompilerException {
		String idstr = element.getAttribute(PropertyFileConstants.ID_ATTRIBUTE);
		if (idstr != null) {
			try {
				int id = Integer.parseInt(idstr);
				String value = element.getTextContent();
				table.getModel().getVerifier().checkConstantProperties(table,
						null, id, value);
				// Document element reference is stored to the model
				TraceModelExtension[] exts = new TraceModelExtension[] { new DocumentElementWrapper(
						element) };
				table.getModel().getFactory().createConstantTableEntry(table,
						id, value, exts);
			} catch (NumberFormatException e) {
				StringErrorParameters parameter = new StringErrorParameters();
				parameter.string = PropertyFileConstants.ID_ATTRIBUTE;
				throw new TraceCompilerException(
						TraceCompilerErrorCode.PROPERTY_FILE_ATTRIBUTE_INVALID,
						parameter, null);
			}
		}
	}
}