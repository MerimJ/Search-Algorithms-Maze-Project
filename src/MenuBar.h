#pragma once
#include <gui/MenuBar.h>
#include "Constants.h"

class MenuBar : public gui::MenuBar
{
    gui::SubMenu _subApp;
    gui::SubMenu _subMaze;
    gui::SubMenu _subAnim;

    void populate()
    {
        auto& appItems = _subApp.getItems();
        appItems[0].initAsQuitAppActionItem(tr("Quit"), "q");

        auto& mazeItems = _subMaze.getItems();
        mazeItems[0].initAsActionItem(tr("regen"), cActionNew);
        mazeItems[1].initAsActionItem(tr("step"),  cActionStep);

        auto& animItems = _subAnim.getItems();
        // checkable start/stop
        animItems[0].initAsActionItem(tr("start"), cActionStartStop);
        animItems[0].setAsCheckable(true);

        _menus[0] = &_subApp;
        _menus[1] = &_subMaze;
        _menus[2] = &_subAnim;
    }

public:
    MenuBar()
    : gui::MenuBar(3)
    , _subApp(cMenuApp, tr("App"), 1)
    , _subMaze(cMenuMaze, tr("Maze"), 2)
    , _subAnim(cMenuAnim, "Run", 1)
    {
        populate();
    }
};
