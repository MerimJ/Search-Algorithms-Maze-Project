#pragma once
#include <vector>
#include <queue>
#include <random>
#include <limits>
#include <algorithm>
#include <cmath>
#include <cstdint>

#include <gui/Shape.h>
#include <gui/Types.h>
#include <td/ColorID.h>

class MazeModel
{
public:
    struct Cell { int r=0,c=0; };

    enum class Algorithm : int
    {
        BFS = 0,
        Dijkstra = 1,
        AStar = 2
    };

    struct Options
    {
        Algorithm algorithm = Algorithm::AStar;
        bool diagonal = false;
        bool guaranteePath = true;
        int sleepMS = 50;                 // for animation thread
        float density = 0.28f;            // wall probability
        int rows = 25;
        int cols = 25;
        td::ColorID visitedColor = td::ColorID::Yellow;
        td::ColorID frontierColor = td::ColorID::Cyan;
        td::ColorID pathColor = td::ColorID::Green;
    };

private:
    Options _opt;
    gui::Size _viewSize{900,900};

    std::vector<unsigned char> _grid;   // 0 free, 1 wall
    std::vector<uint8_t> _state;        // 0 none, 1 visited(closed), 2 frontier(open), 3 path
    std::vector<Cell> _path;

    Cell _start{0,0};
    Cell _goal{24,24};

    // Search state (for step-by-step)
    bool _running = false;
    bool _finished = false;
    bool _solved = false;

    // For BFS/Dijkstra/A*
    struct PQNode { int f,g,idx; };
    struct PQCmp { bool operator()(const PQNode& a, const PQNode& b) const { return a.f > b.f; } };

    std::priority_queue<PQNode, std::vector<PQNode>, PQCmp> _pq;
    std::queue<int> _q;
    std::vector<int> _gScore;
    std::vector<int> _parent;

private:
    static int id(int r,int c,int cols){ return r*cols + c; }
    int N() const { return _opt.rows * _opt.cols; }

    int heuristic(int r,int c) const
    {
        // Manhattan works well for 4-neighborhood, still ok for diagonal as admissible if scaled
        return std::abs(_goal.r-r) + std::abs(_goal.c-c);
    }

    void rebuildPathFrom(int goalIdx)
    {
        _path.clear();
        int t = goalIdx;
        while (t != -1)
        {
            _path.push_back({t / _opt.cols, t % _opt.cols});
            t = _parent[t];
        }
        std::reverse(_path.begin(), _path.end());

        // mark state as path (leave visited/frontier for visualization)
        for (const auto& p : _path)
        {
            int i = id(p.r, p.c, _opt.cols);
            _state[i] = 3;
        }
    }

public:
    MazeModel()
    {
        applySize(_opt.rows, _opt.cols);
        generate();
    }

    // ----- Options -----
    Options& options() { return _opt; }
    const Options& options() const { return _opt; }

    void applySize(int rows, int cols)
    {
        _opt.rows = std::max(5, std::min(rows, 80));
        _opt.cols = std::max(5, std::min(cols, 80));
        _grid.assign(N(), 0);
        _state.assign(N(), 0);
        _gScore.assign(N(), std::numeric_limits<int>::max());
        _parent.assign(N(), -1);
        _start = {0,0};
        _goal = {_opt.rows-1, _opt.cols-1};
    }

    // ----- View & editing helpers -----
    void setViewSize(const gui::Size& s) { _viewSize = s; }

    Cell pointToCell(const gui::Point& p) const
    {
        const int R=_opt.rows, C=_opt.cols;
        gui::CoordType w=_viewSize.width, h=_viewSize.height;
        gui::CoordType cell = std::floor(std::min(w / C, h / R));
        if(cell < 6) cell = 6;
        gui::CoordType gridW = cell * C;
        gui::CoordType gridH = cell * R;
        gui::CoordType ox = (w - gridW) / 2;
        gui::CoordType oy = (h - gridH) / 2;

        int c = int((p.x - ox) / cell);
        int r = int((p.y - oy) / cell);
        if (r < 0) r = 0;
        if (c < 0) c = 0;
        if (r >= R) r = R-1;
        if (c >= C) c = C-1;
        return {r,c};
    }

    void toggleWall(Cell cell)
    {
        if (cell.r == _start.r && cell.c == _start.c) return;
        if (cell.r == _goal.r && cell.c == _goal.c) return;
        int i = id(cell.r, cell.c, _opt.cols);
        _grid[i] = (_grid[i] == 1) ? 0 : 1;
        clearSearchVisualization();
    }

    void setStart(Cell cell)
    {
        if (_grid[id(cell.r,cell.c,_opt.cols)] == 1) return;
        _start = cell;
        clearSearchVisualization();
    }

    void setGoal(Cell cell)
    {
        if (_grid[id(cell.r,cell.c,_opt.cols)] == 1) return;
        _goal = cell;
        clearSearchVisualization();
    }

    void clearSearchVisualization()
    {
        std::fill(_state.begin(), _state.end(), 0);
        _path.clear();
        _running = false;
        _finished = false;
        _solved = false;
        // clear search containers
        while(!_pq.empty()) _pq.pop();
        while(!_q.empty()) _q.pop();
        std::fill(_gScore.begin(), _gScore.end(), std::numeric_limits<int>::max());
        std::fill(_parent.begin(), _parent.end(), -1);
    }

    // ----- Generation -----
    void generate(unsigned seed = 0)
    {
        clearSearchVisualization();

        if(seed == 0)
        {
            std::random_device rd;
            seed = rd();
        }
        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> u(0.f,1.f);

        _grid.assign(N(), 0);
        for(int r=0;r<_opt.rows;r++)
            for(int c=0;c<_opt.cols;c++)
                _grid[id(r,c,_opt.cols)] = (u(rng) < _opt.density) ? 1 : 0;

        auto open=[&](int r,int c){
            if(r>=0 && r<_opt.rows && c>=0 && c<_opt.cols)
                _grid[id(r,c,_opt.cols)] = 0;
        };

        // keep corners open
        for(int dr=0; dr<=1; dr++)
            for(int dc=0; dc<=1; dc++)
            {
                open(0+dr,0+dc);
                open(_opt.rows-1-dr, _opt.cols-1-dc);
            }

        if (_opt.guaranteePath)
        {
            // Guaranteed corridor: top row then right column (simple and robust)
            for (int c = 0; c < _opt.cols; c++)
                _grid[id(0, c, _opt.cols)] = 0;
            for (int r = 0; r < _opt.rows; r++)
                _grid[id(r, _opt.cols - 1, _opt.cols)] = 0;
        }

        _start = {0,0};
        _goal  = {_opt.rows-1, _opt.cols-1};
    }

    // ----- Search control -----
    bool isRunning() const { return _running; }
    bool isFinished() const { return _finished; }
    bool isSolved() const { return _solved; }

    int visitedCount() const
    {
        int c = 0;
        for (auto s : _state) if (s == 1) c++;
        return c;
    }
    int pathLength() const { return (int)_path.size(); }

    void beginSearch()
    {
        clearSearchVisualization();

        int s = id(_start.r,_start.c,_opt.cols);
        _gScore[s] = 0;
        _parent[s] = -1;

        if (_opt.algorithm == Algorithm::BFS)
        {
            _q.push(s);
            _state[s] = 2; // frontier
        }
        else
        {
            int f0 = (_opt.algorithm == Algorithm::AStar) ? heuristic(_start.r,_start.c) : 0;
            _pq.push({f0, 0, s});
            _state[s] = 2;
        }

        _running = true;
        _finished = false;
        _solved = false;
    }

    // Returns: true if state changed (progress), false if already finished
    bool step()
    {
        if (!_running)
            beginSearch();

        if (_finished)
            return false;

        const int R=_opt.rows, C=_opt.cols;
        const int goalIdx = id(_goal.r,_goal.c,C);

        auto pushNeighbor = [&](int curIdx, int nr, int nc, int stepCost)
        {
            if (nr<0 || nr>=R || nc<0 || nc>=C) return;
            int ni = id(nr,nc,C);
            if (_grid[ni] == 1) return; // wall

            if (_opt.algorithm == Algorithm::BFS)
            {
                if (_state[ni] == 0) // unvisited/unseen
                {
                    _state[ni] = 2; // frontier
                    _parent[ni] = curIdx;
                    _q.push(ni);
                }
            }
            else
            {
                int tentative = _gScore[curIdx] + stepCost;
                if (tentative < _gScore[ni])
                {
                    _gScore[ni] = tentative;
                    _parent[ni] = curIdx;
                    int h = (_opt.algorithm == Algorithm::AStar) ? heuristic(nr,nc) : 0;
                    _pq.push({tentative + h, tentative, ni});
                    if (_state[ni] != 1) // keep solved path marking later
                        _state[ni] = 2; // frontier
                }
            }
        };

        auto expandFrom = [&](int curIdx)
        {
            int r = curIdx / C;
            int c = curIdx % C;

            // mark closed/visited
            if (_state[curIdx] != 3) // don't overwrite path marker
                _state[curIdx] = 1;

            // 4-neighborhood
            pushNeighbor(curIdx, r-1, c, 1);
            pushNeighbor(curIdx, r+1, c, 1);
            pushNeighbor(curIdx, r, c-1, 1);
            pushNeighbor(curIdx, r, c+1, 1);

            if (_opt.diagonal)
            {
                // Diagonals with cost 2 (integer approx); keeps it simple
                pushNeighbor(curIdx, r-1, c-1, 2);
                pushNeighbor(curIdx, r-1, c+1, 2);
                pushNeighbor(curIdx, r+1, c-1, 2);
                pushNeighbor(curIdx, r+1, c+1, 2);
            }
        };

        int curIdx = -1;

        if (_opt.algorithm == Algorithm::BFS)
        {
            if (_q.empty())
            {
                _finished = true;
                _running = false;
                _solved = false;
                return false;
            }
            curIdx = _q.front(); _q.pop();
        }
        else
        {
            while(!_pq.empty())
            {
                auto n = _pq.top(); _pq.pop();
                // Skip outdated entries
                if (n.g == _gScore[n.idx])
                {
                    curIdx = n.idx;
                    break;
                }
            }
            if (curIdx == -1)
            {
                _finished = true;
                _running = false;
                _solved = false;
                return false;
            }
        }

        if (curIdx == goalIdx)
        {
            // reconstruct & finish
            rebuildPathFrom(goalIdx);
            _finished = true;
            _running = false;
            _solved = true;
            return true;
        }

        expandFrom(curIdx);
        return true;
    }

    // ----- Drawing -----
    void draw() const
    {
        const int R=_opt.rows, C=_opt.cols;
        if(R<=0 || C<=0) return;

        gui::CoordType w=_viewSize.width;
        gui::CoordType h=_viewSize.height;

        gui::CoordType cell = std::floor(std::min(w / C, h / R));
        if(cell < 6) cell = 6;

        gui::CoordType gridW = cell * C;
        gui::CoordType gridH = cell * R;
        gui::CoordType ox = (w - gridW) / 2;
        gui::CoordType oy = (h - gridH) / 2;

        for(int r=0;r<R;r++)
        {
            for(int c=0;c<C;c++)
            {
                int i = id(r,c,C);
                gui::CoordType x=ox+c*cell;
                gui::CoordType y=oy+r*cell;
                gui::Rect rc(gui::Point(x,y), gui::Size(cell,cell));

                // base: free cell fill
                td::ColorID fill = td::ColorID::White;
                if (_grid[i] == 1) fill = td::ColorID::Black;

                // overlays for search visualization
                if (_grid[i] == 0)
                {
                    if (_state[i] == 2) fill = _opt.frontierColor;     // frontier
                    else if (_state[i] == 1) fill = _opt.visitedColor; // visited
                    else if (_state[i] == 3) fill = _opt.pathColor;    // path
                }

                gui::Shape::drawRect(rc, fill);
                // subtle grid
                gui::Shape::drawRect(rc, td::ColorID::DimGray, 0.8f);
            }
        }

        auto mark=[&](Cell p, td::ColorID col, float thickness)
        {
            gui::CoordType x=ox+p.c*cell;
            gui::CoordType y=oy+p.r*cell;
            gui::Rect rc(gui::Point(x,y), gui::Size(cell,cell));
            gui::Shape::drawRect(rc, col, thickness);
        };

        mark(_start, td::ColorID::Blue, 4.0f);
        mark(_goal, td::ColorID::Red, 4.0f);
    }
};
