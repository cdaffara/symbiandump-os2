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

del *.pem *.cer *.der /q
for %%i in (*.cnf) do d:\openssl\bin\openssl req -x509 -newkey rsa:1024 -keyout %%~ni_key.pem -passout pass:pass -outform der -out %%~ni.der -set_serial 1 -config .\%%i
