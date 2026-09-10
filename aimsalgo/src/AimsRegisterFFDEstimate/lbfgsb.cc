/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cartobase/exception/assert.h>
#include <LBFGSpp/LBFGSBSolver.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// Constants for L-BFGS-B
const int nmax = 3001;    // 10*10*10 * 3
const int mmax = 17;

class LbfgsbOptimizer
{
public:
    LbfgsbOptimizer()
        : _wa(nullptr),
          _iwa(nullptr),
          _iprint(-1),
          _initialized(false),
          _iteration(0),
          _max_iterations(1000)
    {
        // Allocate working arrays (kept for backward compatibility)
        _wa = new double[2 * mmax * nmax + 4 * nmax + 12 * mmax * mmax + 12 * mmax];
        _iwa = new int[3 * nmax];
        strcpy(_task, "START");
        strcpy(_csave, "");
        for (int i = 0; i < 4; i++) _lsave[i] = false;
        for (int i = 0; i < 44; i++) _isave[i] = 0;
        for (int i = 0; i < 44; i++) _dsave[i] = 0.0;
    }

    virtual ~LbfgsbOptimizer()
    {
        delete[] _wa;
        delete[] _iwa;
    }

    void init()
    {
        strcpy(_task, "START");
        _iprint = -1;
        _iteration = 0;
        _initialized = false;
    }

    bool cont()
    {
        return (strncmp(_task, "START", 5) == 0) ||
               (strncmp(_task, "FG", 2) == 0) ||
               (strncmp(_task, "NEW_X", 5) == 0);
    }

    bool updateAvailable()
    {
        return (strncmp(_task, "NEW_X", 5) == 0) ||
               (strncmp(_task, "FG_START", 8) == 0);
    }

    int status()
    {
        cout << "Status " << _task << endl;
        return 1;
    }

    vector<float> getMinDir(vector<float>& p, float val,
                           vector<float>& der,
                           vector<float>& low,
                           vector<float>& upp)
    {
        ASSERT(p.size() == der.size());
        ASSERT(p.size() == low.size());
        ASSERT(p.size() == upp.size());

        size_t n = p.size();

        // Convert input to double precision
        vector<double> x(n);
        vector<double> g(n);
        vector<double> l(n);
        vector<double> u(n);

        for (size_t i = 0; i < n; i++)
        {
            x[i] = p[i];
            g[i] = der[i];
            l[i] = low[i];
            u[i] = upp[i];
        }

        // Set up bounds: 2 = both lower and upper bounds
        vector<int> nbd(n, 2);

        // Configure solver parameters to match Fortran version
        LBFGSpp::LBFGSBParam<double> param;
        param.m = mmax;  // Maximum number of variable metric corrections
        param.max_iterations = 1;  // We'll do one iteration at a time
        param.max_linesearch = 50;
        param.past = 0;
        param.gttol = 1e-5;
        param.factr = 1e2;  // Same as Fortran version
        param.pgtol = 1e-5;  // Same as Fortran version
        param.epsilon = 1e-8;
        param.delta = 0.0;

        // Create solver
        LBFGSpp::LBFGSBSolver<double> solver(param);

        // Set bounds
        solver.SetBounds(l, u, nbd);

        // Store current state for the functor
        _current_x = x;
        _current_g = g;
        _current_f = val;
        _current_n = n;

        // Create a functor for the optimization problem
        // This functor will be called by LBFGS++ to evaluate the function and gradient
        class OptimizationFunctor
        {
        public:
            OptimizationFunctor(LbfgsbOptimizer* optimizer)
                : _optimizer(optimizer)
            {
            }

            double operator()(const vector<double>& x, vector<double>& grad)
            {
                // For the first call, return the initial function value
                if (!_optimizer->_initialized)
                {
                    _optimizer->_initialized = true;
                    // Copy the initial x back
                    for (size_t i = 0; i < x.size(); i++)
                    {
                        _optimizer->_current_x[i] = x[i];
                    }
                    return _optimizer->_current_f;
                }

                // For subsequent calls, we need to update the gradient
                // In the original code, the gradient is computed externally
                // and passed via the der parameter. We'll use that gradient.
                for (size_t i = 0; i < grad.size(); i++)
                {
                    grad[i] = _optimizer->_current_g[i];
                }

                // Update the current x
                for (size_t i = 0; i < x.size(); i++)
                {
                    _optimizer->_current_x[i] = x[i];
                }

                return _optimizer->_current_f;
            }

        private:
            LbfgsbOptimizer* _optimizer;
        };

        OptimizationFunctor functor(this);

        // Run one iteration of the optimization
        double fx;
        int niter = solver.Minimize(functor, x, fx);

        // Update the task string based on the solver status
        if (_iteration == 0)
        {
            strcpy(_task, "FG_START");
            _iteration++;
        }
        else if (niter >= 0)
        {
            strcpy(_task, "NEW_X");
            _iteration++;
        }
        else
        {
            strcpy(_task, "CONVERGENCE");
        }

        // Copy the result back to the input vector
        for (size_t i = 0; i < n; i++)
        {
            p[i] = x[i];
        }

        return p;
    }

private:
    double* _wa;
    int* _iwa;
    char _task[60];
    int _iprint;
    char _csave[60];
    bool _lsave[4];
    int _isave[44];
    double _dsave[44];

    // State for LBFGS++ integration
    vector<double> _current_x;
    vector<double> _current_g;
    double _current_f;
    size_t _current_n;
    int _iteration;
    int _max_iterations;
    bool _initialized;
};