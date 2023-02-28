



struct SizeInfo{
   size_t mnSize;
   size_t mnCount;
   size_t mnVolume;
};

struct Statistics{
   SizeInfo mSizeBinArray[256];
   size_t   mnAllocationCount;
   size_t   mnAllocationVolume;

   Statistics(){
	  memset(this, 0, sizeof(this));
	  for(size_t i(0); i < 256; i++)
		 mSizeBinArray[i].mnSize = 4 + (i * 4);
   }
};

size_t GetBin(size_t nSize){
   size_t nIndex = (nSize > 0 ? ((nSize - 1) / 4) : 0);
   if(nIndex > 255)
	  nIndex = 255;
   return nIndex;
}

typedef std::map<void*, size_t> SizeMap;
SizeMap gSizeMap; 
Statistics gCurrentStatistics;
Statistics gMaxCountStatistics;
Statistics gMaxVolumeStatistics;

///////////////////////////////////////////////////////////////////////////////
// HookFunction
//
void HookFunction(AllocationRecorder::HookInfo* pHookInfo, void* pContext)
{
   AllocationRecorder* const pAllocationRecorder = (AllocationRecorder*)pContext;

   switch(pHookInfo->mHookType)
   {
	  case AllocationRecorder::kHookTypeMalloc:{
		 pHookInfo->mpDataOutput = pAllocationRecorder->GetPlaybackAllocator()->Malloc(pHookInfo->mnSizeInput, pHookInfo->mnAllocationFlags);
		 gSizeMap.insert(SizeMap::value_type(pHookInfo->mpDataOutput, pHookInfo->mnSizeInput));
		 size_t nBin = GetBin(pHookInfo->mnSizeInputTotal);
		 gCurrentStatistics.mSizeBinArray[nBin].mnCount++;
		 gCurrentStatistics.mSizeBinArray[nBin].mnVolume += pHookInfo->mnSizeInputTotal;
		 gCurrentStatistics.mnAllocationCount++;
		 gCurrentStatistics.mnAllocationVolume += pHookInfo->mnSizeInputTotal;
		 break;
	  }

	  case AllocationRecorder::kHookTypeMallocAligned:{
		 pHookInfo->mpDataOutput = pAllocationRecorder->GetPlaybackAllocator()->MallocAligned(pHookInfo->mnSizeInput, pHookInfo->mnAlignmentInput, pHookInfo->mnAlignmentOffsetInput, pHookInfo->mnAllocationFlags);
		 gSizeMap.insert(SizeMap::value_type(pHookInfo->mpDataOutput, pHookInfo->mnSizeInput));
		 size_t nBin = GetBin(pHookInfo->mnSizeInputTotal);
		 gCurrentStatistics.mSizeBinArray[nBin].mnCount++;
		 gCurrentStatistics.mSizeBinArray[nBin].mnVolume += pHookInfo->mnSizeInputTotal;
		 gCurrentStatistics.mnAllocationCount++;
		 gCurrentStatistics.mnAllocationVolume += pHookInfo->mnSizeInputTotal;
		 break;
	  }

	  case AllocationRecorder::kHookTypeCalloc:{
		 pHookInfo->mpDataOutput = pAllocationRecorder->GetPlaybackAllocator()->Calloc(pHookInfo->mnCountInput, pHookInfo->mnSizeInput);
		 gSizeMap.insert(SizeMap::value_type(pHookInfo->mpDataOutput, pHookInfo->mnSizeInputTotal));
		 size_t nBin = GetBin(pHookInfo->mnSizeInputTotal);
		 gCurrentStatistics.mSizeBinArray[nBin].mnCount++;
		 gCurrentStatistics.mSizeBinArray[nBin].mnVolume += pHookInfo->mnSizeInputTotal;
		 gCurrentStatistics.mnAllocationCount++;
		 gCurrentStatistics.mnAllocationVolume += pHookInfo->mnSizeInputTotal;
		 break;
	  }

	  case AllocationRecorder::kHookTypeRealloc:{
		 pHookInfo->mpDataOutput = pAllocationRecorder->GetPlaybackAllocator()->Realloc((void*)pHookInfo->mpDataInput, pHookInfo->mnSizeInput, pHookInfo->mnAllocationFlags);
		 SizeMap::iterator it = gSizeMap.find((void*)pHookInfo->mpDataInput);
		 if(it != gSizeMap.end())
		 {
			size_t nSize = (*it).second;
			size_t nBin  = GetBin(nSize);
			gCurrentStatistics.mSizeBinArray[nBin].mnCount--;
			gCurrentStatistics.mSizeBinArray[nBin].mnVolume -= nSize;
			gCurrentStatistics.mnAllocationCount--;
			gCurrentStatistics.mnAllocationVolume -= nSize;
			gSizeMap.erase(it);
		 }
		 size_t nBin = GetBin(pHookInfo->mnSizeInputTotal);
		 gCurrentStatistics.mSizeBinArray[nBin].mnCount++;
		 gCurrentStatistics.mSizeBinArray[nBin].mnVolume += pHookInfo->mnSizeInputTotal;
		 gSizeMap.insert(SizeMap::value_type(pHookInfo->mpDataOutput, pHookInfo->mnSizeInput));
		 gCurrentStatistics.mnAllocationCount++;
		 gCurrentStatistics.mnAllocationVolume += pHookInfo->mnSizeInputTotal;
		 break;
	  }

	  case AllocationRecorder::kHookTypeFree:{
		 pAllocationRecorder->GetPlaybackAllocator()->Free((void*)pHookInfo->mpDataInput);
		 SizeMap::iterator it = gSizeMap.find((void*)pHookInfo->mpDataInput);
		 if(it != gSizeMap.end())
		 {
			size_t nSize = (*it).second;
			size_t nBin  = GetBin(nSize);
			gCurrentStatistics.mSizeBinArray[nBin].mnCount--;
			gCurrentStatistics.mSizeBinArray[nBin].mnVolume -= nSize;
			gCurrentStatistics.mnAllocationCount--;
			gCurrentStatistics.mnAllocationVolume -= nSize;
			gSizeMap.erase(it);
		 }
		 break;
	  }
   }

   if(gCurrentStatistics.mnAllocationCount > gMaxCountStatistics.mnAllocationCount)
	  gMaxCountStatistics = gCurrentStatistics;
   if(gCurrentStatistics.mnAllocationVolume > gMaxVolumeStatistics.mnAllocationVolume)
	  gMaxVolumeStatistics = gCurrentStatistics;
}





