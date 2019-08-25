// Implementation of MMin-I/ II for unconstrained submodular minimization.
// This acts as a preprocessing step for general purpose SFM algorithms.
// Author: Rishabh Iyer (rkiyer@u.washington.edu)
#ifndef __MMINUC_INCLUDED__
#define __MMINUC_INCLUDED__

namespace smtk {
template <class Container>
void mminUC(SubmodularFunctions& f, Container& minStartSet, Container& maxStartSet);

extern template void mminUC(SubmodularFunctions& f, HashSet& minStartSet, HashSet& maxStartSet);
}
#endif
