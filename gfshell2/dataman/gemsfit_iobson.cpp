/*
*	 Copyright (C) 2013 by Dmytriyeva S. (gems2.support@psi.ch)
*    modified G. Dan Miron
*
*	 This file is part of the thermodynamic fitting program GEMSFIT2.
*
*    GEMSFIT2 is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT2 is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
using namespace std;
#include <cstdlib>


#include "node.h"
#include "io_arrays.h"
#include <omp.h>
#include <sstream>
#include <unistd.h>




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
    { "OptHybridMode",  0, 0, 1, "\n# OptHybridMode: Comment"},
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
    f_OptHybridMode,
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


// ----------- End of  visor.cpp ----------------------------
