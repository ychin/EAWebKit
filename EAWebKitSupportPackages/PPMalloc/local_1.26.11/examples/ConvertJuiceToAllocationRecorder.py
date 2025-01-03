#######################################################################################################################
# ConvertJuiceToAllocationRecorder.py
#
# This script converts Juice recording files to PPMalloc EAAllocationRecorder recording files.
#
# The Juice file format is tab-delimited and is like so:
#       A       2             main       SBA          1352761344   32648          32648         32768             1074085349   16777352
# which means:
#       Alloc   log item #    heap name  Alloc name   return ptr   requested size actual size   requested align   time         ?
#
# and
#       F       <empty>       main       <empty>      1354860528                                                  1153635450   16777352
# which means:
#       Free                  heap name               freed ptr                                                 time         ?
#
# Note that there are many lines in a Juice file that don't begin with "A" or "F" and thus are not allocation records. 
#
# The PPMalloc file format is described in PPMalloc/EAAllocationRecorder.h like so:
#       // As an example, we have the following lines (which don't use the optional final flags argument):
#       //    m c23a17bc 20\n                  // Malloc 20 bytes. Return value was 0xc23a17bc.
#       //    a c23a17bc 20 16\n               // Malloc 20 bytes, with 16 byte alignment. Return value was 0xc23a17bc.
#       //    o c23a17bc 20 16 8\n             // Malloc 20 bytes, with 16 byte allignment and 8 byte alignment offset.
#       //    r c23a17bc 18cb34a2 28\n         // Realloc pointer 0x18cb34a2 to 28 bytes. Return value was 0xc23a17bc.
#       //    c c23a17bc 15 2\n                // Calloc 15 elements of 2 bytes each (i.e. 30 bytes). Return value was 0xc23a17bc.
#       //    f c23a17bc\n                     // Free pointer 0xc23a17bc.
#       //
#       // In the case of the flags argument, the above malloc line might be:
#       //    m c23a17bc 20 e2\n               // Malloc 20 bytes with allocation flags 0xe2. Return value was 0xc23a17bc.
# 
# The
#
#######################################################################################################################


import os, os.path, string, sys


def DecStringToHexString(str):
	return "%X" % int(str)


def ConvertLine(line, dstFile, heapName):
	if (len(line) >= 2) and ((line[0:2] == "A\t") or (line[0:2] == "F\t")):   # If the line is a memory allocation or free.
		itemArray = string.split(line, '\t')

		if (itemArray[2] == heapName):
			if (line[0:2] == "A\t"):
				if (int(itemArray[7]) <= 8):
					dstFile.write("m "  + DecStringToHexString(itemArray[4]) + " " + itemArray[5] + "\n")
				else:
					dstFile.write("a "  + DecStringToHexString(itemArray[4]) + " " + itemArray[5] + " " + itemArray[7] + "\n")
			else:
				dstFile.write("f "  + DecStringToHexString(itemArray[4]) + "\n")


# Function which converts Juice log file
def ConvertJuiceToAllocationRecorder(srcPath, heapName):
	
	if os.path.exists(srcPath) and os.path.isfile(srcPath):
		
		srcPathName, srcPathExtension = os.path.splitext(srcPath)
		dstPath = srcPathName + ".allocationRecording"

		try:
			srcFile = open(srcPath, 'r') # open for reading.

			try:
				dstFile = open(dstPath, 'w') # open for writing, erasing any previous file contents.
				
				try:
					for line in srcFile:
						ConvertLine(line, dstFile, heapName)
				except Exception, anException:
					print "Failure converting lines from file: " + srcPath
					print anException

			except Exception, anException:
				print "Can't open file: " + dstPath
				print anException

		except Exception, anException:
			print "Can't open file: " + srcPath
			print anException
	else:
		print "File doesn't exist: " + srcPath


# The following is the main entry point to this module:
def main(argv):
	for arg in argv:
		heapName = "main"
		srcPath = arg  # = "C:\\Temp\\MOHAPS3_MemLogs\\MemMetricsLog_3227.txt"
		ConvertJuiceToAllocationRecorder(srcPath, heapName)


# Call main.
if __name__ == "__main__":
	main(sys.argv[1:])

