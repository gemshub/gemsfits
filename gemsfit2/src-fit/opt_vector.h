#ifndef OPT_VECTOR_H
#define OPT_VECTOR_H

#include <vector>
#include <string>
#include "node.h"
#include "verror.h"

class opti_vector
{
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
    vi Pindex; /// index formatted value into reading array, index of pahse, species

    // vector containing the first guesses for normalization of the opt vector
    vd optv0; /// vector storing initial parameter values

    bool h_RDc; /// handle for cheking if there are reaction constraints in the input file

    struct RDc /// structure storing reaction constraint information
    {
        int DcIndex; /// index of the dependent component
        double IV; /// initial value
        double logK; /// log K of the reaction
        int nC; /// number of components
        string Dc_name; /// species name
        vs rdc_species; /// Names of species involved in reaction (the last one is the fixed species)
        vi rdc_species_ind; /// indices of RDC species in GEMS3K - read from node class after reading the experimental data
        vi rdc_species_coef; /// reaction coeficients
        string Ref; /// reference for log K
        double std_gibbs; /// standard state Gibbs free energy of the reaction constrained species at T=25 C and P=1 bar
    };
    vector<RDc*> reactions; /// Vector of pointer to reactions (rdc_species struct that hold the reaction dependent species and the reaction properties)


    void get_RDc_indexes (TNode *node, opti_vector *ov);

};

#endif // OPT_VECTOR_H
