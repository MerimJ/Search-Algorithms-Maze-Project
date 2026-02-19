#pragma once
#include <gui/Application.h>
#include "MainWindow.h"
#include "Constants.h"

class Application : public gui::Application
{
protected:
    gui::Window* createInitialWindow() override
    {
        return new MainWindow(0);
    }

public:
    Application(int argc, const char** argv)
    : gui::Application(argc, argv)
    {}
};
