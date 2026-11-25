#include "Stellar.h"
#include "EntryPoint.h"
#include "SandboxApp.h"



Stellar::Application* Stellar::CreateApplication()
{
	return new SandboxApp();
}


