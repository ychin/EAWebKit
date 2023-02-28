#include <IEAController/IEAController.h>

namespace EA
{
namespace Controller
{

IEAController::IEAController(ControllerType type, EA::Allocator::ICoreAllocator* allocator)
	: mAllocator(allocator)
	, mPairedUser(nullptr)
	, mType(type)
	, mRefCount(0)
{}

ControllerType IEAController::GetType() const
{
	return mType;
}

const EA::User::IEAUser* IEAController::GetPairedUser() const
{
	return mPairedUser;
}

int IEAController::AddRef() const
{
	return mRefCount.Increment();
}

int IEAController::Release() const
{
	int32_t rc = mRefCount.Decrement();
	if (rc)
	{
		return rc;
	}

	EA_ASSERT(mAllocator);

	// Prevent double destroy if AddRef/Release is called while in destructor.
	// This can happen if the destructor does things like debug traces.
	mRefCount.SetValue(1);
	this->~IEAController();
	mAllocator->Free((void*)this);

	return 0;
}

void IEAController::SetPairedUser(const EA::User::IEAUser* user)
{
	mPairedUser = user;
}

void IEAController::SetType(ControllerType type)
{
	mType = type;
}

}
}
