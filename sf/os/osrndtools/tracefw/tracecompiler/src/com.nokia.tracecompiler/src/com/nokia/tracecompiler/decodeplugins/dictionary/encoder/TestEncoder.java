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
* Testing class for Encoder
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Testing class for Encoder
 * 
 */
public class TestEncoder {

	/**
	 * 
	 */
	private static final String DICTIONARY1 = "c:\\Temp\\traces.xml"; //$NON-NLS-1$

	/**
	 * 
	 */
	private static final String DICTIONARY2 = "c:\\Temp\\traces2.xml"; //$NON-NLS-1$

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		TraceDictionaryEncoder encoder = new TraceDictionaryEncoder();
		encoder.createNewDictionary(DICTIONARY1);
		Dictionary.startDictionary();
		Dictionary.startMetaData();
		Dictionary.writeMetaData("name", "<Metadata>"); //$NON-NLS-1$ //$NON-NLS-2$
		Dictionary.writeMetaData("id", "1"); //$NON-NLS-1$ //$NON-NLS-2$
		Dictionary.endMetaData();
		TypeDefStore.startTypeDefs();
		TypeDefStore.writeExternalDefFile("<?xml version=\"1.0\" encoding=" //$NON-NLS-1$
				+ "\"UTF-8\"?> <external>c:\\Temp\\ExternalDef.xml</external>"); //$NON-NLS-1$
		TypeDefStore.startTypeDef("TBool", 1, null, DataType.BINARY); //$NON-NLS-1$
		TypeDef.writeTypeMember(0, "EFalse", "BOOL"); //$NON-NLS-1$ //$NON-NLS-2$
		TypeDef.writeTypeMember(-1, "iX", "TInt"); //$NON-NLS-1$ //$NON-NLS-2$
		TypeDef.writeTypeMember(0, "EFalse", null); //$NON-NLS-1$
		TypeDefStore.endTypeDef();
		TypeDefStore.startTypeDef("TPoint", -1, null, DataType.COMPOUND); //$NON-NLS-1$
		TypeDef.writeTypeMember(-1, "iX", "TInt"); //$NON-NLS-1$ //$NON-NLS-2$
		TypeDef.writeTypeMember(-1, "iY", "TInt"); //$NON-NLS-1$ //$NON-NLS-2$
		TypeDefStore.endTypeDef();
		TypeDefStore.writeTypeDef("TPixel", 1, "u", DataType.RAW); //$NON-NLS-1$ //$NON-NLS-2$
		TypeDefStore.endTypeDefs();
		TraceDataStore.startDataStore();
		TraceDataStore.writeData(3, DataType.HEX, "<hex> value is 0xFF"); //$NON-NLS-1$ CodForChk_Dis_Magic
		TraceDataStore.writeData(2, DataType.OCTAL, "<octal> value is 8"); //$NON-NLS-1$ CodForChk_Dis_Magic
		TraceDataStore.writeData(1, DataType.BINARY, "<binary> value is 0"); //$NON-NLS-1$
		TraceDataStore.endDataStore();
		LocationStore.startLocations();
		LocationStore.startPath("<?xml version=\"1.0\" encoding=" //$NON-NLS-1$
				+ "\"UTF-8\"?><file>C:\\Temp\\</file>"); //$NON-NLS-1$
		LocationStore.writeFile(1, "<?xml version=\"1.0\" encoding=" //$NON-NLS-1$
				+ "\"UTF-8\"?><file>C:\\Temp\\file1.txt</file>"); //$NON-NLS-1$
		LocationStore.writeFile(2, "<?xml version=\"1.0\" encoding=" //$NON-NLS-1$ CodForChk_Dis_Magic
				+ "\"UTF-8\"?><file>C:\\Temp\\file2.txt</file>"); //$NON-NLS-1$
		LocationStore.endPath();
		LocationStore.endLocations();
		Dictionary.startComponent(666, "Satan", null, null); //$NON-NLS-1$ CodForChk_Dis_Magic
		TraceComponent.startOptions();
		TraceComponent.writeOption("background", "red"); //$NON-NLS-1$ //$NON-NLS-2$
		TraceComponent.startGroup(1, "Group UNO", "pre", "suf"); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
		TraceGroup.startOptions();
		TraceGroup.writeOption("JeesTag", "<Tampura>"); //$NON-NLS-1$ //$NON-NLS-2$
		TraceGroup.writeOption("NoTag", "<Ilkves>"); //$NON-NLS-1$ //$NON-NLS-2$
		TraceGroup.endOptions();
		TraceGroup.startTrace(-1, null);
		Trace.startOptions();
		Trace.writeOption("MyTag", "Value"); //$NON-NLS-1$ //$NON-NLS-2$
		Trace.endOptions();
		Trace.writeInstance(99, 1, 22, "testMethod", "testClass"); //$NON-NLS-1$ //$NON-NLS-2$ CodForChk_Dis_Magic
		TraceGroup.endTrace();
		TraceGroup.startTrace(-1, null);
		Trace.writeInstance(100, 1, 235, "heyBabe", "auts"); //$NON-NLS-1$ //$NON-NLS-2$ CodForChk_Dis_Magic
		Trace.writeInstance(100, 2, 291, "heyBabe2", "auts2"); //$NON-NLS-1$ //$NON-NLS-2$ CodForChk_Dis_Magic
		TraceGroup.endTrace();
		TraceComponent.endGroup();
		Dictionary.endComponent();
		Dictionary.endDictionary();

		TraceDictionaryEncoder encoder2 = new TraceDictionaryEncoder();
		encoder2.createNewDictionary(DICTIONARY2);
		Dictionary.startDictionary();
		Dictionary.startMetaData();
		Dictionary.writeMetaData("id", "1"); //$NON-NLS-1$ //$NON-NLS-2$
		Dictionary.writeMetaData("name", "Metadata Example"); //$NON-NLS-1$ //$NON-NLS-2$
		Dictionary.endMetaData();
		TypeDefStore.startTypeDefs();
		TypeDefStore.writeExternalDefFile("StandardTypeDefs.xml"); //$NON-NLS-1$
		TypeDefStore.writeTypeDef("TUint", 4, "u", DataType.ENUM); //$NON-NLS-1$ //$NON-NLS-2$ CodForChk_Dis_Magic
		TypeDefStore.startTypeDef("TBool", 1, null, DataType.ENUM); //$NON-NLS-1$
		TypeDef.writeTypeMember(0, "EFalse", null); //$NON-NLS-1$
		TypeDef.writeTypeMember(1, "ETrue", null); //$NON-NLS-1$
		TypeDefStore.endTypeDef();
		TraceDataStore.startDataStore();
		TraceDataStore.writeData(1, DataType.STRING, "My integer value: %d"); //$NON-NLS-1$
		TraceDataStore.endDataStore();
		LocationStore.startLocations();
		LocationStore.startPath("../src"); //$NON-NLS-1$
		LocationStore.writeFile(1, "foo.cpp"); //$NON-NLS-1$
		LocationStore.writeFile(2, "foo2.cpp"); //$NON-NLS-1$ CodForChk_Dis_Magic
		LocationStore.endPath();
		LocationStore.endLocations();
		Dictionary.startComponent(12345678, "ComName", "ComPrefix", null); //$NON-NLS-1$ //$NON-NLS-2$ CodForChk_Dis_Magic
		TraceComponent.startGroup(1, "Flow", "GroupPrefix", "GroupSuffix"); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
		TraceGroup.startTrace(1, null);
		Trace.writeInstance(1, 1, 14, "MyGlobalMethodL", null); //$NON-NLS-1$ CodForChk_Dis_Magic
		Trace.writeInstance(2, 1, 214, "MyClassMethod", null); //$NON-NLS-1$ CodForChk_Dis_Magic
		TraceGroup.endTrace();
		TraceComponent.endGroup();
		Dictionary.endComponent();
		Dictionary.endDictionary();

	}

}
