/*-----------------------------------------------------------------------------
 * eadeprecatedbase.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <EABase/eadeprecated.h>


EA_BASE_DEPRECATED_STRUCT(2100, 0, 0, FINAL, "Do not use deprecated struct")
TestStruct {};

EA_BASE_DEPRECATED(2100, 0, 0, FINAL, "Do not use deprecated typedef")
typedef int TestTypedef;

EA_BASE_DEPRECATED(2100, 0, 0, FINAL, "Do not use deprecated variable")
int TestVariable;

EA_BASE_DEPRECATED_CLASS(2100, 0, 0, FINAL, "Do not use deprecated class")
TestClass {};

union TestUnion 
{ 
	EA_BASE_DEPRECATED(2100, 0, 0, FINAL, "Do not use deprecated data member") int n;
};

EA_BASE_DEPRECATED(2100, 0, 0, FINAL, "Do not use deprecated function")
void TestFunc() {}

EA_BASE_DEPRECATED_ENUM(2100, 0, 0, FINAL, "Do not use deprecated enumeration")
TestEnumeration { TestEnumeration_Value1, TestEnumeration_Value2 };

enum TestEnumerator 
{ 
	EA_BASE_DEPRECATED_ENUMVALUE(TestEnumerator_Value1, 2100, 0, 0, FINAL, "Do not use deprecated enum value") = 5,
	TestEnumerator_Value2 = 4
};

EA_DISABLE_DEPRECATED(EA_VERSION, 2100, 0, 0, FINAL, "Well this is awkward. Disabling deprecated warnings in the deprecated tests.")
void TestDeprecation()
{
	TestFunc();
	TestClass testClass;
	EA_UNUSED(testClass);
	TestStruct testStruct;
	EA_UNUSED(testStruct);
	TestTypedef testTypeDef;
	EA_UNUSED(testTypeDef);
	TestVariable = 0;
	TestUnion tu;
	tu.n = 0;
	EA_UNUSED(tu);
	int value = TestEnumeration_Value1;
	EA_UNUSED(value);
	int value2 = TestEnumerator_Value1;
	EA_UNUSED(value2);
}
EA_RESTORE_DEPRECATED()
