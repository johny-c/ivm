#ifndef CIVM_H
#define CIVM_H
#include "CDataModel.h"
using namespace std;

const double NULOW=1e-16;
const string IVMVERSION="0.1";

class CIvm : public COptimisableModel {
 public:
  // Constructor given a filename.
  CIvm(const string modelFileName, int verbos=2);
  // Constructor given a kernel and a noise model.
  CIvm(const CMatrix& inData, const CMatrix& targetData, 
       CKern& kernel, CNoise& noiseModel, int selectCrit,
       int dVal, int verbos=2);
  CIvm(const CMatrix& actX, const CMatrix& actY, 
       const CMatrix& mmat, const CMatrix& betamat, 
       const vector<int> actSet, CKern& kernel, 
       CNoise& noiseModel, int selectCrit=ENTROPY, 
       int verbos=2);

#ifdef _NDLMATLAB
  // Constructor using file containing ivmInfo.
  CIvm(const CMatrix& inData, 
       const CMatrix& targetData, 
       CKern& kernel, 
       CNoise& noiseModel, 
       const string ivmInfoFile, 
       const string ivmInfoVariable, 
       int verbos=2);
#endif
  // Initialise the storeage for the model.
  void initStoreage();
  // Set the initial values for the model.
  void initVals();
  void selectPoints(); // select active set points.
  void addPoint(int index); // add a point to the model.
  void updateSite(int index); // update the site parameters at index.
  void updateM(int index); // update M at index.

  int selectPointAdd(); // select a point to add to active set.
  int entropyPointAdd(); // add a point selected by entropy change.
  int randomPointAdd();  // add a point selected randomly.
  double entropyChangeAdd(int) const; // entropy change associated with adding a point

  int selectPointRemove();  // select a point to remove from the active set.
  int entropyPointRemove(); // remove a point selected by entropy change.
  int randomPointRemove(); // remove a point selected randomly.
  double entropyChangeRemove(int) const; // entropy change associated with removing a point
  void test(const CMatrix& ytest, const CMatrix& Xin) const;
  void likelihoods(CMatrix& pout, CMatrix& yTest, const CMatrix& Xin) const;
  double logLikelihood(const CMatrix& yTest, const CMatrix& Xin) const;
  void out(CMatrix& yPred, const CMatrix& inData) const;
  void out(CMatrix& yPred, CMatrix& probPred, const CMatrix& inData) const;
  void posteriorMeanVar(CMatrix& mu, CMatrix& varSigma, const CMatrix& X) const;
  string getNoiseName() const
    {
      return noise.getNoiseName();
    }
  inline void changeEntropy(double val)
    {
      cumEntropy += val;
      lastEntropyChange = val;
    }

  // Gradient routines
  void updateCovGradient(int index) const;
  

  inline void setTerminate(const bool val)
    {
      terminate = val;
    }
  inline void setEpUpdate(const bool val)
    {
      epUpdate = val;
    }
  inline bool isTerminate() const
    {
      return terminate;
    }
  inline bool isEpUpdate() const 
    {
      return epUpdate;
    }
  void updateNuG();
  // update K with the kernel computed from the active points.
  void updateK() const;
  // update invK with the inverse of the kernel plus beta terms computed from the active points.
  void updateInvK(int index=0) const;
  // compute the approximation to the log likelihood.
  double approxLogLikelihood() const;
  // compute the gradients of the approximation wrt parameters.
  void approxLogLikelihoodGradient(CMatrix& g) const;
  
  void optimise(int maxIters=15, int kernIters=100, int noiseIters=100);
  bool equals(const CIvm& model, double tol=ndlutil::MATCHTOL) const;
  void display(ostream& os) const;

  inline int getOptNumParams() const
    {
      return kern.getNumParams();
    }    
  void getOptParams(CMatrix& param) const
    {
      kern.getTransParams(param);
    }
  void setOptParams(const CMatrix& param)
    {
      kern.setTransParams(param);
    }
  string getType() const
    {
      return type;
    }
  void setType(const string name)
    {
      type = name;
    }
  string getTypeSelection() const
    {
      switch(selectionCriterion)
	{
	case ENTROPY:
	  return "entropy";
	case RENTROPY:
	  return "rentropy";
	case RANDOM:
	  return "random";
	default:
	  cerr << "Unrecognised selection criterion." << endl;
	}
    }
  void setTypeSelection(const string val) 
    {
      if(val=="entropy")
	selectionCriterion=ENTROPY;
      else if(val=="rentropy")
	selectionCriterion=RENTROPY;
      else if(val=="random")
	selectionCriterion=RANDOM;
      else
	cerr << "Unrecognised selection criterion " << val << "." << endl;
    }
  void setTypeSelection(int val)
    {
      assert(val>=ENTROPY && val<=RANDOM);
      selectionCriterion=val;
    }
  void computeObjectiveGradParams(CMatrix& g) const
    {
      approxLogLikelihoodGradient(g);
      g.negate();
    }
  double computeObjectiveVal() const
    {
      return -approxLogLikelihood();
    }
#ifdef _NDLMATLAB
  mxArray* toMxArray() const;
  void fromMxArray(const mxArray* matlabArray);
#endif
  const CMatrix& X;

  int getActiveSetSize() const
    {
      return activeSetSize;
    }
  int getNumProcesses() const
    {
      return numTarget;
    }
  int getNumInputs() const
    {
      return activeX.getCols();
    }
  double getActiveX(int i, int j) const
    {
      return activeX.getVal(i, j);
    }
  int getActivePoint(int i) const
    {
      return activeSet[i];
    }
  // arguably these are noise model associated.
  const CMatrix& y;
  
  CMatrix nu;
  CMatrix g;
  
  CMatrix Kstore;

  
  // these are IVM associated.
  CMatrix m;
  CMatrix beta;

  // these really just provide local storage
  mutable CMatrix covGrad;
  mutable CMatrix invK;
  mutable double logDetK;
  mutable CMatrix K;

  mutable CMatrix s;
  mutable CMatrix a;
  mutable CMatrix ainv;


  CMatrix activeX;
  CMatrix activeY;

  CMatrix* M;
  CMatrix* L;
  CMatrix* Linv;

  //  COptions options;
  
  vector<int> activeSet;
  vector<int> inactiveSet;

  CKern& kern;
  CNoise& noise;
  enum{ENTROPY, RENTROPY, RANDOM};


 private:
  bool terminate;
  bool epUpdate;
  bool loadedModel;

  int numCovStruct;
  int activeSetSize;

  int numTarget;
  int numData;
  
  double lastEntropyChange;
  double cumEntropy;
  int selectionCriterion; 
  
  string type;
};

// Functions which operate on the object
void writeIvmToStream(const CIvm& model, ostream& out);
void writeIvmToFile(const CIvm& model, const string modelFileName, const string comment="");
CIvm* readIvmFromStream(istream& in);
CIvm* readIvmFromFile(const string modelfileName, int verbosity=2);

#endif