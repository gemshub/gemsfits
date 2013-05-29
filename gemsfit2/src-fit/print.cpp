#include "print.h"
#include <string>
#include <fstream>



using namespace std;

ResPrint::ResPrint(string path_)
{
    path = path_;
}


void ResPrint::print_clear()
{
    experiment.clear();
    what1.clear();
    what2.clear();
    unit.clear();
    measured.clear();
    computed.clear();
    residual.clear();
}

void ResPrint::set_print(string experiment_, string what1_, string what2_, string unit_, double measured_, double computed_, double residual_)
{
    experiment.push_back(experiment_);
    what1.push_back(what1_);
    what2.push_back(what2_);
    unit.push_back(unit_);
    measured.push_back(measured_);
    computed.push_back(computed_);
    residual.push_back(residual_);
}


void ResPrint::print_header( string function)
{
    ofstream fout;
    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; /*exit(1);*/ }

    fout << "experiment,,,unit,measured,computed," <<function<<endl;

}

void ResPrint::print_result()
{
    ofstream fout;
    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; /*exit(1);*/ }

    fout.setf(ios::fixed);
    for (int i=0; i<experiment.size(); ++i)
    {
        fout<<experiment[i]<<","<<what1[i]<<","<<what2[i]<<","<<unit[i]<<","<<measured[i]<<","<<computed[i]<<","<<residual[i]<<endl;
    }
}

void ResPrint::print_result(string experiment, string what2, string unit, double measured, double computed, double residual)
{
    ofstream fout;
    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; /*exit(1);*/ }

    fout<<experiment<<","/*<<what1*/<<","<<what2<<","<<unit<<","<<measured<<","<<computed<<","<<residual<<endl;
}
