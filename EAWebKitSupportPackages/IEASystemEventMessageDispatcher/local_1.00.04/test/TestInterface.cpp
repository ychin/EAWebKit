#ifndef EA_TESTINTERFACE_CPP
#define EA_TESTINTERFACE_CPP
#include <IEASystemEventMessageDispatcher/ISystemEventMessageDispatcher.h>

#include <EATest/EATest.h>

/// TestISystemEventMessageDispatcher
/// 
/// Just references ISystemEventMessageDispatcher to check for syntax errors (as it is only an interface)
int TestISystemEventMessageDispatcher()
{
	EA::SEMD::ISystemEventMessageDispatcher* pSEMD = NULL;
	EA_UNUSED(pSEMD);

	return EA::UnitTest::kTestResultOK; //Always passes for now
}


#endif // EA_TESTINTERFACE_CPP