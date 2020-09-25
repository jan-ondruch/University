<?php 
	
	#XQR:xondru14

	/*
	
	php -d open_basedir=\"\" xqr.php --qf=./xqr-supplementary-tests/test06.qu --input=./xqr-supplementary-tests/test06.in  --output=output.out -n

	 */
	
	require_once 'Parser.php';


	const EXIT_SUCCESSFUL 				= 0;
	const EXIT_WRONG_ARGS 				= 1;
	const EXIT_WRONG_FREAD	 			= 2;
	const EXIT_WRONG_FWRITE 			= 3;
	const EXIT_WRONG_FFORMAT			= 4;
	const EXIT_WRONG_SEMANTICS			= 80;

	// program start
	try {

		parseArgs($argv, $argc);

		$parser = new Parser($inputPath, $outputPath, $queryText);

		// parse input XML data file
		$parser->parseQuery();	// now both qf and query options are in "queryText" -> let's parse

		$xmlData = null;
		$xmlData = $parser->GetXmlText($xmlData);	// get parsed data from Parser class

    	// write root if required
    	if (!empty($rootElem)) 
    		$xmlData = "\n<{$rootElem}>\n" . "$xmlData" . "\n</{$rootElem}>\n";

    	// write head if NOT-required
    	if (!$genHead) 
    		$xmlData = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" . "$xmlData";

    	// write XML data to file
    	$parser->writeOutput($xmlData);

	}
	catch (Exception $e) {
    	echo 'Caught exception: ',  $e->getMessage(), "\n";
    	exit($e->getCode());
	}	

	exit(EXIT_SUCCESSFUL);
	// program end
	

	// PARSING FUNCTIONS //

	/*
	 * Parses arguments
	 * @param array $argv as an input args vector
	 * @param int $argc as an input args count 
	 * @return void 
	 */
	function parseArgs($argv, $argc) {

		$shortopts = "";
		$shortopts = "n"; // These options do not accept values

		$longopts  = array(
			"help", 		// No value
		    "input:",	    // Optional value
		    "output:",      // Optional value
		    "query:",	    // Optional value
		    "qf:",		    // Optional value
		    "root::",	    // Optional value
		);

		// parsing arguments, getopt returns array - $options is array
		$opts = getopt($shortopts, $longopts);
		//var_dump($opts);

		//default variables
		global $inputPath;					// default inputPath (overwritten later if path specified)
		global $outputPath;					// default outputPath (overwritten later if path specified)
		global $rootElem;					// root element if specified
		global $genHead;					// generating header, set to false by default
		global $queryText;					// file xor text on cli

		$inputPath = STDIN;
		$outputPath = STDOUT;
		$rootElem = null;


		// long params
		// iterate over args and parse them in the cycle, deriving a few vars to specify how to create XML selection
		foreach ($opts as $k => $v) {

			static $QueryVal; 


			// catching invalid arguments, e.g. --nonsense
			if (($argc - 1) != count($opts)) 
				throw new Exception('Invalid arguments.', EXIT_WRONG_ARGS);
			// catching the same arguments repeated on the cli
			if (!(count($v) === 1)) 
				throw new Exception('Repeated arguments.', EXIT_WRONG_ARGS);

			// catching non-standalone help
			if (!(strcmp($k, "help"))) {
				if ($argc != 2) 
					throw new Exception('Wrong arguments (standalone help).', EXIT_WRONG_ARGS);

				printHelp();
				exit(EXIT_SUCCESSFUL);
			}

			if (!(strcmp($k, "input"))) $inputPath = $v;
			if (!(strcmp($k, "output"))) $outputPath = $v;
			if (!(strcmp($k, "n"))) $genHead = true;
			if (!(strcmp($k, "root"))) $rootElem = $v;

			// if query && qf -> return 1
			if (!(strcmp($k, "query"))) {
				if ($QueryVal) 
					throw new Exception('Cannot use both --query and --qf', EXIT_WRONG_ARGS);

				$queryText = $v;
				$QueryVal = true;
			}

			// if query && qf -> return 1
			if (!(strcmp($k, "qf"))) {
				if ($QueryVal) 
					throw new Exception('Cannot use both --query and --qf', EXIT_WRONG_ARGS);

				$queryText = "qf" . $v;	// so __construct in Parser knows, if file / cli text
				$QueryVal = true;
			}

		}

	}

	/*
	 * Prints help message
	 * @return void
	 */
	function printHelp() {
		echo "Usage: xqr.php [options] ...
		xqr.php --help to see this message
		both short (-help) and long options (--help) are supported
		--input=filename\t\tInput file in XML format.
		--output=filename\t\tOutput file in XML format.
		--query='dotaz'\t\t\tQuery input in specified query language.
		--qf=filename\t\t\tQuery input in specified query language read from filename.
		--n\t\t\t\tXML header is not generated.
		--root=element\t\t\tRoot element name encapsulating results.
		if --input nor --output options are used, STDIN, resp. STDOUT are used.\n";
	}

?>