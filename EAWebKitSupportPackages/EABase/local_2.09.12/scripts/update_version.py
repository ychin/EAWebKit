# --------------------------------------------------------------------------
# This script updates the EABase version header.
# --------------------------------------------------------------------------

import os
import sys
import subprocess

# -----------------------------------------------------------
def GetScriptFilePath():
	return os.path.abspath(os.path.dirname(sys.argv[0]))

def GetVersionStr():
	return sys.argv[1]

def GetVersionN():
	return GetVersionStr().replace(".", "");

def Stringify(str):
	return '\"{}\"'.format(str)

# -----------------------------------------------------------
def Sed(expr):
	subprocess.call(["sed", "-i", expr, R"..\include\Common\EABase\version.h"], shell=False, cwd=GetScriptFilePath())

def ReplaceMacro(macroName, macroValue):
	Sed("/#define {} /s/.*/    #define {} {}/".format(macroName, macroName, macroValue))

def Main():
	ReplaceMacro("EABASE_VERSION",   Stringify(GetVersionStr()))
	ReplaceMacro("EABASE_VERSION_N", GetVersionN())

# -----------------------------------------------------------
# script entry 
# -----------------------------------------------------------
if __name__ == "__main__":
	Main()
