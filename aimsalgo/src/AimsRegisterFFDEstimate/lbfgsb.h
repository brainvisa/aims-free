/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#ifndef _AIMSLGBFS_H_
#define _AIMSLGBFS_H_

#include <vector>
#include <string>

#define LBFGSB_START 0
#define LBFGSB_FG    1
#define LBFGSB_FG_ST 11
#define LBFGSB_FG_LN 12
#define LBFGSB_NEW_X 2
#define LBFGSB_CONV  3
#define LBFGSB_ABNO  4
#define LBFGSB_ERROR 5

class LbfgsbOptimizer
{
public:
    LbfgsbOptimizer();
    virtual ~LbfgsbOptimizer();
    void init();
    bool cont();
    bool updateAvailable();
    int status();
    std::vector<float> getMinDir(std::vector<float>& x,
                                float val,
                                std::vector<float>& der,
                                std::vector<float>& low,
                                std::vector<float>& upp);

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
    std::vector<double> _current_x;
    std::vector<double> _current_g;
    double _current_f;
    size_t _current_n;
    int _iteration;
    int _max_iterations;
    bool _initialized;
};

#endif