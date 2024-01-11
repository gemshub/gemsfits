//-------------------------------------------------------------------
// $Id: s_formula.h 1373 2009-07-22 12:25:22Z gems $
//
// Declaration of TFormula - gems chemical formulas parser
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001  D.Kulik, S.Dmytriyeva
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#ifndef _s_formula_h_
#define _s_formula_h_

#include <string>
#include <list>
#include <vector>
//using namespace std;

#ifdef IPMGEMPLUGIN

#include "m_const.h"
#define SHORT_EMPTY 	   -32768
const unsigned int
    IC_RKLEN = 32,
    DC_RKLEN = 32;

#endif


extern const char * CHARGE_NAME;

struct ICTERM     // description of parsed element
{
    std::string ick;
    std::string ick_iso;
    int val;             // valence IC
    double stoc;          // stoich. coef.

    ICTERM( const char* aIck, const char* aIso, int aVal, double aStoc ):
            ick(aIck), ick_iso(aIso), val(aVal), stoc(aStoc)
    {}
    ICTERM( const ICTERM& data ):
            val(data.val), stoc(data.stoc)
    {
        ick = data.ick;
        ick_iso = data.ick_iso;
    }
};

struct MOITERM    // description of moiety element
{
    std::string name;
    int  site;        // sublattice site
    double nj;          // moiety-site occupancy.

    MOITERM( const char* aName, int aSite, double aNj ):
            /*name(aName),*/ site(aSite), nj(aNj)
    {
       char val[100];
       sprintf(val, "{%s}%d", aName, site);
       name = std::string(val);
    }

    MOITERM( const MOITERM& data ):
       site(data.site), nj(data.nj)
    {
        name = data.name;
    }
};

class Formuan  // stack for analyzing formula
{
    std::string form_str;
    std::string charge_str;

    //TIArray<ICTERM>  ict_;

    int nSites;             // number of sites in formula
    //TIArray<MOITERM> moit_;

protected:

    void icadd(  std::list<ICTERM>& itt_, const char *icn,
                 const char *iso, int val, double csto );
    void icadd(  std::list<ICTERM>& itt_, ICTERM& it );
    int ictcomp( std::list<ICTERM>::iterator& it, std::string& ick, int val );


    inline bool iscapl( char ch )  // is cap letter
    {
        return( (ch>='A' && ch<='Z') || ch=='$' );
    }
    inline bool islowl( char ch )  // is lcase letter
    {
        return( (ch>='a' && ch<='z') ||  ch == '_' );
    }
    //char *xblanc( char *cur );
    void xblanc( std::string& str );

    void charge(std::list<ICTERM>& tt);
    void   scanCharge();
    void scanFterm( std::list<ICTERM>& itt_, std::string& startPos, char endSimb );
    void scanElem( std::list<ICTERM>& itt_, std::string& cur );
    void getReal( double& real, std::string& cur);
    void scanValence( int& val, std::string& cur);
    void scanIsotope( std::string& isotop, std::string& cur);
    void scanICsymb(  std::string& icName, std::string& cur);


public:

    Formuan( const char * formula );
    ~Formuan();

    int scanMoiety( std::vector<MOITERM>& moit_ );
    void scanFormula( std::list<ICTERM>& tt );

};


class TFormula  // description of disassembled formula token
{
    double aZ;   // calculated charge in Mol

    std::vector<std::string>  aCn;  // list of IC
    std::vector<double> aSC;  // list of stoichiometric coef.
    std::vector<short> aVal;  // list of valence numbers
    //TIArray<MOITERM> moit_;

    std::string aFormula;  // analayzed formula

protected:

    void fo_clear();
    void fo_unpak( std::list<ICTERM>& itt_ );

public:

    TFormula();
    ~TFormula();

    //--- Selectors
    const char* GetCn( int ii ) const
    {
        return aCn[ii].c_str();
    }
    double GetSC( int ii ) const
    {
        return aSC[ii];
    }
    short GetVal( int ii ) const
    {
        return aVal[ii];
    }
    double GetZ() const
    {
        return aZ;
    }
    int GetIn() const
    {
        return aCn.size();
    }
    const char* GetFormula( ) const
    {
        return aFormula.c_str();
    }

    int BuildMoiety( const char * StrForm, std::vector<MOITERM>& moit_ );

    //--- Value manipulation
    void fixup_ics( char* ICs );
    void SetFormula( const char * StrForm ); // and ce_fscan
    int Fmwtz( double &Z, double &mW, double &eSm, short *lAn );
int Fmwtz( double &Z, double &mW, double &eSm, short *lAn, double &Nj );
    void TestIC( const char *key, int N, char *ICsym );
    void Stm_line( int N, double *Sml, char *ICsym, short *ICval );
    void Reset();
    std::string form_extr( int nCk, int L, char *Cfor );
};



#endif  // _s_formula_h


