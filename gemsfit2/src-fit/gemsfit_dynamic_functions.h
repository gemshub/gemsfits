#ifndef GEMSFIT_DYNAMIC_FUNCTIONS_H
#define GEMSFIT_DYNAMIC_FUNCTIONS_H

#include <vector>
#include <iostream>
#include "gemsfit_task.h"

void titration(TGfitTask *sys);

double titfunc(const std::vector<double> &x, std::vector<double> &grad, void *obj_func_data);

void titrationG(TGfitTask *sys);

double golden(double a, double b, double c, double d, int EXPndx, TGfitTask *sys);
double get_pH(double x, int EXPndx, TGfitTask *sys);


#endif // GEMSFIT_DYNAMIC_FUNCTIONS_H
