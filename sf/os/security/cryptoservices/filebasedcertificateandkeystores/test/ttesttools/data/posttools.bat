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

copy \epoc32\winscw\c\backup_cert\cacerts.dat		\epoc32\WINSCW\C\private\101f72a6\cacerts.dat
copy \epoc32\winscw\c\backup_cert\certclients.dat	\epoc32\WINSCW\C\private\101f72a6\certclients.dat
copy \epoc32\winscw\c\backup_cert\keys.dat		\epoc32\WINSCW\C\private\101f72a6\keys.dat

del /s /f /q \epoc32\winscw\c\backup_cert\