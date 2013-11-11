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

rmdir /s/q \epoc32\wins\c
rmdir /s/q \epoc32\winscw\c
rmdir /s/q \epoc32\drive_d
rmdir /s/q \epoc32\drive_e

xcopy /y/e/i/q \backup\wins\c \epoc32\wins\c
xcopy /y/e/i/q \backup\winscw\c \epoc32\winscw\c
xcopy /y/e/i/q \backup\drive_d \epoc32\drive_d
xcopy /y/e/i/q \backup\drive_e \epoc32\drive_e
