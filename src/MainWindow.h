#pragma once
#include <gui/Window.h>
#include <functional>

#include "MenuBar.h"
#include "ToolBar.h"
#include "MainView.h"
#include "StatusBar.h"
#include "Constants.h"

class MainWindow : public gui::Window
{
    MainWindow() = delete;

    gui::Image _imgStart;
    gui::Image _imgStop;
    gui::Image _imgRegen;
    gui::Image _imgStep;

    MenuBar _menuBar;
    ToolBar _toolBar;
    StatusBar _statusBar;

    std::function<void()> _fnUpdateUI;
    std::function<void(const td::String&, int, int, int)> _fnStatus;

    MainView _view;

protected:
    void onInitialAppearance() override
    {
        _view.setFocusToCanvas();
    }

    void updateMenuAndTB()
    {
        bool running = _view.isRunning();

        // Menu checkbox
        gui::MenuItem* mi = _menuBar.getItem(cMenuAnim, 0, 0, cActionStartStop);
        if (mi) mi->setChecked(running);

        // Toolbar item: change start/stop image + tooltip
        gui::ToolBarItem* tb = _toolBar.getItem(cMenuAnim, 0, 0, cActionStartStop);
        if (tb)
        {
            if (running)
            {
                tb->setImage(&_imgStop);
                tb->setLabel(tr("stop"));
                tb->setTooltip(tr("stopTT"));
            }
            else
            {
                tb->setImage(&_imgStart);
                tb->setLabel(tr("start"));
                tb->setTooltip(tr("startTT"));
            }
        }
    }

    void setStatus(const td::String& msg, int visited, int pathLen, int timeMS)
    {
        _statusBar.setMessage(msg);
        _statusBar.setVisited(visited);
        _statusBar.setPath(pathLen);
        _statusBar.setTimeMS(timeMS);
    }

    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override
    {
        auto [menuID, firstSubMenuID, lastSubMenuID, actionID] = aiDesc.getIDs();
        switch(menuID)
        {
            case cMenuMaze:
                if (actionID == cActionNew) { _view.newMaze(); return true; }
                if (actionID == cActionStep){ _view.step();    return true; }
                break;
            case cMenuAnim:
                if (actionID == cActionStartStop){ _view.startStop(); return true; }
                break;
            default:
                break;
        }
        return false;
    }

public:
    MainWindow(td::BYTE /*initialModelID*/ = 0)
    : gui::Window(gui::Size(1200, 800))
    , _imgStart(":start")
    , _imgStop(":stop")
    , _imgRegen(":regen")
    , _imgStep(":step")
    , _toolBar(&_imgStart, &_imgRegen, &_imgStep)
    , _fnUpdateUI(std::bind(&MainWindow::updateMenuAndTB, this))
    , _fnStatus(std::bind(&MainWindow::setStatus, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))
    , _view(_fnUpdateUI, _fnStatus)
    {
        setTitle(tr("Maze"));
        _menuBar.setAsMain(this);
        setToolBar(_toolBar);
        setStatusBar(_statusBar);
        setCentralView(&_view);
        updateMenuAndTB();
    }
};
