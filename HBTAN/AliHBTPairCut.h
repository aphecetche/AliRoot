//Piotr Skowronski@cern.ch
//Class implemnts cut on the pair of particles
//
//more info: http://alisoft.cern.ch/people/skowron/analyzer/index.html

#ifndef ALIHBTPAIRCUT_H
#define ALIHBTPAIRCUT_H


#include "AliHBTParticleCut.h"
#include "AliHBTPair.h"

class AliHbtBasePairCut;

enum AliHBTPairCutProperty
 {
  kHbtPairCutPropQInv, //Q invariant
  kHbtPairCutPropNone
 };

class AliHBTPairCut: public TObject
{
  public:
    AliHBTPairCut();
    AliHBTPairCut(const AliHBTPairCut&);
    
    virtual ~AliHBTPairCut();
    virtual Bool_t Pass(AliHBTPair*);
    
    void SetFirstPartCut(AliHBTParticleCut*);  //sets the cut on the first particle
    void SetSecondPartCut(AliHBTParticleCut*); //sets the cut on the first particle
    
    void SetPartCut(AliHBTParticleCut*);//sets the the same cut on both particles
     
    void AddBasePairCut(AliHbtBasePairCut*);
    
    void SetQInvRange(Double_t min, Double_t max);
    
  protected:
    AliHBTParticleCut*      fFirstPartCut;
    AliHBTParticleCut*      fSecondPartCut;

    AliHbtBasePairCut** fCuts; //!
    Int_t fNCuts;
       
       
    AliHbtBasePairCut* FindCut(AliHBTPairCutProperty);
  private:
    static const Int_t fkgMaxCuts;
  public:
    ClassDef(AliHBTPairCut,1)
 
};
/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHBTEmptyPairCut:  public AliHBTPairCut
{
//Empty - it passes possitively all particles - it means returns always False
//Class describing cut on pairs of particles
  public:
    AliHBTEmptyPairCut(){};
    AliHBTEmptyPairCut(const AliHBTEmptyPairCut&){}; 
    virtual ~AliHBTEmptyPairCut(){};
    
    Bool_t Pass(AliHBTPair*) {return kFALSE;} //accpept everything

    ClassDef(AliHBTEmptyPairCut,1)
 
};



/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHbtBasePairCut: public TObject
 {
   //This class defines the range of some property - pure virtual
   //Property is coded by AliHBTCutTypes type
   
   public:
     
     AliHbtBasePairCut(Double_t min = 0.0, Double_t max = 0.0, AliHBTPairCutProperty prop= kHbtPairCutPropNone):
                      fMin(min),fMax(max),fProperty(prop){}

     virtual   ~AliHbtBasePairCut(){}
     
     Bool_t    Pass(AliHBTPair*);
     
     void      SetRange(Double_t min, Double_t max){fMin = min; fMax = max;}

     void      SetMinimum(Double_t min){fMin = min;}
     void      SetMaximum(Double_t max){fMax = max;}
     
     Double_t  GetMinimum() const {return fMin;}
     Double_t  GetMaximum() const {return fMax;}
     
     AliHBTPairCutProperty GetProperty() const {return fProperty;}
     
   protected:
     virtual Double_t  GetValue(AliHBTPair*) = 0;

     Double_t fMin;
     Double_t fMax;
     
     AliHBTPairCutProperty fProperty;
     
   private:
   public:
     ClassDef(AliHbtBasePairCut,1)
   
 };

inline Bool_t
AliHbtBasePairCut::Pass(AliHBTPair* pair) 
 {
   Double_t value = GetValue(pair);
   if ( (value > fMin) && (value <fMax ) ) return kFALSE; //accepted
   else return kTRUE; //rejected
 }
/******************************************************************/
/******************************************************************/
/******************************************************************/
class AliHBTQInvCut: public AliHbtBasePairCut
 {
   public:
    AliHBTQInvCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBasePairCut(min,max,kHbtPairCutPropQInv){}
    virtual ~AliHBTQInvCut(){}
   protected:
    virtual Double_t  GetValue(AliHBTPair* pair){return pair->GetQInv();}
   private:
   public:
     ClassDef(AliHBTQInvCut,1)
 };

#endif
