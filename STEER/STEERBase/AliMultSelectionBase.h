#ifndef AliMultSelectionBase_H
#define AliMultSelectionBase_H
#include <TNamed.h>

class AliMultSelectionBase : public TNamed {
    
public:
    AliMultSelectionBase();
    AliMultSelectionBase(const char * name, const char * title = "Mult Estimator");
    ~AliMultSelectionBase();
    
    void Clear(Option_t* = "") {}; //dummy
    
    //So that AliRoot knows the function calls
    //Late binding will ensure the correct functionality at run-time
    virtual Float_t GetMultiplicityPercentile(TString lName, Bool_t lEmbedEvSel = kFALSE){ return -123.456; }
    virtual Int_t GetEvSelCode() const { return -123456; }
    
private:
    //Nothing needed: this is a dummy class
    
    ClassDef(AliMultSelectionBase, 1)
    // 1 - original implementation
};
#endif
