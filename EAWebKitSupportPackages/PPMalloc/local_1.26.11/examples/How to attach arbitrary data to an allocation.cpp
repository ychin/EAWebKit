


GeneralAllocatorDebug allocator;
extern int GetThreadId();


// In this example, we show how to explicitly store the current thread id 
// in a block. When storing data right with the block, you need to increase
// the allocation size via an appropriate amount before allocating.
void* Malloc_StoreThreadId_InBlock(size_t n)
{
   void* p = allocator.Malloc(n + allocator.GetDebugDataRecordLength(sizeof(int))); // or could use sizeof(threadId)

   const int threadId = GetThreadId();
   allocator.SetDebugData(p, GeneralAllocatorDebug::kDebugDataIdUser, &threadId, sizeof(threadId), kDebugDataLocationBlock);
   return p;
}



// In this example, we show how to explicity store the current thread id
// separately from a block. In this case, the data can be added at any time.
// Note that this function is almost identical to the block location version.
void* Malloc_StoreThreadId_Separate(size_t n)
{
   void* p = allocator.Malloc(n);

   const int threadId = GetThreadId();
   allocator.SetDebugData(p, GeneralAllocatorDebug::kDebugDataIdUser, &threadId, sizeof(threadId), kDebugDataLocationSeparate);
   return p;
}





