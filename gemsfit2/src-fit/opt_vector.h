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
#include "verror.h"

class opti_vector
{
private:
//    vector<int> Error;
    /**
    * Gets the indexes of the RDc species
    * @author DM
    * @param node
    * @param ov opti_vector class
    * @date 06.05.2013
    */
    void get_RDc_indexes (TNode *node, opti_vector *ov);

    /**
    * Gets the indexes of the linked parameters/elements form bulk composition
    * @author DM
    * @param node
    * @param ov opti_vector class
    * @date 06.05.2013
    */
    void get_Lp_indexes (TNode *node, opti_vector *ov);


public:
    typedef vector<double> vd;        // vector of double
    typedef vector<vector<double> > vvd;  // 2D vector of double
    typedef vector<string> vs;        // vector of string
    typedef vector<int> vi;           // vecotr of integer
    typedef vector<vector<int> > vvi; // 2D vector of integer

    opti_vector( );

    // optimization vector that weill be sent to NLopt
    vd opt; /// position 0 specie 0 etc.
    // optimized parameter information
    vd UB; /// upper boundary
    vd LB; /// lower boundary
    vs Ptype; /// name of parameter e.g G0
    vi Pindex; /// index formatted value into reading array, index of phase, species

    // vector containing the first guesses for normalization of the opt vector
    vd optv0; /// vector storing initial parameter values

    bool h_RDc; /// handle for checking if there are reaction constraints in the input file
    bool h_Lp; /// handle for checking if there are linked parameters in the input file
    bool h_dynfun; /// handle for checking if there are dynamic function in the DataTarget

    struct RDc /// structure storing reaction constraint information
    {
        int DcIndex; /// index of the dependent component
        double IV; /// initial value
//        double logK; /// log K of the reaction
        vd logK_TPpairs; /// log K for all T and P pairs
        vd dG_reaction_TP; /// delta G of the reaction for all T and P pairs
        int nC; /// number of components
        string Dc_name; /// species name
        vs rdc_species; /// Names of species involved in reaction (the last one is the fixed species)
        vi rdc_species_ind; /// indices of RDC species in GEMS3K - read from node class after reading the experimental data
        vi rdc_species_coef; /// reaction coeficients
        string Ref; /// reference for log K
        double std_gibbs; /// standard state Gibbs free energy of the reaction constrained species at T=25 C and P=1 bar
    };
    vector<RDc*> reactions; /// Vector of pointer to reactions (rdc_species struct that hold the reaction dependent species and the reaction properties)

    struct Lp /// structure holding liked parameters information
    {
        int index;
        string type;
        string name; /// name of the linked parameter e.g H
        double IV; /// initial value
        double EV; /// end value
        vs L_param; /// name of the parameters that the linked parameters is linked to eg. Cl in HCl and S in H2SO4
        vi L_param_ind;
        vd L_coef; /// linking coefiecients e.g 1.0 for Cl and 2.0 for S
    };
    vector<Lp*> Lparams;
};

#endif // OPT_VECTOR_H
