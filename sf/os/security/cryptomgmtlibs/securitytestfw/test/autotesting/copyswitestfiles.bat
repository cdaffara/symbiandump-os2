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

@echo Copying SWI test files from the C to the Z drive...
xcopy /y/i/e/s/r/q \epoc32\winscw\c\tswi \epoc32\release\winscw\udeb\z\tswi
xcopy /y/i/e/s/r/q \epoc32\winscw\c\tswi \epoc32\release\winscw\urel\z\tswi

@echo SWI test files on the Z drive.

