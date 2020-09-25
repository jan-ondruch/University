<?php 

#XQR:xondru14


/*
 * Class
 * Parses the whole XML input and selects output specified by query
 * Syntactic + sematics analysis
 */
class Parser
{
	
	/*
     * Parser construct
     * @param string $inputPath specifies XML data input path
     * @param string $outputPath specifies stdout of the selection
     * @param string $queryText specifies query - given by filename / string 
     * @return void 
     */
	function __construct($inputPath, $outputPath, $queryText)
	{

		// read input XML data from STDIN || specified file by --input (-input)
		if ($inputPath == STDIN) $this->xmlText = file_get_contents("php://stdin");
		else {
			if ((!($this->inputPath = fopen($inputPath, "r"))) || (!(is_readable($inputPath))))
            	throw new Exception('Wrong input file - cannot read XML data.', EXIT_WRONG_FREAD);

    		$this->xmlText = fread($this->inputPath, filesize($inputPath));
    		fclose($this->inputPath);
		}

		// set output stream for XML parser - STDOUT or specified file, if done so
		if ($outputPath == STDOUT) 
			$this->outputPath = STDOUT;
		else 
			$this->outputPath = $outputPath;

		// queryText is specified by filepath
		if (!(strcmp("qf", substr($queryText, 0, 2)))) {
			$this->queryText = substr($queryText, 2);

            // forum question -> if cannot read from file specified by --qf -> "chyba dotazu, 80"
            if (!($this->queryFile = fopen($this->queryText, "r"))) 
            	throw new Exception('Wrong input file - cannot read qf file.', EXIT_WRONG_SEMANTICS);

            $this->queryText = fread($this->queryFile, filesize($this->queryText));	// save queryText from file to a string    
            fclose($this->queryFile);	
		}
		else {
			$this->queryText = $queryText;
		}
	}


	/*
     * Writes out the final output to a file / STDOUT
     * @param string $xmlData contains the final XML data selection
     * @return void 
     */
	public function writeOutput($xmlData) {

		if ($this->outputPath == STDOUT)
			fwrite($this->outputPath, $xmlData);
		else {

            if (file_exists($this->outputPath)) unlink($this->outputPath);  // remove file if already exists

			if ((!($this->outputPath = fopen($this->outputPath, "w")))) // || (!(is_writable($this->outputPath))))
            	throw new Exception('Cannot open / write to output file.', EXIT_WRONG_FWRITE);

			fwrite($this->outputPath, $xmlData);
			// close file handle
			fclose($this->outputPath);
		} 
	}


	/*
     * Converts parsed XML data to string 
     * @param string $xmlData as a final string to output
     * @return string $xmlData
     */
	public function GetXmlText($xmlData) {

        if (empty($this->xmlText))
            return $xmlData = "";

        foreach ($this->xmlText as $str) {
            $xmlData = $xmlData . $str->asXML();
        }
		return $xmlData;
	}


	/*
     * Checks validity of the query request
     * Parses input xml text using regex
     * Selects wanted data specified by the query request
     * @return void 
     */
	public function parseQuery() {

		// test xml data for validity & create new object instance of SimpleXMLElement
		try { $this->xmlText = new SimpleXMLElement($this->xmlText); }
		catch (Exception $e) { throw new Exception("$e", EXIT_WRONG_FFORMAT); }

		// (LIMIT)? , (WHERE)? , (ORDER BY)? == optional 
		preg_match("/(SELECT.*(LIMIT)?.*FROM.*(WHERE)?.*(ORDER BY)?.*)/", $this->queryText, $queryValidity);

		if (!$queryValidity) 
			throw new Exception("Invalid query (semantics).", EXIT_WRONG_SEMANTICS);


		$queryRegParts[0]['KEYWORD']  = "SELECT";
		$queryRegParts[0]['REGEX']    = "/(?<=SELECT\s)\w+/";
		$queryRegParts[1]['KEYWORD']  = "LIMIT";
		$queryRegParts[1]['REGEX']    = "/(?<=LIMIT\s)\d+/";
		$queryRegParts[2]['KEYWORD']  = "FROM";
		$queryRegParts[2]['REGEX']    = "/(?<=FROM\s)(\w+)?(?:\.?(\w+)?)/";
		$queryRegParts[3]['KEYWORD']  = "WHERE";
		$queryRegParts[3]['REGEX']    = "/(?<=WHERE\s)((?:NOT\s)*)(\w+)?(?:\.?(\w+)?)\s*(CONTAINS|<|>|=)\s*(-?\d+|\"(\S|\s*)*\")/";
		$queryRegParts[4]['KEYWORD']  = "ORDER BY";
		$queryRegParts[4]['REGEX']    = "/(?<=ORDER BY\s)(\w+)?(?:\.?(\w+)?)\s+(?:ASC|DESC)$/";


		for ($i = 0; $i < count($queryRegParts); $i++) {

			static $parsedQueryCheck = array(null);

        	if (strpos($this->queryText, $queryRegParts[$i]['KEYWORD']) !== false) {	// if you find STRING there
            	preg_match($queryRegParts[$i]['REGEX'], $this->queryText, $queryParSecs[$queryRegParts[$i]['KEYWORD']]);
				array_push($parsedQueryCheck, $queryRegParts[$i]['KEYWORD'], $queryParSecs[$queryRegParts[$i]['KEYWORD']][0]);
        	}
    	}

    	$parsedQueryCheck = implode(" ", $parsedQueryCheck);
        $parsedQueryCheck = substr($parsedQueryCheck, 1);   // remove " " from start of array (implode side effect)

 		// remove all whitespace from the end of input query & beginning of it
        $this->queryText = rtrim($this->queryText, " \t\n\r\0\x0B");
        $this->queryText = trim($this->queryText, " \t\n\r\0\x0B");

        $parsedQueryCheck = rtrim($parsedQueryCheck, " \t\n\r\0\x0B");
        $parsedQueryCheck = trim($parsedQueryCheck, " \t\n\r\0\x0B");

        /*
        echo "$this->queryText\n";
        echo "$parsedQueryCheck\n";

        $a = strlen($this->queryText);
        $b = strlen($parsedQueryCheck);
        echo "A: $a ; B: $b\n";
        */

    	// SYNTAX CHECK
    	if (strcmp($parsedQueryCheck, $this->queryText))
    		throw new Exception("Invalid query (semantics, advanced).", EXIT_WRONG_SEMANTICS);


        $xpathPartFrom = $xpathPartSelect = $xpathPartWhere = $xpathFull = null;
        $isContains = false;


    	// split matched arrays into single semantic elements

    	// if element.attribute -> [0] => element ; [1] => attribute
    	// else if .attribute -> [0] =>  ; [1] => attribute
    	// else if element [0] => element
    	if (isset($queryParSecs['FROM'])) {

            // FROM is empty -> empty output file (+head possibly)
            if (empty($queryParSecs['FROM'][0]) && empty($queryParSecs['FROM'][1]))
                return $this->xmlText = "";
            else
                $queryParSecs['FROM'] = preg_split("/[.]/", $queryParSecs['FROM'][0]);
        }

    	if (isset($queryParSecs['WHERE'])) {

            if (empty($queryParSecs['WHERE'][2]) && empty($queryParSecs['WHERE'][3]))   
                throw new Exception("Invalid query (empty WHERE condition).", EXIT_WRONG_SEMANTICS);

    		// handling "NOT" (2x NOT == no NOT)
			if ($notCount = substr_count($queryParSecs['WHERE'][1], "NOT") % 2 == 0) 
				$queryParSecs['WHERE'][1] = null;
			else 
				$queryParSecs['WHERE'][1] = "NOT"; 		
		}

    	// same as 'FROM' - ASC or DESC are found behind the last element/attribute string (easy to take out ...)
    	if (isset($queryParSecs['ORDER BY']))
        	$queryParSecs['ORDER BY'] = preg_split("/[.]/", $queryParSecs['ORDER BY'][0]);


        // check element name validity
        $this->elementCheck($queryParSecs['FROM'][0]);
        if (isset($queryParSecs['WHERE'])) 
            $this->elementCheck($queryParSecs['WHERE'][2]);


    	// choose data according to $queryParSecs parsed content


    	// SELECT
    	$xpathPartSelect = "//" . $queryParSecs['SELECT'][0];


    	// FROM
    	// can FROM have empty string afterwards ??
    	if (!strcmp($queryParSecs['FROM'][0], "ROOT"))
    		$xpathPartFrom = "//*";	// root is only one / - would it work? 

    	// element.attribute
    	else if (!empty($queryParSecs['FROM'][0]) && !empty($queryParSecs['FROM'][1]))
    		$xpathPartFrom = $xpathPartFrom . "//{$queryParSecs['FROM'][0]}" . "[@{$queryParSecs['FROM'][1]}]" . "[1]";	// last . "[1]" selects only the first occurance

    	// element
    	else if (!empty($queryParSecs['FROM'][0]) && empty($queryParSecs['FROM'][1]))
    		$xpathPartFrom = $xpathPartFrom . "//{$queryParSecs['FROM'][0]}" . "[1]";

    	// .attribute
    	else 
    		$xpathPartFrom = $xpathPartFrom . "//*" . "[@{$queryParSecs['FROM'][1]}]" . "[1]";


    	// WHERE
    	if (isset($queryParSecs['WHERE'])) {

    		//contains()
    		if (!strcmp($queryParSecs['WHERE'][4], "CONTAINS")) {
    			// on number literal cannot be used - CHECK OUT REGEX HERE (floats)
    			preg_match('/^(\+|\-)?\d+$/', $queryParSecs['WHERE'][5], $containsValidity);
    			if ($containsValidity)	// if you find number there (with no parentheses)
    				throw new Exception("Invalid query (semantics, contains on number literal).", EXIT_WRONG_SEMANTICS);

    			$isContains = true;
    		}	

    		// element.attribute
    	 	if (!empty($queryParSecs['WHERE'][2]) && !empty($queryParSecs['WHERE'][3])) {

    			// only attribute
    			if ($isContains) 
    				$xpathPartWhere = $xpathPartWhere . "contains(@{$queryParSecs['WHERE'][3]}, {$queryParSecs['WHERE'][5]})";
    			else
    				$xpathPartWhere = $xpathPartWhere . "{$queryParSecs['WHERE'][2]}" . "[@{$queryParSecs['WHERE'][3]}]";	
    		}
	
    		// element
    		else if (!empty($queryParSecs['WHERE'][2]) && empty($queryParSecs['WHERE'][3])) {

    			if ($isContains) {
    				$xpathPartWhere = $xpathPartWhere . "contains(., {$queryParSecs['WHERE'][5]})";
                }
    			else 
    				$xpathPartWhere = $xpathPartWhere . "{$queryParSecs['WHERE'][2]}";

    		}

    		// .attribute
    		else if (empty($queryParSecs['WHERE'][2]) && !empty($queryParSecs['WHERE'][3])) {
    			
    			if ($isContains) 
    				$xpathPartWhere = $xpathPartWhere . "contains(@{$queryParSecs['WHERE'][3]}, {$queryParSecs['WHERE'][5]})";
    			else
    				$xpathPartWhere = $xpathPartWhere . "@{$queryParSecs['WHERE'][3]}";
    		}

    		// operator - can I add CONTAINS, as well?? -what's the gist?
    		if (!$isContains) {

    			if (!strcmp($queryParSecs['WHERE'][4], "<"))									// < 
    				$xpathPartWhere = $xpathPartWhere . "<{$queryParSecs['WHERE'][5]}";
    			else if (!strcmp($queryParSecs['WHERE'][4], ">")) 								// >
    				$xpathPartWhere = $xpathPartWhere . ">{$queryParSecs['WHERE'][5]}";
    			else 																			// =
    				$xpathPartWhere = $xpathPartWhere . "={$queryParSecs['WHERE'][5]}";
    		}


    		// NOT not() xpath function -rewrite it the other way around 
    		if (!empty($queryParSecs['WHERE'][1]))
    			$xpathPartWhere = "[not({$xpathPartWhere})]";
    		else
    			$xpathPartWhere = "[{$xpathPartWhere}]";

    	}


    	// finalpath = FROM - SELECT - WHERE
    	$xpathFull = $xpathPartFrom . $xpathPartSelect . $xpathPartWhere;
    	//echo "$xpathFull\n";
        //$xpathFull = "//*//room[*[@id]=2]";
    	$this->xmlText = $this->xmlText->xpath($xpathFull);


    	// LIMIT   	
    	if (isset($queryParSecs["LIMIT"]))
    		$this->xmlText = array_slice($this->xmlText, 0, $queryParSecs['LIMIT'][0]);

        // ORDER BY
        // extension, hasn't been completed

	}


    /*
     * Checks validity of elements names
     * @param string $element as an element of query request
     * @return void 
     */
    private function elementCheck($element) {

        if (!strcmp($element, "SELECT") || !strcmp($element, "LIMIT") || 
            !strcmp($element, "FROM") || !strcmp($element, "WHERE") ||
            !strcmp($element, "ORDER BY"))
        throw new Exception("Invalid query (semantics, invalid element name).", EXIT_WRONG_SEMANTICS);

    }

}

?>