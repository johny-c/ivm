#ifndef CDATAMODEL_H
#define CDATAMODEL_H
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "ndlexceptions.h"
#include "ndlstrutil.h"
#include "CNdlInterfaces.h"
#include "CMatrix.h"

// a model of a data set.
class CDataModel : public CStreamInterface
{
 public:
  // Initialise the model.
  CDataModel() {}
  CDataModel(int nData) : numData(nData) {}
  inline string getType() const
  {
    return type;
  }
  // Set a string representing the model type.
  inline void setType(const string name)
  {
    type = name;
  }
  inline string getBaseType() const
  {
    return baseType;
  }
  // Set a string representing the model type.
  inline void setBaseType(const string name)
  {
    baseType = name;
  }
  // Get the long name of the model.
  inline string getName() const
  {
    return modelName;
  }
  // Set the long name of the model.
  inline void setName(const string name)
  {
    modelName = name;
  }
  virtual int getOptNumParams() const=0;
  virtual void display(ostream& os) const=0;
  virtual inline unsigned int getNumData() const 
  {
    return numData;
  }
  virtual inline void setNumData(unsigned int val) 
  {
    numData = val;
  }
  
 private: 
  string baseType;
  string type;
  string modelName;
  unsigned int numData;
};

// a model which maps from one data space to another.
class CMapModel : public CDataModel
{
 public:
  CMapModel() : CDataModel() 
  {
    _init();
  }
  CMapModel(int inDim, int outDim, int nData) : inputDim(inDim), outputDim(outDim), CDataModel(nData) 
  {
    _init();
  }
  // map from an input to an output.
  virtual void out(CMatrix& yPred, const CMatrix& inData) const=0;
  // compute gradient with respect to parameters of an output.
  virtual double outGradParams(CMatrix& g, const CMatrix& Xin, const int pointNo, const int outputNo) const=0;
  virtual double outGradX(CMatrix& g, const CMatrix& Xin, const int pointNo, const int outputNo) const=0;
  
  // Set the input dimension.
  inline void setInputDim(unsigned int dim)
  {
    inputDim = dim;
  }
  // Get the input dimension.
  inline unsigned int getInputDim() const
  {
    return inputDim;
  }
  // Set the output dimension.
  inline void setOutputDim(unsigned int dim)
  {
    outputDim = dim;
  }
  // Get the output dimension.
  inline unsigned int getOutputDim() const
  {
    return outputDim;
  }
    
 private:
  void _init()
  {
    setBaseType("mapmodel");
  }
  unsigned int outputDim;
  unsigned int inputDim;
  string modelName;
  string type;
};
void writeMapModelToStream(const CMapModel& model, ostream& out);
CMapModel* readMapModelFromStream(istream& in);


#endif
