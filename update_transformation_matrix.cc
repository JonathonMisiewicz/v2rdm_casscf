/*
 *@BEGIN LICENSE
 *
 * v2RDM-CASSCF, a plugin to:
 *
 * Psi4: an open-source quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright (c) 2014, The Florida State University. All rights reserved.
 * 
 *@END LICENSE
 *
 */

#include <psi4/psi4-dec.h>
#include <psi4/liboptions/liboptions.h>
#include <psi4/libqt/qt.h>

#include<psi4/libtrans/integraltransform.h>
#include<psi4/libtrans/mospace.h>

#include<psi4/libmints/wavefunction.h>
//#include<psi4/libmints/mints.h>
#include<psi4/libmints/vector.h>
#include<psi4/libmints/matrix.h>
//#include<../bin/fnocc/blas.h>
#include<time.h>

#include"v2rdm_solver.h"

#ifdef _OPENMP
    #include<omp.h>
#else
    #define omp_get_wtime() ( (double)clock() / CLOCKS_PER_SEC )
    #define omp_get_max_threads() 1
#endif

using namespace psi;
//using namespace fnocc;

namespace psi{ namespace v2rdm_casscf{

// update Ca/Cb matrices and repack energy-order transformation matrix as pitzer order
void v2RDMSolver::UpdateTransformationMatrix() {


    SharedMatrix temp ( new Matrix(newMO_) );
    // repack energy-order transformation matrix in pitzer order
    for (int ieo = nfrzc_; ieo < nmo_-nfrzv_; ieo++) {

        int ifull = energy_to_pitzer_order[ieo];
        int hi    = symmetry_full[ifull];
        int i     = ifull - pitzer_offset_full[hi];

        double ** tp = temp->pointer(hi);
        double ** np = newMO_->pointer(hi);
            
        for (int jeo = nfrzc_; jeo < nmo_-nfrzv_; jeo++) {

            int jfull = energy_to_pitzer_order[jeo];
            int hj    = symmetry_full[jfull];
            int j     = jfull - pitzer_offset_full[hj];

            if ( hi != hj ) continue;

            double dum = 0.0;

            for (int keo = nfrzc_; keo < nmo_-nfrzv_; keo++) {

                int kfull = energy_to_pitzer_order[keo];
                int hk    = symmetry_full[kfull];
                int k     = kfull - pitzer_offset_full[hk];

                if ( hi != hk ) continue;

                dum += np[k][j] * orbopt_transformation_matrix_[(ieo-nfrzc_)*(nmo_-nfrzc_-nfrzv_)+(keo-nfrzc_)];
            }
            
            //tp[i][j] = dum;
            tp[i][j] = orbopt_transformation_matrix_[(ieo-nfrzc_)*(nmo_-nfrzc_-nfrzv_)+(jeo-nfrzc_)];
            
        }
    }

    // reset energy-order transformation matrix:
    memset((void*)orbopt_transformation_matrix_,'\0',(nmo_-nfrzc_-nfrzv_)*(nmo_-nfrzc_-nfrzv_)*sizeof(double));
    for (int i = 0; i < nmo_-nfrzc_-nfrzv_; i++) {
        orbopt_transformation_matrix_[i*(nmo_-nfrzc_-nfrzv_)+i] = 1.0;
    }

    // update so/mo coefficient matrix (only need Ca_):
    for (int h = 0; h < nirrep_; h++) {

        double ** tp = temp->pointer(h);
        double **cap = Ca_->pointer(h);
        double **cbp = Cb_->pointer(h);

        for (int mu = 0; mu < nsopi_[h]; mu++) {

            double * temp3 = (double*)malloc(nmopi_[h] * sizeof(double));

            for (int i = 0; i < nmopi_[h]; i++) {
                double dum = 0.0;
                for (int j = 0; j < nmopi_[h]; j++) {
                    dum += cap[mu][j] * tp[i][j];
                }
                temp3[i] = dum;
            }
            for (int i = 0; i < nmopi_[h]; i++) {
                cap[mu][i] = temp3[i];
                cbp[mu][i] = temp3[i];
            }
            free(temp3);
        }
    }
    //newMO_->print();

    SharedMatrix temp2 (new Matrix(temp));
    temp2->gemm(false,false,1.0,newMO_,temp,0.0);
    newMO_->copy(temp2);

}

}}
