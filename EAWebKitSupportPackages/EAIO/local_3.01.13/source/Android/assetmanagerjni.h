// (c) Electronic Arts. All Rights Reserved.

// Note: A copy of this class is used by both EAIO and rwfilesystem.  If you
// make any changes to this file you should also integrate those changes into
// the other package to keep them in sync.
//
// Note: The file including this header must ensure eathread/eathread.h has been included.

#ifndef ASSETMANAGERJNI_H_INCLUDED
#define ASSETMANAGERJNI_H_INCLUDED


class AutoJNIEnv;


#ifndef ASSETMANAGER_INIT_FAILURE_MESSAGE
	#error "Packages using this header should define an init failure message"
#endif


#define ASSETMANAGER_VERIFY_INIT() \
	EA_ASSERT_MSG(AssetManagerJNI::GetVM() != NULL, ASSETMANAGER_INIT_FAILURE_MESSAGE);

class AssetManagerJNI
{
public:

	static void Startup(JNIEnv* env, jobject assetManager, jstring dataDir, jstring filesDir, jstring externalDir);

	static void Shutdown(JNIEnv* env);

	// Returns a java inputStream object and the size of the file opened.
	static jobject Open(const AutoJNIEnv& autoEnv, const char* fileName, uint64_t* outFileSize);

	static int Read(const AutoJNIEnv& autoEnv, jobject inputStream, void* pData, size_t nSize);

	static void Close(const AutoJNIEnv& autoEnv, jobject inputStream);

	// Skips forward by 'n' bytes.
	// Returns the number of bytes actually skipped.
	static jlong Skip(const AutoJNIEnv& autoEnv, jobject inputStream, jlong n);

	// Note: This function reads the entire file in order to determine file size.  This function
	// should only be called as a last resort when the file size is required and the size can't
	// be determined from the file descriptor.
	static uint64_t DetermineFileLengthBySkipping(const AutoJNIEnv& autoEnv, const char* fileName);

	static bool Exists(const AutoJNIEnv& autoEnv, const char* fileName);

	static jobject List(const AutoJNIEnv& autoEnv, const char16_t* path, int pathLength);
	static jobject List(const AutoJNIEnv& autoEnv, const char32_t* path, int pathLength);
	#if EA_WCHAR_UNIQUE
		inline static jobject List(const AutoJNIEnv& autoEnv, const wchar_t* path, int pathLength)
		{
			#if (EA_WCHAR_SIZE == 2)
				return List(autoEnv, reinterpret_cast<const char16_t*>(path), pathLength);
			#elif (EA_WCHAR_SIZE == 4)
				return List(autoEnv, reinterpret_cast<const char32_t*>(path), pathLength);
			#endif
		}
	#endif

	static bool GetListElement(const AutoJNIEnv& autoEnv, jobject list, int elementIndex, char16_t* buffer, size_t bufferSize);
	static bool GetListElement(const AutoJNIEnv& autoEnv, jobject list, int elementIndex, char32_t* buffer, size_t bufferSize);
	#if EA_WCHAR_UNIQUE
		inline static bool GetListElement(const AutoJNIEnv& autoEnv, jobject list, int elementIndex, wchar_t* buffer, size_t bufferSize)
		{
			#if (EA_WCHAR_SIZE == 2)
				return GetListElement(autoEnv, list, elementIndex, reinterpret_cast<char16_t*>(buffer), bufferSize);
			#elif (EA_WCHAR_SIZE == 4)
				return GetListElement(autoEnv, list, elementIndex, reinterpret_cast<char32_t*>(buffer), bufferSize);
			#endif
		}
	#endif

	static void FreeList(const AutoJNIEnv& autoEnv, jobject list);

	static inline JavaVM* GetVM()
	{
		return sVM;
	}

	// Returns the ThreadId of the thread used to initialize the system.
	static inline EA::Thread::ThreadId GetOriginalThreadId()
	{
		ASSETMANAGER_VERIFY_INIT();
		return sOriginalThreadId;
	}
	
	static inline const char* GetDataRoot()
	{
		ASSETMANAGER_VERIFY_INIT();
		return sAndroidDataRoot;
	}

	static inline const char* GetFileRoot()
	{
		ASSETMANAGER_VERIFY_INIT();
		return sAndroidFileRoot;
	}

	static inline const char* GetExternalRoot()
	{
		ASSETMANAGER_VERIFY_INIT();
		return sAndroidExternalRoot;
	}

	static inline const char* GetTempRoot()
	{
		ASSETMANAGER_VERIFY_INIT();
		return sAndroidTempRoot;
	}

private:
	static const size_t BUFFER_SIZE = 65536;
	static EA::Thread::ThreadId sOriginalThreadId;
	static JavaVM*              sVM;
	static char                 sAndroidDataRoot[512];
	static char                 sAndroidFileRoot[512];
	static char                 sAndroidExternalRoot[512];
	static char                 sAndroidTempRoot[512];
	static jobject              sObject_AssetManager;        // The actual instance of an AssetManager that will be used by IO operations.
	static jbyteArray           sByteArray;
	static jmethodID            sMethodId_InputStream_Read;
	static jmethodID            sMethodId_InputStream_Close;
	static jmethodID            sMethodId_InputStream_Skip;
	static jmethodID            sMethodId_AssetManager_Open;
	static jmethodID            sMethodId_AssetManager_OpenFd;
	static jmethodID            sMethodId_AssetManager_List;
	static jmethodID            sMethodId_AssetFileDescriptor_GetLength;
	static jmethodID            sMethodId_AssetFileDescriptor_Close;

};

// This class is used internally to do two things:
// 1. Handle attaching/detaching of the current thread.
// 2. Creating/Destroying a local reference frame.
class AutoJNIEnv
{
public:
	inline AutoJNIEnv()
	{
		ASSETMANAGER_VERIFY_INIT();
		// Try to get the JNIEnv for the current thread
		if(AssetManagerJNI::GetVM()->GetEnv((void**)&mEnv, JNI_VERSION_1_2) == JNI_OK)
		{
			mDetach = false;
		}
		else
		{
			// If the environment doesn't exist then attach.
			jint resultCode = AssetManagerJNI::GetVM()->AttachCurrentThread(&mEnv, NULL);
			(void)resultCode;
			EA_ASSERT_MSG(resultCode == 0, "Unable to attach thread");
			mDetach = true;
		}

		jint pushResult = mEnv->PushLocalFrame(MAX_LOCAL_REFERENCES_REQUIRED);
		(void)pushResult;
		EA_ASSERT_MSG(pushResult == 0, "Unable to create a local reference frame.  This may indicate indicate JNI code (not necessarily related to this code) has used too many local references.");
	}

	inline JNIEnv* GetEnv() const
	{
		return mEnv;
	}

	inline ~AutoJNIEnv()
	{
		mEnv->PopLocalFrame(NULL);

		// Don't try to detach if the environment is the main thread that
		// initialized the AssetManagerJNI class.  Doing so will crash the JVM.
		if(mDetach && AssetManagerJNI::GetOriginalThreadId() != EA::Thread::GetThreadId())
		{
			AssetManagerJNI::GetVM()->DetachCurrentThread();
		}
	}
private:
	static const int MAX_LOCAL_REFERENCES_REQUIRED = 16;
	JNIEnv* mEnv;
	bool mDetach;
};

#endif // ASSETMANAGERJNI_H_INCLUDED
