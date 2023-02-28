#ifndef EA_TESTINTERFACE_CPP
#define EA_TESTINTERFACE_CPP

#include <IEAController/IEAController.h>
#include <EATest/EATest.h>

/// TestIEAController
/// 
/// Just references IEAController to check for syntax errors (as it is only an interface)
int TestIEAController()
{
	EA::Controller::IEAController* pEAController = NULL;
	EA_UNUSED(pEAController);

	return EA::UnitTest::kTestResultOK; //Always passes for now
}

#endif // EA_TESTINTERFACE_CPP