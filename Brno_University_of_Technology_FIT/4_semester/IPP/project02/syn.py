#!/usr/bin/python

#SYN:xondru14


import argparse
import sys
import string
import itertools
import re

import ErrCode
import FormatLine


def parse_args():
	"""
    Parses arguments and throws error if input arguments are invalid.
	Returns:
		@args - dictonary for parsed arguments
    """


    # Standalone help check
	if len(sys.argv) > 2 and "--help" in sys.argv:
		sys.stderr.write("Non-standalone --help message.\n")
		raise Exception(ErrCode.ERR_ARGS)

	# Use argparse for automatic help print and advanced argument check
	parser = argparse.ArgumentParser()
	parser.add_argument('--format', nargs='?', type=str,
						help='input format file to be parsed.')
	parser.add_argument('--input', nargs='?', type=str, default='stdin',
	                    help='input file in UTF-8 encoding.')
	parser.add_argument('--output', nargs='?', type=str, default='stdout',
	                	help='output file in UTF-8 encoding with formatted input.')
	parser.add_argument('--br', action='store_true', 
						help='adds <br /> element to each line of the input file.')
	

	try:
		opts = parser.parse_args()	# Parsed arguments

		# Iterate through arguments and parse according to the given properties
		for arg in sys.argv:	
			if sys.argv.count(arg) > 1:	# Catch multiple flag occurrence
				raise Exception(ErrCode.ERR_ARGS)

		# Dictionary for parsed arguments
		args = {"input":opts.input, "output":opts.output,
					"format":None, "br":opts.br}

		# Add format file if given by arguments
		if opts.format:
			args["format"] = opts.format

		return args 	# Return parsed dictionary

	# Overriding argparse exit code
	except SystemExit:
		# Catching valid --help / --h to avoid exit
		if len(sys.argv) is 2 and (sys.argv[1] == "--help" or sys.argv[1] == "--h"):
			sys.exit(0)

		sys.stderr.write("Invalid input arguments.\n")
		raise Exception(ErrCode.ERR_ARGS)

	except Exception:
		sys.stderr.write("Invalid input arguments.\n")
		raise Exception(ErrCode.ERR_ARGS)



def parse_formatfile(formatFileData, inputFileData):
	"""
    Parses lines of formatfile and stores them into FormatLine class object instances.
    Converts pseudo regex epressions into a valid regex that will be applied later.
    Arguments:
    	@formatFileData - data of the format file
    	@inputFileData - data of the input file
    """

	# Split formatfile by lines and save them into objects
	lineCount = 0
	for line in formatFileData.splitlines():	# Get linecount first
		if line: lineCount+=1					# Don't count empty lines

	# Save each line into an object instance
	# @line_objs - List of object represeting each line
	line_objs = [FormatLine.FormatLine(inputFileData) for i in range(lineCount)] 
	i = 0
	for line in formatFileData.splitlines():
		if line:	# Don't save empty lines
			line_objs[i].set_line(line)
			i+=1

	return line_objs


def main(argv):

	try:

		# Parse arguments
		args = parse_args();

		# Open input and format streams and save data from them
		# Input
		try:
			inputFile = args["input"]
			if inputFile != "stdin":
				f = open(inputFile, mode='r', encoding='utf-8')
				inputFileData = f.read()
			else:
				inputFileData = ''
				while True:
					line = sys.stdin.readline()
					if not line:
						break
					inputFileData+=line
		except Exception:
			sys.stderr.write("Error opening input file.\n")
			raise Exception(ErrCode.ERR_INPUT_READ)

		# Format
		try:
			formatFile = args["format"]
			if formatFile != None:
				with open(formatFile) as f:
					formatFileData = f.read()
		except:
			formatFileData = ""
			pass		

		# No formatfile, inputFileData will be returned
		if formatFile == None or not formatFileData:
			formatFileData = ""

		# Divide and conquer
		line_objs = parse_formatfile(formatFileData, inputFileData)

		# Find spans representing found expressions
		for i in range(len(line_objs)):
			line_objs[i].find_spans()

		# Apply regex to an input file and insert HTML tags
		for i in range(len(line_objs)):
			# Save modified data from the previous line
			inputFileData = line_objs[i].apply_regex(inputFileData)

		# Optional <br /> printing 
		if (args["br"]):
			inputFileData = re.sub('\n','<br />\n', inputFileData)

		# Printing of a final parsed file
		try:
			outputFile = args["output"]
			if outputFile != "stdout":
				outputFile = open(outputFile, mode='w', encoding='utf-8')
			else:
				outputFile = sys.stdout
			print(inputFileData,file=outputFile,end="")
		except Exception:
			sys.stderr.write("Error writing to a file.\n")
			raise Exception(ErrCode.ERR_OUTPUT_WRITE) 

		# Clean up a bit
		outputFile.close()


	except Exception as e:
		sys.exit(int(str(e)))			# Use exception value (string) as a return code


# Initialization of the main function
if __name__ == "__main__":
    main(sys.argv)