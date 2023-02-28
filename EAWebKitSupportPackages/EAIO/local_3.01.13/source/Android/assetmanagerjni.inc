// (c) Electronic Arts. All Rights Reserved.

// Note: A copy of this class is used by both EAIO and rwfilesystem.  If you
// make any changes to this file you should also integrate those changes into
// the other package to keep them in sync.


#include "assetmanagerjni.h"
#include <EAIO/FnEncode.h>

#ifndef EA_STRLCPY_VERIFY
	#define EA_STRLCPY_VERIFY(Strlcpy, DestCapacity) \
	EA_DISABLE_VC_WARNING(4127) \
	  do { \
		size_t ret = Strlcpy; \
		EA_UNUSED(ret); \
		EA_ASSERT(ret < DestCapacity); \
	  } while(0) \
	EA_RESTORE_VC_WARNING()
#endif

// Some useful JNI docs:
//   http://download.oracle.com/docs/cd/E17476_01/javase/1.4.2/docs/guide/jni/spec/jniTOC.html
//   http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html

void AssetManagerJNI::Startup(JNIEnv *env, jobject assetManager, jstring dataDir, jstring filesDir, jstring externalDir)
{
	sOriginalThreadId = EA::Thread::GetThreadId();
	int getJavaVMResult = env->GetJavaVM(&sVM);
	(void)getJavaVMResult;
	EA_ASSERT_MSG(getJavaVMResult == 0, "Unable to get the Java VM from the JNI environment.");

	// Get android paths.
	const char *dataDirNative = env->GetStringUTFChars(dataDir, 0);
	const char *filesDirNative = env->GetStringUTFChars(filesDir, 0);
	EA_STRLCPY_VERIFY(EA::StdC::Strlcpy(sAndroidDataRoot, dataDirNative, EAArrayCount(sAndroidDataRoot)), EAArrayCount(sAndroidDataRoot));
	EA_STRLCPY_VERIFY(EA::StdC::Strlcpy(sAndroidFileRoot, filesDirNative, EAArrayCount(sAndroidFileRoot)), EAArrayCount(sAndroidFileRoot));
	EA_STRLCPY_VERIFY(EA::StdC::Strlcpy(sAndroidTempRoot, filesDirNative, EAArrayCount(sAndroidTempRoot)), EAArrayCount(sAndroidTempRoot));
	EA::StdC::Strlcat(sAndroidTempRoot, "/tmp", EAArrayCount(sAndroidTempRoot));
	env->ReleaseStringUTFChars(filesDir, filesDirNative);
	env->ReleaseStringUTFChars(dataDir, dataDirNative);

	const char *externalDirNative = env->GetStringUTFChars(externalDir, 0);
	EA_STRLCPY_VERIFY(EA::StdC::Strlcpy(sAndroidExternalRoot, externalDirNative, EAArrayCount(sAndroidExternalRoot)), EAArrayCount(sAndroidExternalRoot));
	env->ReleaseStringUTFChars(externalDir, externalDirNative);

	// Get InputStream class and methods
	jclass class_InputStream = env->FindClass("java/io/InputStream");
	EA_ASSERT(class_InputStream != 0);
	sMethodId_InputStream_Read = env->GetMethodID(class_InputStream, "read", "([BII)I");
	EA_ASSERT(sMethodId_InputStream_Read != 0);
	sMethodId_InputStream_Close = env->GetMethodID(class_InputStream, "close", "()V");
	EA_ASSERT(sMethodId_InputStream_Close != 0);
	sMethodId_InputStream_Skip = env->GetMethodID(class_InputStream, "skip", "(J)J");
	EA_ASSERT(sMethodId_InputStream_Skip != 0);

	// Get AssetManager class, instance and methods
	sObject_AssetManager = env->NewGlobalRef(assetManager);
	jclass class_AssetManager = env->GetObjectClass(sObject_AssetManager);
	sMethodId_AssetManager_Open = env->GetMethodID(class_AssetManager, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
	EA_ASSERT(sMethodId_AssetManager_Open != 0);
	sMethodId_AssetManager_OpenFd = env->GetMethodID(class_AssetManager, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");
	EA_ASSERT(sMethodId_AssetManager_OpenFd != 0);
	sMethodId_AssetManager_List = env->GetMethodID(class_AssetManager, "list", "(Ljava/lang/String;)[Ljava/lang/String;");
	EA_ASSERT(sMethodId_AssetManager_List != 0);

	// Get AssetFileDescriptor class and methods
	jclass class_AssetFileDescriptor = env->FindClass("android/content/res/AssetFileDescriptor");
	EA_ASSERT(class_AssetFileDescriptor != 0);
	sMethodId_AssetFileDescriptor_GetLength = env->GetMethodID(class_AssetFileDescriptor, "getLength", "()J");
	EA_ASSERT(sMethodId_AssetFileDescriptor_GetLength != 0);
	sMethodId_AssetFileDescriptor_Close = env->GetMethodID(class_AssetFileDescriptor, "close", "()V");
	EA_ASSERT(sMethodId_AssetFileDescriptor_Close != 0);

	// Create a byte array for reading file data into.
	sByteArray = (jbyteArray)env->NewGlobalRef(env->NewByteArray(BUFFER_SIZE));
}


void AssetManagerJNI::Shutdown(JNIEnv *env)
{
	ASSETMANAGER_VERIFY_INIT();
	EA_ASSERT_MSG( EA::Thread::GetThreadId() == sOriginalThreadId, "The Java thread used to shutdown EAIO didn't match the one used to start it up.");

	// Destroy all global references
	env->DeleteGlobalRef(sObject_AssetManager);
	env->DeleteGlobalRef(sByteArray);
}


jobject AssetManagerJNI::List(const AutoJNIEnv &autoEnv, const char16_t* path, int pathLength)
{
	JNIEnv *env = autoEnv.GetEnv();
	if((pathLength > 0) && (path[pathLength-1] == '/'))
	{
		// Shrink the path length by one if it ends in a trailing '/' (which the Java API's don't expect).
		pathLength--;
	}

	// jchar is 16 bit UTF16.
	jstring jPath = env->NewString((const jchar*)path, pathLength);
	jobject stringArray = env->CallObjectMethod(sObject_AssetManager, sMethodId_AssetManager_List, jPath);

	if((env->ExceptionOccurred() != NULL) || (stringArray == NULL))
	{
		env->ExceptionClear();
		return NULL;
	}

	return env->NewGlobalRef(stringArray);
}


jobject AssetManagerJNI::List(const AutoJNIEnv &autoEnv, const char32_t* path, int pathLength)
{
	JNIEnv *env = autoEnv.GetEnv();

	EA::IO::Path::PathString32 path32(path, (unsigned)pathLength);
	EA::IO::Path::PathString16 path16;
	EA::IO::ConvertPath(path16, path32.c_str());
	EA::IO::Path::StripTrailingSeparator(path16);

	jstring jPath = env->NewString((const jchar*)path16.c_str(), path16.length());
	jobject stringArray = env->CallObjectMethod(sObject_AssetManager, sMethodId_AssetManager_List, jPath);

	if((env->ExceptionOccurred() != NULL) || (stringArray == NULL))
	{
		env->ExceptionClear();
		return NULL;
	}

	return env->NewGlobalRef(stringArray);
}


bool AssetManagerJNI::GetListElement(const AutoJNIEnv &autoEnv, jobject list, int elementIndex, char16_t* buffer, size_t bufferSize)
{
	JNIEnv* env      = autoEnv.GetEnv();
	jsize   arrayLen = env->GetArrayLength((jarray)list);

	if((elementIndex >= 0) && (jsize)elementIndex < arrayLen)
	{
		// jchar and jstring are 16 bit UTF16.
		jstring      str        = (jstring)env->GetObjectArrayElement((jobjectArray)list, (jsize)elementIndex);
		jsize        stringLen  = env->GetStringLength(str);
		size_t       sizeToCopy = (size_t)stringLen < bufferSize ? (size_t)stringLen : bufferSize-1;
		const jchar* strData    = env->GetStringChars( str, NULL);

		// Do a string copy using this internal loop to avoid a dependency on an external string copy function.
		for(size_t i = 0; i < sizeToCopy; i++)
			buffer[i] = (char16_t)strData[i];
		buffer[sizeToCopy] = 0; // Null terminate the string.

		env->ReleaseStringChars(str, strData);
		env->DeleteLocalRef(str); 
		return true;
	}

	return false;
}

bool AssetManagerJNI::GetListElement(const AutoJNIEnv &autoEnv, jobject list, int elementIndex, char32_t* buffer, size_t bufferSize)
{
	JNIEnv* env      = autoEnv.GetEnv();
	jsize   arrayLen = env->GetArrayLength((jarray)list);

	if((elementIndex >= 0) && (jsize)elementIndex < arrayLen)
	{
		// jchar and jstring are 16 bit UTF16.
		jstring      str        = (jstring)env->GetObjectArrayElement((jobjectArray)list, (jsize)elementIndex);
		jsize        stringLen  = env->GetStringLength(str);
		size_t       sizeToCopy = (size_t)stringLen < bufferSize ? (size_t)stringLen : bufferSize-1;
		const jchar* strData    = env->GetStringChars( str, NULL);

		// Do a string copy using this internal loop to avoid a dependency on an external string copy function.
		for(size_t i = 0; i<sizeToCopy; i++)
			buffer[i] = (char32_t)strData[i];
		buffer[sizeToCopy] = 0; // Null terminate the string.

		env->ReleaseStringChars(str, strData);
		env->DeleteLocalRef(str); 
		return true;
	}

	return false;
}


void AssetManagerJNI::FreeList(const AutoJNIEnv &autoEnv, jobject list)
{
	JNIEnv *env = autoEnv.GetEnv();
	env->DeleteGlobalRef(list);
}


// Returns a java inputStream object and the size of the file opened.
jobject AssetManagerJNI::Open(const AutoJNIEnv &autoEnv, const char* fileName, uint64_t *outFileSize)
{
	EA_ASSERT(strstr(fileName, "//") == NULL); // Verify that there aren't any repeating separators, which we currently don't support.
	*outFileSize = -1;

	JNIEnv *env = autoEnv.GetEnv();
	jstring jFileName = env->NewStringUTF(fileName);
	jobject inputStream = env->CallObjectMethod(sObject_AssetManager, sMethodId_AssetManager_Open, jFileName);

	if(env->ExceptionOccurred() != NULL)
	{
		env->ExceptionClear();
		return NULL;
	}

	jobject fileDescriptor = env->CallObjectMethod(sObject_AssetManager, sMethodId_AssetManager_OpenFd, jFileName);
	if(env->ExceptionOccurred() == NULL)
	{
		*outFileSize = env->CallLongMethod(fileDescriptor, sMethodId_AssetFileDescriptor_GetLength);
		env->CallVoidMethod(fileDescriptor, sMethodId_AssetFileDescriptor_Close);
		env->DeleteLocalRef(fileDescriptor);
	}
	else
	{
		// We can only get the file descriptor if the file was stored in
		// the package without compression.  If the file was compressed
		// when it was placed in the package then attempting to get a file
		// descriptor will throw an exception.
		env->ExceptionClear();
	}
	
	fileDescriptor = NULL;
	return env->NewGlobalRef(inputStream);
}


int AssetManagerJNI::Read(const AutoJNIEnv &autoEnv, jobject inputStream, void *pData, size_t nSize)
{
	// Note: locking could be reduced by having the user code explicitly
	// lock/unlock.  However this would be more complicated and error prone
	// so currently we do the locking within this Read call.  The change
	// could be made as a future optimization if the current scheme is too
	// slow.
	JNIEnv *env = autoEnv.GetEnv();
	env->MonitorEnter(sByteArray);

	size_t size = (nSize > BUFFER_SIZE) ? BUFFER_SIZE : nSize;
	int amountRead = env->CallIntMethod(inputStream, sMethodId_InputStream_Read, sByteArray, 0, size);
	if(amountRead > 0)
	{
		env->GetByteArrayRegion(sByteArray, 0, amountRead, reinterpret_cast<jbyte*>(pData));
	}

	env->MonitorExit(sByteArray);
	return amountRead;
}


void AssetManagerJNI::Close(const AutoJNIEnv &autoEnv, jobject inputStream)
{
	JNIEnv *env = autoEnv.GetEnv();
	env->CallVoidMethod(inputStream, sMethodId_InputStream_Close);
	env->DeleteGlobalRef(inputStream);
	inputStream = NULL;
}


// Skips forward by 'n' bytes.
// Returns the number of bytes actually skipped.
jlong AssetManagerJNI::Skip(const AutoJNIEnv &autoEnv, jobject inputStream, jlong n)
{
	JNIEnv *env = autoEnv.GetEnv();
	jlong result = env->CallLongMethod(inputStream, sMethodId_InputStream_Skip, n);
	if(env->ExceptionOccurred() != NULL)
	{
		env->ExceptionClear();
		return -1;
	}
	return result;
}


// Note: This function reads the entire file in order to determine file size.  This function
// should only be called as a last resort when the file size is required and the size can't
// be determined from the file descriptor.
uint64_t AssetManagerJNI::DetermineFileLengthBySkipping(const AutoJNIEnv &autoEnv, const char* fileName)
{
	JNIEnv *env = autoEnv.GetEnv();
	jstring jFileName = env->NewStringUTF(fileName);
	jobject inputStream = env->CallObjectMethod(sObject_AssetManager, sMethodId_AssetManager_Open, jFileName);

	if(env->ExceptionOccurred() != NULL)
	{
		env->ExceptionClear();
		return -1;
	}

	// Now that the file is opened it can be 'skipped' until the end of the file to measure the file length.
	const jlong SKIP_LENGTH = 256*1024;
	uint64_t length = 0;
	for(;;)
	{
		jlong skippedAmount = Skip(autoEnv, inputStream, SKIP_LENGTH);
		if(skippedAmount <= 0)
		{
			break;
		}
		else
		{
			length += static_cast<uint64_t>(skippedAmount);
		}
	}

	env->CallVoidMethod(inputStream, sMethodId_InputStream_Close);
	return length;
}


bool AssetManagerJNI::Exists(const AutoJNIEnv &autoEnv, const char* fileName)
{
	// We can implement this in one of two ways: 
	//    1 use open to see if we can open the file.
	//    2 use list to get a list of files and see if fileName is a member.
	// Currently we use the first.

	EA_ASSERT(strstr(fileName, "//") == NULL); // Verify that there aren't any repeating separators, which we currently don't support.

	JNIEnv *env = autoEnv.GetEnv();
	jstring jFileName = env->NewStringUTF(fileName);
	jobject inputStream = env->CallObjectMethod(sObject_AssetManager, sMethodId_AssetManager_Open, jFileName);

	if(env->ExceptionOccurred() != NULL) // If the open failed...
	{
		env->ExceptionClear();
		return false;
	}

	env->CallVoidMethod(inputStream, sMethodId_InputStream_Close);
	return true;
}


EA::Thread::ThreadId    AssetManagerJNI::sOriginalThreadId;
JavaVM*                 AssetManagerJNI::sVM = NULL;
char                    AssetManagerJNI::sAndroidDataRoot[512];
char                    AssetManagerJNI::sAndroidFileRoot[512];
char                    AssetManagerJNI::sAndroidExternalRoot[512];
char                    AssetManagerJNI::sAndroidTempRoot[512];
jobject                 AssetManagerJNI::sObject_AssetManager;
jbyteArray              AssetManagerJNI::sByteArray;
jmethodID               AssetManagerJNI::sMethodId_InputStream_Read;
jmethodID               AssetManagerJNI::sMethodId_InputStream_Close;
jmethodID               AssetManagerJNI::sMethodId_InputStream_Skip;
jmethodID               AssetManagerJNI::sMethodId_AssetManager_Open;
jmethodID               AssetManagerJNI::sMethodId_AssetManager_OpenFd;
jmethodID               AssetManagerJNI::sMethodId_AssetManager_List;
jmethodID               AssetManagerJNI::sMethodId_AssetFileDescriptor_GetLength;
jmethodID               AssetManagerJNI::sMethodId_AssetFileDescriptor_Close;

