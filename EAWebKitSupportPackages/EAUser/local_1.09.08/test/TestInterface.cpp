#ifndef EA_TESTINTERFACE_CPP
#define EA_TESTINTERFACE_CPP
#include <IEAUser/IEAUser.h>

#include <EATest/EATest.h>

/// TestIEAUser
/// 
/// Just references IEAUser to check for syntax errors (as it is only an interface)
int TestIEAUser()
{
	EA::User::IEAUser* pEAUser = NULL;
	EA_UNUSED(pEAUser);

	return EA::UnitTest::kTestResultOK; //Always passes for now
}


#endif // EA_TESTINTERFACE_CPP