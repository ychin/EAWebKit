###########################################################################
# RemoveGlyphs
#
# FontLab Python script to remove unneeded glyphs.
# by Paul Pedriana
#
# This script must be run from within FontLab.
# This script should be placed in the FontLab/Macros/<DirNameOfYourChoice> directory.
#
# This script removes all glyphs from a font based on a list of Unicode
# characters to retain. It is useful for taking some large font and
# paring it down to a minimum required set of supported characters.
# This allows for saving disk space and runtime memory. This is useful
# only if you know that you will need just the characters you are
# retaining in the font, as might be the case with applications that
# use a fixed number of characters known ahead of time.
# This script isn't smart enough to understand OpenType tables that may
# be present in the font and thus this script may remove more glyphs
# than intended for some fonts, in particular fonts with Arabic, Thai,
# and similar complex OpenType information.
#
# Currently it expects a file called C:\temp\KeepListUnicode.txt
# which has one line per Unicode character to retain, like so:
#   0x0020
#   0x0021
#   0x0022
# The list doesn't need to be sorted, doesn't need to use hex numbering,
# and it doesn't need to be unique.
#
###########################################################################


import string


# The echo variable sets whether verbose actions are echoed to the output.
# Set it to the ORd value of any of the kEcho flags; set to 0 for silence.
# For example, setting echo to 3 causes saves and deletions to be echoed.
kEchoSaved = 1
kEchoDeleted = 2
kEchoComponents = 4
echo = 1

# The doRemoval variable sets whether the removal is actually done.
# Set it to 1 in order to do the removal; set to 0 in order to simply
# echo what removals would occur (with echo = 1).
doRemoval = 0

# keepListUnicode is a list of all Unicode characters to retain.
# This script removes glyphs that aren't associated with the given Unicode characters.
keepListUnicode = []

# keepListGlyph is a temporary list for internal usage by this script.
keepListGlyph = []

# Fill in keepListUnicode.
f = open("C:\\temp\\KeepListUnicode.txt", 'r')
for line in f.readlines():
	keepListUnicode.append(string.atoi(line, 0))
f.close()

# We do a first pass over the glyphs in order to make a list of all the
# glyphs to keep (keepListGlyph).
for i in range(len(fl.font.glyphs)):
	keepGlyph = 0

	# Each glyph is referenced by zero or more Unicode characters.
	for u in fl.font.glyphs[i].unicodes:
		if u in keepListUnicode:
			keepGlyph = 1

	if keepGlyph == 1:
		keepListGlyph.append(i)

		# Each glyph is composed of 1 or more component glyphs.
		for c in fl.font.glyphs[i].components:
			if echo & kEchoComponents:
				print "Glyph component to save: " + str(c.index)
			keepListGlyph.append(c.index)

# We do a second pass which removes the glyphs that are not in keepListGlyph.
# We do the second pass backwards, as we are removing glyphs and thus changing
# the indexes of glyphs after each deleted glyph.
for i in range(len(fl.font.glyphs) -1, -1, -1):        
	if i not in keepListGlyph:
		if echo & kEchoDeleted:
			print "Deleting glyph: " + str(i)
		if doRemoval:
			del fl.font.glyphs[i]
	else:
		if echo & kEchoSaved:
			print "Saving glyph: " + str(i) + " " + fl.font.glyphs[i].name

# End of script
