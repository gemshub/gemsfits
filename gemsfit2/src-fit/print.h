#ifndef PRINT_H
#define PRINT_H

#include <vector>
#include <string>
#include <iostream>
#include <optimization.h>
using namespace std;

class ResPrint
{
private:
    typedef vector<double>  double_v;
    typedef vector<string>  string_v;
    string_v experiment, what1, what2, unit, fucntion;
    double_v measured, computed, Weighted_Tfun_residual;
    string path;

public:
    ResPrint(string path_, optimization *opti);

    ~ResPrint ();

    optimization* res_opti;

    void print_header(string function, string weight_);

    void print_clear();

    void set_print(string experiment_, string what1_, string what2_, string unit_, double measured_, double computed_, double Weighted_Tfun_residual_, double weight_ );

    void print_result();
    void print_result(string experiment, string what2, string unit, double measured, double computed, double residual);
};






#endif // PRINT_H
