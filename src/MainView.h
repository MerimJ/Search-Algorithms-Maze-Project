#pragma once
#include <functional>
#include <chrono>
#include <algorithm>

#include <td/Types.h>
#include <td/String.h>

#include <gui/View.h>
#include <gui/Label.h>
#include <gui/NumericEdit.h>
#include <gui/CheckBox.h>
#include <gui/ColorPicker.h>
#include <gui/GridLayout.h>
#include <gui/GridComposer.h>
#include <gui/Slider.h>
#include <gui/ComboBox.h>
#include <gui/Timer.h>

#include "MazeCanvas.h"

class MainView : public gui::View
{
public:
    using FnUpdateUI = std::function<void()>;
    using FnStatus   = std::function<void(const td::String& msg, int visited, int pathLen, int timeMS)>;

private:
    // Controls
    gui::Label _lblAlg;
    gui::ComboBox _cmbAlg;

    gui::Label _lblSpeed;
    gui::Slider _slSpeed;

    gui::Label _lblDensity;
    gui::Slider _slDensity;

    gui::Label _lblSize;
    gui::NumericEdit _edRows;
    gui::NumericEdit _edCols;

    gui::CheckBox _chkDiagonal;
    gui::CheckBox _chkGuarantee;

    gui::Label _lblVisitedColor;
    gui::ColorPicker _visitedColor;

    gui::Label _lblPathColor;
    gui::ColorPicker _pathColor;

    gui::Label _lblEdit;
    gui::ComboBox _cmbEdit;

    gui::GridLayout _gl;

    // Canvas
    MazeCanvas _canvas;

    // Callbacks to MainWindow
    FnUpdateUI _fnUpdateUI;
    FnStatus _fnStatus;

    // Animation (UI thread)
    gui::Timer _timer;
    bool _animating = false;

    int _lastRows = 25;
    int _lastCols = 25;
    const int _maxDelayMS = 2000;

    // Run bookkeeping (prevents any stale timer events from affecting a new run)
    td::UINT4 _runId = 0;
    std::chrono::high_resolution_clock::time_point _t0;

private:
    void publishStatus(const td::String& msg, int timeMS = 0)
    {
        if (!_fnStatus) return;
        const auto& m = _canvas.model();
        _fnStatus(msg, m.visitedCount(), m.pathLength(), timeMS);
    }

    void stopTimerInternal()
    {
        if (_timer.isRunning())
            _timer.stop();
        _animating = false;
    }

    void applyControlsToModel(bool allowRegenerateOnSizeChange = true)
    {
        auto& model = _canvas.model();
        auto& opt = model.options();

        opt.algorithm = MazeModel::Algorithm(_cmbAlg.getSelectedIndex());
        opt.diagonal = _chkDiagonal.isChecked();
        opt.guaranteePath = _chkGuarantee.isChecked();

        int sliderVal = int(_slSpeed.getValue());
        opt.sleepMS = std::max(_maxDelayMS - sliderVal, 0);

        double d = _slDensity.getValue();
        opt.density = float(0.05 + (0.45 - 0.05) * (d / 100.0));

        opt.visitedColor = _visitedColor.getValue();
        opt.pathColor = _pathColor.getValue();
        opt.frontierColor = td::ColorID::Cyan;

        _canvas.setEditMode(MazeCanvas::EditMode(_cmbEdit.getSelectedIndex()));

        int rows = _lastRows, cols = _lastCols;
        _edRows.getValue(rows);
        _edCols.getValue(cols);

        rows = std::max(5, std::min(rows, 80));
        cols = std::max(5, std::min(cols, 80));

        if (rows != _lastRows || cols != _lastCols)
        {
            _lastRows = rows;
            _lastCols = cols;
            model.applySize(rows, cols);
            if (allowRegenerateOnSizeChange)
                model.generate();
        }
    }

    void configureTimerInterval()
    {
        int sleepMS = _canvas.model().options().sleepMS;
        sleepMS = std::max(0, std::min(sleepMS, _maxDelayMS));
        float interval = (sleepMS == 0) ? 0.001f : float(sleepMS) / 1000.0f;
        _timer.setInterval(interval);
    }

    void onTimerTick(td::UINT4 tickRunId)
    {
        // Ignore any stale ticks from a previous run
        if (!_animating || tickRunId != _runId)
            return;

        // One step per tick: canvas redraw happens inside
        _canvas.stepForAnimation();

        if (_canvas.isFinished())
        {
            auto t1 = std::chrono::high_resolution_clock::now();
            int ms = int(std::chrono::duration_cast<std::chrono::milliseconds>(t1 - _t0).count());

            if (_canvas.isSolved()) publishStatus(tr("statusSolved"), ms);
            else publishStatus(tr("statusNoPath"), ms);

            stopTimerInternal();
            if (_fnUpdateUI) _fnUpdateUI();
        }
        else
        {
            publishStatus(tr("statusRunning"), 0);
        }
    }

    void startTimer()
    {
        // Fully reset run state
        stopTimerInternal();
        applyControlsToModel(false);

        // Clear previous search so a second run doesn't carry finished state
        _canvas.clearSearch();

        configureTimerInterval();

        _runId++;
        _t0 = std::chrono::high_resolution_clock::now();
        _animating = true;

        publishStatus(tr("statusRunning"));
        _timer.start();
        if (_fnUpdateUI) _fnUpdateUI();
    }

public:
    MainView(const FnUpdateUI& fnUpdateUI, const FnStatus& fnStatus)
    : _lblAlg(tr("Algorithm"))
    , _lblSpeed(tr("Speed"))
    , _lblDensity(tr("Density"))
    , _lblSize(tr("Size"))
    , _edRows(td::int4)
    , _edCols(td::int4)
    , _chkDiagonal(tr("Diagonal"))
    , _chkGuarantee(tr("Guarantee"))
    , _lblVisitedColor(tr("VisitedColor"))
    , _lblPathColor(tr("PathColor"))
    , _lblEdit(tr("Edit"))
    , _gl(2, 18)
    , _canvas(fnUpdateUI)
    , _fnUpdateUI(fnUpdateUI)
    , _fnStatus(fnStatus)
    , _timer(this, 0.05f, false)
    {
        _cmbAlg.addItem("BFS");
        _cmbAlg.addItem("Dijkstra");
        _cmbAlg.addItem("A*");
        _cmbAlg.selectIndex(2);

        _cmbEdit.addItem(tr("Wall"));
        _cmbEdit.addItem(tr("Start"));
        _cmbEdit.addItem(tr("Goal"));
        _cmbEdit.selectIndex(0);

        _slSpeed.setRange(0, _maxDelayMS);
        _slSpeed.setValue(_maxDelayMS - 50);

        _slDensity.setRange(0, 100);
        _slDensity.setValue(((0.28 - 0.05) / (0.45 - 0.05)) * 100.0);

        _edRows.setMinValue(5); _edRows.setMaxValue(80);
        _edCols.setMinValue(5); _edCols.setMaxValue(80);
        _edRows.setText("25");
        _edCols.setText("25");

        _chkDiagonal.setChecked(false);
        _chkGuarantee.setChecked(true);

        _visitedColor.setValue(td::ColorID::Yellow);
        _pathColor.setValue(td::ColorID::Green);

        // Timer callback set ONCE (important: avoids stacking callbacks and crashes)
        _timer.onTimer([this]() {
            this->onTimerTick(this->_runId);
        });

        // Handlers
        _cmbAlg.onChangedSelection([this](){
            stop();
            applyControlsToModel(false);
            _canvas.clearSearch();
            publishStatus(tr("Ready"));
            if (_fnUpdateUI) _fnUpdateUI();
        });

        _slSpeed.onChangedValue([this](){
            applyControlsToModel(false);
            configureTimerInterval();
        });

        _slDensity.onChangedValue([this](){
            stop();
            applyControlsToModel(false);
            publishStatus(tr("Ready"));
            if (_fnUpdateUI) _fnUpdateUI();
        });

        _edRows.onFinishEdit([this](){
            stop();
            applyControlsToModel(true);
            publishStatus(tr("Ready"));
            if (_fnUpdateUI) _fnUpdateUI();
        });

        _edCols.onFinishEdit([this](){
            stop();
            applyControlsToModel(true);
            publishStatus(tr("Ready"));
            if (_fnUpdateUI) _fnUpdateUI();
        });

        _chkDiagonal.onClick([this](){
            stop();
            applyControlsToModel(false);
            _canvas.clearSearch();
            publishStatus(tr("Ready"));
            if (_fnUpdateUI) _fnUpdateUI();
        });

        _chkGuarantee.onClick([this](){
            stop();
            applyControlsToModel(true);
            publishStatus(tr("Ready"));
            if (_fnUpdateUI) _fnUpdateUI();
        });

        _visitedColor.onChangedValue([this](){
            applyControlsToModel(false);
            _canvas.clearSearch();
        });

        _pathColor.onChangedValue([this](){
            applyControlsToModel(false);
            _canvas.clearSearch();
        });

        _cmbEdit.onChangedSelection([this](){
            applyControlsToModel(false);
        });

        gui::GridComposer gc(_gl);
        gc.startNewRowWithSpace(5, 0)
            << _lblAlg << _cmbAlg
            << _lblSpeed << _slSpeed
            << _lblDensity << _slDensity
            << _lblSize << _edRows << _edCols
            << _chkDiagonal << _chkGuarantee
            << _lblVisitedColor << _visitedColor
            << _lblPathColor << _pathColor
            << _lblEdit << _cmbEdit;

        gc.appendRow(_canvas, -1);
        setLayout(&_gl);

        applyControlsToModel(true);
        _canvas.newMaze();
        publishStatus(tr("Ready"));
    }

    ~MainView()
    {
        stopTimerInternal();
    }

    bool isRunning() const { return _animating; }

    void startStop()
    {
        if (isRunning())
            stop();
        else
            startTimer();
    }

    void stop()
    {
        stopTimerInternal();
        publishStatus(tr("Ready"));
        if (_fnUpdateUI) _fnUpdateUI();
    }

    void step()
    {
        stop();
        applyControlsToModel(false);

        auto t0 = std::chrono::high_resolution_clock::now();
        _canvas.stepOnce();
        auto t1 = std::chrono::high_resolution_clock::now();
        int ms = int(std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count());

        if (_canvas.isFinished())
        {
            if (_canvas.isSolved()) publishStatus(tr("Solved"), ms);
            else publishStatus(tr("NoPath"), ms);
        }
        else
        {
            publishStatus(tr("Running"), ms);
        }

        if (_fnUpdateUI) _fnUpdateUI();
    }

    void newMaze()
    {
        stop();
        applyControlsToModel(true);
        _canvas.newMaze();
        publishStatus(tr("Ready"));
        if (_fnUpdateUI) _fnUpdateUI();
    }

    void setFocusToCanvas()
    {
        _canvas.setFocus();
    }
};
