#pragma once
#include <gui/Canvas.h>
#include <functional>
#include "MazeModel.h"

class MazeCanvas : public gui::Canvas
{
public:
    enum class EditMode : int
    {
        ToggleWall = 0,
        SetStart   = 1,
        SetGoal    = 2
    };

private:
    MazeModel _model;
    EditMode _editMode = EditMode::ToggleWall;

    // notify UI (status bar / toolbar) when run state changes
    std::function<void()> _fnUpdateUI;

protected:
    void onResize(const gui::Size& newSize) override
    {
        _model.setViewSize(newSize);
        reDraw();
    }

    void onDraw(const gui::Rect&) override
    {
        _model.draw();
    }

    void onPrimaryButtonReleased(const gui::InputDevice& inputDevice) override
    {
        const gui::Point& mp = inputDevice.getModelPoint();
        auto cell = _model.pointToCell(mp);

        switch (_editMode)
        {
            case EditMode::ToggleWall: _model.toggleWall(cell); break;
            case EditMode::SetStart:   _model.setStart(cell);   break;
            case EditMode::SetGoal:    _model.setGoal(cell);    break;
        }
        reDraw();
    }

public:
    MazeCanvas(const std::function<void()>& fnUpdateUI)
    : gui::Canvas({gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::PrimaryClicks})
    , _fnUpdateUI(fnUpdateUI)
    {
        setPreferredFrameRateRange(60, 60);
        enableResizeEvent(true);
    }

    MazeModel& model() { return _model; }
    const MazeModel& model() const { return _model; }

    void setEditMode(EditMode m) { _editMode = m; }

    void newMaze()
    {
        _model.generate();
        reDraw();
    }

    // One search step (used by Step button)
    void stepOnce()
    {
        _model.step();
        reDraw();
    }

    // Start/stop is handled by worker thread in MainView (for animation),
    // but we expose step() for it.
    bool stepForAnimation()
    {
        bool changed = _model.step();
        reDraw();
        return changed;
    }

    void clearSearch()
    {
        _model.clearSearchVisualization();
        reDraw();
    }

    void requestRedraw()
    {
        reDraw();
    }

    bool isRunning() const { return _model.isRunning(); }
    bool isFinished() const { return _model.isFinished(); }
    bool isSolved() const { return _model.isSolved(); }
};
