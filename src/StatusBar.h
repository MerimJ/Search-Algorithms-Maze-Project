#pragma once
#include <gui/StatusBar.h>
#include <gui/Label.h>
#include <gui/HorizontalLayout.h>
#include <gui/Font.h>

class StatusBar : public gui::StatusBar
{
    gui::Label _lblMsg;
    gui::Label _lblVisitedT;
    gui::Label _lblVisited;
    gui::Label _lblPathT;
    gui::Label _lblPath;
    gui::Label _lblTimeT;
    gui::Label _lblTime;

public:
    StatusBar()
    : gui::StatusBar(14)
    , _lblMsg(tr("statusReady"))
    , _lblVisitedT("Visited:")
    , _lblVisited("0")
    , _lblPathT("Path:")
    , _lblPath("0")
    , _lblTimeT("Time:")
    , _lblTime("0 ms")
    {
        _lblMsg.setFont(gui::Font::ID::SystemBoldItalic);
        _lblMsg.setResizable();

        setMargins(6, 0, 6, 4);
        _layout.setSpaceBetweenCells(0);

        _layout << _lblMsg;
        _layout.appendSpace(10);
        _layout << _lblVisitedT << _lblVisited;
        _layout.appendSpace(10);
        _layout << _lblPathT << _lblPath;
        _layout.appendSpace(10);
        _layout << _lblTimeT << _lblTime;

        setLayout(&_layout);
    }

    void setMessage(const td::String& s) { _lblMsg.setTitle(s); }
    void setVisited(int v)
    {
        td::String s; s.format("%d", v);
        _lblVisited.setTitle(s);
    }
    void setPath(int p)
    {
        td::String s; s.format("%d", p);
        _lblPath.setTitle(s);
    }
    void setTimeMS(int ms)
    {
        td::String s; s.format("%d ms", ms);
        _lblTime.setTitle(s);
    }
};
