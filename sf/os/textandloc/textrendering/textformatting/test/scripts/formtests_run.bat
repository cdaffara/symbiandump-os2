@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

@echo on
testexecute.exe  z:\form\app-framework\form\form_t_customcharmapping.script
testexecute.exe  z:\form\app-framework\form\form_t_tagmaimp.script
testexecute.exe  z:\form\app-framework\form\form_t_tmsource.script
testexecute.exe  z:\form\app-framework\form\form_t_bidicursorpos.script
testexecute.exe  z:\form\app-framework\form\form_t_tmcode.script
testexecute.exe  z:\form\app-framework\form\form_t_unique.script
testexecute.exe  z:\form\app-framework\form\form_t_undo.script
testexecute.exe  z:\form\app-framework\form\form_t_interpreter.script
testexecute.exe  z:\form\app-framework\form\form_t_format.script
testexecute.exe  z:\form\app-framework\form\form_t_inlinetext.script
testexecute.exe  z:\form\app-framework\form\form_t_graphemeiterator.script
testexecute.exe	 z:\form\app-framework\form\form_t_formbenchmark.script
testexecute.exe	 z:\form\app-framework\form\form_t_linepag.script
testexecute.exe	 z:\form\app-framework\form\form_t_formhindi.script

@echo off
md e:\logs
md e:\logs\testexecute

copy c:\logs\testexecute\form_t_*.htm e:\logs\testexecute\