//--------------------------------------------------------------------
// $Id: gemsfit_task.cpp 789 2012-12-19 17:08:55Z kulik $
//
/// \file gemsfit_task.cpp
/// Implementation of TGfitTask class functionality - advanced
/// interface between GEM IPM and GEMSFIT node array
/// working with one DATACH structure and arrays of DATABR structures
//
// Copyright (C) 2013 G.D.Miron, D.Kulik, S.V.Dmytrieva
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
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

#include <cmath>
#include <iomanip>
#include "gemsfit_task.h"
#include <GEMS3K/gdatastream.h>
#include "gemsfit_iofiles.h"
#include "keywords.h"
#include "gemsfit_global_functions.h"
#include "gemsfit_target_functions.h"

// Main function that calculates the residual
double TGfitTask::get_residual(int exp, TGfitTask::TargetFunction::obj_fun &objfun, int &count)
{
    double residual = 0.0;
    count = 0;

    // loop trough objective function
    /// Target function
    if ((objfun.exp_phase !="NULL") && (this->experiments[exp]->expphases.size() > 0) && (objfun.exp_phase !="prop"))
    {
        // loop trough all phases
        for (unsigned int p=0; p<this->experiments[exp]->expphases.size(); ++p)
        {
            if ((objfun.exp_CT == keys::IC) /*&& (objfun->exp_property =="NULL")*/)
            {
                // loop trough all elements
                for (unsigned int e=0; e<this->experiments[exp]->expphases[p]->phIC.size(); ++e)
                {
                    if ((this->experiments[exp]->expphases[p]->phIC[e]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phIC[e]->Qunit;
                        check_unit(exp, p, e, objfun.exp_unit, this );
                        residual =  residual_phase_elem (exp, p, e, objfun, this);
                        count++;
                    }
                }
            } else
                if ((objfun.exp_CT == keys::MR) /*&& (objfun->exp_property =="NULL")*/)
                {
                    // loop trough all elements
                    for (unsigned int f=0; f<this->experiments[exp]->expphases[p]->phMR.size(); ++f)
                    {
                        if ((this->experiments[exp]->expphases[p]->phMR[f]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {
                            // check for unit
                            check_MR_unit(exp, p, f, objfun.exp_unit, this );
                            residual = residual_phase_elemMR (exp, p, f, objfun, this);
                            count++;
                        }
                    }
                } else
                if ((objfun.exp_CT == keys::property) && (this->experiments[exp]->expphases[p]->phprop.size() > 0) /*&& (objfun->exp_dcomp == "NULL")*/)
                {
                // loop trough all properties
                for (unsigned int pp = 0; pp< this->experiments[exp]->expphases[p]->phprop.size(); ++pp)
                {
                    if ((this->experiments[exp]->expphases[p]->phprop[pp]->property == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phprop[pp]->Qunit;
                        check_prop_unit(exp, p, pp, objfun.exp_unit, this );
                        residual =  residual_phase_prop (exp, p, pp, objfun, this);
                        count++;
                    }
                }
            } else
                if (/*(objfun->exp_property !="NULL") &&*/ (this->experiments[exp]->expphases[p]->phDC.size() > 0) && (objfun.exp_CT == keys::DC))
                {
                    // loop trough all dependent components
                    for (unsigned int dc = 0; dc< this->experiments[exp]->expphases[p]->phDC.size(); ++dc)
                    {
                        if ((this->experiments[exp]->expphases[p]->phDC[dc]->DC == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {
                            // loop trough all dep comp properties
                            for (unsigned int dcp = 0; dcp < this->experiments[exp]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                            {
                                if (this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->property == objfun.exp_DCP)
                                {
                                    // check for unit
                                    if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit;
                                    check_unit_dcomp(exp, p, dc,dcp, objfun.exp_unit, this );
                                    residual = residual_phase_dcomp (exp, p, dc, dcp, objfun, this);
                                    count++;
                                }
                            }
                        }
                    }
                }
        }
    } else
    if ((objfun.exp_CT !="NULL") && (this->experiments[exp]->props.size() > 0))
    {
        for (unsigned int p=0; p<this->experiments[exp]->props.size(); p++)
        {
            if (this->experiments[exp]->props[p]->prop == objfun.exp_CN)
            {
                residual =  residual_properties (exp, p, objfun, this);
                count++;
            }
        }
    }
    if (residual == std::numeric_limits<double>::infinity() || residual == -std::numeric_limits<double>::infinity())
        residual = 777777;
    return residual;
}

// function for counting the total numbers of residuals that will be calculated
int TGfitTask::get_number_of_residuals( )
{
    // loop trough objective function
    int count = 0, nr_res=0;
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    /// Target function
    // Loop trough all experiments
        for (unsigned int i=0; i<this->experiments.size(); ++i)
        {
            get_residual(i, aTfun[i].objfun[j],count);
            nr_res +=count;
            if (count > 0)
                Tfun->objfun[j].isComputed = true;
        }
    }
    return nr_res;
}

double TGfitTask::get_sum_of_residuals( )
{
    // loop trough objective function
    // think about pararelizing it
    double residual = 0.0, ofun_residual;
    int count = 0;
    std::vector<double> residuals; residuals.resize(this->experiments.size());
    std::vector<double> ofun_residuals; ofun_residuals.resize(this->experiments.size());

    // Loop trough target function
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    // Loop trough all experiments
        ofun_residual =0.0;   
        std::fill(ofun_residuals.begin(), ofun_residuals.end(), 0.0);

#ifdef useomp
    omp_set_num_threads(this->MPI);
#ifdef _WIN32
    #pragma omp parallel for schedule(static)
#else
    #pragma omp parallel for schedule(dynamic)
#endif
#endif
        for (int i=0; i<this->experiments.size(); ++i)
        {
            double res = get_residual(i, aTfun[i].objfun[j],count);
            residuals[i] = residuals[i] + res;
            ofun_residuals[i] = ofun_residuals[i] + res;
//            ofun_residual = ofun_residual + res;
//            residual = residual + res;
        }

        for(std::vector<double>::iterator it = ofun_residuals.begin(); it != ofun_residuals.end(); ++it)
            ofun_residual += *it;
        Tfun->objfun[j].SumWTFun = ofun_residual;
    }

    for(std::vector<double>::iterator it = residuals.begin(); it != residuals.end(); ++it) {
        //gpf->flog<<"residual "<< *it<<std::endl;
        residual += *it;
   }

    return residual;
}

void TGfitTask::set_average_objfun ()
{
    double average = 0.0;
    int count = 0;
    minimum_value = 1e9;

    for (unsigned int j = 0; j<Tfun->objfun.size(); j++)
    {
        for (unsigned int i = 0; i <aTfun.size(); i++)
        {
            if (aTfun[i].objfun[j].isComputed)
            {
                average += aTfun[i].objfun[j].results.measured_value;
                if (aTfun[i].objfun[j].results.measured_value < minimum_value)
                    minimum_value = aTfun[i].objfun[j].results.measured_value;
                count++;
            }
        }
        average = average / count;
        Tfun->objfun[j].meas_average = average;
        average = 0.0;
        count = 0;
    }

//#ifdef useomp
//    omp_set_num_threads(this->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned int j = 0; j<Tfun->objfun.size(); j++)
    {
        for (unsigned int i = 0; i <aTfun.size(); i++)
        {
            if (aTfun[i].objfun[j].isComputed)
            {
                aTfun[i].objfun[j].meas_average = Tfun->objfun[j].meas_average;
            }
        }
    }
}


void TGfitTask::add_MC_scatter( std::vector<double> scatter)
{
    int count = 0;
    // loop trough objective function
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    /// Target function
    // Loop trough all experiments
        for (unsigned int i=0; i<this->experiments.size(); ++i)
        {
            if ((this->Tfun->objfun[j].exp_phase !="NULL") && (this->experiments[i]->expphases.size() > 0))
            {
                // loop trough all phases
                for (unsigned int p=0; p<this->experiments[i]->expphases.size(); ++p)
                {
                    if ((Tfun->objfun[j].exp_CT == keys::IC) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                    {
                        // loop trough all elements
                        for (unsigned int e=0; e<this->experiments[i]->expphases[p]->phIC.size(); ++e)
                        {
                            if ((this->experiments[i]->expphases[p]->phIC[e]->comp == this->Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                            {
                                if (this->experiments[i]->weight < 50.0)
                                    this->experiments[i]->expphases[p]->phIC[e]->Qnt = scatter[count];

                                ++count;
                            }
                        }
                    } else
                        if ((Tfun->objfun[j].exp_CT == keys::MR) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                        {
                            // loop trough all elements
                            for (unsigned int f=0; f<this->experiments[i]->expphases[p]->phMR.size(); ++f)
                            {
                                if ((this->experiments[i]->expphases[p]->phMR[f]->comp == this->Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                                {
                                    if (this->experiments[i]->weight < 50.0)
                                        this->experiments[i]->expphases[p]->phMR[f]->Qnt = scatter[count];

                                    ++count;
                                }
                            }
                        } else

                        if ((Tfun->objfun[j].exp_CT == keys::property) && (this->experiments[i]->expphases[p]->phprop.size() > 0) /*&& (this->Tfun->objfun[j]->exp_dcomp == "NULL")*/)
                        {
                        // loop trough all properties
                        for (unsigned int pp = 0; pp< this->experiments[i]->expphases[p]->phprop.size(); ++pp)
                        {
                            if ((this->experiments[i]->expphases[p]->phprop[pp]->property == Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                            {
                                if (this->experiments[i]->weight < 50.0)
                                    this->experiments[i]->expphases[p]->phprop[pp]->Qnt = scatter[count];

                                ++count;
                            }
                        }
                    } else
                        if (/*(Tfun->objfun[j]->exp_property !="NULL") &&*/ (this->experiments[i]->expphases[p]->phDC.size() > 0) && (Tfun->objfun[j].exp_CT == keys::DC))
                        {
                            // loop trough all dependent components
                            for (unsigned int dc = 0; dc< this->experiments[i]->expphases[p]->phDC.size(); ++dc)
                            {
                                if ((this->experiments[i]->expphases[p]->phDC[dc]->DC == Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                                {
                                    // loop trough all dep comp properties
                                    for (unsigned int dcp = 0; dcp < this->experiments[i]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                                    {
                                        if (this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->property == Tfun->objfun[j].exp_DCP)
                                        {
                                            if (this->experiments[i]->weight < 50.0)
                                                this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt = scatter[count];

                                            ++count;
                                        }
                                    }
                                }
                            }
                        }
                }
            }
        }
    }
}

void TGfitTask::get_addout_calc (int exp, TGfitTask::TargetFunction::obj_fun &addout )
{
    // loop trough objective function
    /// Target function
    if ((addout.exp_phase !="NULL") &&  (addout.exp_CT == keys::IC) /*&& (objfun->exp_property =="NULL")*/)
    {
       residual_phase_elem (exp, -1, -1,  addout, this);
    }

    if ((addout.exp_CT == keys::MR) /*&& (objfun->exp_property =="NULL")*/)
    {
       residual_phase_elemMR (exp,  -1, -1, addout, this);
    }

    if (addout.exp_CT == keys::property)
    {
       residual_phase_prop (exp,  -1,-1, addout, this);
    }

    if (addout.exp_CT == keys::DC)
    {
       residual_phase_dcomp (exp, -1, -1, -1, addout, this);
    }
}


void TGfitTask::set_weights ()
{
    // For onjective functions
//#ifdef useomp
//    omp_set_num_threads(this->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned i = 0; i<aTfun.size(); i++)
    {
        for (unsigned j = 0; j < aTfun[i].objfun.size(); j++)
        {
            if (Opti->OptUserWeight > 0)
                aTfun[i].objfun[j].weight = aTfun[i].objfun[j].weight * experiments[i]->weight;
            else
                if ((h_datasetlist > 0))
                    aTfun[i].objfun[j].weight = aTfun[i].objfun[j].weight * experiments[i]->weight;
                    else
                        aTfun[i].objfun[j].weight = aTfun[i].objfun[j].weight;

        }

    }

    // For nested functions
//#ifdef useomp
//    omp_set_num_threads(this->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned i = 0; i<aTfun.size(); i++)
    {
        for (unsigned j = 0; j < aTfun[i].nestfun.size(); j++)
        {
            if (Opti->OptUserWeight > 0)
                aTfun[i].nestfun[j].weight = aTfun[i].nestfun[j].weight * experiments[i]->weight;
            else
                aTfun[i].nestfun[j].weight = aTfun[i].nestfun[j].weight;
        }

    }
}


void TGfitTask::set_results ( TGfitTask::TargetFunction::obj_fun &objfun, double computed, double measured, double error, double Weighted_Tfun_residual, double Tfun_residual, double weight )
{
    objfun.results.computed_value = computed;
    objfun.results.measured_value = measured;
    objfun.results.error_value = error;
    objfun.results.residual = measured - computed;
    objfun.results.weight = weight;
    objfun.results.Tfun_residual = Tfun_residual;
    objfun.results.WTfun_residual = Weighted_Tfun_residual;
}

void TGfitTask::set_print_param()
{
    int np = Opti->optv.size();
    fitparam.clear();

//    for(unsigned i=0; i< optv_.size(); i++ ) // cols
//    {
    size_t npx = 0;
    for (size_t e =0; e < Opti->optParam.size(); e++)
    {
        for (size_t i=0; i<Opti->optParam[e]->Get_optFPsize(); ++i)
        {
            // Print optimized parameter values to file
            fitparam.push_back(std::make_shared<parameters>());

            fitparam[i+npx]->Ptype = Opti->optParam[e]->Get_optType();
            fitparam[i+npx]->Pfittype = "F";

            Opti->optParam[e]->Get_Fparam(i, fitparam[i+npx]->Pname,
                    fitparam[i+npx]->Ival, fitparam[i+npx]->Fval);
            fitparam[i+npx]->CSS = 0.0;
            fitparam[i+npx]->mc95 = 0.0;
            fitparam[i+npx]->mcSTDEV = 0.0;
            fitparam[i+npx]->mcMEAN = 0.0;
        }
        npx = fitparam.size();
    }

//    }

    for (size_t e =0; e < Opti->optParam.size(); e++)
    {
        for (size_t i=0; i<Opti->optParam[e]->Get_optRPsize(); ++i)
        {
            fitparam.push_back(std::make_shared<parameters>());
            fitparam[np+i]->Ptype = Opti->optParam[e]->Get_optType();
            fitparam[np+i]->Pfittype = "R";

            Opti->optParam[e]->Get_Rparam(i, fitparam[np+i]->Pname,
                    fitparam[np+i]->Ival, fitparam[np+i]->Fval);
            fitparam[np+i]->CSS = 0.0;
            fitparam[np+i]->mc95 = 0.0;
            fitparam[np+i]->mcSTDEV = 0.0;
            fitparam[np+i]->mcMEAN = 0.0;

        }
    }
    np = fitparam.size();
    for (size_t e =0; e < Opti->optParam.size(); e++)
    {
        for (size_t i=0; i<Opti->optParam[e]->Get_optLPsize(); ++i)
        {
            fitparam.push_back(std::make_shared<parameters>());
            fitparam[np+i]->Ptype = Opti->optParam[e]->Get_optType();
            fitparam[np+i]->Pfittype = "L";

            Opti->optParam[e]->Get_Lparam(i, fitparam[np+i]->Pname,
                    fitparam[np+i]->Ival, fitparam[np+i]->Fval);
            fitparam[np+i]->CSS = 0.0;
            fitparam[np+i]->mc95 = 0.0;
            fitparam[np+i]->mcSTDEV = 0.0;
            fitparam[np+i]->mcMEAN = 0.0;
        }
    }

}

void TGfitTask::print_param()
{
    set_print_param();
    unsigned int nrcor = 0;
    gpf->fparam.open(gpf->FITparamFile().c_str(), std::ios::trunc);
    if( gpf->fparam.fail() )
    { std::cout<<"Fit parameters fileopen error"<<std::endl; exit(1); }

    // print param
    gpf->fparam << "ptype,parameter," << "name,"	<< "init.value,"
                   << "fittted.value," << "mc.mean," << "mc.stdev," << "confi99gauss,confi95gauss,confi90gauss," << "CSS.sensitivity,";

    for (unsigned i= 0; i<fitparam.size(); i++)
    {
        if (fitparam[i]->Pfittype == "F")
        {
            gpf->fparam << "correl.coef." << fitparam[i]->Pname  <<",";
            nrcor++;
        }
    }

    gpf->fparam << std::endl;

    for (unsigned i= 0; i<fitparam.size(); i++)
    {
        gpf->fparam << fitparam[i]->Pfittype;
        gpf->fparam << "," << fitparam[i]->Ptype;
        gpf->fparam << "," << fitparam[i]->Pname << ",";


        gpf->fparam << std::setprecision(12) << fitparam[i]->Ival << ",";
        gpf->fparam << std::setprecision(12) <<fitparam[i]->Fval << ",";

        if (fitparam[i]->mcSTDEV != 0)
        {
           gpf->fparam << fitparam[i]->mcMEAN << ",";
           gpf->fparam << fitparam[i]->mcSTDEV << ",";
           for (unsigned j=0; j < fitparam[i]->mcconfi.size(); j++)
           {
               gpf->fparam << fitparam[i]->mcconfi[j] << ",";
           }
//           gpf->fparam << fitparam[i]->mc95 << ",";
        }

        else
            gpf->fparam << ",,,,,";
        if (fitparam[i]->CSS != 0)
        gpf->fparam << fitparam[i]->CSS << ",";
        else
            gpf->fparam << "0,";

        for ( unsigned j=0; j < fitparam[i]->correl.size(); j++)
        {
           if (fitparam[i]->Pfittype == "F")
           {
               if (fitparam[i]->correl[j] !=0)
               {
                   gpf->fparam << std::setprecision(4)<< fitparam[i]->correl[j] << ",";
               } else
                   gpf->fparam << "0,";
           } else
               gpf->fparam << "0,";
        }

        if ((fitparam[i]->Pfittype == "F") && (fitparam[i]->correl.size() == 0))
        for ( unsigned j=0; j < (nrcor ); j++)
        {
            gpf->fparam << "0,";
        }

        if (fitparam[i]->Pfittype == "R")
        {
            for ( unsigned j=0; j < (nrcor ); j++)
            {
                gpf->fparam << "0,";
            }
        }


    gpf->fparam << std::endl;
    }

gpf->fparam.close();

}

void TGfitTask:: print_global_results ()
{
//    gpf->fres << "experiment,,,unit,measured,computed,residual," <<"residual_"+Tfun->type <<  endl;
    // GEMSFIT results file for all test runs. Keeps a log of all runs. The file has to be deleted manually.
    std::string path_ = gpf->FITFile();
    gpf->fres.open(path_.c_str(), std::ios::trunc);
    if( gpf->fres.fail() )
    { std::cout<<"Results fileopen error"<<std::endl; exit(1); }

    int prec = 12;
    std::setprecision(prec);
    scientific(gpf->fres);
//    gpf->fres.setf(ios::fixed);

    std::vector<std::string> header;
    header.push_back(keys::expsample);
    header.push_back(keys::expdataset);
    header.push_back(keys::sT);
    header.push_back(keys::sP);

//    gpf->fres << " <" << this->param_file <<"> <" << this->anNodes<<" samples> " <<endl;

    for (unsigned int i = 0; i <aTfun.size(); i++)
    {
        for (unsigned j = 0; j <Tfun->addout.size(); j++)
        {
            if (Tfun->addout[j].Otype == keys::meas)
                // before get_residual function was used
                get_addout_meas(i, aTfun[i].addout[j] );
            if (Tfun->addout[j].Otype == keys::calc)
                get_addout_calc(i, aTfun[i].addout[j] );
            if (Tfun->addout[j].exp_CT == keys::comp)
                get_addout_input(i, aTfun[i].addout[j] );
        }
    }

//    for (unsigned int i = 0; i <aTfun.size(); i++)
//    {
//        for (unsigned j = 0; j <Tfun->addout.size(); j++)
//        {
//            if (Tfun->addout[j].Otype == keys::calc)
//            get_addout_calc(i, aTfun[i].addout[j] );
//        }
//    }


    for (unsigned i = 0; i < Tfun->objfun.size(); i++)
    {
        std::string temp ="";
        temp += Tfun->objfun[i].exp_phase + "." + Tfun->objfun[i].exp_CN ;
        header.push_back(temp + "." + keys::meas);
        header.push_back(temp + "." + keys::calc);
        header.push_back("residual");
        header.push_back("%residual");
        header.push_back("weight");
//        header.push_back("residual."+ Tfun->type);
    }

    for (unsigned i = 0; i < Tfun->addout.size(); i++)
    {
        std::string temp ="";
        if (Tfun->addout[i].exp_phase != "NULL")
        {
        temp += Tfun->addout[i].exp_phase;
        if ((Tfun->addout[i].exp_DCP != "NULL") && (Tfun->addout[i].expr == "NULL"))
            temp += "." + Tfun->addout[i].exp_DCP;
        temp += "." + Tfun->addout[i].exp_CN;
        }

        if (Tfun->addout[i].expr != "NULL")
            if (Tfun->addout[i].exp_DCP != "NULL")
                temp += "." + Tfun->addout[i].exp_DCP;
//            temp += "." + Tfun->addout[i].expr;

        if (Tfun->addout[i].Otype == keys::calc)
        header.push_back(temp + "." + keys::calc);
        if (Tfun->addout[i].Otype == keys::meas)
        header.push_back(temp + "." + keys::meas);

        if (Tfun->addout[i].exp_phase == "NULL" && Tfun->addout[i].exp_CT == keys::comp)
        {
            temp += keys::comp;
            temp += "." + Tfun->addout[i].exp_CN;
            header.push_back(temp);
        }

    }

    for (unsigned i = 0; i<header.size(); i++)
    {
        gpf->fres << header[i] << ",";
    }

    gpf->fres << std::endl;

    for (unsigned i = 0; i<aTfun.size(); i++)
    {
        gpf->fres << experiments[i]->sample <<",";
        gpf->fres << experiments[i]->expdataset <<",";
        gpf->fres << experiments[i]->sT <<",";
        gpf->fres << experiments[i]->sP <<",";
        for (unsigned j=0; j < Tfun->objfun.size(); j++)
        {
            if (aTfun[i].objfun[j].isComputed)
            {
                gpf->fres << std::setprecision(prec) << aTfun[i].objfun[j].results.measured_value <<","<<
                             std::setprecision(prec) << aTfun[i].objfun[j].results.computed_value << ","<<
                             std::setprecision(prec) << aTfun[i].objfun[j].results.residual<<","/*<< aTfun[i].objfun[j].results.WTfun_residual << ","*/ <<
                             std::setprecision(prec) << (100*aTfun[i].objfun[j].results.residual/aTfun[i].objfun[j].results.measured_value)<<","<<
                             std::setprecision(prec) << aTfun[i].objfun[j].results.weight <<",";
            } else
                gpf->fres << ","<< ","<<","<< ","<< ",";

        }

        for (unsigned j=0; j < Tfun->addout.size(); j++)
        {
            if (aTfun[i].addout[j].exp_CT == keys::comp && aTfun[i].addout[j].results.input_value != -1.0)
                gpf->fres << std::setprecision(prec) << aTfun[i].addout[j].results.input_value <<",";
            else
            if (aTfun[i].addout[j].isComputed)
            {
                if (aTfun[i].addout[j].Otype == keys::calc)
                gpf->fres << std::setprecision(prec) << aTfun[i].addout[j].results.computed_value <<",";
                if (aTfun[i].addout[j].Otype == keys::meas)
                gpf->fres << std::setprecision(prec) << aTfun[i].addout[j].results.measured_value <<",";
            } else
                gpf->fres << ",";
        }

        gpf->fres << std::endl;
    }

    gpf->fres.close();

    // print rhoW csv
    std::ofstream fRHO;
    path_ = gpf->OutputDirPath() + "RHO.csv";
    fRHO.open(path_.c_str(), std::ios::trunc);
    if( fRHO.fail() )
    { std::cout<<"Results fileopen error"<<std::endl; exit(1); }
    fRHO << "Pb,TK,RHO_gcm-3" << std::endl;
    fRHO << 1 <<","<< 25+273.15 <<"," << NodT[0]->DenH2Ow(100000,25+273.15) << std::endl;

    for (unsigned i = 0; i<TP_pairs[0].size(); i++)
    {
        fRHO <<TP_pairs[1][i] <<","<<TP_pairs[0][i]+273.15 <<"," << NodT[i]->DenH2Ow(TP_pairs[1][i]*100000,TP_pairs[0][i]+273.15) << std::endl;
    }

    fRHO.close();
}

void TGfitTask:: print_nested_results ()
{
    gpf->fnfres.open(gpf->FITnfunFile().c_str(), std::ios::trunc);
    if( gpf->fnfres.fail() )
    { std::cout<<"Nested Results fileopen error"<<std::endl; exit(1); }

    gpf->fnfres << "sample,phase,name,unit,sT,sP,measured,computed,residual,%residual";

    for (size_t i = 0; i<Opti->optNFParam.size(); i++)
    {
        gpf->fnfres << Opti->optNFParam[i]->Print_param();
    }

    gpf->fnfres << std::endl;
    std::setprecision(12);
    scientific(gpf->fnfres);

//    gpf->fnfres.setf(ios::fixed);

    for (size_t  i=0; i<aTfun.size(); i++)
    {
        for (size_t  j = 0; j <aTfun[i].nestfun.size(); j++)
        {
            if (aTfun[i].nestfun[j].isComputed)
            {
                gpf->fnfres << experiments[i]->sample <<"," << aTfun[i].nestfun[j].exp_phase <<","<< aTfun[i].nestfun[j].exp_CN <<","<< aTfun[i].nestfun[j].exp_unit <<",";

                if (aTfun[i].nestfun[j].sT > -1 && aTfun[i].nestfun[j].sP > -1)
                    gpf->fnfres << aTfun[i].nestfun[j].sT << "," << aTfun[i].nestfun[j].sP <<",";
                else
                    gpf->fnfres << experiments[i]->sT << "," << experiments[i]->sP <<",";

                gpf->fnfres << aTfun[i].nestfun[j].results.measured_value <<","<< aTfun[i].nestfun[j].results.computed_value << ","<< aTfun[i].nestfun[j].results.residual << "," << (100*aTfun[i].nestfun[j].results.residual/aTfun[i].nestfun[j].results.measured_value);

                for (size_t o = 0; o<Opti->optNFParam.size(); o++)
                {

                    for (size_t p= 0; p<Opti->optNFParam[o]->Get_optFPsize(); p++)
                    {
                        gpf->fnfres << "," << Opti->optNFParam[o]->Get_Fparam(p, i);
                    }
                }

                for (size_t o = 0; o<Opti->optNFParam.size(); o++)
                {

                    for (size_t p= 0; p<Opti->optNFParam[o]->Get_optLPsize(); p++)
                    {
                        gpf->fnfres << "," << Opti->optNFParam[o]->Get_Lparam(p, i);
                    }
                }
                gpf->fnfres << std::endl;
            }
        }
    }
    gpf->fnfres.close();
}

void TGfitTask::get_addout_input(int exp, TGfitTask::TargetFunction::obj_fun &objfun )
{
    for (size_t c=0; c<this->experiments[exp]->sbcomp.size(); c++)
    {
        if (objfun.exp_CN == this->experiments[exp]->sbcomp[c]->comp)
        {
            if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->sbcomp[c]->Qunit;
            objfun.results.input_value = this->experiments[exp]->sbcomp[c]->Qnt;
        }
    }
}


void TGfitTask::get_addout_meas(int exp, TGfitTask::TargetFunction::obj_fun &objfun )
{
    // loop trough objective function
    /// Target function
    if ((objfun.exp_phase !="NULL") && (this->experiments[exp]->expphases.size() > 0))
    {
        // loop trough all phases
        for (unsigned int p=0; p<this->experiments[exp]->expphases.size(); ++p)
        {
            if ((objfun.exp_CT == keys::IC) /*&& (objfun->exp_property =="NULL")*/)
            {
                // loop trough all elements
                for (unsigned int e=0; e<this->experiments[exp]->expphases[p]->phIC.size(); ++e)
                {
                    if ((this->experiments[exp]->expphases[p]->phIC[e]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phIC[e]->Qunit;
                        objfun.results.measured_value = this->experiments[exp]->expphases[p]->phIC[e]->Qnt;
                        objfun.results.error_value = this->experiments[exp]->expphases[p]->phIC[e]->Qerror;
                        objfun.isComputed = true;
                        ///
                    }
                }
            } else
                if ((objfun.exp_CT == keys::MR) /*&& (objfun->exp_property =="NULL")*/)
                {
                    // loop trough all elements
                    for (unsigned int f=0; f<this->experiments[exp]->expphases[p]->phMR.size(); ++f)
                    {
                        if ((this->experiments[exp]->expphases[p]->phMR[f]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {

                            objfun.results.measured_value = this->experiments[exp]->expphases[p]->phMR[f]->Qnt;
                            objfun.results.error_value = this->experiments[exp]->expphases[p]->phMR[f]->Qerror;
                            objfun.isComputed = true;
                            ///

                        }
                    }
                } else
                if ((objfun.exp_CT == keys::property) && (this->experiments[exp]->expphases[p]->phprop.size() > 0) /*&& (objfun->exp_dcomp == "NULL")*/)
                {
                // loop trough all properties
                for (unsigned int pp = 0; pp< this->experiments[exp]->expphases[p]->phprop.size(); ++pp)
                {
                    if ((this->experiments[exp]->expphases[p]->phprop[pp]->property == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phprop[pp]->Qunit;
                        objfun.results.measured_value = this->experiments[exp]->expphases[p]->phprop[pp]->Qnt;
                        objfun.results.error_value = this->experiments[exp]->expphases[p]->phprop[pp]->Qerror;
                        objfun.isComputed = true;
                        ///
                    }
                }
            } else
                if (/*(objfun->exp_property !="NULL") &&*/ (this->experiments[exp]->expphases[p]->phDC.size() > 0) && (objfun.exp_CT == keys::DC))
                {
                    // loop trough all dependent components
                    for (unsigned int dc = 0; dc< this->experiments[exp]->expphases[p]->phDC.size(); ++dc)
                    {
                        if ((this->experiments[exp]->expphases[p]->phDC[dc]->DC == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {
                            // loop trough all dep comp properties
                            for (unsigned int dcp = 0; dcp < this->experiments[exp]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                            {
                                if (this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->property == objfun.exp_DCP)
                                {
                                    // check for unit
                                    if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit;
                                    objfun.results.measured_value = this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
                                    objfun.results.error_value = this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror;
                                    objfun.isComputed = true;
                                    ///
                                }
                            }
                        }
                    }
                }
        }
    }

}

void TGfitTask::set_fixed_parameters()
{
    if (mLook == 0)
    {
#ifdef useomp
    omp_set_num_threads(this->MPI);
    #pragma omp parallel for schedule(static)
#endif
        for (int n=0; n<NodT.size(); n++  )
            for (int e = 0; e <Opti->optParam.size(); e++)
                if ( Opti->optParam[e]->Get_optType() != "G0" )
                    Opti->optParam[e]->Adjust_Sparam(NodT[n].get() );
    }
    else
#ifdef useomp
    omp_set_num_threads(this->MPI);
    #pragma omp parallel for schedule(static)
#endif
        for (int n=0; n<NodT.size(); n++  )
            for (int e = 0; e <Opti->optParam.size(); e++)
                Opti->optParam[e]->Adjust_Sparam(NodT[n].get());
}
