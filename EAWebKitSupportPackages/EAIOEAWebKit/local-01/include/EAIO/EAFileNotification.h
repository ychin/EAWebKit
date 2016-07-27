/*
Copyright (C) 2007, 2009-2010 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// EAFileNotification.h
// Created and maintained by Paul Pedriana - 2007
//
// Implements automatic notifications of changes in the file system.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// The FileChangeNotification class detects file system changes as they occur.
// This is useful for having asset management systems whereby you want to 
// detect changes during development time. It's also useful for implementing
// user plug-in systems whereby you want to detect that the user has added
// something at runtime.
///////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EAFILENOTIFICATION_H) && !defined(FOUNDATION_EAFILENOTIFICATION_H)
#define EAIO_EAFILENOTIFICATION_H
#define FOUNDATION_EAFILENOTIFICATION_H // For backward compatability. Eventually, we'll want to get rid of this.


#ifndef INCLUDED_eabase_H
    #include <EABase/eabase.h>
#endif
#include <EAIO/internal/Config.h>
#ifndef FOUNDATION_EAFILEBASE_H
    #include <EAIO/EAFileBase.h>
#endif
#include <coreallocator/icoreallocator_interface.h>
#ifndef EAIO_EASTLCOREALLOCATOR_H
    #include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#endif


#if FCN_POLL_ENABLED
    #ifndef EAIO_REFCOUNT_H
        #include <EAIO/internal/EAIOEARefCount.h>
    #endif
    #if EAIO_THREAD_SAFETY_ENABLED
        #ifndef EATHREAD_EATHREAD_THREAD_H
            #include <eathread/eathread_thread.h>
        #endif
        #ifndef EATHREAD_EATHREAD_ATOMIC_H
            #include <eathread/eathread_atomic.h>
        #endif
    #endif
    #include <EASTL/fixed_string.h>
    #include <EASTL/hash_set.h>
    #include <time.h>

    // We define the following macros from CoreAllocator here to avoid a dependency 
    // on the MemoryMan package.
    #ifndef EA_CA_NEW
        #define EA_CA_NEW(Class, pAllocator, pName) \
            new ((pAllocator)->Alloc(sizeof(Class), pName, 0, EA_ALIGN_OF(Class), 0)) Class
    #endif
    #ifndef EA_CA_DELETE
        #define EA_CA_DELETE(pObject, pAllocator) EA::IO::delete_object(pObject, pAllocator)
    #endif
    namespace EA
    {
        namespace IO
        {
            // This function was copied from MemoryMan/CoreAllocatorNew.h in order to
            // avoid a dependency on the MemoryMan package.

            /// delete_object
            ///
            /// Deletes an object created by create_object.
            /// See create_object for specifications and examples.
            ///
            template <typename T>
            inline void delete_object(T* pObject, Allocator::ICoreAllocator* pAllocator)
            {
                if(pObject) // As per the C++ standard, deletion of NULL results in a no-op.
                {
                    pObject->~T();
                    pAllocator->Free(pObject);
                }
            }
        }
    }
#endif



namespace EA
{
    namespace IO
    {
        // Forward declarations
        class FileChangeNotification;


        /// FileChangeCallback
        ///
        /// Defines a callback function the user supplies for notification callbacks.
        /// Do not save the directory path or file name pointers beyond this call.
        /// Note that directory paths in this EAFile/EADirectory system are defined as 
        /// ending with a directory separator. This is done because it makes it simpler
        /// to work with directories and makes the specification of what a directory 
        /// path is be more concise.
        ///
        /// Example implementation:
        ///    void MyCallback(FileChangeNotification* pFCN, const char16_t* pDirectoryPath,
        ///                     const char16_t* pFileName, int nChangeTypeFlags, void* pContext)
        ///    {
        ///        printf("Changed file path: %ls%ls.\n", pDirectoryPath, pFileName);
        ///    }
        ///
        typedef void (*FileChangeCallback)(FileChangeNotification* pFileChangeNotification, /// The class object which generated this callback.
                                            const char16_t* pDirectoryPath,                 /// The directory in which the change took place, which will the registered directory or a subdirectory of it.
                                            const char16_t* pFileName,                      /// The file name (not path) of the file that changed. Will be NULL if the change was a directory change. Can be NULL if the file which changed could not be determined.
                                            int nChangeTypeFlags,                           /// The actual change flags. May be more than one.
                                            void* pContext);                                /// The user-specified context.


        ///////////////////////////////////////////////////////////////////////////////
        /// FileChangeNotification
        ///
        /// The FileChangeNotification class detects file system changes as they occur.
        /// This is useful for having asset management systems whereby you want to 
        /// detect changes during development time. It's also useful for implementing
        /// user plug-in systems whereby you want to detect that the user has added
        /// something at runtime.
        ///
        /// Example usage (neglecting error checking):
        ///
        ///     class AssetManager {
        ///         void Init() {
        ///             mFCN.Init();
        ///             mFCN.SetWatchedDirectory(L"/app/data/");
        ///             mFCN.SetChangeTypeFlags(FileChangeNotification::kChangeTypeFlagModified);
        ///             mFCN.SetOptionFlags(FileChangeNotification::kOptionFlagWatchSubdirectories);
        ///             mFCN.SetNotificationCallback(AssetManagerFileChangeCallback, this);
        ///             mFCN.Start();
        ///         }
        ///
        ///         void Shutdown() {
        ///             mFCN.Shutdown();
        ///         }
        ///
        ///         FileChangeNotification mFCN;
        ///     }; 
        ///
      #if !FCN_POLL_ENABLED
        class EAIO_API FileChangeNotification
      #elif EAIO_THREAD_SAFETY_ENABLED
        class EAIO_API FileChangeNotification : public EA::Thread::IRunnable
      #else
        class EAIO_API FileChangeNotification
      #endif
        {
        public:
            /// enum ChangeTypeFlags
            /// Allows the user to specify what kind of changes the user wants to be 
            /// notified about. Not all platforms support all of these flags.
            enum ChangeTypeFlags
            {
                kChangeTypeFlagNone          = 0x0000,          /// No flags in particular.
                kChangeTypeFlagFileName      = 0x0001,          /// Notify of file name changes; this includes creation and deletion of files.
                kChangeTypeFlagDirectoryName = 0x0002,          /// Notify of directory name changes; this includes creation and deletion of directories.
                kChangeTypeFlagModified      = 0x0004,          /// Notify of file's "last written" time changes.
                kChangeTypeFlagAttributes    = 0x0008,          /// Notify of file or directory attribute changes.
                kChangeTypeFlagSize          = 0x0010,          /// Notify of file size changes.
                kChangeTypeFlagDefaults      = kChangeTypeFlagNone
            };


            /// enum OptionFlags
            /// Specifies options regarding the notification mechanism.
            /// These are flags, so they can be or'd together.
            enum OptionFlags
            {
                kOptionFlagNone                       = 0x00,    /// No options.
                kOptionFlagWatchSubdirectories        = 0x01,    /// If set, then we watch subdirectories of the specified directory as well as the specified directory.
                kOptionFlagStopAfterFirstNotification = 0x02,    /// If set, then we automatically Stop after the first received change notification.
                kOptionFlagMultithreaded              = 0x04,    /// If set, then a background thread is used to implement change checking. Otherwise, the user is expected to manually trigger checks via the Poll function.
                kOptionFlagDefaults                   = kOptionFlagMultithreaded
            };


            /// FileChangeNotification
            ///
            /// Default constructor. 
            /// This function allocates no memory, sets the default ChangeTypeFlags to 
            /// kChangeTypeFlagDefaults, sets the default option flags to kOptionFlagDefaults.
            /// Initialization of a FileChangeNotification instance must be done separately
            /// from construction, due to the requirements of initialization. See Init for more.
            ///
            FileChangeNotification(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);


            /// ~FileChangeNotification
            ///
            /// Destructor.
            /// Stops the file change notification if it was Started.
            /// Shuts down the file change notification if it was initialized.
            virtual ~FileChangeNotification();


            /// Init
            ///
            /// Initializes an instance of this class for use (Start, Stop, Poll, etc.).
            /// Does any required setup to make a class instance ready for use. This may
            /// including calling operating system services. 
            ///
            /// Returns true if the Initialization was successful. On some platforms, the 
            /// initialization is guaranteed to always be successful. These platforms currenly
            /// include Windows, XBox 360, PS3.
            ///
            bool Init();


            /// Shutdown
            ///
            /// Shuts down an instance which was initialized via Init. Returns the state to 
            /// that before Init was called. Stops the file change notification if it was
            /// started.
            /// 
            /// Returns true if the class is initialized upon the call to Shutdown.
            ///
            bool Shutdown();


            /// Start
            ///
            /// Starts file change notification with the current options if file change 
            /// notification hasn't already been started. You must set all options before
            /// calling this function, and no options can be changed after calling this 
            /// function unless otherwise specified. This function will allocate any 
            /// required memory for data structures via the user-supplied allocator.
            /// On some platforms, additional memory may be allocated after the Start,
            /// but only if the directory structure changes.
            ///
            /// Returns true if the file change notification could be started. Change notfication
            /// will fail if the directory specfied by SetWatchedDirectory doesn't exist.
            /// If the directory specifed by SetWatchedDirectory is deleted after Start,
            /// file change notification is still considered to be started.
            ///
            bool Start();


            /// GetIsStarted
            ///
            /// Returns true if file change notification is started.
            ///
            bool GetIsStarted() const;


            /// Stop
            ///
            /// Stops file change notification and returns the class instance to the same state
            /// as before Start was called. Note that under mulithreaded implementations 
            /// of FileChangeNotification (currently only Windows) it is possible that a file
            /// change notice could occur just as you call this function and which completes 
            /// after this function completes. 
            ///
            /// Returns true if file change notification was started upon the call to Stop.
            ///
            bool Stop();


            /// Poll
            ///
            /// Polls for file changes. This function will result in one or more callback notifications
            /// if file changes were detected during the poll. The user is expected to call this 
            /// function periodically, usually once every few seconds.
            ///
            /// This function only works if FCN_POLL_ENABLED is defined, otherwise it does nothing.
            ///
            /// Example usage:
            ///    void DoAppLoop()
            ///    {
            ///        #ifdef FCN_POLL_ENABLED // This isn't required, but saves a function call if it's not needed.
            ///            if((++gFrameCount % 512) == 0
            ///                gFileNotification.Poll(30);
            ///        #endif
            ///    }
            ///
            void Poll(int timeoutMS = 100000);


            /// GetWatchedDirectory
            ///
            /// Returns the directory which was set by SetWatchedDirectory as a nul-terminated string.
            /// The directory path ends with a path separator.
            ///
            const char16_t* GetWatchedDirectory() const;


            /// SetWatchedDirectory
            ///
            /// Sets the base directory to be watched. This must be called before Start is called.
            ///
            /// Note that this function and all other functions in the EAFile/EADirectory system requires
            /// a directory path name that ends in a path separator. This is by design as it simplifies
            /// the specification of and manipulation of paths.
            ///
            /// Returns true if the input directory could be set, which will be so if the file change
            /// notification hasn't been started.
            ///
            bool SetWatchedDirectory(const char16_t* pDirectory);
            bool SetWatchedDirectory(const char8_t* pDirectory);


            /// SetNotificationCallback
            ///
            /// Sets the FileChangeCallback function, which is called when file changes are detected.
            /// The pContext parameter is passed to the callback function and is usually used to pass
            /// a 'this' pointer to a C++ class.
            ///
            /// The callback function will be called from the thread that calls Poll if FCN_POLL_ENABLED
            /// is enabled, and will be called from an arbitrary other othread if FCN_POLL_ENABLED is disabled.
            ///
            /// This function can be called at any time, including after file notification has been started.
            ///
            void SetNotificationCallback(FileChangeCallback pFileChangeCallback, void* pContext);


            /// SendNotification
            ///
            /// This function manually executes a file change callback to the registered callback function.
            /// You can use this function to force a file change notification to occur for an arbitrary file.
            ///
            /// Note that this function and all other functions in the EAFile/EADirectory system requires
            /// a directory path name that ends in a path separator. This is by design as it simplifies
            /// the specification of and manipulation of paths.
            ///
            void SendNotification(const char16_t* pDirectory, const char16_t* pFile, int nChangeTypeFlags);


            /// GetChangeTypeFlags
            ///
            /// Returns the ChangeTypeFlags set by SetChangeTypeFlags. 
            ///
            int GetChangeTypeFlags() const;


            /// SetChangeTypeFlags
            ///
            /// Sets the ChangeTypeFlags, which can be or'd together. This must be called before Start is called.
            /// Returns true if the flags could be set, which will be so if the file change
            /// notification hasn't been started.
            ///
            /// Example usage:
            ///     mFileChangeNotification.SetChangeTypeFlags(FileChangeNotification::kChangeTypeFlagFileName | 
            ///                                                FileChangeNotification::kChangeTypeFlagModified);
            ///
            bool SetChangeTypeFlags(int nChangeTypeFlags);


            /// GetOptionFlags
            ///
            /// Returns the OptionFlags set by SetOptionFlags. 
            ///
            int GetOptionFlags() const;


            /// SetOptionFlags
            ///
            /// Sets the OptionFlags, which can be or'd together. This must be called before Start is called.
            /// Returns true if the flags could be set, which will be so if the file change
            /// notification hasn't been started.
            ///
            /// Example usage:
            ///     mFileChangeNotification.SetOptionFlags(FileChangeNotification::kOptionFlagWatchSubdirectories);
            ///
            bool SetOptionFlags(int nOptionFlags);


            /// SetAllocator
            /// Sets the memory allocator to use with this class.
            /// This function must be called before this class is used.
            /// If this function is not called then the default global new/delete
            /// allocator will be used.
            virtual void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);


        protected:
            // Member functions and data
            #if !FCN_POLL_ENABLED && defined(EA_PLATFORM_WINDOWS)
                static unsigned __stdcall ThreadFunction(void* pContext);
                void ThreadFunctionRDC (uint32_t dwNotifyFilter);    ///< Version of thread that uses NT's ReadDirectoryChangesW().
                void ThreadFunctionFFCN(uint32_t dwNotifyFilter, const char16_t *pWatchedDirectory);  ///< Version of thread that uses Win32's FindFirstChangeNotification().

            #else
                struct  FSEntry;
                typedef EAIOAutoRefCount<FSEntry> FSEntryARC;
                typedef eastl::fixed_string<char16_t, 32, true, Allocator::EAIOEASTLCoreAllocator> EntryNameString; // Most directory name entries fit within this many characters.

                struct FSEntryHashCompare
                {
                     size_t operator()(const FSEntryARC& fseARC) const;
                     bool   operator()(const FSEntryARC& fseARC1, const FSEntryARC& fseARC2) const;
                };

                typedef eastl::hash_set<FSEntryARC, FSEntryHashCompare,
                                        FSEntryHashCompare, EA::Allocator::EAIOEASTLCoreAllocator> FSEntrySet;

                /// FSEntry
                /// As of this writing, this doesn't need to be thread-safe.
                struct FSEntry
                {
                    int16_t                        mnRefCount;
                    int16_t                        mnChangeFlags;
                    EntryNameString                msName;
                    bool                           mbDirectory;
                    time_t                         mnTime;
                    size_type                      mnSize;
                    FSEntry*                       mpParentEntry;
                    FSEntrySet                     mChildEntrySet;
                    FSEntrySet::iterator           mChildEntrySetIt;
                    EA::Allocator::ICoreAllocator* mpCoreAllocator;

                    FSEntry() : mnRefCount(0)
                              , mpParentEntry(NULL)
                              , mChildEntrySet(EASTL_NAME(EAIO_ALLOC_PREFIX "EAFileNotification/FSEntry"))
                              , mpCoreAllocator(NULL) 
                              { } // Other members don't matter here.

                    int  AddRef();
                    int  Release();
                    void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);
                };

                void     GetChildFileEntrySet(FSEntry* pFSEntry, FSEntrySet& childEntrySet);
                void     BuildEntry(FSEntry* pEntry, const char16_t* pName, FSEntry* pEntryParent, bool bDirectory);
                void     GetEntryPath(const FSEntry* pEntry, char16_t* pPath, uint32_t nMaxPermittedLength = kMaxPathLength);
                bool     EntryExists(const FSEntry* pFSEntry);
                void     GetFileTimeAndSize(const FSEntry* pFSEntry, time_t* pTime, size_type* pSize);
                void     TraceEntryTree(const FSEntry* pEntry, int nCurrentDepth = 0);
                bool     PollInternal(FSEntry* pFSEntry, int timeoutAbsolute);
                intptr_t Run(void* pContext); // Inherited from EA::Thread::IRunnable.
            #endif

            // Member data
            bool                        mbInitialized;
            Allocator::ICoreAllocator*  mpCoreAllocator;
            char16_t                    mDirectoryPath[kMaxDirectoryLength];
            int                         mnChangeTypeFlags;
            int                         mnOptionFlags;
            FileChangeCallback          mpFileChangeCallback;
            void*                       mpFileChangeCallbackContext;

            #if !FCN_POLL_ENABLED && defined(EA_PLATFORM_WINDOWS)
                typedef void* FCNHandle;

                struct FCNOverlapped        // This is a replica of the windows.h OVERLAPPED data structure. It allows us to avoid #including windows.h here.
                {
                     uintptr_t Internal;
                     uintptr_t InternalHigh;
                     union
                     {
                          uint32_t Offset[2];
                          void*    Pointer;
                     };
                     FCNHandle hEvent;
                };

                FCNHandle     mhEvents[2];              /// Event array for waiting. First handle is the directory change, while the second is the shutdown event.
                FCNHandle     mhWaitingThread;          /// Thread handle for waiting thread.

                // ReadDirectoryChanges data
                FCNHandle     mhDirectory;              /// Used for ReadDirectoryChangesW.
                FCNOverlapped mOverlapped;              /// Used by the Win32 API to store overlapped IO info.

                // FindFirstChangeNotification data
                volatile bool mbThreadShouldRun;        /// Used as a message to tell the waiting thread to quit.

            #else
              #if EAIO_THREAD_SAFETY_ENABLED
                EA::Thread::AtomicInt32 mnBusy;                   /// Used to detect reentrancy.
                EA::Thread::Thread      mThread;                  /// Used for background operations.
              #endif
                bool                    mbThreadShouldRun;        /// Used as a message to tell the waiting thread to quit.
                volatile bool           mbStarted;                /// True if change notification has been started (e.g. by the Start function).
                FSEntry                 mEntryDirectoryRoot;      /// Pointer to the entry which is the root.
                FSEntry*                mpEntryDirectoryCurrent;  /// Pointer to current entry during iteration. 
                FSEntrySet              mCurrentChangeSet;        /// Set of currently changed files.

            #endif
        };


    } // namespace IO


} // namespace EA


#endif // Header include guard























