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

copy /y \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat

copy /y \epoc32\release\winscw\udeb\tupsinteg_1.exe \epoc32\release\winscw\udeb\tupsinteg_data.exe
call \epoc32\release\winscw\udeb\testexecute z:\tups\integ\scripts\tups_integ.script
