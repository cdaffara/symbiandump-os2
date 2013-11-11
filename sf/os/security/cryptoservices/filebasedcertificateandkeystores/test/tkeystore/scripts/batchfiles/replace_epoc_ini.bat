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

copy /Y \epoc32\data\epoc.ini \epoc32\data\backup_epoc.ini
del /f /q \epoc32\data\epoc.ini
copy /Y \epoc32\winscw\c\tkeystore\scripts\batchfiles\epoc_keystoreusesauthserver.ini \epoc32\data\epoc.ini







