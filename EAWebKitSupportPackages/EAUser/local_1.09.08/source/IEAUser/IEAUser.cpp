#include <IEAUser/IEAUser.h>
#include <IEAController/IEAController.h>

namespace EA
{
namespace User
{

void IEAUser::AddController(const EA::Controller::IEAController* pController)
{
	mControllerList.push_back(pController);
}

void IEAUser::RemoveController(const EA::Controller::IEAController* pController)
{
	mControllerList.remove(pController);
}

void IEAUser::AddController(EA::Controller::IEAControllerSmartPtr pController)
{
	AddController(pController.get());
}

void IEAUser::RemoveController(EA::Controller::IEAControllerSmartPtr pController)
{
	RemoveController(pController.get());
}

}
}
