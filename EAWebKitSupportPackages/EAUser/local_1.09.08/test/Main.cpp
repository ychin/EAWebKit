#include <MemoryMan/MemoryMan.inl>
#include <MemoryMan/CoreAllocator.inl>
#include <MemoryMan/CoreAllocatorGeneral.h>
#include "EAMain/EAEntryPointMain.inl"
#include <EATest/EATest.h>
#include "TestInterface.h"

int EAMain(int argc, char** argv)
{
	EA::EAMain::PlatformStartup();

	EA::UnitTest::TestApplication testSuite("IEAUser Unit Tests", argc, argv);
	testSuite.AddTest("Interface", TestIEAUser);
	const int testResult = testSuite.Run();

	EA::EAMain::PlatformShutdown(testResult);
	return testResult;
}
