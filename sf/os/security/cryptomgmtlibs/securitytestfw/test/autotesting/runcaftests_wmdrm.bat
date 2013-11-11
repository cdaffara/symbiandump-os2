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

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\tcaf.script
move c:\logs\testexecute\tcaf.htm e:\testresults\tcaf.htm
del c:\tcaf\*.*

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\tcafnodrmcap.script
move c:\logs\testexecute\tcafnodrmcap.htm e:\testresults\tcafnodrmcap.htm
del c:\tcaf\*.*

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\drmfileperformance_armv5.script
move c:\logs\testexecute\drmfileperformance_armv5.htm e:\testresults\drmfileperformance_armv5.htm
del c:\tcaf\*.*

md c:\rta
copy /s z:\rta\* c:\rta
testexecute c:\rta\rta.script
move c:\logs\testexecute\rta.htm e:\testresults\rta.htm
del c:\rta\*.*

md c:\tcaf
md c:\tcaf\tscaf
copy /s z:\tcaf\tscaf\* c:\tcaf\tscaf
testexecute c:\tcaf\tscaf\tscaf.script
move c:\logs\testexecute\tscaf.htm e:\testresults\tscaf.htm

md c:\rta
copy /s z:\rta\* c:\rta
testexecute c:\rta\rta_64bit.script
move c:\logs\testexecute\rta_64bit.htm e:\testresults\rta_64bit.htm
del c:\rta\*.*

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\consumerstep_64bit.script
move c:\logs\testexecute\consumerstep_64bit.htm e:\testresults\consumerstep_64bit.htm

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\tcafwmdrmsupport.script
move c:\logs\testexecute\tcafwmdrmsupport.htm e:\testresults\tcafwmdrmsupport.htm

md c:\tcaf
md c:\tcaf\tmtputils
copy /s z:\tcaf\tmtputils\* c:\tcaf\tmtputils
testexecute c:\tcaf\tmtputils\tmtputils.script
move c:\logs\testexecute\tmtputils.htm e:\testresults\tmtputils.htm

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\tcafhelper.script
move c:\logs\testexecute\tcafhelper.htm e:\testresults\tcafhelper.htm
