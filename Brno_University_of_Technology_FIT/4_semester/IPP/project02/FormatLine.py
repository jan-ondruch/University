

#SYN:xondru14


import ErrCode
import re
import weakref
import sys


class FormatLine:
	"""
	Stores elements of one line of the formatfile.
	"""

	# Dictionary for commands translation into HTML tags
	# "T" - represents text; "N" - represents number (dec || hex)
	commands = {"bold":"<b>T</b>", 
				"italic":"<i>T</i>",
				"underline":"<u>T</u>",
				"teletype":"<tt>T</tt>",
				re.compile(r'^size:[1-7]$'):"<font size=[N]>T</font>",
				re.compile(r'^color:[0-9A-Fa-f]{6}$'):"<font color=#[N]>T</font>"
	}

	# Dictonary for tags lenthts
	# Stores opening tag length and closing tag lenght for each tag
	# Used for calculation of span shifts
	# Key:[OT,CT]
	tagLengths = {"bold":[3,4], 
				"italic":[3,4],
				"underline":[3,4],
				"teletype":[4,5],
				re.compile(r'^size:[1-7]$'):[13,7],
				re.compile(r'^color:[0-9A-Fa-f]{6}$'):[20,7]
	}


	_instances = set()
	class_counter = 0


	def __init__(self, inputData):
		self.line = None			# Whole line
		self.regex = None			# Regex part
		self.coms = []				# Commands part 
		self.splits = []			# Splitted line
		self.specials = False		# Counter for % symbols
		self.c = 0 					# Counter
		self.spans = []				# Spans of matched expressions 
		self.inputData = inputData	# Input data

		self._instances.add(weakref.ref(self)) # Reference to other instances
		
		# Set ID for each class instance
		self.id = FormatLine.class_counter
		FormatLine.class_counter += 1


	@classmethod
	def getinstances(cls):
		dead = set()
		for ref in cls._instances:
			obj = ref()
			if obj is not None:
				yield obj
			else:
				dead.add(ref)
		cls._instances -= dead


	def set_line(self, line):
		"""
		Sets line.
		Splits the line into 2 parts.
		Converts the regex.
		"""

		self.line = line
		self.split_line()
		self.convert_regex()


	def split_line(self):
		"""
		Splits line of the formatfile into 2 parts - regex and HTML codes.
		"""

		# Check if regex is separated by tabs from the commands
		# Regex can start with / can contain spaces (tabs not allowed)
		if re.search(r'^[ ]*\S+([ ]|\S)*\t+', self.line) is None:
			sys.stderr.write("Invalid format file format.\n")
			raise Exception(ErrCode.ERR_INPUT_FORMAT )

		# Save parts of the line 
		# Don't remove whitechars from regex
		self.splits = self.line.split('\t')
		self.regex = self.splits[0]	
		self.splits.remove(self.splits[0])	# Remove saved regex part

		# Save commands to a list
		for s in self.splits:
			s = s.replace(","," ")	# Remove ','
			# split() didn't split spaces
			# Need to separate elements manually now
			if re.search(r'\s', s):
				s = s.split()
				for subs in s:
					self.coms.append(subs)
				continue	# Return back after sub splitting
			if s:	# If not empty space
				self.coms.append(s)

		# Commands validity check
		# Try to find each command in commands dictionary as key
		# If found -> coms are valid, otherwise raise error
		for s in self.coms:
			found = False
			for k in self.commands:
   				if re.match(k, s):
   					found = True
			if found is False:
				sys.stderr.write("Invalid tag in format file.\n")
				raise Exception(ErrCode.ERR_INPUT_FORMAT)


	def convert_regex(self):
		"""
		Converts pseudo-regex into a valid regex.
		Returns:
			@self.regex - modified regex
		"""

		# !%a je správný a za chybu se nepovažuje. 
		# Jak správně píšete, žádný řetězec mu nevyhovuje.
		#self.regex = re.sub('!%a', r'', self.regex)

		# Invalid regex: ! followed by [|*+()!] 
		if re.search(r'!(?=[|*+()!])', self.line) is not None:
			sys.stderr.write("Invalid regex - forbidden char negation.\n")
			raise Exception(ErrCode.ERR_INPUT_FORMAT)


		# Iterate through the whole regex and convert each value 
		while self.c in range(len(self.regex)):

			# '%' character found
			if self.regex[self.c] is '%':
				self.specials = True

			# '!' negation character followed by '%' followed by 'X'
			elif self.regex[self.c] is '!' and self.regex[self.c + 1] is '%':

				if self.regex[self.c + 2] is 's':
					self.regex = re.sub('!%s', r'\S', self.regex, 1) # First occur.
					self.c+=1	# Move to the next char in regex
								# Compensates varied regex string length
								# Caused by re.sub
				elif self.regex[self.c + 2] is 'd':
					self.regex = re.sub('!%d', r'\D', self.regex, 1)
					self.c+=1

				elif self.regex[self.c + 2] is 'l': # ![a-z]
					self.regex = re.sub('!%l', r'[^a-z]', self.regex, 1)
					self.c+=5

				elif self.regex[self.c + 2] is 'L':	# ![A-Z]
					self.regex = re.sub('!%L', r'[^A-Z]', self.regex, 1)
					self.c+=5	

				elif self.regex[self.c + 2] is 'w':	# Nonword character
					self.regex = re.sub('!%w', r'\W', self.regex, 1)
					self.c+=1

				elif self.regex[self.c + 2] is 'W':	# SHOULD BE \W ???
					self.regex = re.sub('!%W', r'[^\w]', self.regex, 1)
					self.c+=4

				elif self.regex[self.c + 2] is 't':	# No tab
					self.regex = re.sub('!%t', r'[^\t]', self.regex, 1)
					self.c+=4

				elif self.regex[self.c + 2] is 'n':	# No newline
					self.regex = re.sub('!%n', r'[^\n]', self.regex, 1)
					self.c+=4

				# What about !%+  !%!  ... ?! -> kill them or add them

				else:
					sys.stderr.write("Invalid regex - forbidden negation.\n")
					raise Exception(ErrCode.ERR_INPUT_FORMAT)

			else:

				# Covering forbidden regex expressions
				# '(' succeeded by '.|'
				if self.regex[self.c] is '(' and (self.regex[self.c + 1] is '.' or 
					self.regex[self.c + 1] is '|'):
					sys.stderr.write("Invalid regex - forbidden combination.\n")
					raise Exception(ErrCode.ERR_INPUT_FORMAT)

				# '|' succeeded by '.|*+)'
				elif self.regex[self.c] is '|' and (self.regex[self.c + 1] is '.' or 
					self.regex[self.c + 1] is '|' or self.regex[self.c + 1] is '*' or 
					self.regex[self.c + 1] is '+' or self.regex[self.c + 1] is ')'):
					sys.stderr.write("Invalid regex - forbidden combination.\n")
					raise Exception(ErrCode.ERR_INPUT_FORMAT)

				# '.' succeeded by '.|+*)'
				elif self.regex[self.c] is '.' and (self.regex[self.c + 1] is '.' or 
					self.regex[self.c + 1] is '|' or self.regex[self.c + 1] is '*' or 
					self.regex[self.c + 1] is '+' or self.regex[self.c + 1] is ')'):
					sys.stderr.write("Invalid regex - forbidden combination.\n")
					raise Exception(ErrCode.ERR_INPUT_FORMAT)

				# '+' succeeded by '*' or vice versa
				# Checking existence of succeeding char to avoid indexing non-existing char
				try:
					if ((self.regex[self.c] is '+' and self.regex[self.c + 1] is '*') or
						(self.regex[self.c] is '*' and self.regex[self.c + 1] is '+')):
						sys.stderr.write("Invalid regex - forbidden combination.\n")
						raise Exception(ErrCode.ERR_INPUT_FORMAT)
				except:
					pass


				# Convert 'a.a' to 'aa'
				if self.regex[self.c] is '.':
					self.regex = re.sub('\.', r'', self.regex, 1)
					self.c-=1


				# %!  %.  %*  - Special character
				# %w  %L  %n  - Special regexes
				# %% - Special character '%'
				if self.specials is True:

					# Special characters
					if re.search(r'(?=[.|!*+()%])', self.regex[self.c]) is not None:
						a = '%'+self.regex[self.c]
						self.regex = re.sub('%'+self.regex[self.c], 
							self.regex[self.c], self.regex, 1)

					# Special regexes
					elif self.regex[self.c] is 's':	# BEWARE HERE, \v not included!
						self.regex = re.sub('%s', r'\s', self.regex, 1)

					elif self.regex[self.c] is 'a':	# Single character (\n included)
						self.regex = re.sub('%a', r'[.|\n]', self.regex, 1)
						self.c+=3

					elif self.regex[self.c] is 'd':	# [0-9] Digits
						self.regex = re.sub('%d', r'\d', self.regex, 1)

					elif self.regex[self.c] is 'l':	# [a-z] 
						self.regex = re.sub('%l', r'[a-z]', self.regex, 1)
						self.c+=3

					elif self.regex[self.c] is 'L': # [A-Z]
						self.regex = re.sub('%L', r'[A-Z]', self.regex, 1)
						self.c+=3

					elif self.regex[self.c] is 'w': # [a-zA-Z]
						self.regex = re.sub('%w', r'[a-zA-Z]', self.regex, 1)
						self.c+=6

					elif self.regex[self.c] is 'W': # Single word character
						self.regex = re.sub('%W', r'\w', self.regex, 1)	

					elif self.regex[self.c] is 't': # Tab char '\t'
						self.regex = re.sub('%t', r'\t', self.regex, 1)

					elif self.regex[self.c] is 'n': # Newline char '\n'
						self.regex = re.sub('%n', r'\n', self.regex, 1)


					self.specials = False	# Wait for the next '%'

			self.c+=1


		try:
			self.regex = re.compile(self.regex)
		except:
			sys.stderr.write("Invalid regex format.\n")
			raise Exception(ErrCode.ERR_INPUT_FORMAT)

		# Leaving this method undone, it suffices for the next progress


	def apply_regex(self, inputData):
		"""
		Inserts HTML tags into the inputfile.
		Wraps up the inputfile string data specified by the regex.
		Arguments:
			@inputData - input data string
		Returns:
			@inputData - modified input data string
		"""


		self.inputData = inputData	# Load data from the previous line (obejct)
		
		# For all matched expressions, find propper tags and eclose them
		# @span <class 'tuple'> as index description of one matched expression
		for span in self.spans:
			
			# Save the span
			# @tmp_s as the enclosed expression
			tmp_s = self.inputData[span[0]:span[1]]

			# Save original span length for propper recount_spans()
			sp_orig = span

			# Loop through the commands and insert the HTML tags
			# @tagLen stores total lenght of added tags
			tagLen = [0,0]
			for com in self.coms:

				# Find HTML tag in a dictionary
				# @tag as the found HTML representation
				for k in self.commands:
   					if re.match(k, com):
   						tag = self.commands[k]
   						break

   				# Find this tag length in a dictionary
   				# @tagLen [OT,ST]
				for k in self.tagLengths:
					if re.match(k, com):
						tmp_tagLen = self.tagLengths[k]
						tagLen = [x + y for x, y in zip(tagLen, self.tagLengths[k])]
						break

				# Insert the tmp_s into the tag
				# And insert the tag into the data
				if re.search(r'<font size=\[N\]>', tag) is not None:	
					tag = re.sub(r'\[N\]', com[-1:], tag, 1)	# size tag
				elif re.search(r'<font color=#\[N\]>', tag) is not None:
					tag = re.sub(r'\[N\]', com[-6:], tag, 1)	# size tag
				tag = re.sub('T', tmp_s, tag, 1)	# color tag

				# Insert the data on the indexes positions
				# Convert strings to lists to manipulate with them
				# Don't touch ... 
				tmp_l = list(self.inputData)
				tmp_l2 = tmp_l[span[1]:]
				tmp_l[span[0]:] = []
				tmp_t = list(tag)
				tmp_l = tmp_l + tmp_t + tmp_l2
				self.inputData = "".join(tmp_l)
				
				# Mofify current span position and update it in self.spans
				sp_prev = span	# Storing for other spans shifting
				span = (span[0]+tmp_tagLen[0], span[1]+tmp_tagLen[0])
				self.spans[self.spans.index(sp_prev)] = span


			# Save class instances in propper order 
			# (set later as class method called once?)
			instances = []
			for obj in FormatLine.getinstances():
				instances.append(obj)
			# Order instances
			instances = sorted(instances, key=lambda x: x.id, reverse=False)


			# To compensate inserted tags in the all class instances
			for j in range(len(instances)):

				# Modify only those instances, that haven't been processed
				obj = instances[j]
				if obj.id < self.id:
					continue
			
				# Modify all spans in the object
				modNext = False	# Trigger
				for i, sp_mod in enumerate(obj.spans):

					# No comparison for other objects - all spans are modified
					if obj.id is not self.id:
						modNext = True

					# Start to modify not worked out spans
					if modNext is False:	# Possible footshot?
						if (sp_mod is not span):
							continue
						else:
							modNext = True

					# Modify
					else:
						# Replace the original value of span in self.spans with sp_mod
						# Algorithm expects original span position
						obj.spans[i] = self.recount_spans(sp_orig, sp_mod, tagLen)

				modNext = False

		return self.inputData


	def recount_spans(self, sp_curr, sp_mod, tagLen):
		"""
		Compares span values, dedicates how much to shift the modified span
		Arguments:
			@sp_curr - original span position to which the span (sp_mod) is shifted
			@sp_mod - span we modify
			@tagLen - shift size
		Returns:
			modified span
		"""

		# tagLen parts - OT, CT
		ot = tagLen[0]
		ct = tagLen[1]
		
		# Span values (x,y)
		x1 = sp_curr[0]
		y1 = sp_curr[1]
		x2 = sp_mod[0]
		y2 = sp_mod[1]


		# sp_curr >> sp_mod; (6,8) >> (3,6)
		if y2 <= x1:
			return sp_mod
		# sp_curr << sp_mod; (3,6) << (6,8)
		elif x2 >= y1:
			return (x2+ot+ct,y2+ot+ct)
		# sp_curr fully covers sp_mod; (3,6) fully covers (3,6)
		elif x1 <= x2 and y1 >= y2:
			return (x2+ot,y2+ot)
		# sp_curr covers from right sp_mod; (3,6) covers right (2,6)
		elif x1 > x2 and y1 >= y2:
			return (x2,y2+ot)
		# sp_curr covers from left sp_mod; (3,6) covers left (3,7)
		elif x1 <= x2 and y1 < y2:
			return (x2+ot,y2+ot+ct)
		# sp_curr is covered by sp_mod; (3,6) covered by (2,7)
		else:
			return (x2,y2+ot+ct)



	def find_spans(self):
		"""
		Search all regex matches in the input data text.
		Save the matches as index spans into a list.
		"""

		# Search until there are matches
		prev_index = -1
		index = 0
		while(1):
			# Save the found MatchObject as @result
			result = self.regex.search(self.inputData, index)
			# If no match, exit the loop
			if result is None or prev_index is index:
				break
			else: 
				# 'x*' - have to check the whole string 
				# span(0,0) is ignored and index is shifted by 1
				# To search possible valid occurrences
				if result.start() is result.end():
					if index is len(self.inputData):
						break
					else:
						index+=1
						continue

				prev_index = index
				index = result.end()	# Move to the next occurrence
				self.spans.append(result.span())	# Save span