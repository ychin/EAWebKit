This ReadMe.txt file goes with the EAText test data folder.

The files in this folder are inlined into the source code and do not need to be present as independent files with the unit test executable. 

Any .ttf.inl files that are in this file are C++ source code files that are a C binary array declaration equivalent to their corresponding .ttf file. There is a utility that Paul Pedriana maintains and which is called ConvertBinaryFileToCArrayDeclaration and can convert any binary file to a C array declaration suitable for use in a C or C++ program. This file should be part of the UTF Tools source code.

Example usage:

"C:\Program Files (x86)\ConvertBinaryFileToCArrayDeclaration.exe"  D:\Projects\EAOS\UTF\DL\UTFApp\EAText\dev\test\source\data\Test.bmpFont > D:\Projects\EAOS\UTF\DL\UTFApp\EAText\dev\test\source\data\Test.bmpFont.inl



