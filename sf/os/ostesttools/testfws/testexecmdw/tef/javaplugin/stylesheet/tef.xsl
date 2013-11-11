<?xml version="1.0"  encoding="ISO-8859-1"?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0"
    > 
 <xsl:output method="xml" indent="yes"/>

<!-- global template match -->    
<xsl:template match="/">
    <xsl:apply-templates/>
</xsl:template>

<!-- This is used for the overall report-->
<xsl:template match="LOGFILE">
    <viewer>
       <tableLayout>
        			<column position="1" name="Time" type="string" width="100"/>
       			<column position="2" name="Severity" type="string" width="100"/>
        			<column position="3" name="Thread" type="integer" width="50"/>
        			<column position="4" name="FileName" type="string" width="100"/>
        			<column position="5" name="LineNumber" type="integer" width="50"/>
        			<column position="6" name="Text" type="string" width="100"/>
        			<column position="7" name="SuiteName" type="string" width="100"/>
       			<column position="8" name="ProgramName" type="string" width="100"/>
        			<column position="9" name="ScriptName" type="string" width="100"/>
        			<column position="10" name="TestCaseName" type="string" width="100"/>
        			<column position="11" name="Timeout" type="integer" width="100"/>
        			<column position="12" name="ServerName" type="string" width="100"/>
        			<column position="13" name="TestStepName" type="string" width="100"/>
        			<column position="14" name="IniFileName" type="string" width="100"/>
        			<column position="15" name="SectionName" type="string" width="100"/>
        			<column position="16" name="ExpectedResult" type="integer" width="50"/>
        			<column position="17" name="ActualResult" type="integer" width="50"/>        			        			  		
 				<column position="18" name="ExpectedErrorCode" type="integer" width="50"/>        			
				<column position="19" name="ActualErrorCode" type="integer" width="50"/>        			
        			<column position="20" name="HeapSize" type="string" width="100"/>        			
        			<column position="21" name="Command" type="string" width="100"/>
        			<column position="22" name="Result" type="string" width="100"/>
        			<column position="23" name="Iterations" type="integer" width="50"/> 
        			<column position="24" name="Pass" type="integer" width="50"/> 
        			<column position="25" name="Fail" type="integer" width="50"/> 
        			<column position="26" name="Abort" type="integer" width="50"/>       
        			<column position="27" name="Unknown" type="integer" width="50"/> 
        			<column position="28" name="Inconclusive" type="integer" width="50"/>
        			<column position="29" name="Panic" type="integer" width="50"/>
        			<column position="30" name="Unexecuted" type="integer" width="50"/>
        			<column position="31" name="CommentedCommands" type="integer" width="50"/>        			        			        				
        </tableLayout>
         <tableData>
	       	<xsl:for-each select="MESSAGE">
		
				<row>
					<xsl:element name="element">
						<xsl:attribute name="position">1</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="TIME"/></xsl:attribute>
					</xsl:element>

					<xsl:element name="element">
						<xsl:attribute name="position">2</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="SEVERITY"/></xsl:attribute>
					</xsl:element>
	
					<xsl:element name="element">
						<xsl:attribute name="position">3</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="THREAD"/></xsl:attribute>
					</xsl:element>
		
					<xsl:element name="element">
						<xsl:attribute name="position">4</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="FILENAME"/></xsl:attribute>
					</xsl:element>
		
					<xsl:element name="element">
						<xsl:attribute name="position">5</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="LINENUMBER"/></xsl:attribute>
					</xsl:element>
	
					<xsl:element name="element">
						<xsl:attribute name="position">6</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="TEXT"/></xsl:attribute>
					</xsl:element>
	
					<xsl:element name="element">
						<xsl:attribute name="position">7</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="SUITE_NAME"/></xsl:attribute>
					</xsl:element>

					<xsl:element name="element">
						<xsl:attribute name="position">8</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="PROGRAMNAME"/></xsl:attribute>
					</xsl:element>
	
					<xsl:element name="element">
						<xsl:attribute name="position">9</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="SCRIPTNAME"/></xsl:attribute>
					</xsl:element>
	
					<xsl:element name="element">
						<xsl:attribute name="position">10</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="TESTCASENAME"/></xsl:attribute>
					</xsl:element>
	
					<xsl:element name="element">
						<xsl:attribute name="position">11</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="TIMEOUT"/></xsl:attribute>
					</xsl:element>
					
					<xsl:element name="element">
						<xsl:attribute name="position">12</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="SERVERNAME"/></xsl:attribute>
					</xsl:element>
				
					<xsl:element name="element">
						<xsl:attribute name="position">13</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="TESTSTEPNAME"/></xsl:attribute>
					</xsl:element>
				
					<xsl:element name="element">
						<xsl:attribute name="position">14</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="INIFILENAME"/></xsl:attribute>
					</xsl:element>
			
					<xsl:element name="element">
						<xsl:attribute name="position">15</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="SECTIONNAME"/></xsl:attribute>
					</xsl:element>

					<xsl:element name="element">
						<xsl:attribute name="position">16</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="EXPECTEDRESULT"/></xsl:attribute>
					</xsl:element>
			
					<xsl:element name="element">
						<xsl:attribute name="position">17</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="ACTUALRESULT"/></xsl:attribute>
					</xsl:element>
		
					<xsl:element name="element">
						<xsl:attribute name="position">18</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="EXPECTEDERRORCODE"/></xsl:attribute>
					</xsl:element>
				
					<xsl:element name="element">
						<xsl:attribute name="position">19</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="ERRORCODE"/></xsl:attribute>
					</xsl:element>				
	
					<xsl:element name="element">
						<xsl:attribute name="position">20</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="HEAPSIZE"/></xsl:attribute>
					</xsl:element>				
				
					<xsl:element name="element">
						<xsl:attribute name="position">21</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="COMMAND"/></xsl:attribute>
					</xsl:element>				
				
					<xsl:element name="element">
						<xsl:attribute name="position">22</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="RESULT"/></xsl:attribute>
					</xsl:element>				
		
					<xsl:element name="element">
						<xsl:attribute name="position">23</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="ITERATIONS"/></xsl:attribute>
					</xsl:element>			
				
					<xsl:element name="element">
						<xsl:attribute name="position">24</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="PASS"/></xsl:attribute>
					</xsl:element>					
								
					<xsl:element name="element">
						<xsl:attribute name="position">25</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="FAIL"/></xsl:attribute>
					</xsl:element>				
								
					<xsl:element name="element">
						<xsl:attribute name="position">26</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="ABORT"/></xsl:attribute>
					</xsl:element>				
								
					<xsl:element name="element">
						<xsl:attribute name="position">27</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="UNKNOWN"/></xsl:attribute>
					</xsl:element>				
				
					<xsl:element name="element">
						<xsl:attribute name="position">28</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="INCONCLUSIVE"/></xsl:attribute>
					</xsl:element>				
	
					<xsl:element name="element">
						<xsl:attribute name="position">29</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="PANIC"/></xsl:attribute>
					</xsl:element>				
			
					<xsl:element name="element">
						<xsl:attribute name="position">30</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="UNEXECUTED"/></xsl:attribute>
					</xsl:element>				
		
					<xsl:element name="element">
						<xsl:attribute name="position">31</xsl:attribute>
						<xsl:attribute name="value"><xsl:value-of select="COMMENTEDCOMMANDS"/></xsl:attribute>
					</xsl:element>				
							
				</row>	
			</xsl:for-each>	
        </tableData>
    	</viewer>
</xsl:template>


</xsl:stylesheet>