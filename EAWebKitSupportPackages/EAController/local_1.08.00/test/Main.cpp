#include <coreallocator/icoreallocator_interface.h>
#include <MemoryMan/MemoryMan.inl>
#include <MemoryMan/CoreAllocatorGeneral.h>
#include <MemoryMan/CoreAllocator.inl>
#include "EAMain/EAEntryPointMain.inl"
#include <EATest/EATest.h>
#include "TestInterface.h"

int EAMain(int argc, char** argv)
{
	EA::EAMain::PlatformStartup();

	EA::UnitTest::TestApplication testSuite("IEAController Unit Tests", argc, argv);
	testSuite.AddTest("Interface", TestIEAController);
	const int testResult = testSuite.Run();

	EA::EAMain::PlatformShutdown(testResult);
	return testResult;
}
