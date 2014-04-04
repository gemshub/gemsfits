//-------------------------------------------------------------------
// $Id: gemsfit_iobson.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Read/Write GEMSFIT input configuration file functions
//
// Copyright (C) 2014  S.V.Dmytriyeva, G.D.Miron
// Uses Qwt (http://qwt.sourceforge.net), EJDB (http://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.5 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
# include <sstream>
using namespace std;

#include "node.h"
#include "io_arrays.h"
#include "keywords.h"

void out_db_specs_txt_bson( fstream& ff, bson *obj, bool with_comments  );
void out_stat_param_txt_bson( fstream& ff, bson *obj, bool with_comments );
void out_nlopt_param_txt_bson( fstream& ff, bson *obj, bool with_comments );

//extern outField DataCH_static_fields[14];
extern outField DataCH_dynamic_fields[30];
extern outField MULTI_dynamic_fields[70];
extern outField DataBR_fields[f_lga+1/*58*/];


//// Read input specifications from configurator
void get_bson_from_gems_fit_txt( const string& fname, bson *obj )
{
    // open file for reading
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(30, DataCH_dynamic_fields, ff);

    string label;
    rdar.getNext( label);
    while( !label.empty() )
    {
       rdar.readFormatArrayToBson( label.c_str(),  obj );
       rdar.getNext( label);
     }

}

void out_gems_fit_txt_bson( fstream& ff, /*TNode* node,*/ bson *obj, bool _comment )
{
    //DATACH* CSD = node->pCSD();
    //DATABR* CNode = node->pCNode();

    if(_comment )
    {
        ff << "\n#  GEM input parameters to fit can be marked by preceding them with the F, L, and R letter "
              "\n#    in <TKval>, <Pval>, <G0>,  <PMc>, <DMc>, <fDQF>, <bIC> ... data object entries, see below."
              "\n#    Values without markup will not be modified by GEMSFIT2 routines, just used in GEM runs.\n"
              "\n#  Mark with F the parameter value you want to fit independently of other parameters. "
              "\n#    Shorthand option: F<initval> - initial value with default (10%) upper- and lower boundaries."
              "\n#    Example:  ... F-833444 ... (<initval> can be modified, if necessary)."
              "\n#    Full JSON-style markup syntax for the independently adjusted parameter: as in this example"
              "\n#      for fitting G0(298) of SiO2@ "
              "\n#      F{ \"IV\" :-833444, \"UB\" : -800000, \"LB\" : -900000 } "
              "\n#      where:"
              "\n#         \"IV\": initial value; "
              "\n#         \"UB\": upper boundary; "
              "\n#         \"LB\": lower bundary (in this case, all in J/mol). \n"
              "\n#  Mark with R the G0(298) value of a Dependent Component, which depends on G0 of other DCs"
              "\n#     via a reaction constraint, by copy-pasting the following template in place of G0(298) value,"
              "\n#     and editing it as desired:"
              "\n#      R{ \"IV\" : -36819, \"Ref\" : \"SUPCRT92\", \"order\" : \"1\", \"nC\" : 4, \"rDC\" : \"KOH@\","
              "\n#      \"RC\" : [ \"K+\", \"H2O@\", \"H+\", \"KOH@\" ], \"Rcoef\" : [ -1, -1, 1, 1 ] }"
              "\n#     Here,  \"IV\": initial value; "
              "\n#         \"Ref\": bibliographic reference (optional);"
              "\n#         \"order\": reaction order | 1, 2, ... |, important for many reaction with common species;"
              "\n#         \"nC\": numer of components (species) involved in the reaction;"
              "\n#         \"DC\": name of Dependent Component whose properties are constrained with this reaction; "
              "\n#         \"RC\": list [ ] of names of all components (species) involved in reaction (comma-separated);"
              "\n#         \"Rcoef\": array [ ] of reaction stoichiometry coeficients (comma-separated), "
              "\n#           in the same order as in the \"RC\" list. This example describes a reaction: "
              "\n#           K+ + H2O@ = H+ + KOH@ in which G0(KOH@) follows variations of G0(K+) induced by the fitting, "
              "\n#           such that the logK of this reaction at (T,P) remains constant. \n"
              "\n#  Mark with L the bIC (system bulk composition) value of Independent Component, "
              "\n#     which depends on bIC of other elements via a titration constraint,"
              "\n#     by copy-pasting the following template in place of the bIC value, and editing as desired:"
              "\n#      L{ \"LE\" :\"H\", \"IV\" :113.016746705914, \"LEs\" :[\"S\", \"Cl\"], \"Lcoef\" :[2,1]}"
              "\n#     In this example: the amount of H is linked to S and Cl by titration with H2SO4 and HCl, "
              "\n#         with stoichiometry coeficients of 2 and 1, respectively."
              "\n#        \"LE\": linked element "
              "\n#        \"IV\": initial value "
              "\n#        \"LEs\": the elements linked to "
              "\n#        \"Lcoef\": linkage coefficients (stoichiometry coefficients) "
              "\n#         Whenever the bIC values of S or Cl are varied as (independent) titration parameters,"
              "\n#         the bIC value of H will be adjusted to titrate the system either with H2SO4 or with HCl."
              "\n#  Restrictions: You can't fit G0's together with TK and P!"
           << endl;
    }

    TPrintArrays  prarCH(30, DataCH_dynamic_fields, ff);

    if( _comment )
        ff << "\n# ICNL: List of Independent Component names (for readability; no need to modify)";
    prarCH.writeArrayFromBson(  "ICNL", obj, MaxICN );
    if( _comment )
            ff << "\n# DCNL: List of Dependent Component names (for readability; no need to modify)";
    prarCH.writeArrayFromBson(  "DCNL", obj, MaxDCN );
    if( _comment )
            ff << "\n# PHNL: List of Phase names (for readability; no need to modify)";
    prarCH.writeArrayFromBson(  "PHNL", obj, MaxPHN );
//    prarCH.writeArrayF(  f_ccPH, CSD->ccPH, CSD->nPH, 1L,_comment, brief_mode );
//    prarCH.writeArray(  f_nDCinPH, CSD->nDCinPH, CSD->nPH, -1L,_comment, brief_mode);
//    prarCH.writeArray(  f_TKval, CSD->TKval, CSD->nTp, -1L,_comment, brief_mode );
//    prarCH.writeArray(  f_Pval, CSD->Pval, CSD->nPp,  -1L,_comment, brief_mode );
    prarCH.writeArrayFromBson(  "G0", obj, 1, true);

    if( _comment )
     ff << "\n \n# logK: Look-up array for logK at T * P * nr reactions. "
          "\n#    If at least one G0 parameter is marked as \'R\' (reaction-constrained)"
          "\n#    and the list below is left commented out, then logK values for all T,P pairs and reactions"
          "\n#    will be calculated based on the initial values of all parameters, and this logK array"
          "\n#    will be used throughout the fitting process. " << endl;
    prarCH.writeArrayFromBson(  "logK", obj, 0, true );

    if(_comment )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>> Input for fitting GEM input parameters >>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }

    TPrintArrays  prarIPM( 70, MULTI_dynamic_fields, ff);

    if( _comment )
         ff << "\n# Initial data for multicomponent phases (fragment of GEMS3K *IMP.dat input file)"
               "\n#    (see the DCH file for the dimension nPHs)";
    prarIPM.writeArrayFromBson(  f_sMod, obj, 8L, _comment );

    //long int LsModSum;
    //long int LsIPxSum;
    //long int LsMdcSum;
    //long int LsMsnSum;
    //long int LsSitSum;
    //node->pMulti()->getLsModsum( LsModSum, LsIPxSum );
    //node->pMulti()->getLsMdcsum( LsMdcSum, LsMsnSum, LsSitSum );

    prarIPM.writeArrayFromBson(  f_LsMod, obj, 3L, _comment);

    //if(LsIPxSum )
      {
       if( _comment )
          ff << "\n\n# IPxPH: Index lists (in TSolMod convention) for interaction parameters of non-ideal solutions";
       prarIPM.writeArrayFromBson(  "IPxPH", obj,  10L/*LsIPxSum*/, true);
      }
    //if(LsModSum )
       {
         if( _comment )
            ff << "\n\n# PMc: Tables (in TSolMod convention) of interaction parameter coefficients for non-ideal solutions";
        prarIPM.writeArrayFromBson(  "PMc", obj,  10L/*LsModSum*/, true );
       }

       prarIPM.writeArrayFromBson(  f_LsMdc, obj, 3L, _comment);
     //  if(LsMdcSum )
       {   if( _comment )
              ff << "\n\n# DMc: Tables (in TSolMod convention) of  parameter coefficients for Dependent Components";
        prarIPM.writeArrayFromBson(  "DMc", obj,  10L/*LsMdcSum*/);
       }
       //if(LsMsnSum )
       {   if( _comment )
              ff << "\n\n# MoiSN:  End member moiety / site multiplicity number tables (in TSolMod convention) ";
        prarIPM.writeArrayFromBson(  "MoiSN", obj,  10L/*LsMsnSum*/, true);
       }
    prarIPM.writeArrayFromBson(  f_fDQF, obj, -1L, _comment, 0 );


    if( _comment )
       ff << "\n\n# This part for the system composition data is taken from the *DBR.dat file\n";

    // from *DBR.dat
    TPrintArrays  prar(f_bSP+1/*52*/, DataBR_fields, ff);


    if( _comment )
      ff<< "\n# For fitting T and P parameters in thermobarometry application, give the upper and lower values"
         "\n#   corresponding to the interpolation range that you selected when exporting GEMS3K input files ";

    prar.writeArrayFromBson(f_TK, obj, 0, _comment, true  );
    prar.writeArrayFromBson(f_P, obj, 0, _comment, true  );
    if( _comment )
     {   ff << "\n\n## (4) Data for Independent Components";
         //prar.writeArray(  NULL, CSD->ICNL[0], CSD->nIC, MaxICN );
     }
    prar.writeArrayFromBson(  f_bIC,  obj, -1L,_comment, true );
    if( _comment )
    {    ff << "\n\n## (5) Data for Dependent Components";
         //prar.writeArray(  NULL, CSD->DCNL[0], CSD->nDC, MaxDCN );
     }
    prar.writeArrayFromBson(  f_xDC,  obj, -1L,_comment, true  );
    prar.writeArrayFromBson(  f_dll,  obj, -1L,_comment, true  );
    prar.writeArrayFromBson(  f_dul,  obj, -1L,_comment, true  );
    if( _comment )
    {    ff << "\n\n## (6) Data for Phases";
         // prar.writeArray(  NULL, CSD->PHNL[0], CSD->nPH, MaxPHN );
    }
    prar.writeArrayFromBson(  f_aPH,  obj, -1L,_comment, true );

    prar.writeArrayFromBson(f_Vs, obj, -1L,_comment, true  );
    prar.writeArrayFromBson(f_Ms, obj, -1L,_comment, true  );
    prar.writeArrayFromBson(f_Gs, obj, -1L,_comment, true  );
    //if( CSD->ccPH[0] == PH_AQUEL )
    {
       prar.writeArrayFromBson(f_IS, obj, -1L,_comment, true  );
       prar.writeArrayFromBson(f_pH, obj, -1L,_comment, true  );
       prar.writeArrayFromBson(f_pe, obj, -1L,_comment, true  );
       prar.writeArrayFromBson(f_Eh, obj, -1L,_comment, true  );
    }
    if( _comment )
     {
         ff << "\n\n## (4) Data for Independent Components";
         //prar.writeArray(  NULL, CSD->ICNL[0], CSD->nIC, MaxICN );
     }
    prar.writeArrayFromBson(  f_uIC,  obj, -1L,_comment, true );
    prar.writeArrayFromBson(  f_bSP,  obj, -1L,_comment, true );

    if( _comment )
    {    ff << "\n\n## (5) Data for Dependent Components";
         //prar.writeArray(  NULL, CSD->DCNL[0], CSD->nDC, MaxDCN );
    }
    //prar.writeArray(  f_xDC,  CNode->xDC, CSD->nDCb, -1L,_comment, brief_mode  );
    prar.writeArrayFromBson(  f_gam,  obj, -1L,_comment, true  );

    if( _comment )
    {    ff << "\n\n## (6) Data for Phases";
          //prar.writeArray(  NULL, CSD->PHNL[0], CSD->nPH, MaxPHN );
    }
    prar.writeArrayFromBson(  f_xPH,  obj, -1L,_comment, true );
    prar.writeArrayFromBson(  f_vPS,  obj, -1L,_comment, true );
    prar.writeArrayFromBson(  f_mPS,  obj, -1L,_comment, true );
    prar.writeArrayFromBson(  f_xPA,  obj, -1L,_comment, true );

    if( _comment )
     {
       ff << DataBR_fields[f_bPS].comment.c_str();
       // prar.writeArray(  NULL, CSD->ICNL[0], CSD->nIC, MaxICN );
     }
    prar.writeArrayFromBson(  f_bPS,  obj, 10L, false, true );
}


/// Mode GEMSFIT to generate input configuration file
int generateTxtfromBson( string gemsfitfile, bson *obj, bool with_comments )
{
   fstream ff(gemsfitfile.c_str(), ios::out );
   ErrorIf( !ff.good() , gemsfitfile.c_str(), "OptParamFile text open error");

   // Writing Data sources section
   out_db_specs_txt_bson( ff, obj, with_comments );

   // Writing Parameters to Fit section &
    out_gems_fit_txt_bson( ff, /*node,*/ obj, with_comments  );

   // Writing Optimization Methods section
    out_nlopt_param_txt_bson(ff, obj, with_comments );

   // Writing statistics section
    out_stat_param_txt_bson( ff, obj, with_comments );

    return 0;
}

outField Data_Manager_fields[9] =
{
    { "MPI", 0, 0, 1, "\n# MPI: Number of threads for parallelization\n" },
    { "DataDB",  0, 0, 1, "\n# DataDB: EJDB database path (please, edit to put the actual path)\n" },
    { "DataCollection",  0, 0, 1, "\n# DataCollection: database collection name (please, edit to put the actual name)\n" },
    { "DataSource",  1, 0, 1, "\n# DataSource: get experimental data from the EJDB format database"
      "\n#     (default:0, no other sources yet implemented in GEMSFIT2)\n" },

    { "DataSelect", 0, 0, 1, "# DataSelect: query for obtaining the experimental data from the database."
      "\n# Options: "
      "\n#      leave empty ot { } to select all data in the database"
      "\n#    \'{ ... }\': script in JSON format (in braces) describing what experimental data to select. "
      "\n#      \"usedatasets\": [...]: list of comma-separated names of experimental datasets to be used, "
      "\n#         or empty string \"\" to select all available datasets."
      "\n#      \"skipdatasets\": [...]: list of comma-separated names of experimental datasets to be skipped, "
      "\n#         or empty string \"\" to not skip (in this case, \"usedatasets\" will apply)."
      "\n#      \"usesamples\": [...]: list of comma-separated names of samples, or empty string \"\" "
      "\n#         to select all available samples. Note that samples with the same name can be present "
      "\n#         in two or more different experimental datasets."
      "\n#      \"skipsamples\": [...]: list of comma-separated names of samples to be skipped, "
      "\n#         or empty string \"\" to not to skip (in this case, \"usesamples\" will apply)."
      "\n#      \"usepairs\": [...]: list of pairs each consisting of an experimental dataset name and a list"
      "\n#         of comma-separated sample names to be taken from this dataset only; or an empty string \"\" "
      "\n#         for other options to apply."
      "\n#      \"skippairs\": [...]: list of pairs each consisting of an experimental dataset name and a list"
      "\n#         of comma-separated sample names to be skipped from this dataset only; or an empty string \"\" "
      "\n#         for other options to apply."
      "\n#      \"sT\": [...]: list of comma-separated minimum and maximum temperature, C, "
      "\n#         or empty string \"\" to select data for all available temperatures. "
      "\n#      \"sP\": [...]: list of comma-separated minimum and maximum pressures, bar, "
      "\n#         or empty string \"\" to select data for all available pressures.  "
      "\n#      Example: "
      "\n#      \'{ \"usedatasets\" : [\"CH04\", \"CH04D\"],"
      "\n#          \"skipdatasets\" : [\"CH03\", \"CH03D\"],"
      "\n#          \"usesamples\" : \"\", "
      "\n#          \"skipsamples\" : \"\", "
      "\n#          \"usepairs\" : \"\", "
      "\n#          \"skippairs\" : [ "
      "\n#                            { \"dataset\": \"CHO4\", \"samples\": [\"01\", \"02\"]}, "
      "\n#                            { \"dataset\": \"CHO4D\", \"samples\": [\"01\", \"02\"]}, "
      "\n#                          ], "
      "\n#          \"sT\" : [100, 1000],"
      "\n#          \"sP\" : [1, 2500] }\'"
      "\n#      Copy the example and paste it below, remove comment symbols (#), and edit as necessary."
      "\n#      Any main query option that has no value (empty string \"\") can be left out. In this example,"
      "\n#        the lines for \"usesamples\", \"skipsamples\", and \"usepairs\" can be left out. \n"
    },

    { "DataTarget",  0, 0, 1, "# DataTarget: Target function for parameter fitting, described in JSON style"
      "\n#     as text in single quotes and braces: \'{ ... }\' "
      "\n#  \"Target\":  name of the target (objective) function (optional)"
      "\n#  \"TT\":  type of the target function as sum of the following terms: "
      "\n#     \"lsq\":       w*(measured-simulated)^2;"
      "\n#     \"lsq-norm\":  w*(measured/avgm-simulated/avgm)^2;"
      "\n#      ... "
      "\n#      here, avgm is the arithmetic mean of measured values."
      "\n#  \"WT\":  weighting scheme for samples, one of the following options:"
      "\n#       empty string \"\": w = 1 (equal weights);"
      "\n#      \"inverr\": w=1/error; \"inverr2\": w=1/error^2; \"inverr3\": w=1/measured^2;"
      "\n#      \"inverr_norm\": w=1/(error/avgm)^2; "
      "\n#       ... "
      "\n#  \"OFUN\": objective function, a list [] of terms {} for measured properties to compare"
      "\n#     with their computed counterparts. Each term can contain property-value pairs:"
      "\n#      \"EPH\": for what phase from the experiments"
      "\n#      \"CT\":  for the type of data to compare:"
      "\n#          \"IC\" for independent component; \"DC\" for dependent component; "
      "\n#          \"prop\" for phase property; \"MR\" for mole fraction; "
      "\n#      \"CN\":  the name/formula of the data to compare. e.g. \"Al\" if \"CT\" is \"IC\","
      "\n#          or \"pH\" if \"CT\" is \"prop\", or \"K/(Na+K)\" if \"CT\" is \"MR\" "
      "\n#      \"DCP\": used only if \"CT\" is \"DC\", to represent the name of dependent component property:"
      "\n#          \"Q\" for amount; \"@coef\" for activity coeficient"
      "\n#      \"WT\": weight assigned to the objective function as real number e.g. \"WT\": 100. "
      "\n#      \"unit\":  units of measurement (override those given in the database for this value):"
      "\n#          \"molal\":  mol/(kg H2O), \"logm\": log(molal), \"-loga\": negated log(activity) for pH;"
      "\n#          \"g\"; \"kg\"; \"cm3\"; \"m3\"; \"molfrac\": mole fraction; J/mol for Gex "
      "\n#           ... (conversions will be performed automatically).\n"
      "\n#  \"NFUN\": nested objective function, a list [] of terms {} for measured properties to compare"
      "\n#    with their computed counterparts for each experiment independently of \"OFUN\" for adjusting"
      "\n#    a input property \"Ptype\" to obtain an output value given by \"EPH\", \"CT\" and \"CN\" pairs."
      "\n#       \"Ptype\": can have a value \"bIC\" or \"T-P\". The former refers to ajusting some parts of"
      "\n#         the bulk composition of the system representing the sample. The latter refers to adjusting"
      "\n#         the temperature, the pressure, or both."
      "\n#    IMPORTANT: When \"NFUN\" is specified in DataTarget section, at least one element in \"Ptype\" "
      "\n#         property object must be marked for fitting. More than one NFUN entries with different "
      "\n#         \"Ptype\" input properties can be used with caution.\n "
      "\n#     The comparison options are: "
      "\n#       aqueous phase (\"aq_gen\") elemental composition in \"molal\" or \"loga\" "
      "\n#       aqueous phase (\"aq_gen\") properties (\"prop\"): \"pH\" in \"-loga\" (or \"molal\" "
      "\n#          - molality concentration of H+); \"Q\" - mass in \"g\" or \"kg\"  "
      "\n#       other phases composition as element bulk amount in moles (\"mol\") "
      "\n#          or to /Si molar ratio (\"Simolfrac\")"
      "\n#       other phases properties (\"prop\"): \"Q\" - mass in \"g\" or \"kg\"; "
      "\n#          \"pV\" - volume in \"cm3\" or \"m3\";  \"Eh\" - volume in \"Volts\"; "
      "\n#          \"Gex\"  - excess Gibbs energy of mixing (in \"J/mol\"). "
      "\n#       dependent components (\"DC\") properties: \"Q\" - amount in \"mol\"; "
      "\n#          \"@coef\" - activity coefficient < add activity and mole fraction >"
      "\n#           ..."
      "\n#  Example of target (objective) function definition:"
      "\n#  \'{ \"Target\": \"name\", \"TT\": \"lsq\", \"WT\": \"inverr\", \"OFUN\":"
      "\n#      ["
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Si\", \"unit\": \"molal\" },"
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Al\", \"unit\": \"molal\" },"
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"prop\", \"CN\": \"pH\", \"unit\": \"-loga\" },"
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"DC\", \"CN\": \"SiO2@\", \"DCP\" : \"Q\", \"unit\": \"mol\"}"
      "\n#      ]"
      "\n#   }\'"
      "\n# "
      "\n#  Example of a target function with a nested objective function to titrate for a given pH:"
      "\n#  \'{ \"Target\": \"name\", \"TT\": \"lsq\", \"WT\": \"inverr\", \"OFUN\":"
      "\n#      ["
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Al\", \"unit\": \"logm\" },"
      "\n#      ], \"NFUN\": "
      "\n#      ["
      "\n#         { \"Ptype\": \"bIC\", \"EPH\": \"aq_gen\", \"CT\": \"prop\", \"CN\": \"pH\", \"unit\": \"-loga\" } "
      "\n#      ]"
      "\n#   }\'"
      "\n#  Copy one of the examples, paste it below, remove the comment symbols (#), and edit as necessary. "
      "\n#     Remove unnecessary rows. Note that in the example for nested function, at least one element in"
      "\n#     the \"bIC\" vector should be marked as freely adjustable parameter 'F'. \n"
    },
    { "SystemFiles",  0, 0, 1, "\n# SystemFiles: path to the list of GEMS3K input files (also used for this template file)\n" },
//    { "RecipeFiles",  0, 0, 1, "\n# RecipeFiles: Comment"},
    { "LimitOfDetection",  0, 0, 1, "\n# LimitOfDetection: Limit of detection of the measured values."
      "\n#     Ensures that wrongly computed output values due to non-physical input parameter values are ignored.\n" }
};

typedef enum { /// Field index into outField structure
    f_MPI = 0,
    f_DataDB,
    f_DataCollection,
    f_DataSource,
    f_DataSelect,
    f_DataTarget,
    f_SystemFiles,
//    f_RecipeFiles,
    f_LimitOfDetection
} Data_Manager_FIELDS;


/// Writes structure to the GEMSFIT2 input specification file
void out_db_specs_txt_bson( fstream& ff, bson *obj, bool with_comments  )
{

    TPrintArrays  prar(10, Data_Manager_fields, ff);
    // define data that must be written

    if(with_comments )
    {
        ff << "#########################################################################" << endl;
        ff << "#>>>>>>>>>>> GEMSFIT2 input specification file template >>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
        ff << "#  Character '#' means a comment line ";
    }

    prar.writeArrayFromBson(f_MPI, obj, 0L, with_comments  );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>>>> Data Sources section >>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################";
    }
    prar.writeArrayFromBson(f_DataSource, obj, 0L, with_comments  );
    prar.writeArrayFromBson(f_DataDB, obj, 0L, with_comments  );
    prar.writeArrayFromBson(f_DataCollection, obj, 0L, with_comments  );
    prar.writeArrayFromBson(f_SystemFiles, obj, 0L, with_comments  );
//    prar.writeField(f_RecipeFiles, "", with_comments, brief_mode  );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>>> DataSelect section >>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
    prar.writeArrayFromBson(f_DataSelect,  obj, 0L, with_comments );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>>> DataTarget section >>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
    prar.writeArrayFromBson(f_DataTarget,  obj, 0L, with_comments );
    prar.writeArrayFromBson(f_LimitOfDetection,  obj, 0L, with_comments  );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>> ParameterMarkup section >>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
}

//-----------------------------------------------------------------------------------------

outField statistics_fields[4] =
{
    { "StatMCbool",  0, 0, 1, "\n# StatMCbool: perform Monte Carlo runs -> no (0) "
                              "\n#             yes (1) scatter added to (ideal) computed values | yes (2) scatter added to measured values "},
    { "StatMCruns",  0, 0, 1, "\n# StatMCruns: number of Monte Carlo runs for confidence interval generation"},
    { "StatSensitivity",  0, 0, 1, "\n# StatSensitivity: number of evaluations points per parameter for sensitivity evaluation"},
    { "StatPerturbator", 0, 0, 1, "\n# StatPerturbator: used for calculating sensitivities by central diference, see ref [2]"}
};

typedef enum {  /// Field index into outField structure
    f_StatMCbool = 0,
    f_StatMCruns,
    f_StatSensitivity,
    f_StatPerturbator
} statistics_FIELDS;


///// Writes  structure to  the GEMSFIT configuration file
void out_stat_param_txt_bson( fstream& ff, bson *obj, bool with_comments )
{
    TPrintArrays  prar(4, statistics_fields, ff);
    prar.writeArrayFromBson(f_StatMCbool, obj, 0L, with_comments  );
    prar.writeArrayFromBson(f_StatMCruns, obj, 0L, with_comments  );
    prar.writeArrayFromBson(f_StatSensitivity, obj, 0L, with_comments );
    prar.writeArrayFromBson(f_StatPerturbator, obj, 0L, with_comments  );

    if(with_comments )
    {
        ff << endl << endl;
        ff << "#########################################################################" << endl;
        ff << "#>>>>>>>>>> end of GEMSFIT2 input specification file template >>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
}

////-------------------------------------------------------------------------------------------------

outField optimization_fields[25] =
{
    { "OptAlgo",  0, 0, 1, "\n# OptAlgo: specify algorithm: GN_ISRES | GN_ORIG_DIRECT | GN_ORIG_DIRECT_L | LN_COBYLA | LN_BOBYQA "},
    { "OptThreads",  0, 0, 1, "\n# OptThreads: Comment"},
    { "OptUpBounds",  0, 0, 1, "\n# OptUpBounds: Comment"},
    { "OptLoBounds",  0, 0, 1, "\n# OptLoBounds: Comment"},
    { "OptTolRel",  0, 0, 1, "\n# OptTolRel: stopping criterion -> specify relative tolerance (default = 1e-04) of function value"},
    { "OptMaxEval",  0, 0, 1, "\n# OptMaxEval: specify max number of evaluations"},
    { "OptUpConstraints",  0, 0, 1, "\n# OptUpConstraints: specify parameter vectors for constraint function"},
    { "OptLoConstraints",  0, 0, 1, "\n# OptLoConstraints: Comment"},
    { "OptConstraints",  0, 0, 1, "\n# OptConstraints:  Optimization: apply constraints (1=yes, 0=no)"},
    { "OptDoWhat",  0, 0, 1, "\n# OptDoWhat: perform optimization and statistics (0); only optimization with basic Statistics (1);"
                             "\n#            only Statistics (2) with initial guesses as best fit parametters"},
    { "OptTitration",  0, 0, 1, "\n# OptTitration: Adjusts the computed pH by changing NaOH or HCl amount to match the measured pH"
                                "\n#               read from the database for each experiment"},
    { "OptTuckey",  0, 0, 1, "\n# OptTuckey: (1) Use Tuckey Biweight for all data. (2) Use Tuckey Biweight for each OFUN independently. "},
    { "OptUserWeight",  0, 0, 1, "\n# OptUserWeight: (1) Use the weights provided in the \"weight\" column of the database. "},
    { "OptTolAbs",  0, 0, 1, "\n# OptTolAbs: stopping criterion -> specify absolute tolerance (default = 1e-04) of function value"},
    { "OptHybridTolRel",  0, 0, 1, "\n# OptHybridTolRel: Comment"},
    { "OptHybridTolAbs",  0, 0, 1, "\n# OptHybridTolAbs: Comment"},
    { "OptHybridMaxEval",  0, 0, 1, "\n# OptHybridMaxEval: Comment"},
    { "OptEquilibrium",  0, 0, 1, "\n# OptEquilibrium: (1) Use full GEMS3K to calculate thermodynamic equilibrium. (0) Use TSolMod shortcut "
                                  "\n#                  without calculating equilibrium (only fitting activity model parameters)"},
    { "OptNmultistart",  0, 0, 1, "\n# OptNmultistart: Comment"},
    { "OptPerturbator",  0, 0, 1, "\n# OptPerturbator: The delta/difference used to to calculate the d(function_value)/d(parameter_value) gradient"},
    { "OptInitStep",  0, 0, 1, "\n# OptInitStep: specify initial stepsize for local minimizers "
                               "\n#              (factor will be multiplied to all optimization parameters); 0 => use default"},
    { "OptScaleParam",  0, 0, 1, "\n# OptScaleParam: Comment"},
    { "OptNormParam",  0, 0, 1, "\n# OptNormParam: Normalize bounds/constraints/fitting parameters with the initial guess vector"},
    { "OptBoundPerc",  0, 0, 1, "\n# OptBoundPerc: Generate bounds from initial guess vector: specify percentage deviation "
                                "\n#               (user-specific, user-defined bounds when set to -1)"}
};

typedef enum {  /// Field index into outField structure
    f_OptAlgo= 0,
    f_OptThreads,
    f_OptUpBounds,
    f_OptLoBounds,
    f_OptTolRel,
    f_OptMaxEval,
    f_OptUpConstraints,
    f_OptLoConstraints,
    f_OptConstraints,
    f_OptDoWhat,
    f_OptTitration,
    f_OptTuckey,
    f_OptUserWeight,
    f_OptTolAbs,
    f_OptHybridTolRel,
    f_OptHybridTolAbs,
    f_OptHybridMaxEval,
    f_OptEquilibrium,
//    f_OptHybridMode,
    f_OptNmultistart,
    f_OptPerturbator,
    f_OptInitStep,
    f_OptScaleParam,
    f_OptNormParam,
    f_OptBoundPerc

} optimization_FIELDS;


///// Writes  structure to  the GEMSFIT configuration file
void out_nlopt_param_txt_bson( fstream& ff, bson *obj, bool with_comments )
{
    TPrintArrays  prar(24, optimization_fields, ff);
    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>> OptimizationMethods section >>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }

    prar.writeArrayFromBson( f_OptDoWhat,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptEquilibrium,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptUserWeight,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptTuckey,  obj, 0L, with_comments);
//    prar.writeField( f_OptTitration,  (long int)OptTitration, with_comments, brief_mode);
    prar.writeArrayFromBson( f_OptAlgo,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptBoundPerc,  obj, 0L, with_comments );
    prar.writeArrayFromBson( f_OptTolRel,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptTolAbs,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptMaxEval,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptInitStep,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptNormParam,  obj, 0L, with_comments);
    prar.writeArrayFromBson( f_OptPerturbator, obj, 0L, with_comments);

}

//-----------------------------------------------------------------------------------------------------
// from file # include "csvtoejdb.cpp"
using namespace keys;


// keeps one row of the CSV file to bson structure
// keeps headline of the CSV file

void csvToBson( bson *exp, const  vector<string>& headline, const vector<string>& row )
{
    int is=0,       // counts the number of processed samples (.csv data rows)
        ic=0,       // counts the number of system comp (recipe entries), independent components per pahse, phase properties per pahse, and dependent components properties per dependent component
        phc = 0,    // counts the number of phases per system/experiment
        dcc = 0,    // counts the number of dependent components per phase
        sk = 0,     // counts the number of metastability constraints per constraint type
        mf = 0;     // counts the number of molar fraction entries per system/exmperiment
    string ph_new, ph_old, sss;
    vector<string> phases, dcomps; // keeps the already read phases and dcomps
    stringstream ss;
    bool h_phprop = false, h_phases = false, h_phIC = false, h_dcomp = false, h_UMC= false,
         h_LMC = false, h_phMR = false, h_phDC = false; // handle that is true if we have the entry in the CSV file

    //string line;

   // getting the data from CSV line by line and processing it into BSON
    {
        is++;
        ic = 0;
        phc = 0;
        dcc = 0;
        // going trough the headline markers to identify the data type, based on which it is assigned into the database
        bson_init(exp);

        //first level objects: sample, expdataset, sT, sP.
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            // check for U and L metastability constraint columns
            if ((strncmp(headline[i].c_str(),LMC, strlen(LMC)) == 0))
            {
                h_LMC = true;
            }
            if ((strncmp(headline[i].c_str(),UMC, strlen(UMC)) == 0))
            {
                h_UMC = true;
            }
            if ((headline[i]==expsample) || (headline[i]==expdataset) || (headline[i]==Tunit) || (headline[i]==Punit)|| (headline[i]==Vunit) || (headline[i]==Type))
            {
                bson_append_string(exp, headline[i].c_str(), row[i].c_str());
                // for query
                if (headline[i]==expdataset)
                {
                    if (row[i].empty()) { Error( "Loading csv file", "expdataset column cannot be emppty. Exiting..." );}
                }
                if (headline[i]==expsample)
                {
                    if (row[i].empty()) { Error( "Loading csv file", "sample (sample name) column cannot be emppty. Exiting...");}
                } // end for query
            }
            else if  ((headline[i]==sT) || (headline[i]==sP))
            {
                if (row[i].empty()) { Error( "Loading csv file", "P or T columns cannot be emppty. Exiting...");}
                bson_append_double(exp, headline[i].c_str(), atof(row[i].c_str()));
            }
            else if (headline[i]==sV)
            {
                bson_append_double(exp, headline[i].c_str(), atof(row[i].c_str()));
            } else if (headline[i] == Weight)
            {
                if (!row[i].empty())
                {
                    bson_append_double(exp, headline[i].c_str(), atof(row[i].c_str()));
                } else bson_append_double(exp, headline[i].c_str(), 1.0);
            }
         }

         // 2nd level - upper and lower metastability restrictions
        if (h_UMC)
        {
            bson_append_start_array(exp, UMC);
            for (unsigned int i=0; i<headline.size(); ++i)
            {
                if ((strncmp(headline[i].c_str(),UMC, strlen(UMC)) == 0) && (!row[i].empty()))
                {
                    int pos_start, pos_end;
                    string component, type;
                    string f1(".");

                    // getting the name and the type: dependent component (DC) or phase
                    pos_start = headline[i].find(f1);
                    pos_end   = headline[i].find(f1,pos_start+1);
                    type      = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                    component = headline[i].substr((pos_end+f1.length()),( headline[i].size() -1));

                    ss << sk;
                    sss = ss.str();
                    ss.str("");
                    bson_append_start_object(exp, sss.c_str());
                    sk++;
                    if (type == DC) // dependent component
                    bson_append_string(exp, DC, component.c_str());
                    else
                        if (type == phase) // phase
                            bson_append_string(exp, phase, component.c_str());

                    bson_append_double(exp, Qnt, atof(row[i].c_str()));

                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if (i+1 < headline.size())
                    {
                        if ((headline[i+1]==Qerror))
                        {
                            ++i;
                            if ((!row[i].empty()))
                            {
                            bson_append_double(exp, Qerror, atof(row[i].c_str()));
                            }
                        }
                        if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                        {
                            ++i;
                            bson_append_string(exp, Qunit, row[i].c_str());
                        }
                    }
                    bson_append_finish_object(exp);
                }
            }
            //++ END array UMC ++//
            bson_append_finish_array(exp);
            sk=0;

        }

        if (h_LMC)
        {
            bson_append_start_array(exp, LMC);
            for (unsigned int i=0; i<headline.size(); ++i)
            {
                if ((strncmp(headline[i].c_str(),LMC, strlen(LMC)) == 0) && (!row[i].empty()))
                {
                    int pos_start, pos_end;
                    string component, type;
                    string f1(".");

                    // getting the name and the type: dependent component (DC) or phase
                    pos_start = headline[i].find(f1);
                    pos_end   = headline[i].find(f1,pos_start+1);
                    type      = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                    component = headline[i].substr((pos_end+f1.length()),( headline[i].size() -1));

                    ss << sk;
                    sss = ss.str();
                    ss.str("");
                    bson_append_start_object(exp, sss.c_str());
                    sk++;
                    if (type == DC) // dependent component
                    bson_append_string(exp, DC, component.c_str());
                    else
                        if (type == phase) // phase
                            bson_append_string(exp, phase, component.c_str());
                    bson_append_double(exp, Qnt, atof(row[i].c_str()));

                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if (i+1 < headline.size())
                    {
                        if ((headline[i+1]==Qerror))
                        {
                            ++i;
                            if ((!row[i].empty()))
                            {
                            bson_append_double(exp, Qerror, atof(row[i].c_str()));
                            }
                        }
                        if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                        {
                            ++i;
                            bson_append_string(exp, Qunit, row[i].c_str());
                        }
                    }
                    bson_append_finish_object(exp);
                }
            }
            //++ END array LMC ++//
            bson_append_finish_array(exp);
            sk=0;
        }

        // 2nd level - bulk composition of chemical system for the current experiment
        // array of components
        //++ START array sbcomp ++//
        bson_append_start_array(exp, sbcomp);
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),comp, strlen(comp)) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string component;
                string f1(".");

                // getting the name of the component e.g. SiO2 form comp.SiO2
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
                component = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                ss << ic;
                sss = ss.str();
                ss.str("");
                bson_append_start_object(exp, sss.c_str());
                ic++;
                bson_append_string(exp, comp, component.c_str());
                bson_append_double(exp, Qnt, atof(row[i].c_str()));

                // checking if there are errors and units included in the CSV and adding tem in the database
                if (i+1 < headline.size())
                {
                    if ((headline[i+1]==Qerror))
                    {
                        ++i;
                        if ((!row[i].empty()))
                        {
                        bson_append_double(exp, Qerror, atof(row[i].c_str()));
                        }
                    }
                    if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(exp, Qunit, row[i].c_str());
                    }
                }
                bson_append_finish_object(exp);
            }
        }
        //++ END array sbcomp ++//
        bson_append_finish_array(exp);
        ic=0;

        // 2nd level - data for phases charactrised/measured in this experiment
        //++ START array expphases ++//
        bson_append_start_array(exp, expphases);
        // going trough the headline and searching for "phase" keword
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),phase, strlen(phase)) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string phase_name, ph_prop, ph_prop_1, ph_prop_2, ph_prop_3;
                string f1(".");

                // getting the name of the phase e.g. aq_gen form phase.aq_gen
                pos_start  = headline[i].find(f1);
                pos_end    = headline[i].find(f1,pos_start+1);
                phase_name = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                ph_new = phase_name;
                h_phases = false;

                // only if we have a new pahse name we add new pahse name string, as pahse names can appear multiple times as keys in the headline
                if ((ph_new != ph_old))
                {
                    // check if pahse name was not present before
                    for (unsigned int j=0; j<phases.size(); ++j)
                    {
                        if (phase_name == phases[j])
                        {
                            h_phases = true;
                        }
                    }

                    if (!h_phases) // START if h_phases. Every phase is scaned only once troughout out the headline.
                        // The program goes trough this if only at the first enocunter of the phase name and scans the document for recuring of the phase.
                    {
                        ss << phc;
                        sss = ss.str();
                        ss.str("");
                        bson_append_start_object(exp, sss.c_str()); // START phase object
                        phc++;
                        bson_append_string(exp, phase, phase_name.c_str());
                        phases.push_back(phase_name); // vector that keeps already present phases
                        ic = 0;

                        // START check if there is phprop & phcomp data in the CSV
                        for (unsigned int j=0; j<headline.size(); ++j)
                        {
                            if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty())) // checks where in the headline the same pahse name is present
                            {
                                pos_start = headline[j].find(f1);
                                pos_end   = headline[j].find(f1,pos_start+1);
                                // getting the phase properties and composition, what follows fter phase name e.g. aq_gen.IC.Si.Q
                                if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                {
                                    // getting the name of the property phase e.g. IC.Si.Q from phase.aq_gen.IC.Si.Q, or Q from pahse.aq_gen.Q
                                    ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                                    pos_start = ph_prop_1.find(f1);
                                    pos_end   = ph_prop_1.find(f1,pos_start+1);
                                    ph_prop   = ph_prop_1.substr((0),(pos_start));

                                    // if property present
                                    if (((ph_prop == Qnt) || (ph_prop == pH) || (ph_prop == pV) ||  (ph_prop == Eh) || (ph_prop == IS) || (ph_prop == all) ||  (ph_prop == sArea) || (ph_prop == RHO) || (ph_prop == Gex) || (ph_prop == pe) || (ph_prop == oscw)) && (!row[j].empty()))
                                    {
                                        h_phprop = true;
                                    }
                                    // if composition present
                                    if (ph_prop == IC/*((ph_prop != pQnt) && (ph_prop != pH) && (ph_prop != pV) &&  (ph_prop != Eh) && (ph_prop != IS) && (ph_prop != all) &&  (ph_prop != sArea) && (ph_prop != RHO)) && (strncmp(ph_prop.c_str(),"Dc", 2) != 0) && (!row[j].empty())*/)
                                    {
                                        h_phIC = true;
                                    }
                                    if (ph_prop == MR)
                                    {
                                        h_phMR = true;
                                    }
                                    if (ph_prop == DC)
                                    {
                                        h_phDC = true;
                                    }
                                }
                            }
                        } // END check if there is phprop & phcomp data in the CSV

                        //++ START array phprop ++//
                        if (h_phprop)
                        {
                            bson_append_start_array(exp, phprop);
                            // get phase poperties
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase properties
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property
                                        ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                                        // amount of the property of the phase in the experiment
                                        if (((ph_prop == Qnt) || (ph_prop == pH) || (ph_prop == pV) ||  (ph_prop == Eh) || (ph_prop == IS) || (ph_prop == all) ||  (ph_prop == sArea) || (ph_prop == RHO) || (ph_prop == Gex) || (ph_prop == pe) || (ph_prop == oscw)) && (!row[j].empty()))
                                        {
                                            if (((ph_prop == pH) || (ph_prop == Eh) || (ph_prop == IS) || (ph_prop == all) || (ph_prop == pe) || (ph_prop == oscw)) && (phase_name != aqueous))
                                            {
                                                cout << ph_prop << " only works with aqueous phase (aq_gen)!"<< endl;
                                                exit(1);
                                            }

                                            ss << ic;
                                            sss = ss.str();
                                            ss.str("");
                                            bson_append_start_object(exp, sss.c_str()); // START property object
                                            ic++;
                                            bson_append_string(exp, property, ph_prop.c_str());
                                            bson_append_double(exp, Qnt, atof(row[j].c_str()));

                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                            if (j+1 < headline.size())
                                            {
                                                if ((headline[j+1]==Qerror))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if (j+1 < headline.size())
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(exp); // END property object
                                            ph_old = phase_name;
                                        }
                                    }
                                }
                            }
                            //++ END array phprop ++//
                            bson_append_finish_array(exp);
                            ic =0;
                        } h_phprop = false;

                        //++ START array phcomp IC ++//
                        if (h_phIC)
                        {
                            bson_append_start_array(exp, phIC);
                            // get phase comp
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase_name properties and composition from CSV
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property phase e.g. Si from phase.aq_gen.IC.Si.Q,
                                        ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));
                                        pos_start = ph_prop_1.find(f1);
                                        pos_end   = ph_prop_1.find(f1,pos_start+1);
                                        ph_prop_2 = ph_prop_1.substr((0),(pos_start));
                                        ph_prop   = ph_prop_1.substr((pos_start+1),(ph_prop_1.size()));
                                        pos_start = ph_prop_1.find(f1,pos_start +1);
                                        pos_end   = ph_prop_1.find(f1,pos_end+1);
                                        ph_prop_1 = ph_prop_1.substr((pos_start+1),(pos_start));

                                        ph_prop_3 = ph_prop;
                                        pos_start = ph_prop_3.find(f1,0);
                                        pos_end   = ph_prop_3.find(f1,pos_end+1);
                                        ph_prop   = ph_prop_3.substr((0),(pos_start));

                                        // qunatity of this comp in the phase
                                        if (((ph_prop != Qnt) && (ph_prop_1 == Qnt) && (ph_prop_2 == IC) && (ph_prop != pH) && (ph_prop != pV) &&  (ph_prop != Eh) && (ph_prop != IS) && (ph_prop != all) &&  (ph_prop != sArea) && (ph_prop != RHO) && (ph_prop != Gex) && (ph_prop != pe) && (ph_prop != oscw)) && (strncmp(ph_prop.c_str(),"Dc", 2) != 0) && (!row[j].empty()))
                                        {
                                            ss << ic;
                                            sss = ss.str();
                                            ss.str("");
                                            bson_append_start_object(exp, sss.c_str()); // START phase element object
                                            ic++;
                                            bson_append_string(exp, IC, ph_prop.c_str());
                                            bson_append_double(exp, Qnt, strtod(row[j].c_str(), NULL));
//                                            cout << strtod(row[j].c_str(), NULL) << endl;
//                                            cout << atof(row[j].c_str()) << endl;

                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                            if (j+1 < headline.size())
                                            {
                                                if ((headline[j+1]==Qerror))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if (j+1 < headline.size())
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(exp); // END phase element object
                                            ph_old = phase_name;
                                        }
                                    }
                                }
                            }
                            //++ END array phcomp IC ++//
                            bson_append_finish_array(exp);
                            ic = 0;
                        } h_phIC = false;

                        //++ START getting data reported as molar fraction
                        if (h_phMR)
                        {
                            bson_append_start_array(exp, phMR);
                            // get phase comp
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase_name properties and composition from CSV
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property phase e.g. Si from phase.aq_gen.IC.Si.Q,
                                        ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));
                                        pos_start = ph_prop_1.find(f1);
                                        pos_end   = ph_prop_1.find(f1,pos_start+1);
                                        ph_prop_2 = ph_prop_1.substr((0),(pos_start));
                                        ph_prop   = ph_prop_1.substr((pos_start+1),(ph_prop_1.size()));
                                        pos_start = ph_prop_1.find(f1,pos_start +1);
                                        pos_end   = ph_prop_1.find(f1,pos_end+1);
                                        ph_prop_1 = ph_prop_1.substr((pos_start+1),(pos_start));

                                        ph_prop_3 = ph_prop;
                                        pos_start = ph_prop_3.find(f1,0);
                                        pos_end   = ph_prop_3.find(f1,pos_end+1);
                                        ph_prop   = ph_prop_3.substr((0),(pos_start));

                                        if ((ph_prop_2 == MR) && (!row[j].empty()))
                                        {
                                            ss << mf;
                                            sss = ss.str();
                                            ss.str("");
                                            bson_append_start_object(exp, sss.c_str()); // START phase element object
                                            mf++;
                                            bson_append_string(exp, MR, ph_prop.c_str());
                                            bson_append_double(exp, Qnt, atof(row[j].c_str()));

                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                            if ((j+1) < headline.size())
                                            {
                                                if ((headline[j+1]==Qerror))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if (j+1 < headline.size())
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(exp); // END phase element object
                                            ph_old = phase_name;
                                        }
                                    }
                                }
                            }
                            //++ END array phMR ++//
                            bson_append_finish_array(exp);
                            mf = 0;
                        } h_phMR = false;


                        //++ START array phspecies ++//
                        if (h_phDC)/*((ph_prop_2 == DC) && (!row[i].empty()))*/ // check if there is species data in the CSV header
                        {
                            string dcomp_new, dcomp_old;
                            bson_append_start_array(exp, phDC);
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty())) // check for pahse. key
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase species
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()))) // check for phase name
                                    {
                                        ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));
                                        // species
                                        if ((strncmp(ph_prop.c_str(),"DC.", 1) == 0) && (!row[j].empty()))
                                        {
                                            string dcomp_name, ph_dcomp;

                                            ph_dcomp = "phase." + phase_name + ".";

                                            ph_prop = ph_prop.substr(3,ph_prop.length()); // deleting the "Dc" - species name

                                            // getting the name of the phase e.g. aq_gen form phase.aq_gen
                                            pos_start  = -1;
                                            pos_end    = ph_prop.find(f1);
                                            dcomp_name = ph_prop.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                                            dcomp_new  = dcomp_name;
                                            h_dcomp    = false;

                                            if ((dcomp_new != dcomp_old))
                                            {
                                                // check if dcomp name was not present before
                                                for (unsigned int j=0; j<dcomps.size(); ++j)
                                                {
                                                    if (dcomp_name == dcomps[j])
                                                    {
                                                        h_dcomp = true;
                                                    }
                                                }

                                                //++ START if h_dcomp
                                                if (!h_dcomp)
                                                {
                                                    string dcomp_prop;
                                                    ss << dcc;
                                                    sss = ss.str();
                                                    ss.str("");
                                                    //++ START species object
                                                    bson_append_start_object(exp, sss.c_str());
                                                    dcc++;
                                                    bson_append_string(exp, DC, dcomp_name.c_str());
                                                    dcomps.push_back(dcomp_name);
                                                    ph_dcomp += "DC." + dcomp_name + ".";
                                                    ic = 0;

                                                    // loop to get all the properties of current dcomp
                                                    bson_append_start_array(exp, DCprop);
                                                    for (unsigned int j=0; j<headline.size(); ++j)
                                                    {
                                                        if ((strncmp(headline[j].c_str(),ph_dcomp.c_str(), ph_dcomp.size()) == 0) && (!row[j].empty()))
                                                        {
//                                                            cout << j << endl;

                                                            pos_start  = ph_dcomp.size();
                                                            pos_end    = headline[j].size();
                                                            dcomp_prop = headline[j].substr(pos_start, pos_end);

                                                            ss << ic;
                                                            sss = ss.str();
                                                            ss.str("");
                                                            bson_append_start_object(exp, sss.c_str()); // START property object
                                                            ic++;
                                                            bson_append_string(exp, property, dcomp_prop.c_str());
                                                            bson_append_double(exp, Qnt, atof(row[j].c_str())); // quantity of the property

                                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                                            if (j+1 < headline.size())
                                                            {
                                                                if ((headline[j+1]==Qerror))
                                                                {
                                                                    ++j;
                                                                    if ((!row[j].empty()))
                                                                    {
                                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                                    }
                                                                }
                                                                if (j+1 < headline.size())
                                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                                {
                                                                    ++j;
                                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                                }
                                                            }
                                                            //++ END dcomp property object
                                                            bson_append_finish_object(exp);
                                                            dcomp_old = dcomp_name;
                                                        } // end search for dcomp name in the headline of the csv file
                                                    }
                                                    //++ END dcompprop array
                                                    bson_append_finish_array(exp);
                                                } //++ END if h_dcomp
                                            } // END comparison to check fo unique dcomps
                                            ph_old = phase_name;
                                        } // search for "DC" indicator of dependent component entry in the csv headline
                                    } //
                                    dcomps.clear();
                                }
                            }
                            //++ END array phspecies ++//
                            bson_append_finish_array(exp);
                        } h_phDC = false;
                        // end object in the array phases
                        bson_append_finish_object(exp); //++ END phase object ++
                    } // END if h_phases
                } // END ph_new != ph_old
            } // END check for key phase in the headline
        }
        //++ END array expphases ++//
        bson_append_finish_array(exp);
        bson_finish(exp);
     }

}



// ----------- End of  gemsfit_iobson.cpp ----------------------------
