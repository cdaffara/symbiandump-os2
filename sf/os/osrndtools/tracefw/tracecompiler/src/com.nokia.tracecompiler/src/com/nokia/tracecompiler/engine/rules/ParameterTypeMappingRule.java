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
* Trace parameter rule for adding casting and pointer operations to source code
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.model.TraceModelPersistentExtension;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceUtils;
import com.nokia.tracecompiler.source.TypeMapping;

/**
 * Trace parameter rule for adding casting and pointer operations to source code
 * 
 */
final class ParameterTypeMappingRule extends TraceParameterFormattingRuleBase
		implements TraceModelPersistentExtension {

	/**
	 * TTime.Int64() function
	 */	
	private static final String INT64_FUNCTION = ".Int64()"; //$NON-NLS-1$

	/**
	 * The type mapping
	 */
	private TypeMapping typeMapping;

	/**
	 * Storage name for this parameter
	 */
	static String STORAGE_NAME = "TypeMapping"; //$NON-NLS-1$

	/**
	 * Constructor for reflection
	 */
	ParameterTypeMappingRule() {
		typeMapping = new TypeMapping(null);
	}

	/**
	 * Creates a new type mapping rule
	 * 
	 * @param mapping
	 *            the type mapping to be used
	 */
	ParameterTypeMappingRule(TypeMapping mapping) {
		this.typeMapping = mapping;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.rules.TraceParameterFormattingRuleBase#
	 *      mapNameToSource(java.lang.String)
	 */
	@Override
	public String mapNameToSource(String originalName) {
		StringBuffer source = new StringBuffer();
		if (typeMapping.type.equals(TraceParameter.TIME)) {
			source.append(originalName);
			source.append(INT64_FUNCTION);
		} else {
			if (typeMapping.needsCasting) {
				source.append(SourceConstants.START_PARAMETERS);
				TraceParameter param = (TraceParameter) getOwner();
				source.append(SourceUtils.mapParameterTypeToSymbianType(param));
				source.append(SourceConstants.END_PARAMETERS);
			}
			if (typeMapping.valueToPointer) {
				source.append('&');
			}
			source.append(SourceConstants.START_PARAMETERS);
			source.append(originalName);
			source.append(SourceConstants.END_PARAMETERS);
		}
		return source.toString();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelPersistentExtension#getData()
	 */
	public String getData() {
		StringBuffer sb = new StringBuffer();
		if (typeMapping.needsCasting) {
			sb.append('C');
		}
		if (typeMapping.valueToPointer) {
			sb.append('V');
		}
		return sb.toString();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelPersistentExtension#getStorageName()
	 */
	public String getStorageName() {
		return STORAGE_NAME;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelPersistentExtension#setData(java.lang.String)
	 */
	public boolean setData(String data) {
		boolean retval = false;
		if (data != null) {
			for (int i = 0; i < data.length(); i++) {
				char c = data.charAt(i);
				switch (c) {
				case 'C':
					typeMapping.needsCasting = true;
					retval = true;
					break;
				case 'V':
					typeMapping.valueToPointer = true;
					retval = true;
					break;
				}
			}
		}
		return retval;
	}

}