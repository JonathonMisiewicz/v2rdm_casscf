#ifndef FORTRAN_H
#define FORTRAN_H

/**
 * wrappers to Greg's Fortran functions
 */

#ifndef FC_SYMBOL
#define FC_SYMBOL 2
#endif

#if   FC_SYMBOL==1
#define F77NAME(x) x
#elif FC_SYMBOL==2
#define F77NAME(x) x##_
#endif

// for greg
extern "C" {
    void F77NAME(focas_interface)(double*jacobi_transformation_matrix,
                                   double*oei_full_sym,
                                   int &oei_full_sym_dim,
                                   double*tei_full_sym,
                                   long int &tei_full_sym_dim,
                                   double*d1_full_sym,
                                   int &d1_full_sym_dim,
                                   double*d2_full_sym,
                                   int &d2_full_sym_dim,
                                   int *symmetry_energy_order,
                                   int &nfrzc,
                                   int &nmo,
                                   int &nfrzv,
                                   int &nirrep_,
                                   double*jacobi_data,
                                   char*jacobi_file);
};
inline void Jacobi(double*jacobi_transformation_matrix,
                   double*oei_full_sym,
                   int &oei_full_sym_dim,
                   double*tei_full_sym,
                   long int &tei_full_sym_dim,
                   double*d1_full_sym,
                   int &d1_full_sym_dim,
                   double*d2_full_sym,
                   int &d2_full_sym_dim,
                   int *symmetry_energy_order,
                   int &nfrzc,
                   int &nmo,
                   int &nfrzv,
                   int &nirrep_,
                   double*jacobi_data,
                   char*jacobi_file){
    F77NAME(focas_interface)(jacobi_transformation_matrix,oei_full_sym,oei_full_sym_dim,tei_full_sym,tei_full_sym_dim,d1_full_sym,d1_full_sym_dim,d2_full_sym,d2_full_sym_dim,symmetry_energy_order,nfrzc,nmo,nfrzv,nirrep_,jacobi_data,jacobi_file);
};

#endif
