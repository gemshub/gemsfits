#ifndef _system_properties_h_
#define _system_properties_h_

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iomanip>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>

using namespace std;
using namespace boost;


class System_Properties
{
    public:
    // Constructor
    System_Properties();
    // Destructor
    ~System_Properties();

    typedef vector<double> vd;
    typedef vector<string> vs;
    typedef vector<int> vi;
    typedef vector<vector<int> > vvi;

    // System specifications
    string phase_name;
    string param_file;
    string meas_db;
    vs species;
    vd charges;

    enum act_model
    {
        ELVIS = 0,
        Pitzer,
        EUNIQUAC
    };
    act_model activity_model;

    // Struct holding optimization and statistics data
    struct opti_stat
    {
        // optimization
        int fitmode;
        int perform;
        int algo;
        int nodes;
        double tolerance;
        double maxeval;

        // statistics
        bool MC;
        int MCruns;
        int MCbars;
        int SSRpoints;
        int SSRplotpoints;

        // measurement data
        int data_source;
        string csv_file;
        string dbname;
        string passwd;
        string username;
        string tablename;

    };
    opti_stat* optstat;

    // Struct with system specific measurement data
    struct parameters
    {
        // dimensionalities of the parameter arrays
        int cols_aIPc;
        int rows_aIPc;
        int cols_aIPx;
        int cols_aDCc;

        // parameters vectors
        vector<string> aIPc;
        vector<string> aDCc;
        vector<string> aIPx;

        // vectors containing first guesses of aIPc/aDCc arrays
        vector<double> aIPc_fit_val;
        vector<double> aDCc_fit_val;

        // vectors containing the position indices of the values
        // that will be fitted in the aIPc/aDCc arrays
        vector<int> aIPc_fit_ind;
        vector<int> aDCc_fit_ind;

        // vectors containing the bound and constraint values
        vector<double> bound_lo;
        vector<double> bound_up;

        bool constraint_bool;
        vector<double> constraint_lo;
        vector<double> constraint_up;

    };

    parameters* sysparam;
    void getparam( parameters* sysparam );

    void uploadparam( string phase_name, string param_file );

    void uploadparam_GEMSFIT();

    struct print_info
    {
        // Meas - Comp
        vector<string> meascomp_temp;
        vector<string> meascomp_pres;
        vector<string> meascomp_mollo;
        vector<string> meascomp_molup;
        string meascomp_X0;
        string meascomp_Xstep;
        string meascomp_Xend;
        string meascomp_Y0;
        string meascomp_Ystep;
        string meascomp_Yend;
        string meascomp_filename;
        string meascomp_fileformat;
        int meascomp_valcode;
        string meascomp_labelX;
        string meascomp_labelY;
        string meascomp_headline;

        // Residual
        vector<string> res_temp;
        vector<string> res_pres;
        vector<string> res_mollo;
        vector<string> res_molup;
        string res_X0;
        string res_Xstep;
        string res_Xend;
        string res_Y0;
        string res_Ystep;
        string res_Yend;
        string res_filename;
        string res_fileformat;
        int	res_valcode;
        string res_labelX;
        string res_labelY;
        string res_headline;

        // Residual percentage
        vector<string> resperc_temp;
        vector<string> resperc_pres;
        vector<string> resperc_mollo;
        vector<string> resperc_molup;
        string resperc_X0;
        string resperc_Xstep;
        string resperc_Xend;
        string resperc_Y0;
        string resperc_Ystep;
        string resperc_Yend;
        string resperc_filename;
        string resperc_fileformat;
        int resperc_valcode;
        string resperc_labelX;
        string resperc_labelY;
        string resperc_headline;

        // Q-Q plot
        vector<string> qq_temp;
        vector<string> qq_pres;
        vector<string> qq_mollo;
        vector<string> qq_molup;
        string qq_X0;
        string qq_Xstep;
        string qq_Xend;
        string qq_Y0;
        string qq_Ystep;
        string qq_Yend;
        string qq_filename;
        string qq_fileformat;
        int qq_valcode;
        string qq_labelX;
        string qq_labelY;
        string qq_headline;


    };

    print_info* printinfo;


};

#endif
