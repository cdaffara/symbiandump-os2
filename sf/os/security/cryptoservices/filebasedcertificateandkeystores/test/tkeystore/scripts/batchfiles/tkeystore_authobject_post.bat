@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
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

del /f /q \epoc32\winscw\c\sys\bin\policytest_t_keystore.exe

rd /s /q  \epoc32\winscw\c\sys\

del /f /q \epoc32\winscw\c\t_secdlg_in.dat

del /f /q \epoc32\winscw\c\tkeystore\data\exported_banana.der

del /f /q \epoc32\winscw\c\tkeystore\data\exported_encrypted_banana.der

del /f /q \epoc32\winscw\c\tkeystore\data\exported_encrypted_mango.der

del /f /q \epoc32\winscw\c\tkeystore\data\exported_mango.der


