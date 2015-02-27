#ifndef KEYWORDS_H
#define KEYWORDS_H

#pragma GCC diagnostic ignored "-Wunused-variable"

#include "string"
#include "vector"

namespace keys
{ // Keywords for the fitting task definition
//                                  Keyword long                       short
// ------------------------------  --------------------------------------------------------
//
static const char *DBPath[2]        = {"DatabasePath",                    "DBPath"};
static const char *DBColl[2]        = {"DataCollection",                  "DBColl"};
static const char *G3Ksys[2]        = {"GEMSInputFilesPath",              "GEMSFPath"};
static const char *DSelect[2]       = {"DataSelection",                   "DataSelect"};
static const char *DTarget[2]       = {"DataTargetFunction",              "DataTarget"};
// global parameters
static const char *OptParameters[2] = {"ParametersToOptimize",      "ParToOpt"};
static const char *G0[2]            = {"GibbsEnergyOfDependentComponent","G0"};
static const char *PMc[2]           = {"InteractionParametersInPhase",    "PMc"};
static const char *IPCs[2]          = {"InteractionParameterCoefficients","IPCs"};
static const char *IPCN[2]          = {"InteractionParameterCoefficientName","IPCN"};
static const char *DMc[2]           = {"ParametersOfDependentComponentsInPhase","DMc"};
static const char *PDCC[2]          = {"ParameterOfDependentComponentCoefficients","PDCC"};

static const char *IV[2]            = {"InitialValue","IV"}; //
static const char *UB[2]            = {"UpperBound","UB"}; //
static const char *LB[2]            = {"LowerBound","LB"}; //
static const char *ICN[2]           = {"IndependentComponentName","ICN"};
static const char *DCN[2]           = {"DependentComponentName",          "DCN"};
static const char *PType[2]         = {"ParameterType","PType"};

// nested function paramteters
static const char *NFndx[2]         = {"NestedFunctionIndex","NFndx"};
static const char *LICs[2]          = {"LinkedIndependentComponents","LICs"};
static const char *LCoef[2]         = {"LinkCoefficients","LCoef"};
static const char *OptNFParameters[2]= {"NestedFunctionParametersToOptimize","NFunParToOpt"};
static const char *bIC[2]           = {"BulkIndependentComponent","bIC"};
static const char *Pb[2]            = {"PressureBar","Pb"};
static const char *TK[2]            = {"TemperatureKelvin","TK"};

static const char *DatLogK[2]       = {"LogKFunctionData",                "LogKFun"};
static const char *LogK[2]          = {"ArrayofUserProvidedLogKValues",   "LogKVal"};

static const char *OptSet[2]        = {"OptimizerSettings",               "OptSet" };
static const char *MPI[2]           = {"NumberOfMPIThreadsForParallelRun", "NumMPI"};
// Propose to get rid of "Optimization" "Opt" for the group of keywords below
static const char *OptDW[2]         = {"WhatOptStatCombinationToDo",      "DoWhat"};
static const char *OptEQ[2]         = {"UseEquilibriumCalculation",       "UseEqC"};
static const char *OptUW[2]         = {"UseWeightsFromExperimentalData",  "UDWts"};
static const char *OptAlg[2]        = {"OptimizationAlgorithm",           "OptAlg"};
static const char *OptPBP[2]        = {"ParameterBoundsPercentage",       "PBPerc"};
static const char *OptTRel[2]       = {"ConvergenceToleranceRelative",    "ConTolRel"};
static const char *OptTAbs[2]       = {"ConvergenceToleranceAbsolute",    "ConTolAbs"};
static const char *OptMEv[2]        = {"MaximumNumberOptimizationLoops",  "MaxLoops"};
static const char *OptNormP[2]      = {"NormalizeAllParameters",          "NormPar"};
static const char *OptPer[2]        = {"GradientMethodPerturbatorValue",  "GradPert"};
static const char *OptTu[2]         = {"UseTuckeyBiweights",              "OptTu"};
static const char *OptTuVal[2]      = {"OptimizationTuckeyCValue",        "OptTuC"};

// Propose to drop "Statistics" "Stat" from the group of keywords below
static const char *StatMC[2]        = {"UseMonteCarloParameterEvaluation","UseMCPE"};
static const char *StatMCr[2]       = {"NumberOfMonteCarloTrials",        "NumMCT"};
static const char *StatPer[2]       = {"SensitivitiesPerturbatorValue",   "SensPert"};

static const char *DataSyn[2]       = {"DataSynonyms",                    "DataSyn"};
static const char *PhNames[2]       = {"PhaseNames",                      "PHN"};
static const char *PhPropNames[2]   = {"PhasePropertyNames",          "PHPN"};
static const char *DcNames[2]       = {"DependentComponentNames",         "DCN"}; // see "DCN" above ?
static const char *DcPropNames[2]   = {"DependentComponentPropertyNames","DCPN"};
static const char *NameSys[2]       = {"NameInGEMSystem",                 "NIS"};
static const char *Syn[2]           = {"Synonyms",                        "Syn"};


// General keywords used for quantity, error and unit
static const char *Qnt              = "Q";//                                                quantity
static const char *Qunit            = "unit";      //                                       unit
static const char *Qerror           = "error";    //                                        error

// Database structure keywords
// ---------------------------------------------------------------- Level-----Type------------Comment
static const char *experiments      = "experiments"; //               0    collection       data for experimental samples
static const char *expsample        = "sample"; //                    1	   string           ID of this experimental sample
static const char *samplelist       = "samplelist"; //
static const char *datasetlist      = "datasetlist";
static const char *SA               = "SA";
static const char *DS               = "DS";
static const char *samples          = "samples";
static const char *dataset          = "dataset";
static const char *usesamples       = "usesamples";
static const char *skipsamples      = "skipsamples";
static const char *skippair         = "skippairs";
static const char *usepair          = "usepairs";
static const char *expdataset       = "expdataset"; //                 1    string          ID of set of experimental data
static const char *usedatasets      = "usedatasets"; //                1    string		    ID of set of experimental data
static const char *skipdatasets     = "skipdatasets"; //               1	string		    ID of set of experimental data
static const char *usedataset       = "usedataset"; //                 1	string		    ID of set of experimental data
static const char *skipdataset      = "skipdataset"; //                1	string		    ID of set of experimental data
static const char *type             = "type";//
static const char *comment          = "comment";//
static const char *Weight           = "weight"; //                     1    double          artificial assigned weight
static const char *sT               = "sT"; //                         1	float		    temperature for this experimental sample
static const char *Tunit            = "Tunit"; //                      1    string		    units of temperature { 'C' (default) or 'K' or 'F' }
static const char *sP               = "sP"; //                         1    float		    pressure for this experimental sample
static const char *Punit            = "Punit"; //		               1    string		    units of pressure { 'bar' (default) or 'kPa' or 'MPa' or 'GPa' }
static const char *sV               = "sV"; //                         1    float		    (system) volume for this experimental sample
static const char *Vunit            = "Vunit"; //	                   1    string		    units of volume { 'cm3' (default) or 'dm3' or 'm3' }

    // Describes bulk composition of the system
static const char *sbcomp           = "sbcomp"; //     		           1    array		    defines bulk composition of chemical system for this experiment
    static const char *comp         = "comp"; //                       2    string		    formula defining PCO stoichiometry (GEM formula syntax)


    // Describes system phases
static const char *expphases        = "expphases"; //	               1    array		    data for phases characterised (measured) in this experiment
    static const char *phase        = "phase"; //	                   2	string		    phase ID (name)
        // Phase independent components
    static const char *phIC         = "phIC"; //	                   2   	array		    array of measured composition of the phase
        static const char *IC       = "IC";//                          3    string          name of independent component (element formula, e.g. Al)
    static const char *phMR         = "phMR";//                        2    array           array of reported measurments in molar ratio
        static const char *MR       = "MR"; //                         3    string          molar ratio formula
        // Phase dependent components
    static const char *phDC         = "phDC"; //                       2	array		    array of dependent components (end member, phase component)
        static const char *DC       = "DC"; //                         3    string          dependent component
        static const char *DCprop   = "DCprop"; //                     3    array           array of dependent components properties (Qnt, etc.)
              static const char *actcoef = "@coef"; //     	        ->4	    string		    name of peorperty - activity coef
              static const char *activity ="activity";
                        // with Qerror and Qunit
        // Phase properties
    static const char *phprop       = "phprop"; //                      2    array		    known bulk properties of the phase
        static const char *property = "prop"; //                        3    string         name of property category
        static const char *pH       = "pH"; //  	                    ->3  float		    name of property - pH (for aqueous phase only)
        static const char *Eh       = "Eh"; //                          ->3	 float		    name of property - Eh (for aqueous phase only)
        static const char *IS       = "IS"; //                          ->3	 float 		    name of property - Ionic strength (molal)
        static const char *all      = "alk"; //                         ->3	 float		    name of property - Alkalinity (aqueous phase only)
        static const char *pV       = "pV"; //                          ->3	 float		    name of property - volume of phase
        static const char *sArea    = "sArea"; //                       ->3  float		    name of property - specific surface area of the phase
        static const char *RHO      = "RHO"; //                         ->3  double         name of property - density of the phase
        static const char *Gex      = "Gex"; //                         ->3  double         name of property - excess Gibbs energy of mixing in the phase
        static const char *oscw     = "oscw"; //
        static const char *pe       = "pe"; //
        // Upper and Lower metastability constraints
        static const char *UMC      = "UMC"; //                         1    array          array of Upper metastability constraints
        static const char *LMC      = "LMC"; //                         1    array          array of Lower metastability constraints
        static const char *mChainL  = "mChainL"; //

// general keywords for error and unit present in the header of the input csv file
static const char *_error           = "error";
static const char *_unit            = "unit";

// kewords for adjusted parameters marked in the input file
// Reaction dependent G0 specific
static const char *Ref              = "Ref"; //                             string          reference for logK
static const char *nC               = "nC"; //                              integer         number of components involved in the reaction
static const char *RC               = "RC"; //                              array           array of components names
static const char *Rcoef            = "RCoef"; //                           array           array of reaction coeficients
static const char *Rndx             = "Rndx"; //                            integer         order of the reactions
static const char *Tforumla         = "Tformula"; //                        array           name of titrants
static const char *Telem            = "Telem"; //                           array           name of elements

// LogKFun
static const char *FunList          = "FunList";
static const char *Ftype            = "FType";
static const char *Fcoef            = "FCoef";

//keywords DatTarget
static const char *Target           = "Target"; //                           string          name of target function
static const char *TT               = "TT"; //                               string          target function type
static const char *WT               = "WT"; //                               string          weight type
static const char *OFUN             = "OFUN"; //                             array           Objective function
static const char *NFUN             = "NFUN"; //
static const char *ADDOUT           = "ADDOUT"; //
    static const char *SRC          = "SRC"; //
    static const char *calc         = "calc"; //
    static const char *meas         = "meas"; //
    static const char *CT           = "CT"; //                               string          type of component/data to compare
    static const char *CN           = "CN"; //                               string          name of data to compare e.g. Al, pH, etc.
    static const char *EPH[2]       = {"SystemPhaseName","EPH"};  //         string          experiment phase
    static const char *DCP          = "DCP"; //                              string          dependent comp property
    static const char *expr         = "expr";


// Name of Target Functions and weights
static const char *lsq              = "lsq"; //                              string          least square function (measured-computed)^2
static const char *lsq_norm         = "lsq_norm"; //                         string          normalized to experimental average, least square
static const char *abs_dif          = "abs_dif";//                           string          form nested optimization use abs_dif
static const char *inverr           = "inverr"; //                           string          inverse weight as 1/error
static const char *inverr2          = "inverr2"; //                          string          inverse weight as 1/error^2
static const char *inverr3          = "inverr3"; //                          string          inverse weight as 1/measured^2
static const char *inverr_norm      = "inverr_norm"; //                      string          normalized <Xmeas>/error

// Kaywords for units
static const char *molal            = "molal"; //                            string          molality moles/kg
static const char *loga             = "loga"; //                             string          log(activity)
static const char *logm             = "log_molal"; //                        string          log(molality)
static const char *_loga            = "-loga"; //                            string          -log(consntration) used for pH
static const char *gram             = "g"; //                                string          mass, grams
static const char *kgram            = "kg"; //                               string          mass, kg
static const char *mole             = "mole"; //                             string          amount, moles
static const char *m3               = "m3"; //                               string          volume, m3
static const char *cm3              = "cm3"; //                              string          volume, cm3
static const char *molratio         = "molratio"; //                         string          mole ratio
static const char *log_molratio     = "log_molratio";
static const char *molfrac          = "molfrac"; //                          string          mole fraction
static const char *log_molfrac      = "log_molfrac";
static const char *ln_molfrac       = "ln_molfrac";
static const char *g_cm3            = "g/cm3"; //                            string          density, g/cm3
static const char *Volts            = "Volts"; //                            string          Eh, V
static const char *J_mol            = "J/mol"; //                            string          energy per mole, J/mol
static const char *kJ_mol           = "kJ/mol"; //                           string          energy per mole, kJ/mol

static const char *aq = "a"; ///

// optimization specific
static const int bperc = 10; ///                                    integer         default up/lo bound percentage of the parameters

static const double mNaCl = 58.442308;
static const double mNaOH = 39.997115;
static const double mKCl = 74.550840;
static const double mKOH = 56.105647;
static const double lg_to_ln = 2.302585093;
static const double R_CONSTANT = 8.31451;



/// database not implemented yet
static const char *expdatarefs = "expdatarefs"; ///               0     collection 	    components (recipe in oxides; e.g. SiO2, H2O, Al2O3, etc.)
//const char *expdataset = "expdataset"; /// 		              1	    string		    ID of experimental dataset e.g. 'Kennedy1950',
static const char *Prange = "Prange"; ///		                  1	    struct   	    pressure range of experiments in a dataset
    static const char *Pmin = "Pmin"; ///	                      2	    float		    minimum pressure in the range
    static const char *Pmax = "Pmax"; ///	                      2	    float		    maximum pressure in the range
static const char *Vrange = "Vrange"; ///	                      1		struct		    volume range
    static const char *Vmin = "Vmin"; ///	                      2	    float
    static const char *Vmax = "Vmax"; ///	                      2	    float
static const char *Trange = "Trange"; ///	                      1		struct		    temparature range
    static const char *Tmin = "Tmin"; ///	                      2	    float
    static const char *Tmax = "Tmax"; ///	                      2	    float
static const char *reference = "reference"; ///	                  1	    string		    bib.reference(s) for the experimental dataset
//static const char *comment	= "comment"; ///	              1	    string		    comment
static const char *file = "file"; ///	                          1		string 		    file name(s), e.g. image, word,
static const char *url = "url"; ///	                              1		string 		    URL to source of experimental data on web
//----------------------------------------------------------------------------------------------------------------------------------------------------------




}
#endif // KEYWORDS_H
