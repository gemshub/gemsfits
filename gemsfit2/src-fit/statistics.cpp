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

/**
 *	@file statistics.cpp
 *
 *	@brief this header file contains implementation of the statistics class and the functions for statistical calculations.
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */


#include "statistics.h"
#include "gemsfit_global_functions.h"
#include <armadillo>
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/chi_squared.hpp>


#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <boost/random/normal_distribution.hpp>

// Constructor
statistics::statistics(TGfitTask *gfittask, double Weighted_Tfun_sum_of_residuals_, int num_of_params_, int num_of_runs_ )
{
    double mean_res = 0.0, Abs_mean_res = 0.0, Weighted_TF_mean_res = 0.0;

//    gfittask->test();

    Weighted_mean_res = 0.0;
    number_of_measurements = 0;
    number_of_measurements += gfittask->computed_values_v.size();
    Tfun_sum_of_residuals = 0.0;
    Abs_sum_of_residuals = 0.0; Weighted_Abs_sum_of_residuals = 0.0;
    min_res =0.0; max_res = 0.0; Weighted_min_res = 0.0; Weighted_max_res = 0.0; neg_residuals = 0; pos_residuals = 0;

    // this is the minimized value during oprimization
    Weighted_Tfun_sum_of_residuals 		= Weighted_Tfun_sum_of_residuals_;
    if (Weighted_Tfun_sum_of_residuals == 0.0)
        Weighted_Tfun_sum_of_residuals = gfittask->_init_residuals_sys;
    //
    for (unsigned int i=0; i<number_of_measurements; ++i)
    {
        Tfun_sum_of_residuals += gfittask->Tfun_residuals_v[i];                         // sum residuals of the minimized function without weight
         Abs_sum_of_residuals += fabs(gfittask->residuals_v[i]);                        // sum of the absolute value of residuals
Weighted_Abs_sum_of_residuals += fabs(gfittask->residuals_v[i])*gfittask->weights[i];   // sum of the absolute value of residuals * weight

        if (gfittask->weights[i] != 1)
        weighted_residuals.push_back(gfittask->residuals_v[i]*gfittask->weights[i]);
        else weighted_residuals.push_back(gfittask->residuals_v[i]);
        measured_norm_residuals.push_back(gfittask->residuals_v[i] / gfittask->measured_values_v[i] * 100);

        // getting the samlest and largest values of residuals and weighted residuals
        // counting the number of negative and positivie residuals
        if (min_res > gfittask->residuals_v[i]) min_res = gfittask->residuals_v[i];
        if (Weighted_min_res > weighted_residuals[i]) Weighted_min_res = weighted_residuals[i];
        if (max_res < gfittask->residuals_v[i]) max_res = gfittask->residuals_v[i];
        if (Weighted_max_res < weighted_residuals[i]) Weighted_max_res = weighted_residuals[i];
        if (gfittask->residuals_v[i] < 0) ++neg_residuals;
        if (gfittask->residuals_v[i] >= 0) ++pos_residuals;
    }

    num_of_runs		= num_of_runs_;

std::cout<<" Statistics Constructor: sum of squares: "<<Weighted_Tfun_sum_of_residuals<<std::endl;

    number_of_ind_parameters   = num_of_params_;
    number_of_parameters = number_of_ind_parameters /*+ gfittask->Opti->reactions.size() + gfittask->Opti->Lparams.size()*/;

std::cout<<" Statistics Constructor: number_of_parameters: "<<number_of_parameters<<std::endl;

    get_stat_param(gfittask->OptSet); // <----------------------------------------------------++++++
//stat->set_plotfit_vars_txt();

//    /// Instantiate pointer to PlotFit class to print results
//    Plot_Stat = new PlotFit();

    // Degrees of freedom
    degrees_of_freedom = number_of_measurements-number_of_parameters;

    // Compute standard deviation of residuals
    for (unsigned int i=0; i<number_of_measurements; i++)
    {
Weighted_TF_mean_res += gfittask->Weighted_Tfun_residuals_v[i];
        Abs_mean_res += fabs(gfittask->residuals_v[i]);
   Weighted_mean_res += weighted_residuals[i];
            mean_res += gfittask->residuals_v[i];

    }

// copute means
 Weighted_TF_mean_res = Weighted_TF_mean_res / number_of_measurements;  // mean of residuals that summ up to give the minimized value
         Abs_mean_res = Abs_mean_res / number_of_measurements;          // mean of the absolute value of residuals
    Weighted_mean_res = Weighted_mean_res / number_of_measurements;     // mean of residuals*weight (same as means_res if weight is 1)
             mean_res = mean_res / number_of_measurements;              // mean of residuals

 Weighted_TF_SD_of_residuals = 0.0;
         Abs_SD_of_residuals = 0.0;
    Weighted_SD_of_residuals = 0.0;
             SD_of_residuals = 0.0;

    for (unsigned int i=0; i<number_of_measurements; i++)
    {
        Weighted_TF_SD_of_residuals += pow((gfittask->Weighted_Tfun_residuals_v[i]-Weighted_TF_mean_res),2);
                Abs_SD_of_residuals += pow((fabs(gfittask->residuals_v[i])-Abs_mean_res),2);
           Weighted_SD_of_residuals += pow((weighted_residuals[i]-Weighted_mean_res),2);
                    SD_of_residuals += pow((gfittask->residuals_v[i]-mean_res),2);
    }

    Weighted_TF_SD_of_residuals = sqrt((Weighted_TF_SD_of_residuals/degrees_of_freedom));   // standard deviation of residuals that summ up to give the minimized value
            Abs_SD_of_residuals = sqrt((Abs_SD_of_residuals/degrees_of_freedom));           // standard deviation of the absolute value of residuals
       Weighted_SD_of_residuals = sqrt((Weighted_SD_of_residuals/degrees_of_freedom));      // standard deviation of residuals*weight (same as means_res if weight is 1)
                SD_of_residuals = sqrt((SD_of_residuals/degrees_of_freedom));               // standard deviation of residuals
/*////////////////////////////////////////////////////////*/

  // calculate the stddev of residuals for each individual objfun
  for (unsigned j = 0; j<gfittask->Tfun->objfun.size(); j++)
  {
      min_res = 0.0; max_res =0.0; int obj_neg_residuals = 0; int obj_pos_residuals = 0;
      objfun_stat.push_back(std::make_shared<statistics::objfunstat>());
      objfun_stat[j]->stdev_res = 0.0;
      objfun_stat[j]->mean_res = 0.0;
      objfun_stat[j]->mean_meas = 0.0;
      objfun_stat[j]->norm_stdev_res = 0.0;
      objfun_stat[j]->norm_mean_res = 0.0;
      objfun_stat[j]->MSE = 0.0;
      objfun_stat[j]->RMSE = 0.0;
      objfun_stat[j]->R2 = 0.0;
      objfun_stat[j]->nr = 0;
      objfun_stat[j]->isComputed = false;
      objfun_stat[j]->SumWTFun = 0.0;

      for (unsigned i=0; i<gfittask->aTfun.size(); i++)
      {

          if (gfittask->aTfun[i].objfun[j].isComputed)
          {
              double test_norm_res = gfittask->aTfun[i].objfun[j].results.residual / gfittask->aTfun[i].objfun[j].results.measured_value;
              if (test_norm_res > 1)
              {
                  objfun_stat[j]->norm_mean_res += gfittask->aTfun[i].objfun[j].results.residual;
              }
              else objfun_stat[j]->norm_mean_res += gfittask->aTfun[i].objfun[j].results.residual / gfittask->aTfun[i].objfun[j].results.measured_value;
              objfun_stat[j]->mean_res += gfittask->aTfun[i].objfun[j].results.residual;
              objfun_stat[j]->mean_meas += gfittask->aTfun[i].objfun[j].results.measured_value;
              objfun_stat[j]->orderd_res.push_back (gfittask->aTfun[i].objfun[j].results.residual) ;
              objfun_stat[j]->SumWTFun += gfittask->aTfun[i].objfun[j].results.WTfun_residual;
              objfun_stat[j]->MSE += pow(gfittask->aTfun[i].objfun[j].results.measured_value-gfittask->aTfun[i].objfun[j].results.computed_value,2);

              if (min_res > gfittask->aTfun[i].objfun[j].results.residual) min_res = gfittask->aTfun[i].objfun[j].results.residual;
              if (max_res < gfittask->aTfun[i].objfun[j].results.residual) max_res = gfittask->aTfun[i].objfun[j].results.residual;
              if (gfittask->aTfun[i].objfun[j].results.residual < 0) ++obj_neg_residuals;
              if (gfittask->aTfun[i].objfun[j].results.residual >= 0) ++obj_pos_residuals;

              objfun_stat[j]->nr++;
              objfun_stat[j]->isComputed = true;
          }
      }
      objfun_stat[j]->norm_mean_res = objfun_stat[j]->norm_mean_res / objfun_stat[j]->nr;
      objfun_stat[j]->mean_res = objfun_stat[j]->mean_res / objfun_stat[j]->nr;
      objfun_stat[j]->mean_meas = objfun_stat[j]->mean_meas / objfun_stat[j]->nr;
      objfun_stat[j]->MSE = objfun_stat[j]->MSE / objfun_stat[j]->nr;

      objfun_stat[j]->min_res = min_res;
      objfun_stat[j]->max_res = max_res;
      objfun_stat[j]->nr_pos_res = obj_pos_residuals;
      objfun_stat[j]->nr_neg_res = obj_neg_residuals;

      int dscount = -1;
      std::string dataset = "";
      for (unsigned i=0; i<gfittask->aTfun.size(); i++)
      {
          if (gfittask->aTfun[i].objfun[j].isComputed)
          {
              if (dataset == gfittask->experiments[i]->expdataset)
              {
                  objfun_stat[j]->exp_dataset[dscount].measured_value.push_back(gfittask->aTfun[i].objfun[j].results.measured_value);
                  objfun_stat[j]->exp_dataset[dscount].measured_error.push_back(gfittask->aTfun[i].objfun[j].results.error_value);
                  objfun_stat[j]->exp_dataset[dscount].residuals.push_back(gfittask->aTfun[i].objfun[j].results.residual);

                  objfun_stat[j]->exp_dataset[dscount].norm_residuals.push_back(
                              gfittask->aTfun[i].objfun[j].results.residual / gfittask->aTfun[i].objfun[j].results.measured_value);
              } else
              {
                  statistics::objfunstat::expdataset expdataset;
                  objfun_stat[j]->exp_dataset.push_back(expdataset);
                  dscount++;
                  objfun_stat[j]->exp_dataset[dscount].name = gfittask->experiments[i]->expdataset;
                  dataset = gfittask->experiments[i]->expdataset;
                  objfun_stat[j]->exp_dataset[dscount].measured_value.push_back(gfittask->aTfun[i].objfun[j].results.measured_value);
                  objfun_stat[j]->exp_dataset[dscount].measured_error.push_back(gfittask->aTfun[i].objfun[j].results.error_value);
                  objfun_stat[j]->exp_dataset[dscount].residuals.push_back(gfittask->aTfun[i].objfun[j].results.residual);

                  objfun_stat[j]->exp_dataset[dscount].norm_residuals.push_back(
                              gfittask->aTfun[i].objfun[j].results.residual / gfittask->aTfun[i].objfun[j].results.measured_value);
              }
          }
      }

      for (unsigned d =0; d < objfun_stat[j]->exp_dataset.size(); d++)
      {
          for (unsigned s = 0; s < objfun_stat[j]->exp_dataset[d].residuals.size(); s++)
          {
            objfun_stat[j]->exp_dataset[d].scatter.push_back(0.0);
          }
      }
  }


  for (unsigned j = 0; j<gfittask->Tfun->objfun.size(); j++)
  {
      for (unsigned i=0; i<gfittask->aTfun.size(); i++)
      {
          if (gfittask->aTfun[i].objfun[j].isComputed)
          {
              double test_norm_res = gfittask->aTfun[i].objfun[j].results.residual / gfittask->aTfun[i].objfun[j].results.measured_value;
              if (test_norm_res > 1)
              {
                  objfun_stat[j]->norm_stdev_res += pow (((gfittask->aTfun[i].objfun[j].results.residual) - objfun_stat[j]->mean_res), 2);
              }
              else objfun_stat[j]->norm_stdev_res += pow (((gfittask->aTfun[i].objfun[j].results.residual/gfittask->aTfun[i].objfun[j].results.measured_value) - objfun_stat[j]->norm_mean_res), 2);
              objfun_stat[j]->stdev_res += pow (((gfittask->aTfun[i].objfun[j].results.residual) - objfun_stat[j]->mean_res), 2);
          }
      }
      objfun_stat[j]->norm_stdev_res = sqrt(objfun_stat[j]->norm_stdev_res / objfun_stat[j]->nr);
      objfun_stat[j]->stdev_res = sqrt(objfun_stat[j]->stdev_res / objfun_stat[j]->nr);
  }

}


// Destructor
statistics::~statistics()
{

}

void statistics::get_stat_param(const std::string& optset_json)
{
    int mode = gpf->KeysNdx;
    common::JsonFree db_opt_object = fromJsonString(optset_json);
    MCbool = db_opt_object.value(keys::StatMC[mode], MCbool);
    num_of_MC_runs = db_opt_object.value(keys::StatMCr[mode], num_of_MC_runs);
    perturbator = db_opt_object.value(keys::StatPer[mode], perturbator);
}

// Perform basic statistical analysis
void statistics::basic_stat( std::vector<double> &optv_, TGfitTask *gfittask )
{
    // Variable declarations
    size_t i;
    double mean = 0.;
    double ResSumSquares = 0., TotalSumSquares = 0.;
    bool noqq = false;
//    double Nom_CC = 0.0, Dnom_CC = 0.0;
//    double Correlation_coef = 0.0;
//    double Res = 0.;
    double m2 = 0., m3 = 0., m4 = 0.;
    double sqrtb1, b2, Y, beta2_sqrtb1, W2, delta, alpha, Z_sqrtb1, E_b2, Var_b2, x, sqrt_beta1_b2, A, Z_b2, K2/*, K2test*/;
    std::vector<double> percentiles_v;
    std::vector<double> quantiles_v;
//    std::vector<double> abs_residuals_v;

    gpf->fstat.open(gpf->FITStatisticsFile().c_str(), std::ios::trunc);
    if( gpf->fstat.fail() )
    { std::cout<<"Summary and statistics fileopen error"<<std::endl; exit(1); }

    gpf->fsens.open(gpf->FITsensFile().c_str(), std::ios::trunc);
    if( gpf->fsens.fail() )
    { std::cout<<"Sensitivities fileopen error"<<std::endl; exit(1); }

    gpf->fqq.open(gpf->FITqqFile().c_str(), std::ios::trunc);
    if( gpf->fqq.fail() )
    { std::cout<<"QQ plot data fileopen error"<<std::endl; exit(1); }

    size_t np = optv_.size();

//    for(unsigned i=0; i< optv_.size(); i++ ) // cols
//    {
    size_t npx = 0;
    for (size_t e =0; e < gfittask->Opti->optParam.size(); e++)
    {
        for (size_t i=0; i<gfittask->Opti->optParam[e]->Get_optFPsize(); ++i)
        {
            // Print optimized parameter values to file
            fitparam.push_back(std::make_shared<parameters>());

            fitparam[i+npx]->Ptype = gfittask->Opti->optParam[e]->Get_optType();
            fitparam[i+npx]->Pfittype = "F";

            gfittask->Opti->optParam[e]->Get_Fparam(i, fitparam[i+npx]->Pname,
                    fitparam[i+npx]->Ival, fitparam[i+npx]->Fval);
            fitparam[i+npx]->CSS = 0.0;
            fitparam[i+npx]->mc95 = 0.0;
            fitparam[i+npx]->mcSTDEV = 0.0;
            fitparam[i+npx]->mcMEAN = 0.0;
        }
        npx = fitparam.size();
    }

//    }

    for (size_t e =0; e < gfittask->Opti->optParam.size(); e++)
    {
        for ( i=0; i<gfittask->Opti->optParam[e]->Get_optRPsize(); ++i)
        {
            fitparam.push_back(std::make_shared<parameters>());
            fitparam[np+i]->Ptype = gfittask->Opti->optParam[e]->Get_optType();
            fitparam[np+i]->Pfittype = "R";

            gfittask->Opti->optParam[e]->Get_Rparam(i, fitparam[np+i]->Pname,
                    fitparam[np+i]->Ival, fitparam[np+i]->Fval);
            fitparam[np+i]->CSS = 0.0;
            fitparam[np+i]->mc95 = 0.0;
            fitparam[np+i]->mcSTDEV = 0.0;
            fitparam[np+i]->mcMEAN = 0.0;

        }
    }
    np = fitparam.size();
    for (size_t e =0; e < gfittask->Opti->optParam.size(); e++)
    {
        for (size_t i=0; i<gfittask->Opti->optParam[e]->Get_optLPsize(); ++i)
        {
            fitparam.push_back(std::make_shared<parameters>());
            fitparam[np+i]->Ptype = gfittask->Opti->optParam[e]->Get_optType();
            fitparam[np+i]->Pfittype = "L";

            gfittask->Opti->optParam[e]->Get_Lparam(i, fitparam[np+i]->Pname,
                    fitparam[np+i]->Ival, fitparam[np+i]->Fval);
            fitparam[np+i]->CSS = 0.0;
            fitparam[np+i]->mc95 = 0.0;
            fitparam[np+i]->mcSTDEV = 0.0;
            fitparam[np+i]->mcMEAN = 0.0;
        }
    }


    // Compute R^2: coefficient of determination for all data
        mean = 0;
    for (i=0; i< number_of_measurements; i++)
    {
        mean += gfittask->measured_values_v[i];
    }
    mean = mean / gfittask->measured_values_v.size();

    assert( gfittask->computed_values_v.size() == gfittask->measured_values_v.size() );

    for(i=0; i< number_of_measurements; i++ )
    {
        ResSumSquares += pow( (gfittask->measured_values_v[i] - gfittask->computed_values_v[i]), 2);
        TotalSumSquares += pow( (gfittask->measured_values_v[i] - mean), 2);
    }
    coeff_of_determination = 1 - ResSumSquares / TotalSumSquares;
    ResSumSquares = 0.0; TotalSumSquares = 0.0;

    // Compute R^2 for each objfun term
    for (unsigned j = 0; j<gfittask->Tfun->objfun.size(); j++)
    {
        for (unsigned i=0; i<gfittask->aTfun.size(); i++)
        {
            if (gfittask->aTfun[i].objfun[j].isComputed)
            {
                ResSumSquares += pow( (gfittask->aTfun[i].objfun[j].results.measured_value - gfittask->aTfun[i].objfun[j].results.computed_value), 2);
                TotalSumSquares += pow( (gfittask->aTfun[i].objfun[j].results.measured_value - objfun_stat[j]->mean_meas), 2);
            }
        }
        objfun_stat[j]->R2 = 1 - ResSumSquares / TotalSumSquares;
    }

    // Pearson Chi Square test
    Pearsons_chi_square = 0.;
    for( i=0;  i< number_of_measurements; i++ )
    {
        if (gfittask->measured_values_v[i] != 0)
        Pearsons_chi_square += (abs(gfittask->computed_values_v[i]) - abs(gfittask->measured_values_v[i]))*(abs(gfittask->computed_values_v[i]) - abs(gfittask->measured_values_v[i])) / abs(gfittask->measured_values_v[i]);
    }

    // Reduced Chi Square
    reduced_chi_square = Pearsons_chi_square / degrees_of_freedom;

    // Error variance
    error_variance = Weighted_Tfun_sum_of_residuals/degrees_of_freedom;

    // Generate object containing normally distributed data with mean = 0 and standard deviation = SD_of_residuals
    if (gfittask->residuals_v.size() > 1)
    {
        unsigned int N = (int) gfittask->residuals_v.size();
        try {
            boost::math::normal dist(  0., SD_of_residuals );

            sort( gfittask->residuals_v.begin(), gfittask->residuals_v.end() );
            sort(weighted_residuals.begin(), weighted_residuals.end());

            // Compute percentile
            for( i=0; i< N; i++ )
                percentiles_v.push_back( (i+1-0.5)/N );

            // Compute Correlation coeficient
            arma::vec e0 = arma::zeros<arma::vec>( N, 1 );
            arma::vec tau = arma::zeros<arma::vec>( N, 1 );
            for( i=0; i< N; i++ )
            {
                e0(i) = weighted_residuals[i]-Weighted_mean_res;
                tau(i) = percentiles_v[i];
            }

            double sum1=0.0, sum2=0.0, sum3=0.0;
            for (i=0; i<N; i++)
            {
                sum1 += e0(i)*tau(i);
                sum2 += e0(i)*e0(i);
                sum3 += tau(i)*tau(i);
            }
            //    Correlation_coef = (sum1*sum1)/(sum2*sum3);

            // Rank-based z-scores
            for( i=0; i< N; i++ )
                quantiles_v.push_back( boost::math::quantile( dist, percentiles_v[i] ) );

        }
        catch(const std::exception& e)
        {
            std::cout <<
                         "\n""Message from thrown exception was:\n   " << e.what() << std::endl;
        }

        // Generate Q-Q Plot (Quantile-Quantile Plot) for each term of the objfun

        for (unsigned j = 0; j < objfun_stat.size(); j++)
        {
            if (objfun_stat[j]->isComputed)
            {
                try
                {
                    boost::math::normal distobj(  0., objfun_stat[j]->stdev_res );
                    for (i = 0; i < objfun_stat[j]->nr; i++)
                    {
                        objfun_stat[j]->percentiles.push_back( (i+1-0.5)/ objfun_stat[j]->nr );
                        objfun_stat[j]->quantiles.push_back( boost::math::quantile( distobj, objfun_stat[j]->percentiles[i] ));
                    }
                    // sorting residuals
                    sort( objfun_stat[j]->orderd_res.begin(), objfun_stat[j]->orderd_res.end() );
                }
                catch (boost::exception const&  ex  )
                {
                    noqq = true;
                }
            }
        }


    // Generate Q-Q Plot (Quantile-Quantile Plot)
    int exp = 0;

    if (!noqq)
    {
    gpf->fqq << "sample,";
    for (unsigned k = 0; k<gfittask->Tfun->objfun.size(); k++)
    {
        gpf->fqq << gfittask->Tfun->objfun[k].exp_phase + "." + gfittask->Tfun->objfun[k].exp_CN;
        gpf->fqq << ".residual"<<",";
        gpf->fqq << gfittask->Tfun->objfun[k].exp_phase + "." + gfittask->Tfun->objfun[k].exp_CN;
        gpf->fqq << ".quantile"<<",";
    }

    gpf->fqq << std::endl;
    bool waswritten = false;
    for( unsigned int j=0; j<  N;  j++ )
    {
        j--;
        gpf->fqq << gfittask->experiments[exp]->sample << ",";
        for (unsigned p = 0; p<gfittask->Tfun->objfun.size(); p++)
        {
            if (gfittask->aTfun[exp].objfun[p].isComputed)
            {
                j++;
                for (i = 0; i< objfun_stat[p]->orderd_res.size(); i++)
                {
                    if ((gfittask->aTfun[exp].objfun[p].results.residual == objfun_stat[p]->orderd_res[i]) && (!waswritten))
                    {
                    gpf->fqq <<objfun_stat[p]->orderd_res[i]<<","<<objfun_stat[p]->quantiles[i]<< ",";
                    waswritten = true;
                    }
                }
            } else
            {
                for(unsigned j=0; j<  optv_.size(); j++ )
                {
                    gpf->fqq << ",";
                }
            }
            waswritten = false;
        }
        exp++;
        gpf->fqq << std::endl;
    }
    gpf->fqq.close();
}

//    gpf->fqq << std::endl;
//    bool waswritten = false;
//    for( int j=0; j<  N;  j++ )
//    {
//        j--;
//        gpf->fqq << gfittask->experiments[exp]->sample << ",";
//        for (unsigned p = 0; p<gfittask->Tfun->objfun.size(); p++)
//        {
//            if (gfittask->aTfun[exp].objfun[p].isComputed)
//            {
//                j++;
//                for (i = 0; i< gfittask->residuals_v.size(); i++)
//                {
//                    if ((gfittask->aTfun[exp].objfun[p].results.residual == gfittask->residuals_v[i]) && (!waswritten))
//                    {
//                    gpf->fqq <<gfittask->residuals_v[i]<<","<<quantiles_v[i]<< ",";
//                    waswritten = true;
//                    }
//                }


//            } else
//            {
//                for(unsigned j=0; j<  optv_.size(); j++ )
//                {
//                    gpf->fqq << ",";
//                }
//            }
//            waswritten = false;
//        }
//        exp++;
//        gpf->fqq << std::endl;
//    }
//    gpf->fqq.close();

    // D'Agostino K square test for normality
    mean = 0;
    for (unsigned i=0; i< gfittask->residuals_v.size(); i++)
    {
        mean += weighted_residuals[i];
    }
    mean = mean / gfittask->residuals_v.size();

    for( i=0; i<N; i++ )
    {
        m2 += pow( (weighted_residuals[i] - mean), 2. ) / N;
        m3 += pow( (weighted_residuals[i] - mean), 3. ) / N;
        m4 += pow( (weighted_residuals[i] - mean), 4. ) / N;
    }

    sqrtb1 = m3 / pow( m2, (3./2.));
    b2     = (m4 / pow( m2, 2.))-3;

    // D'Agostino K square test: test of skewness
    Y             = sqrtb1 * sqrt( ((N+1.)*(N+3.))/(6*(N-2.)) );
    beta2_sqrtb1  = ( 36*(N*N + 2*N - 5)*(N - 7.)  ) / ( (N - 2.)*(N + 5.)*(N + 7.)*(N + 9.) );
    W2            = -1. + sqrt(2 * (beta2_sqrtb1 + 2.) );
    if( W2 < 1.)  // workaround to suppress nan() and zdiv crash
         W2 = 1.001;
    delta         = 1. / sqrt( log( sqrt(W2) ) ) ;
    alpha         = sqrt( 2. / (W2 - 1) );
    Z_sqrtb1      = delta * log( Y / alpha + sqrt( pow( (Y/alpha), 2 ) + 1 ) );

    // D'Agostino K square test: test of kurtosis
    // mean of b2
    E_b2          = -6./(N+1.);
    // variance of b2
    Var_b2        = ( 24.*N*(N - 2.)*(N - 3.) ) / ( (N + 1.)*(N + 1.)*(N + 3.)*(N + 5.) );
    // standardized version of b2
    x             = ( b2 - E_b2 ) / ( sqrt(Var_b2) );
    // third standardized moment of b2
    sqrt_beta1_b2 = 6.*(N*N - 5.*N + 2.) / ( (N + 7.)*(N + 9.) ) * sqrt(( 6. * (N + 3.)*(N + 5.) )/ ( N * (N - 2.) * (N - 3.) ) );
    A             = 6. + ( 8. / sqrt_beta1_b2 ) * ( 2. / sqrt_beta1_b2 + sqrt( 1 + 4./(sqrt_beta1_b2*sqrt_beta1_b2) ) );
    Z_b2          = ( (1.-2./(9.*A)) - pow( ((1.-2./A) / (1 + x*sqrt(2./(A - 4.))) ), (1./3.) ) ) / sqrt( 2./(9.*A) );

    // K square
    K2 = Z_sqrtb1*Z_sqrtb1 + Z_b2*Z_b2;
}

    // Create chi-squared distribution object
//    if(W2 != 1){
//    boost::math::chi_squared chi_dist( 2 );
    // 1 - cumulative distribution function
//    K2test = 1 - boost::math::cdf( chi_dist, K2 );}


    // Write first statistcs to file
//        gpf->fstat << "############################################" << std::endl;
//        gpf->fstat << "# Summary and statistics from Gemsfits run #" << std::endl;
//        gpf->fstat << "############################################" << std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Best fit results for parameters from regression : "                         <<std::endl;
//        for(unsigned i=0; i< optv_.size(); i++ ) // cols
//        {
//            // Print optimized parameter values to file
//            if (gfittask->Opti->Ptype[i] == "G0")
//            {
//                gpf->fstat <<"          parameter G0 "<<
//                         gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i])
//                       <<" : " << optv_[i] << std::endl;
////                cout /*<< gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i])*/ << std::endl;
//            } else
//              gpf->fstat <<"            parameter " << gfittask->Opti->Ptype[i] << " : "
//                     << setprecision(8) << optv_[i] << std::endl;
//        }
//        if (gfittask->Opti->h_RDc)
//        {
//            for (unsigned i=0; i<gfittask->Opti->reactions.size(); ++i)
//            {
//                gpf->fstat <<"          Reac parameter "<<gfittask->Opti->reactions[i]->Dc_name <<" : "<<setprecision(8)<<gfittask->Opti->reactions[i]->std_gibbs<<std::endl;
//            }
//        }
//        if (gfittask->Opti->h_Lp)
//        {
//            for (unsigned i=0; i<gfittask->Opti->Lparams.size(); ++i)
//            {
//                gpf->fstat <<"          Linked parameter "<<gfittask->Opti->Lparams[i]->name <<" : "<<setprecision(8)<<gfittask->Opti->Lparams[i]->EV<<std::endl;
//            }
//        }
//        gpf->fstat << std::endl;
//        gpf->fstat << " Basic statistics:" << std::endl;
        gpf->fstat << "Statistics-Parameter,Value" << std::endl;
        gpf->fstat << "Number-of-measurements," << number_of_measurements     << std::endl;
        gpf->fstat << "Number-of-parameters," << number_of_parameters 	<< std::endl;
        gpf->fstat << "Number-of-runs-needed-for-regression," << num_of_runs 		<< std::endl;
        gpf->fstat << "Degrees-of-freedom," << degrees_of_freedom	 	<< std::endl;
//        gpf->fstat << " - - - - - - - RESIDUAL STATISTICS - - - - - - - " << std::endl;
        gpf->fstat << "Value-of-the-minimized-function-sum," << Weighted_Tfun_sum_of_residuals 		<< std::endl;
//        gpf->fstat << " Target function sum  :                                                	" << Tfun_sum_of_residuals 		<< std::endl;
//        gpf->fstat << std::endl;
        gpf->fstat << "Sum-of-absolute-values-of-residuals," << Abs_sum_of_residuals << std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Sum-of-absolute-values-of-residuals:        " << Weighted_Abs_sum_of_residuals << std::endl;
        gpf->fstat << "Standard-deviation-of-residuals," << SD_of_residuals 		<< std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Standard deviation of the residuals :       " << Weighted_SD_of_residuals 		<< std::endl;
//        gpf->fstat << " Standard deviation of the weighted target function values :             " << Weighted_TF_SD_of_residuals 		<< std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Standard deviation of the absolute values of the residuals :            " << Abs_SD_of_residuals 		<< std::endl;
//        gpf->fstat << std::endl;
        gpf->fstat << "Average-residual," << Weighted_mean_res 		<< std::endl;
        gpf->fstat << "Maximum-residual," << Weighted_max_res 		<< std::endl;
        gpf->fstat << "Minimum-residual," << Weighted_min_res 		<< std::endl;
        gpf->fstat << "Number-of-positive-residuals," << pos_residuals 		<< std::endl;
        gpf->fstat << "Number-of-negative-residuals," << neg_residuals 		<< std::endl;
        gpf->fstat << "Coefficient-of-determination-R^2," << coeff_of_determination     << std::endl;
//        gpf->fstat << " Correlation coefficient (Brockwell and Davis, 1987, p304) RN^2:         " << Correlation_coef     << std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Pearson's Chi Square test :                                             " << Pearsons_chi_square   	<< std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Reduced_chi_square :                                                	" << reduced_chi_square 	<< std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Error variance :                                                     	" << error_variance 	<< std::endl;
//        gpf->fstat << std::endl;

//        gpf->fstat << " D'Agostino K square test, K squared :                                   " << K2 << std::endl;
//        gpf->fstat << std::endl;
//        gpf->fstat << " Probability that K squared follows Chi-squared distribution :           " << K2test << std::endl;
//        gpf->fstat << std::endl;
        gpf->fstat <<", ," << std::endl;
//        gpf->fstat.close();

        for (unsigned o = 0; o<gfittask->Tfun->objfun.size(); o++)
        {
            gpf->fstat << "OFUN-"<< gfittask->Tfun->objfun[o].exp_CT << "-"<< gfittask->Tfun->objfun[o].exp_CN << ","<<std::endl;
            gpf->fstat << "minimized-function-sum-("<<std::setprecision(2) << objfun_stat[o]->SumWTFun*100/Weighted_Tfun_sum_of_residuals<<"%-of-total)," << objfun_stat[o]->SumWTFun 	<<std::endl;
            gpf->fstat << "Standard-deviation-of-residuals," << objfun_stat[o]->stdev_res 		<< std::endl;
            gpf->fstat << "Mean-squared-error," << objfun_stat[o]->MSE 		<< std::endl;
            gpf->fstat << "Root-mean-squared-error," << sqrt(objfun_stat[o]->MSE)		<< std::endl;
            gpf->fstat << "Average-residual," << objfun_stat[o]->mean_res 		<< std::endl;
            gpf->fstat << "Maximum-residual," << objfun_stat[o]->max_res 		<< std::endl;
            gpf->fstat << "Minimum-residual," << objfun_stat[o]->min_res 		<< std::endl;
            gpf->fstat << "Number-of-positive-residuals," << objfun_stat[o]->nr_pos_res 		<< std::endl;
            gpf->fstat << "Number-of-negative-residuals," << objfun_stat[o]->nr_neg_res 		<< std::endl;
            gpf->fstat << "Coefficient-of-determination-R^2," << objfun_stat[o]->R2     << std::endl;

            gpf->fstat <<", ," << std::endl;
        }

        gpf->fqq.close();
}

double round_to_digits(double value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}


// perform sensitivity analysis of selected parameters
void statistics::sensitivity_correlation( std::vector<double> &optv_, TGfitTask* gfittask )
{
        unsigned int i, j, k, len_meas;
        double residual_sys;

        std::vector<double> computed_up;
        std::vector<double> computed_lo;
        std::vector<double> opt_scan;
//        std::vector<double> ssr_param;
//        std::vector<double> opt_scan_v;

        int prec = 12;

        opt_scan.resize( optv_.size() );
//        ssr_param.resize( sensitivity_points );
//        opt_scan_v.resize( sensitivity_points );
//        std::vector< std::vector< double> > array_ssr;

        len_meas = (int) gfittask->Weighted_Tfun_residuals_v.size();

        // Compute Jacobian matrix (= Sensitivity matrix)
        arma::mat SensitivityMatrix( len_meas, (int) optv_.size() );
        arma::mat DimensionlessScaledSensitivities( len_meas, (int) optv_.size() );
        arma::mat OnePercentScaledSensitivities( len_meas, (int) optv_.size() );
        arma::mat WeightMatrix = arma::zeros<arma::mat>(len_meas, len_meas);
        arma::vec CompositeScaledSensitivities = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec ParameterStandardDeviation = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec CoefficientOfVariation = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec Parameter_t_Statistic = arma::zeros<arma::vec>( (int) optv_.size(), 1 );


        double delta = perturbator;
        for( i=0; i< optv_.size(); i++ )
        {
            opt_scan = optv_;
            std::vector<double> sens;

            // Central finite differences:
            opt_scan[i] = optv_[i] + optv_[i]*delta;
            residual_sys = 0.;
            if (gfittask->Opti->OptEquilibrium)
            gems3k_wrap( residual_sys, opt_scan, gfittask );
            else tsolmod_wrap(residual_sys, opt_scan, gfittask);
//std::cout<<"residual_sys = "<<residual_sys<<std::endl;
            computed_up = gfittask->computed_values_v;

            opt_scan[i] = optv_[i] - optv_[i]*delta;
            residual_sys = 0.;
            if (gfittask->Opti->OptEquilibrium)
            gems3k_wrap( residual_sys, opt_scan, gfittask );
            else tsolmod_wrap(residual_sys, opt_scan, gfittask);
//            gems3k_wrap( residual_sys, opt_scan, gfittask );
//std::cout<<"residual_sys = "<<residual_sys<<std::endl;
            computed_lo = gfittask->computed_values_v;

            for( k=0; k< len_meas; k++ )
            {
                SensitivityMatrix(k,i) 	 			  = ( computed_up[k] - computed_lo[k] ) / ( optv_[i]*delta*2 );
                sens.push_back(SensitivityMatrix(k,i));

                // OnePercentScaledSensitivities(k,i) = (SensitivityMatrix(k,i)) * (fabs(optv_[i]) / 100); // DM 24.09.2018
                OnePercentScaledSensitivities(k,i) = (SensitivityMatrix(k,i)) * (fabs(optv_[i]) / 100) * (100*sqrt(gfittask->weights[k]));

/*                if (gfittask->Tfun->weight == keys::inverr) // the weight matrix is added (weighting with 1/error)
                {
                    DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * gfittask->weights[k];
                } else
                    if ((gfittask->Tfun->weight == keys::inverr2) || (gfittask->Tfun->weight == keys::inverr3) || (gfittask->Tfun->weight == keys::inverr_norm)) // no re
                    {
                        DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * sqrt(gfittask->weights[k]);
                    }
                    else */ DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * sqrt(gfittask->weights[k]);
                CompositeScaledSensitivities(i)      += sqrt( (DimensionlessScaledSensitivities(k,i)*DimensionlessScaledSensitivities(k,i))/len_meas );
            }
//            gfittask->print->sensitivity.push_back(sens);
        }


//for (unsigned pr = 0; pr <optv_.size(); pr++)
//{
//    arma::vec CompositeScaledSensitivities2 = arma::zeros<arma::vec>( (int) gfittask->Tfun->objfun.size(), 1 );
//    int meas =0;
//        for ( j=0; j<gfittask->Tfun->objfun.size(); ++j)
//        {
//            arma::mat DimensionlessScaledSensitivities2( objfun_stat[j]->nr, (int) optv_.size() );
//            int nr = 0;
//            for (i = 0; i < gfittask->aTfun.size(); i++)
//            {
//                if (gfittask->aTfun[i].objfun[j].isComputed)
//                {
//                DimensionlessScaledSensitivities2(nr, pr) = SensitivityMatrix(meas,pr) * fabs( gfittask->aTfun[i].objfun[j].results.measured_value );
//                nr++;
//                meas++;

//                CompositeScaledSensitivities2(j) += sqrt( DimensionlessScaledSensitivities(nr, pr)*DimensionlessScaledSensitivities(nr, pr)/objfun_stat[j]->nr);
//                }
//            }

//            cout << CompositeScaledSensitivities2(j) << std::endl;
//        }

//}

        if (gfittask->Opti->OptEquilibrium)
        gems3k_wrap( residual_sys, optv_, gfittask );
        else tsolmod_wrap( residual_sys, optv_, gfittask );


//        gpf->fstat << " Sensitivity matrix over each parameter [j] measurement point [i]: "<<std::endl;
//        gpf->fstat << " Calculated using central diferences, see ref. [1] section 4.3 "<<std::endl;
        int exp = 0;

        gpf->fsens << "sample,";

        size_t n_Rparam = 0;
        for (size_t e = 0; e<gfittask->Opti->optParam.size(); e++)
        {
            n_Rparam += gfittask->Opti->optParam[e]->Get_optRPsize();
        }

        for (k = 0; k<gfittask->Tfun->objfun.size(); k++)
        {
            for (i=0; i < (fitparam.size() - n_Rparam) ; i++)
            {
                gpf->fsens << gfittask->Tfun->objfun[k].exp_phase + "." + gfittask->Tfun->objfun[k].exp_CN;
                gpf->fsens << ".param." << i <<",";
            }
        }

        gpf->fsens << std::endl;

        for( i=0; i<  computed_up.size();  i++ )
        {
            i--;
            gpf->fsens << gfittask->experiments[exp]->sample << ",";
            for (k = 0; k<gfittask->Tfun->objfun.size(); k++)
            {
                if (gfittask->aTfun[exp].objfun[k].isComputed)
                {
                    i++;
//                    gpf->fsens << gfittask->experiments[exp]->sample << ",";
//                    gpf->fsens << gfittask->aTfun[exp].objfun[k].exp_phase + "." + gfittask->aTfun[exp].objfun[k].exp_CN << ",";
                    for( j=0; j<  optv_.size(); j++ )
                    {
                        // Write sensitivities to file
                        gpf->fsens<< std::setprecision(prec)<<SensitivityMatrix(i,j) <<",";
                    }

                } else
                {
                    for( j=0; j<  optv_.size(); j++ )
                    {
                        gpf->fsens << ",";
                    }
                }
            }
            exp++;
            gpf->fsens << std::endl;
        }


//        gpf->fstat << " Composite Scaled Sensitivities (CSS): "<<std::endl;
        {
            // Write sensitivities to file
            for( i=0; i< optv_.size(); i++ )
            {
                fitparam[i]->CSS = CompositeScaledSensitivities(i);

//                gpf->fstat <<"            parameter "<< i;
//                         if (gfittask->Opti->Ptype[i] == "G0") gpf->fstat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
//                         else gpf->fstat << "  " << gfittask->Opti->Ptype[i];
//                gpf->fstat <<" :	     " << CompositeScaledSensitivities(i)	<< std::endl;
            }
        }
//        gpf->fstat << std::endl;


//SensitivityMatrix.print("SensitivityMatrix:");

        for(i=0; i<gfittask->weights.size(); ++i)
        {
            WeightMatrix(i,i) = gfittask->weights[i];
        }

        // Get transpose of sensitivity matrix
        arma::mat SensitivityMatrix_T = arma::trans(SensitivityMatrix);

//SensitivityMatrix_T.print("SensitivityMatrix_T:");

        // Compute Fisher matrix (is a proxy for the Hessian matrix)
        arma::mat FisherMatrix = SensitivityMatrix_T * WeightMatrix * SensitivityMatrix;


//        if ((gfittask->Tfun->weight == "inverr2") /*|| (gfittask->Tfun->weight == "inverr1")*/ )
//        {
//            FisherMatrix = SensitivityMatrix_T * sqrt(WeightMatrix) * SensitivityMatrix;
//        }
//        if ((gfittask->Tfun->weight == keys::inverr3) || (gfittask->Tfun->weight == keys::inverr) || (gfittask->Tfun->weight == keys::inverr_norm))
//        {
//            FisherMatrix = SensitivityMatrix_T * SensitivityMatrix;
//        }
//        FisherMatrix.print("Fisher Matrix:");

        // Compute variance-covariance matrix
        arma::mat VarCovarMatrix = error_variance * arma::inv( FisherMatrix);
///*        if (gfittask->Tfun->weight == "inverr2")
//        {
//            VarCovarMatrix = error_variance * arma::inv( FisherMatrix, false );
//        } else */VarCovarMatrix = 1 * arma::inv( FisherMatrix, false );
//        VarCovarMatrix.print("Variance Covariance Matrix:");

        // Print Variance-Covariance matrix to file
//        gpf->fstat << " Variance-Covariance matrix: "<<std::endl;
//        for(unsigned i=0; i< optv_.size(); i++ )
//        {
//            if( i== 0 )
//            {
//                gpf->fstat <<"                                                      ";
//                if (gfittask->Opti->Ptype[i] == "G0") gpf->fstat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
//                else gpf->fstat << "  " << gfittask->Opti->Ptype[i];
//            }
//            else
//            {
//                gpf->fstat <<"              ";
//                if (gfittask->Opti->Ptype[i] == "G0") gpf->fstat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
//                else gpf->fstat << "  " << gfittask->Opti->Ptype[i];
//            }
//        }
//        gpf->fstat << std::endl;
//        for( i=0; i<  optv_.size(); i++ )
//        {
//            gpf->fstat <<"			parameter "<< i;
//                     if (gfittask->Opti->Ptype[i] == "G0") gpf->fstat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
//                     else gpf->fstat << "  " << gfittask->Opti->Ptype[i];
//            for( j=0; j<  optv_.size(); j++ )
//            {
//                if( j==0 )
//                    gpf->fstat <<"          " <<VarCovarMatrix(i,j);
//                else
//                    gpf->fstat <<"          " <<VarCovarMatrix(i,j);
//            }
//            gpf->fstat << std::endl;
//        }
//        gpf->fstat << std::endl;

        // Correlation matrix
        arma::mat CorellationMatrix( (int) optv_.size(), (int) optv_.size() );
//        CorellationMatrix.print("CorellationMatrix = ");

        for( i=0; i< optv_.size(); i++ )
        {
            for( k=i; k< optv_.size(); k++ )
            {
                // 			 			 Covar(i,k)          / sqrt( Var(k,k)            * Var(i,i)            )
                CorellationMatrix(i,k) = VarCovarMatrix(i,k) / (sqrt( VarCovarMatrix(k,k)) * sqrt(VarCovarMatrix(i,i)));
                CorellationMatrix(k,i) = CorellationMatrix(i,k);
            }
            ParameterStandardDeviation(i) = sqrt(VarCovarMatrix(i,i));
            CoefficientOfVariation(i) 	  = ParameterStandardDeviation(i) / optv_[i];
            Parameter_t_Statistic(i) 	  = 1. / CoefficientOfVariation(i);
        }


//        // Print Correlation matrix to file

        for(unsigned i=0; i <  optv_.size(); i++ )
        {
            for( j=0; j< optv_.size(); j++ )
            {
                fitparam[i]->correl.push_back(CorellationMatrix(i,j));
            }
        }


        /// significant digits
        if (gfittask->Opti->OptPrcParamDigits>0.0)
        {
            auto rounded_v = optv_;
            //Weighted_Tfun_sum_of_residuals
            double sum_residuals = residual_sys;
        for( i=0; i< optv_.size(); i++ )
        {
            double diff_prec = 0.0;
//            int j = 1;
            int x = 1;

            do
            {
                opt_scan = optv_;
                double param = optv_[i];

                opt_scan[i] = round_to_digits(param, x);
                //double round_val = round( opt_scan[i] * (10*j) );
                //opt_scan[i] = round_val / (10*j);

                if (gfittask->Opti->OptEquilibrium)
                gems3k_wrap( residual_sys, opt_scan, gfittask );
                else tsolmod_wrap(residual_sys, opt_scan, gfittask);

                diff_prec = fabs((sum_residuals-residual_sys)/sum_residuals*100);
                j= 10*j;
                x++;
                std::cout << "parameter " << opt_scan[i] << " roundoff % error " << diff_prec << std::endl;
            } while (diff_prec>gfittask->Opti->OptPrcParamDigits);
            fitparam[i]->Roundval = opt_scan[i];
            rounded_v[i] = opt_scan[i];
           // gfittask->fitparam
        }

        if (gfittask->Opti->OptEquilibrium)
        gems3k_wrap( residual_sys, rounded_v, gfittask );
        else tsolmod_wrap(residual_sys, rounded_v, gfittask);

        double diff_prec2 = fabs((sum_residuals-residual_sys)/sum_residuals*100);
        std::cout << "Total roundoff % error " << diff_prec2 << std::endl;

        }

        gpf->fstat.close();

}


void statistics::MC_confidence_interval( std::vector<double> &optv_, TGfitTask* gfittask/*, int &countit */)
{

//std::cout<<"pid : "<<pid<<" entered Statistics::MC_confidence_interval ..."<<std::endl;

    unsigned int i,n_param,id,imc;
    unsigned int j, p_=1, pid_=0; // k
//    double sum_of_squares_MC;
    std::vector<double> scatter_v, MC_computed_v;
    std::vector<double> optv_backup;
    std::vector<std::vector<double> > measured_values_backup;
    std::vector<std::vector<double> > computed_values_backup;
    std::vector<double> sum_res;

    gpf->fmc.open(gpf->FITmcFile().c_str(), std::ios::trunc);
    if( gpf->fmc.fail() )
    { std::cout<<"Fit Monte Carlo fileopen error"<<std::endl; exit(1); }

    unsigned int n_Rparam = 0;
    for (unsigned e = 0; e<gfittask->Opti->optParam.size(); e++)
    {
        n_Rparam += gfittask->Opti->optParam[e]->Get_optRPsize();
    }

//     = gfittask->Opti->reactions.size();

    scatter_v.resize(number_of_measurements);
    MC_computed_v.resize(number_of_measurements);

    // loop over systems and backup measurement values and computed values
        // Store originals measurements and computed values
        computed_values_backup.push_back( gfittask->computed_values_v );
        measured_values_backup.push_back( gfittask->measured_values_v );
        optv_backup = optv_;

    n_param = (int) optv_.size();

    double*  MC_fitted_parameters_all_storage = (double *) malloc ( num_of_MC_runs * n_param * sizeof(double) );
    double** MC_fitted_parameters_all		  = (double **) malloc ( num_of_MC_runs * sizeof(double *) );
    for( i=0; i<num_of_MC_runs; i++ )
        MC_fitted_parameters_all[i] = &MC_fitted_parameters_all_storage[ i * n_param ];

    double*  MCR_fitted_parameters_all_storage = (double *) malloc ( num_of_MC_runs * n_Rparam * sizeof(double) );
    double** MCR_fitted_parameters_all		  = (double **) malloc ( num_of_MC_runs * sizeof(double *) );
    for( i=0; i<num_of_MC_runs; i++ )
        MCR_fitted_parameters_all[i] = &MCR_fitted_parameters_all_storage[ i * n_Rparam ];

    //double* scatter_all 	  = new double[ number_of_measurements * num_of_MC_runs ];

    int count = 0;

    if ((MCbool == 5) || (MCbool == 6) || (MCbool == 8) || (MCbool == 10))
    {
        for (i = 0; i < num_of_MC_runs; i++)
        {
            for (j = 0; j<gfittask->Tfun->objfun.size(); j++)
            {
                for (unsigned d = 0; d < objfun_stat[j]->exp_dataset.size(); d++)
                {
                    if ((MCbool != 8))
                    {
                        if (MCbool != 10)
                        for ( unsigned int r = 0; r < objfun_stat[j]->exp_dataset[d].residuals.size(); r++ )
                        {
                            // mersenne twister generator
                            typedef boost::mt19937 RNGType;
                            RNGType rng(i+j+d+r+1);
                            boost::normal_distribution<> rdist(0, fabs(objfun_stat[j]->exp_dataset[d].residuals[r]));
                            boost::variate_generator< RNGType, boost::normal_distribution<> > get_rand(rng, rdist);

                            objfun_stat[j]->exp_dataset[d].scatter[r] = get_rand();;
                            count ++;
                        } else
                            for ( unsigned int r = 0; r < objfun_stat[j]->exp_dataset[d].measured_error.size(); r++ )
                            {
                                double sigma = fabs(objfun_stat[j]->exp_dataset[d].measured_error[r]);
                                // mersenne twister generator
                                typedef boost::mt19937 RNGType;
                                RNGType rng(i+j+d+r+1);
                                boost::normal_distribution<> rdist(0, sigma);
                                boost::variate_generator< RNGType, boost::normal_distribution<> > get_rand(rng, rdist);

                                objfun_stat[j]->exp_dataset[d].scatter[r] = get_rand();;
                                count ++;
                            }
                    }
                    else
                    {
                        for ( unsigned int r = 0; r < objfun_stat[j]->exp_dataset[d].residuals.size(); r++ )
                        {
                            double fivp = fabs(gfittask->measured_values_v[i])*0.05;
                            double fifp = fabs(gfittask->measured_values_v[i])*0.15;
                            double sigma = fabs(objfun_stat[j]->exp_dataset[d].residuals[r]);
                            if (sigma< fivp)
                                sigma = fivp;
                            if (sigma> fifp)
                                sigma = fifp;
                            // mersenne twister generator
                            typedef boost::mt19937 RNGType;
                            RNGType rng(i+j+d+r+1);
                            boost::normal_distribution<> rdist(0, sigma);
                            boost::variate_generator< RNGType, boost::normal_distribution<> > get_rand(rng, rdist);

                            objfun_stat[j]->exp_dataset[d].scatter[r] = get_rand();;
                            count ++;
                        }
                    }
                    // add to the scatter
                    objfun_stat[j]->scatter.insert( objfun_stat[j]->scatter.end(), objfun_stat[j]->exp_dataset[d].scatter.begin(), objfun_stat[j]->exp_dataset[d].scatter.end() );
                }
            }
        }
    }
    if ((MCbool == 3) || (MCbool == 4)) // bootstrap sampling
    {
        for (i = 0; i < num_of_MC_runs; i++)
        {
            for (j = 0; j<gfittask->Tfun->objfun.size(); j++)
            {
                for (unsigned d = 0; d < objfun_stat[j]->exp_dataset.size(); d++)
                {
                    typedef boost::mt19937 RNGType;
                    RNGType rng(i+1);
                    boost::uniform_int<> interval ( 0 ,  objfun_stat[j]->exp_dataset[d].residuals.size()-1 );
                    boost::variate_generator< RNGType, boost::uniform_int<> >
                                  dice(rng, interval);
                    for ( unsigned int r = 0; r < objfun_stat[j]->exp_dataset[d].residuals.size(); r++ )
                    {
                        int x  = dice();
                        // the residual will be de-normalized when added to the scatter std::vector in the MC runs
                        objfun_stat[j]->exp_dataset[d].scatter[r] = objfun_stat[j]->exp_dataset[d].norm_residuals[x] /* * objfun_stat[j]->exp_dataset[d].measured_value[r]*/;
                        count ++;
                    }
                    // add to the scatter
                    objfun_stat[j]->scatter.insert( objfun_stat[j]->scatter.end(), objfun_stat[j]->exp_dataset[d].scatter.begin(), objfun_stat[j]->exp_dataset[d].scatter.end() );
                }
            }
        }
    }
    if ((MCbool == 1) || (MCbool == 2)) // sampling per each individual objfun
    {
        // make scatter object std::vector for each objfun
        for (j = 0; j<gfittask->Tfun->objfun.size(); j++)
        {
            for (i = 0; i< num_of_MC_runs * objfun_stat[j]->nr; i++)
            {
                objfun_stat[j]->scatter.push_back(0.0);
            }
        }

        // get the scatter values for each individual objfun for all MC runs
        for (j= 0; j<gfittask->Tfun->objfun.size(); j++)
        {
            // mersenne twister generator
            typedef boost::mt19937 RNGType;
            RNGType rng(j+1);

            boost::normal_distribution<> rdist(objfun_stat[j]->norm_mean_res, objfun_stat[j]->norm_stdev_res);

            boost::variate_generator< RNGType, boost::normal_distribution<> > get_rand(rng, rdist);

            for( i=0; i<(objfun_stat[j]->nr * num_of_MC_runs); i++ )
            {
                 objfun_stat[j]->scatter[i] = get_rand();
            }
        }
    } /*else
    {
        if (MCbool != 0)
        cout << "MCBool doesn't have a correct value between 0-4! "<< std::endl;
        exit(1);
    }*/

    pid_ = 0;
    id = 0;
    // Perform Monte Carlo runs
    for( imc=pid_; imc<num_of_MC_runs; ++imc /*+= p*/)
    {
//        sum_of_squares_MC = 0.;

        gpf->flog << " #MC: " << imc << std::endl;
        std::cout << " #MC: " << imc << std::endl;

        i= 0;
        // make the scatter std::vector by adding the scatter value for each objfun for the #imc mc run
        for (j = 0; j<objfun_stat.size(); j++)
        {
            for (unsigned n = imc*objfun_stat[j]->nr; n < ((imc*objfun_stat[j]->nr) + objfun_stat[j]->nr); n++)
            {
                scatter_v[i] = objfun_stat[j]->scatter[n];
                i++;
            }
        }

        if ((MCbool == 1) || (MCbool == 3) || (MCbool == 5))// the new simulated values are from scatter + computed values
            for (i=0; i<number_of_measurements; i++)
            {
                if (MCbool == 5)
                    MC_computed_v[i] = scatter_v[i] + gfittask->computed_values_v[i];
                    else
                    MC_computed_v[i] = (scatter_v[i]*gfittask->measured_values_v[i]) + gfittask->computed_values_v[i];
            }
        else
            if ((MCbool == 2) || (MCbool == 4) || (MCbool == 6) || (MCbool == 8) || (MCbool == 10))  // the new simulated values are from scatter + measured values
            {
                for (i=0; i<number_of_measurements; i++)
                {
                    if (MCbool == 6 || MCbool == 8 || MCbool == 10)
                        MC_computed_v[i] = scatter_v[i] + gfittask->measured_values_v[i];
                        else
                        MC_computed_v[i] = (scatter_v[i]*gfittask->measured_values_v[i]) + gfittask->measured_values_v[i];
                }
            }

        // adding the new syntetic measured values
        gfittask->add_MC_scatter(MC_computed_v);


        optv_ = optv_backup;
        // always staring form the fitted parameters
        gfittask->Opti->optv_0 = optv_backup;
//        optv_ = gfittask->Opti->opt;

        gfittask->Ainit_optim( optv_ ); //, sum_of_squares_MC);

        sum_res.push_back(gfittask->weighted_Tfun_sum_of_residuals);

        // Store fitted parameters of each run in process specific array
        for( j=0; j<n_param; j++ )
        {
            MC_fitted_parameters_all[ id ][ j ] = optv_[ j ];

        }

        // Store reaction parameters
        std::string name; double IV, EV;
        j=0;

        for (size_t e = 0; e<gfittask->Opti->optParam.size(); e++)
        {
            for (size_t r = 0; r< gfittask->Opti->optParam[e]->Get_optRPsize(); r++)
            {
                gfittask->Opti->optParam[e]->Get_Rparam(r, name, IV, EV );
                MCR_fitted_parameters_all[ id ][ j ] = EV;
                j++;
            }
        }



        // loop over systems and subtract MC scatter to retain the original measurement data
        // Retain original computed values
        gfittask->computed_values_v = computed_values_backup[0];
        // Retain original measurements
        gfittask->measured_values_v = measured_values_backup[0];
        // puts back the original mesured values
        gfittask->add_MC_scatter(measured_values_backup[0]);

        id++;
    }// end Monte Carlo for-loop

    double residual_sys;
    residual_sys = gfittask->get_sum_of_residuals( );


    if (gfittask->Opti->OptEquilibrium)
    gems3k_wrap( residual_sys, optv_backup, gfittask );
    else tsolmod_wrap( residual_sys, optv_backup, gfittask );

        double StandardDeviation = 0.;
        arma::vec MCparams( num_of_MC_runs );
        arma::vec MCRparams( num_of_MC_runs );


        std::vector<double> SD_Fparam, SD_Rparam, MN_Fparam, MN_Rparam;

        gpf->fstat << " Confidence intervals of MC parameters : "<<std::endl;
        gpf->fstat << " -> standard deviation of parameters generated during Monte Carlo runs "<<std::endl;



        for( j=0; j<n_param; j++ ) // cols
        {
            for( i=0; i<num_of_MC_runs; i++ ) // rows
            {
                MCparams(i) = MC_fitted_parameters_all[ i ][ j ];
            }
            // compute standard deviation of generated parameters
            StandardDeviation = arma::stddev( MCparams, 0 );
            SD_Fparam.push_back(StandardDeviation);
            double mean = arma::mean( MCparams );
            MN_Fparam.push_back(mean);

//            // Print Standard Deviations of parameters generated during MC runs to file
//            gpf->fstat <<"			parameter "<< j;
//            if (gfittask->Opti->Ptype[j] == "G0") gpf->fstat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[j]);
//            else gpf->fstat << "  " << gfittask->Opti->Ptype[j];
//            gpf->fstat <<" :	           " << StandardDeviation << std::endl;

        }

        for( j=0; j<n_Rparam; j++ ) // cols
        {
            for( i=0; i<num_of_MC_runs; i++ ) // rows
            {
                MCRparams(i) = MCR_fitted_parameters_all[ i ][ j ];
            }
            // compute standard deviation of generated parameters
            StandardDeviation = arma::stddev( MCRparams, 0 );
            SD_Rparam.push_back(StandardDeviation);
            double mean = arma::mean( MCRparams );
            MN_Rparam.push_back(mean);

//            // Print Standard Deviations of parameters generated during MC runs to file
//            gpf->fstat <<"			parameter "<< j;
//            if (gfittask->Opti->Ptype[j] == "G0") gpf->fstat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[j]);
//            else gpf->fstat << "  " << gfittask->Opti->Ptype[j];
//            gpf->fstat <<" :	           " << StandardDeviation << std::endl;

        }
        gpf->fstat << std::endl;
        gpf->fstat.close();


//        ofstream gpf->fmc;
//        std::string path2 = gpf->OutputDirPath() + "gpf->fmc.csv";
//        gpf->fmc.open(path2.c_str(), ios::trunc);


        // print mc-results.csv
//        gpf->fmc << "MC_run,";
        int p=0;

        for( j=0; j<n_param; j++ ) // cols
        {
            gpf->fmc << "F."<<fitparam[j]->Ptype<<"."<<fitparam[j]->Pname<<",";
            p++;
        }

        for( j=0; j<n_Rparam; j++ ) // cols
        {
            gpf->fmc << "R."<<fitparam[j+p]->Ptype<<"."<<fitparam[j+p]->Pname<<",";
        }
        gpf->fmc << "minimized_sum";
        gpf->fmc << std::endl;

        for( i=0; i<num_of_MC_runs; i++ ) // rows
        {
//            gpf->fmc << i+1 << ",";
            for( j=0; j<n_param; j++ ) // cols
            {
                gpf->fmc <<MC_fitted_parameters_all[ i ][ j ]<<",";
            }

            for( j=0; j<n_Rparam; j++ ) // cols
            {
                gpf->fmc << MCR_fitted_parameters_all[ i ][ j ]<<",";
            }
            gpf->fmc << sum_res[i];
            gpf->fmc << std::endl;
        }

        gpf->fmc.close();
//        gpf->fmc << "Standard_deviation,";

        double T, w;
        p=0;
        int DegreesOfFreedom = number_of_measurements - number_of_parameters;
        boost::math::students_t dist( DegreesOfFreedom );

        for( j=0; j<n_param; j++ ) // cols
        {
//            gpf->fmc << SD_Fparam[j]<<",";
            T = boost::math::quantile(boost::math::complement(dist, 0.01/ 2));
            w = T * SD_Fparam[j] /*/ sqrt(double( number_of_measurements ))*/;
            fitparam[j]->mc95 = w;
            fitparam[j]->mcconfi.push_back(w);
            T = boost::math::quantile(boost::math::complement(dist, 0.05/ 2));
            w = T * SD_Fparam[j] /*/ sqrt(double( number_of_measurements ))*/;
            fitparam[j]->mcconfi.push_back(w);
            T = boost::math::quantile(boost::math::complement(dist, 0.1/ 2));
            w = T * SD_Fparam[j] /*/ sqrt(double( number_of_measurements ))*/;
            fitparam[j]->mcconfi.push_back(w);
            fitparam[j]->mcSTDEV = SD_Fparam[j];
            fitparam[j]->mcMEAN = MN_Fparam[j];
            p++;
        }
//        p--;

        for( j=0; j<n_Rparam; j++ ) // cols
        {
//            gpf->fmc << SD_Rparam[j]<<",";
            T = boost::math::quantile(boost::math::complement(dist, 0.01/ 2));
            w = T * SD_Rparam[j] /*/ sqrt(double( number_of_measurements ))*/;
            fitparam[j+p]->mc95 = w;
            fitparam[j+p]->mcconfi.push_back(w);
            T = boost::math::quantile(boost::math::complement(dist, 0.05/ 2));
            w = T * SD_Rparam[j] /*/ sqrt(double( number_of_measurements ))*/;
            fitparam[j+p]->mcconfi.push_back(w);
            T = boost::math::quantile(boost::math::complement(dist, 0.1/ 2));
            w = T * SD_Rparam[j] /*/ sqrt(double( number_of_measurements ))*/;
            fitparam[j+p]->mcSTDEV = SD_Rparam[j];
            fitparam[j+p]->mcMEAN = MN_Rparam[j];
            fitparam[j+p]->mcconfi.push_back(w);
        }


    free (MC_fitted_parameters_all[0]);
    free (MC_fitted_parameters_all);

    free (MCR_fitted_parameters_all[0]);
    free (MCR_fitted_parameters_all);

    //delete[] scatter_all;



}// end of function MC_confidence_interval

void statistics::print_param()
{

    unsigned int nrcor = 0;
    gpf->fparam.open(gpf->FITparamFile().c_str(), std::ios::trunc);
    if( gpf->fparam.fail() )
    { std::cout<<"Fit parameters fileopen error"<<std::endl; exit(1); }

    // print param
    gpf->fparam << "ptype,parameter," << "name,"	<< "init.value,"
                   << "fittted.value," << "mc.mean," << "mc.stdev," << "confi99,confi95,confi90," << "CSS.sensitivity,";

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
        if (fitparam[i]->Roundval != 0.0)
            gpf->fparam << std::setprecision(12) <<fitparam[i]->Roundval << ",";
        else
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

        if (fitparam[i]->Pfittype == "L")
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

