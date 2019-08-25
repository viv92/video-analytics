/*
 *	Implementation of the Minimum Norm Point Algorithm for Submodular Minimization.
	The problem is: \min_{X} f(X), where f is submodular.
	
	Author: Hui Lin (hlin@tsuri.ee.washington.edu)
	This was modified to fit into the framework of HSSFOTK.
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <queue>
#include "stdio.h"
#include <stdarg.h>
#include<assert.h>
#include <algorithm>
#include <cmath>
using namespace std;

#include "../utils/error.h"
#include "minNormPoint.h"


namespace smtk {
//#define DEBUG
#define INF 1e30
#define NO_INFO 0
#define BASIC_INFO 1
#define MORE_INFO 10
#define HUGE_INFO 100

// Global variables

vector< vector<double> > R; // the upper triangular matrix described in Wolfe 1976
vector< vector<double> > S; // the set of bases
totalOrder<double> initialOrder; // an initial total order that used for warm start
double tol = 1e-4;
double maxNormInS = 0;
double eps = 1e-10;
long int numGreedyCall = 1;
double lowerBoundDualityGap = 0;
bool earlyStopEnabled = true;

void affineCombine(vector<double> &x, vector<double> mu, int n){
	for(int i=0;i<n;++i) x[i] = 0.0 ;
	for(int i=0;i<S.size();++i){
		  for(int j=0;j<n;++j)
			x[j] += S[i][j]*mu[i];
	}        
}

double innerProd(vector<double> a, vector<double> b){
	assert(a.size()==b.size());
	double sum = 0 ;
	for(int i=0;i<a.size();i++) sum += a[i]*b[i];
	return sum ;
}

void addVector2S(vector<double>& x){
	double norm = innerProd(x,x);
	//update the R matrix
	// R is an upper triangle matrix such that
	// R^t*R = e*e^t + S^t*S,
	// therefore, whenever S changes, we update R.
	// See section 5, method D in Wolfe 1976 for details
	vector<double> newr ;
	newr.clear();
	double rr=0;
	for(int i=0;i<S.size();i++){
		double r = 1.0 + innerProd(S[i],x);
		for(int j=0;j<i;j++){
			r -= newr[j]*R[i][j];
		}
		r /= R[i][i] ;
		newr.push_back(r);
		rr += r*r;
	}
	newr.push_back(sqrt(1+norm-rr));
	S.push_back(x);
	R.push_back(newr);
	if(norm>maxNormInS) maxNormInS = norm ;
}


void findIntersectionPoint(vector<double> x0_mu, vector<double>& xstar_mu){

  //assert(x0_mu.size()==xstar_mu.size());

	double beta = 1;
	for(int i=0;i<x0_mu.size();i++){
		if( xstar_mu[i]-x0_mu[i] > 0 ){
			double tmpv = xstar_mu[i]/(xstar_mu[i]-x0_mu[i]) ;
		if(tmpv < beta) beta = tmpv ;
		}
	}
	vector<double> z_mu ;
	z_mu.clear();
	// modify S
	std::vector<int> tobedel ;
	tobedel.clear();
	for(int i=0;i<x0_mu.size();i++){
		double tmpv = (1-beta)*xstar_mu[i]+beta*x0_mu[i] ;
	if(tmpv<eps) tobedel.push_back(i);
	else z_mu.push_back(tmpv);
	}
	for(int i=0;i<tobedel.size();i++){
		vector< std::vector<double> >::iterator its = S.begin(); std::advance(its, tobedel[i]-i);
		vector< std::vector<double> >::iterator itr = R.begin(); std::advance(itr, tobedel[i]-i);
		S.erase(its);
		R.erase(itr);
		//update R such that it maintains upper-triangle structure
		for(int j=tobedel[i]-i;j<S.size();j++){
			double a = R[j][j] ;
			double b = R[j][j+1];
			double c = sqrt(a*a+b*b);
			for(int k=j;k<S.size();k++){
				double d = R[k][j]; double e = R[k][j+1] ;
				R[k][j] = (a*d + b*e)/c ;
				R[k][j+1] = (-b*d + a*e)/c ;
			}
		}

		for(int j=tobedel[i]-i;j<S.size();j++){
			R[j].pop_back();
		}
	}
	xstar_mu = z_mu ;
}



/////////////////////////////////////////////////////
// find the min norm point in the affine hull of S //
/////////////////////////////////////////////////////
bool minNormAffineSpan(vector<double> & mu){
	if(S.size()<=1){  mu.clear(); mu.push_back(1); return true; }
	int m = S.size();
	vector<double> ubar(m,0.0);
	vector<double> u(m,0.0);
	for(int i=0;i<m;i++){
		ubar[i] = 1.0 ;
		for(int j=0;j<i;j++){
			ubar[i] -= ubar[j]*R[i][j] ;
		}
		ubar[i] /= R[i][i] ;
	}

	double sum_u = 0 ;
	for(int i=m-1;i>=0;i--){
		u[i] = ubar[i] ;
		for(int j=m-1;j>i;j--){
			u[i] -= u[j]*R[j][i];
		}
		u[i] /= R[i][i] ;
		sum_u += u[i] ;
	}
	mu.clear();
	for(int i=0;i<m;i++) mu.push_back(u[i]/sum_u);
	bool isInterior = true ;
	for(int i=0;i<S.size();i++){
		if(mu[i] < eps){ 
			isInterior = false ;
			break;
		}
	}
	return isInterior ;
}

template <class Container>
bool setOptCheckDuality(vector<double> x, SubmodularFunctions& f){

    vector<double> myx ;
    Container A;
    double sum = 0;
    A.clear();
    for(int i=0;i<x.size();i++){
      if(x[i] <=0){
        	sum += x[i];
        	A.insert(i);
      }
    }
    if(f.eval(A) - sum > lowerBoundDualityGap) return false;

    return true ;
}

bool checkIfPointInS(vector<double> x, int n){
	bool flag = true;
	for(int i = 0; i < S.size(); i++){
		for(int j = 0; j < n; j++){
			if(fabs(S[i][j]-x[j]) > eps){
				flag = false;
				break;
			}
		}
		if(flag) return flag;
	}
	return false;
}

  ////////////////////////////////////////////////////////////////
  // Debug utilities                                            //
  ////////////////////////////////////////////////////////////////

void printVector(vector<double> x,const char* name){
	printf("%s=[",name);
	for(int i=0;i<x.size();i++) printf("%.5e ",x[i]);
	printf("]\n");
  }

void infoMsg(int verb, int verbosity, const char * const format, ...){
	if(verbosity >= verb){
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
  }

template <class Container>
bool minNormPoint(SubmodularFunctions& f, Container& minSolSet, Container& maxSolSet, std::vector<double>& xstar, int verbosity){
	Container groundset = Container(f.size(), true);
	Container emptySet = Container(); 
	int n = f.size(); 
	vector<double> xhat; // minimum weighted base xhat of B(f)
	vector<double> x0; // min-norm point in the affine hull spanned by points in S
	vector<double> x0_mu; //linear coefficients s.t. x0 = \sum_{s_i \in S} mu_i s_i
	xhat.resize(n); 
	x0.resize(n); 
	xstar.resize(n);
	vector<double> xstar_mu;
	totalOrder<double> order(n) ;

	//initialization: xhat is initialized as an extreme point in the base polytope
	if(initialOrder.size() == groundset.size())
		f.getSubgradient(initialOrder, emptySet, xhat);
	else
		f.getSubgradient(order, emptySet, xhat);

	numGreedyCall = 0;
	maxNormInS = 0;
	S.clear();
	R.clear();
	addVector2S(xhat);
	xstar=xhat;
	xstar_mu.push_back(1);

	while(1){
		// step 2: find the minimum weighted base xhat using greedy algorithm
		//xhat.clear();
		affineCombine(xstar,xstar_mu, n);

		// update xhat using the ordering based on xstar, which is equivalent to
		// xhat = min <xstar,x>, x \in B(f), where B(f) is the base polytope of f.
		order.resetOrderDec(xstar);
		f.getSubgradient(order, emptySet, xhat);
		numGreedyCall++;
		double sum = 0 ;
		for(int i=0;i<xstar.size();i++) sum += xstar[i] * (xstar[i]-xhat[i]);
		infoMsg(MORE_INFO, verbosity, "termination(%.4e,%.4e),|x|:%.25e,# bases:%d\n",sum, tol*maxNormInS,innerProd(xstar,xstar),S.size());
		if(sum < tol * maxNormInS){
			infoMsg(BASIC_INFO, verbosity, "Minimum norm point found!\n"); // xstar is the min norm solution
			break;
		}
		if (earlyStopEnabled > 0 && setOptCheckDuality<Container>(xstar, f)){
    			infoMsg(BASIC_INFO, verbosity, "Early (duality) stopping triggered!!\n");
    			break ;
    		}
		if(checkIfPointInS(xhat, n)){
			infoMsg(BASIC_INFO, verbosity, "point already in S!\n");
			break;
		}
		//add xhat to set S, and update the norm matrix
		addVector2S(xhat);
		xstar_mu.push_back(0);
		if(S.size() > groundset.size()){
			infoMsg(BASIC_INFO, verbosity, "Trying to augment full-dimensional S!\n");
			break;
		}
		//setp 3:
		while(1){
			if ( minNormAffineSpan(x0_mu)){ // x0 is in the interior of the simplex S
				xstar_mu = x0_mu ;
				break ;
			}
			else{
				findIntersectionPoint(x0_mu,xstar_mu);
			}
		}
	}

	minSolSet.clear();
	maxSolSet.clear();
	bool isUnique = true ;
	for(int i=0;i<xstar.size();i++){
		if(xstar[i] < -eps ) minSolSet.insert(i);
		if(xstar[i] <= eps ) maxSolSet.insert(i);
		if(fabs(xstar[i]) < eps) isUnique = false ;
	}

	return isUnique ;
}

template bool minNormPoint(SubmodularFunctions& f, HashSet& minSolSet, HashSet& maxSolSet, std::vector<double>& xstar, int verbosity = 1);
}

