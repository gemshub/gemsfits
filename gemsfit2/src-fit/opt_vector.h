/// \file opt_vector.h
/// Contains declaration of opti_vector class used in GEMSFIT for
/// storing optimized vector information
//
/// \class opti_vector opt_vector.h
///  The opti_vector class, retrieves and stores the adjusted paramaters and
/// reaction constraints from the GEMSFIT input file.

//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//


#ifndef OPT_VECTOR_H
#define OPT_VECTOR_H

#include <vector>
#include <string>
#include "node.h"


/// Base class for the optimized parameters
class OptParameter
{
    protected:

        typedef vector<double> vd;        // vector of double
        typedef vector<vector<double> > vvd;  // 2D vector of double
        typedef vector<string> vs;        // vector of string
        typedef vector<int> vi;           // vecotr of integer
        typedef vector<vector<int> > vvi; // 2D vector of integer

        string Ptype; /// stores the type of parameter (G0, bIC, DMc, PMc, TK, P, etc.)

        vector<string> Jdata; /// JSON string containing the parameter information read from the configuration file

        double OptBoundPerc; /// Bounds percentage read from the configuration file

        int mode;

        vector<double> TP_pairs[2];

        struct parameter /// base structure for storing parameter information
        {
            double IV; /// initial value
            double opt; /// parameter value that gets changed during the optimization
            int    optNdx; /// index in the global optimization vector
            double Pndx; /// parameter index
            string Pname; /// parameter name (DCName, ICname, etc.)
            vd IVv; /// vector of initial values for each node/experiment - used in nested optimization
            vd EVv; /// vector of end values for each node/experiment - used in nested optimization
        };

        struct F_parameter : parameter /// structure storing F-type parameter iformation
        {
//            string Ftype; /// function type (OFUN, NFUN)
            int    Fndx; /// function index
            double UB; /// upper boundary
            double LB; /// lower boundary
        };
        vector<F_parameter*> optFP; /// vector of pointers to F-type parameters

        struct R_parameter : parameter /// structure storing reaction constraint information
        {
            vd logK_TPpairs; /// log K for all T and P pairs
            vd dG_reaction_TP; /// delta G of the reaction for all T and P pairs
            int nC; /// number of components
            vs rdc_species; /// Names of species involved in reaction (the last one is the fixed species)
            vi rdc_species_ind; /// indices of RDC species in GEMS3K - read from node class after reading the experimental data
            vi rdc_species_coef; /// reaction coeficients
            string Ref; /// reference for log K
        };
        vector<R_parameter*> optRP; /// Vector of reactions (rdc_species struct that hold the reaction dependent species and the reaction properties)

        struct L_parameter : parameter /// structure holding liked parameters information
        {
            vd delta; /// c = a + b; delta = (a + b) initial
            vs L_param; /// name of the parameters that the linked parameters is linked to eg. Cl in HCl and S in H2SO4
            vi L_param_ind; /// GEMS3K indexes of the linked parameters
            vd L_param_coef; /// linking coefiecients e.g 1.0 for Cl and 2.0 for S
        };
        vector<L_parameter*> optLP; /// Vector of pointers to L-type parameters

        struct S_parameter : parameter /// structure holding liked parameters information
        { };
        vector<S_parameter*> optSP; /// Vector of pointers to S-type parameters

        /**
        * Reads in the attributes of the F-type parameters
        * @author DM
        * @param p counts the parameters
        * @param data JSON string holding the parameter attributes
        * @param *opt pointer to the F_parameter object
        * @date 17.10.2014
        */
        void Pval_to_optF (int p, string data, F_parameter *opt);

        void Pval_to_optR (int p, string data, R_parameter *opt);

        /**
        * Reads in the attributes of the L-type parameters
        * @author DM
        * @param p counts the parameters
        * @param data JSON string holding the parameter attributes
        * @param *opt pointer to the L_parameter object
        * @date 17.10.2014
        */
        void Pval_to_optL (int p, string data, L_parameter *opt);

    public:
        /// Generic constructor
        OptParameter (vector<string> data, double OptBoundPerc );
        /// Destructor
        virtual ~OptParameter();


        virtual long int Set_param( )
        {
            return 0;
        };

        virtual long int Set_logKTP(TNode *node, vector<double> TP_pairs[2] )
        {
            return 0;
        };

        virtual long int Set_logKTP(int RPndx, double logK  )
        {
            return 0;
        };

        virtual long int Adjust_param(TNode *node, vector<double> opt)
        {
            return 0;
        };

        virtual long int Adjust_Fparam(TNode *node, int Pndx, double Pval)
        {
            return 0;
        };

        virtual long int Adjust_Sparam(TNode *node)
        {
            return 0;
        };

        virtual long int Adjust_Lparam(TNode *node, int exp)
        {
            return 0;
        };

        virtual long int SetIndex_param(TNode node)
        {
            return 0;
        };

        virtual string Print_param(  )
        {
            return 0;
        };

        virtual long int SetIVvEVvDelta(TNode *node)
        {
            return 0;
        };

        /**
        * Gets the size of optFP, F parameters vector
        * @author DM
        * @date 17.10.2014
        */
        long int Get_optFPsize ()
        { return optFP.size(); }

        long int Get_optLPsize ()
        { return optLP.size(); }

        long int Get_optRPsize ()
        { return optRP.size(); }

        string Get_optType ()
        { return Ptype; }

        void Set_TPpairs (vector<double> TPpairs[2]);

        /**
        * Gets the size of F parameter GEMS3K index
        * @author DM
        * @param ndx index on the optFP vector
        * @date 17.10.2014
        */
        long int Get_FPndx(int ndx)
        { return optFP[ndx]->Pndx; }

        void Get_IVparam(vd &opt, vd &UB, vd &LB );

        double Get_Fparam(int ndx, int exp );

        double Get_Lparam(int ndx, int exp );

        void Get_Fparam(int ndx, string &name, double &IV, double &EV );

        void Get_Rparam(int ndx, string &name, double &IV, double &EV );

        void Get_R_vNdx_vCoef(int ndx, vi &vNdx, vi &vCoef );

        void Get_Lparam(int ndx, string &name, double &IV, double &EV );

        /**
        * Gets the attributes of the F-type parameter with index ndx
        * @author DM
        * @param ndx index on the optFP vector
        * @param &Fndx retrieves the index of the OFUN or NFUN function
        * @param &Pndx retrieves the GEMS3K index of the parameter
        * @param &Pval retrieves the value of the parameter
        * @param &Ub retrieves the upper bound of the parameter
        * @param &Lb retrieves the lower bound of the parameter
        * @date 17.10.2014
        */
        long int Get_Fparam(int ndx, int &Fndx, int &Pndx, double &Pval, double &Ub, double &Lb);

        /**
        * Gets the attributes of the F-type parameter with index ndx and OptPndx
        * @author DM
        * @param ndx index on the optFP vector
        * @param exp index of the current node/experiment
        * @param &Fndx retrieves the index of the OFUN or NFUN function
        * @param &Pndx retrieves the GEMS3K index of the parameter
        * @param &Pval retrieves the value of the parameter
        * @param &Ub retrieves the upper bound of the parameter
        * @param &Lb retrieves the lower bound of the parameter
        * @date 17.10.2014
        */
        long int Get_Fparam(int ndx, int exp, int &Fndx, int &Pndx, double &Pval, double &Ub, double &Lb);

        /**
        * Sets the value of the F-type parameter with index ndx
        * @author DM
        * @param ndx index of the optFP vector
        * @param Pval new value of the parameter
        * @date 17.10.2014
        */
        void Set_Fparam(int ndx, double Pval);

        /**
        * Sets the value of the F-type parameter with index ndx
        * @author DM
        * @param ndx index in the optFP vector
        * @param exp index of the current node/experiment
        * @param Pval new value of the parameter
        * @date 17.10.2014
        */
        void Set_Fparam(int ndx, int exp, double Pval);
};

class Opt_bIC : public OptParameter
{
    public:

    /// Constructor
    Opt_bIC (vector<string> data, double OptBoundPrc, unsigned &p );
    /// Destructor
    virtual ~Opt_bIC();

    long int SetIndex_param(TNode *node);

    long int SetIVvEVvDelta(TNode *node);

    long int Adjust_Fparam(TNode *node, int Pndx, double Pval);

    long int Adjust_Lparam(TNode *node, int exp);

    string Print_param();
};

class Opt_Tk : public OptParameter
{
    public:

    /// Constructor
    Opt_Tk (vector<string> data, double OptBoundPrc, unsigned &p );
    /// Destructor
    virtual ~Opt_Tk();

    long int SetIndex_param();

    long int SetIVvEVvDelta(TNode *node);

    long int Adjust_Fparam(TNode *node, int Pndx, double Pval);

//    long int Adjust_Lparam(TNode *node, int exp);

    string Print_param();
};

class Opt_P : public OptParameter
{
    public:

    /// Constructor
    Opt_P (vector<string> data, double OptBoundPrc, unsigned &p );
    /// Destructor
    virtual ~Opt_P();

    long int SetIndex_param();

    long int SetIVvEVvDelta(TNode *node);

    long int Adjust_Fparam(TNode *node, int Pndx, double Pval);

//    long int Adjust_Lparam(TNode *node, int exp);

    string Print_param();
};


class Opt_G0 : public OptParameter
{
    public:

    /// Constructor
    Opt_G0 (vector<string> data, double OptBoundPrc, unsigned &p );
    /// Destructor
    virtual ~Opt_G0();

    long int SetIndex_param(TNode *node);

//    long int Get_param(vector<double> &opt);

//    long int Set_param(vector<double> opt, unsigned &p);

    long int Set_logKTP(TNode *node, vector<double> TP_pairs[2] );

    long int Set_logKTP(int RPndx, double logK  );

    long int Adjust_param(TNode *node, vector<double> opt);

    long int Adjust_Fparam(TNode *node, int Pndx, double Pval);

    long int Adjust_Sparam(TNode *node );

    long int Adjust_Rparam(TNode *node, R_parameter &optR );

//    long int Adjust_Lparam(TNode *node, int exp);

//    string Print_param();
};


class Opt_PMc : public OptParameter
{
    public:

    /// Constructor
    Opt_PMc (vector<string> data, double OptBoundPrc, unsigned &p );
    /// Destructor
    virtual ~Opt_PMc();

    long int Adjust_param(TNode *node, vector<double> opt);

    long int Adjust_Fparam(TNode *node, int Pndx, double Pval);

    long int Adjust_Sparam(TNode *node );

};

class Opt_DMc : public OptParameter
{
    public:

    /// Constructor
    Opt_DMc (vector<string> data, double OptBoundPrc, unsigned &p );
    /// Destructor
    virtual ~Opt_DMc();

    long int Adjust_param(TNode *node, vector<double> opt);

    long int Adjust_Fparam(TNode *node, int Pndx, double Pval);

    long int Adjust_Sparam(TNode *node );

};


#endif // OPT_VECTOR_H
