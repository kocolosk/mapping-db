#ifndef STAR_StEmcDbMaker
#define STAR_StEmcDbMaker

// $Id$

/*****************************************************************************
 * @class StEmcDbMaker
 * @author Adam Kocoloski
 *
 * Caches BEMC mapping tables to optimize lookups by non-primary keys
 *****************************************************************************/

#include "StMaker.h"
#include "StEmcUtil/database/StVirtualEmcMappingDb.h"

class St_bemcMap;
class St_bsmdeMap;
class St_bsmdpMap;
class St_bprsMap;

class StEmcDbMaker : public StMaker, public StVirtualEmcMappingDb
{
public:
    StEmcDbMaker(const char* name = "emcDb");
    
    virtual Int_t Init();
    virtual Int_t Make();
    
    const bemcMap_st* bemc() const;
    const bemcMap_st& bemc(int softId) const;
    
    const bprsMap_st* bprs() const;
    const bprsMap_st& bprs(int softId) const;
    
    const bsmdeMap_st* bsmde() const;
    const bsmdeMap_st& bsmde(int softId) const;

    const bsmdpMap_st* bsmdp() const;
    const bsmdpMap_st& bsmdp(int softId) const;
    
    int softIdFromMES(StDetectorId det, int m, int e, int s) const;
    
    // these are for towers
    int softIdFromCrate(StDetectorId det, int crate, int channel) const;
    int softIdFromDaqId(StDetectorId det, int daqId) const;
    int softIdFromTDC(StDetectorId det, int TDC, int channel) const;
    
    // this is for SMD/PRS
    int softIdFromRDO(StDetectorId det, int rdo, int channel) const;
    
    virtual const char *GetCVS() const {
        static const char cvs[]="Tag $Name:  $ $Id$ built "__DATE__" "__TIME__ ; 
        return cvs;
    }
    
private:
    // DB tables provided by St_db_Maker -- prefer to use these
    St_bemcMap  *mBemcMap;
    St_bprsMap  *mBprsMap;
    St_bsmdeMap *mSmdeMap;
    St_bsmdpMap *mSmdpMap;
    
    // version info from StMaker::GetValidity -- used to expire caches
    Int_t mBemcVersion;
    Int_t mBprsVersion;
    Int_t mSmdeVersion;
    Int_t mSmdpVersion;
    
    mutable short mCacheCrate[30][160];
    mutable short mCacheDaqId[4800];
    mutable short mCacheTDC[30][160];
    mutable short mCacheBprsRdo[4][4800];
    mutable short mCacheSmdRdo[8][4800];
    
    void reset_bemc_cache() const;
    void reset_bprs_cache() const;
    void reset_smde_cache() const;
    void reset_smdp_cache() const;
    
    ClassDef(StEmcDbMaker, 1)
};

inline const bemcMap_st& 
StEmcDbMaker::bemc(int softId) const { return bemc()[softId-1]; }

inline const bprsMap_st& 
StEmcDbMaker::bprs(int softId) const { return bprs()[softId-1]; }

inline const bsmdeMap_st& 
StEmcDbMaker::bsmde(int softId) const { return bsmde()[softId-1]; }

inline const bsmdpMap_st& 
StEmcDbMaker::bsmdp(int softId) const { return bsmdp()[softId-1]; }

#endif

/*****************************************************************************
 * $Log$
 *****************************************************************************/
