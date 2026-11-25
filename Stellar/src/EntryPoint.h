#pragma once

#include "Core.h"
#include "Stellar/Application.h"

#ifdef ST_PLATFORM_WINDOWS

int main(int argc, char** argv)
{
    Stellar::Application* app = Stellar::CreateApplication();
    app->Run();
    delete app;
    return 0;
}
#else
#error Stellar only supports Windows at the moment!
#endif
