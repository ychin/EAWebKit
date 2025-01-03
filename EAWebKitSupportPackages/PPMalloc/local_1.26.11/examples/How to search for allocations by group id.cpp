

void DoGroupAudit(){
   const void* const pContext = allocator.ReportBegin(NULL, GeneralAllocator::kBlockTypeAllocated, true);

   for(const GeneralAllocator::BlockInfo* pBlockInfo = allocator.ReportNext(pContext); pBlockInfo; pBlockInfo = allocator.ReportNext(pContext)){
	  int groupId;

	  if(allocator.GetDebugData(pBlockInfo->mpData, GeneralAllocatorDebug::kDebugDataIdGroup, &groupId, sizeof(groupId))){
		 // Do something with groupId. Possibly check to see if it is an unexpected value.
	  }
   }

   allocator.ReportEnd(pContext);
}



