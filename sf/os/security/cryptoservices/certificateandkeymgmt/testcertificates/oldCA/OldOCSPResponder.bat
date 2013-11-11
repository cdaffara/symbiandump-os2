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

openssl ocsp -CA Root1\certs\ca.pem -rsigner %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\certman\testcertificates\oldCA\OCSPSigningRoot\certs\01.pem -rkey %SECURITYSOURCEDIR%\testframework\testcertificates\certman\testcertificates\oldCA\OCSPSigningRoot\private\Signer-OCSPR.key.pem -port 18000 -index Root1\index.txt -ignore_err