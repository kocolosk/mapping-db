#ifndef STAR_StEmcDbMaker
#define STAR_StEmcDbMaker

// $Id$

/*****************************************************************************
 * @class StEmcDbMaker
 * @author Adam Kocoloski
 *
 * Helper for BEMC DB.  Among other things it provides optimized methods for 
 * looking up mapping information by a non-primary key (e.g. rdo/channel).
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
    
    const bemcMap_st* bemcMap() const;
    const bemcMap_st& bemcMap(int softId) const;
    
    const bprsMap_st* bprsMap() const;
    const bprsMap_st& bprsMap(int softId) const;
    
    const bsmdeMap_st* bsmdeMap() const;
    const bsmdeMap_st& bsmdeMap(int softId) const;

    const bsmdpMap_st* bsmdpMap() const;
    const bsmdpMap_st& bsmdpMap(int softId) const;
    
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
    // DB tables provided by St_db_Maker
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
StEmcDbMaker::bemcMap(int softId) const { return bemcMap()[softId-1]; }

inline const bprsMap_st& 
StEmcDbMaker::bprsMap(int softId) const { return bprsMap()[softId-1]; }

inline const bsmdeMap_st& 
StEmcDbMaker::bsmdeMap(int softId) const { return bsmdeMap()[softId-1]; }

inline const bsmdpMap_st& 
StEmcDbMaker::bsmdpMap(int softId) const { return bsmdpMap()[softId-1]; }

#endif

/*****************************************************************************
 * $Log$
 *****************************************************************************/
