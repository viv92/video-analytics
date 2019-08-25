/*
 *	Scaling a submodular function h(X) = \lambda f(X) -- f is submodular and lambda > 0.
	Melodi Lab, University of Washington, Seattle
 *
 */
#include "ScaleSubmodularFunctions.h"
#include <vector>
using namespace std;

namespace smtk {
ScaleSubmodularFunctions::ScaleSubmodularFunctions(const SubmodularFunctions& in_f, const double lambda): 
SubmodularFunctions(in_f.size()), lambda(lambda){
	assert(lambda > 0);
	f = in_f.clone();
}

ScaleSubmodularFunctions::ScaleSubmodularFunctions(const ScaleSubmodularFunctions& s): 
SubmodularFunctions(s), lambda(s.lambda){
	assert(lambda > 0);
	f = s.f->clone();
}

ScaleSubmodularFunctions::~ScaleSubmodularFunctions(){
	delete f;
}

ScaleSubmodularFunctions* ScaleSubmodularFunctions::clone() const{
	return new ScaleSubmodularFunctions(*this);
}

double ScaleSubmodularFunctions::eval(const Set& sset) const{
	return lambda*f->eval(sset);
}

double ScaleSubmodularFunctions::evalFast(const Set& sset) const{
	return lambda*f->evalFast(sset);
}

double ScaleSubmodularFunctions::evalGainsadd(const Set& sset, int item) const{
	return lambda*f->evalGainsadd(sset, item);
}

double ScaleSubmodularFunctions::evalGainsremove(const Set& sset, int item) const{
	return lambda*f->evalGainsremove(sset, item);
}

double ScaleSubmodularFunctions::evalGainsaddFast(const Set& sset, int item) const{
	return lambda*f->evalGainsaddFast(sset, item);
}

double ScaleSubmodularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
	return lambda*f->evalGainsremoveFast(sset, item);
}

void ScaleSubmodularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
	f->updateStatisticsAdd(sset, item);
}

void ScaleSubmodularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
	f->updateStatisticsRemove(sset, item);
}

void ScaleSubmodularFunctions::clearpreCompute() const{
	f->clearpreCompute();
}

void ScaleSubmodularFunctions::setpreCompute(const Set& sset) const{
	f->setpreCompute(sset);
}
}
