This is a project which creates a DLL for the EACallstack test. 

The purpose of such a DLL is to test EACallstack's ability to decipher 
symbols coming from DLLs as opposed to the main .exe.

This project is separate from the primary Framework2 .build file because 
Framework2 doesn't currently support the concept of an application being 
built as part static library, part DLL. It wants everything to be statically 
linked or everything to be a DLL. Until that limitation is lifted, we simply
have a tiny project here which builds a DLL. The DLL only needs to be built 
once, as it is not used in a way that is directly related to EACallstack.



