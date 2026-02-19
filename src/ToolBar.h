#pragma once
#include <gui/ToolBar.h>
#include <gui/Image.h>
#include "Constants.h"

class ToolBar : public gui::ToolBar
{
public:
    ToolBar(gui::Image* imgStart, gui::Image* imgRegen, gui::Image* imgStep)
    : gui::ToolBar("mainTB", 3)
    {
        // cMenuMaze
        addItem(tr("regen"), imgRegen, tr("regenTT"), cMenuMaze, 0, 0, cActionNew);
        addItem(tr("step"),  imgStep,  tr("stepTT"),  cMenuMaze, 0, 0, cActionStep);
        // cMenuAnim
        addItem(tr("start"), imgStart, tr("startTT"), cMenuAnim, 0, 0, cActionStartStop);
    }
};
