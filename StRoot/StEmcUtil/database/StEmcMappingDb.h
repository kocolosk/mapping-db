#ifndef STAR_StEmcMappingDb
#define STAR_StEmcMappingDb

// $Id: StEmcMappingDb.h,v 1.1 2009/01/08 02:16:19 kocolosk Exp $

/*****************************************************************************
 * @class StEmcMappingDb
 * @author Adam Kocoloski
 *
 * Implements StVirtualEmcMappingDb interface without St_db_Maker requirement.
 * This class will use St_db_Maker if it's present, but if you're in a chain 
 * you should really be using StEmcDbMaker instead.
 *****************************************************************************/

#include "TObject.h"
#include "StVirtualEmcMappingDb.h"

#include "TDatime.h"

class St_bemcMap;
class St_bsmdeMap;
class St_bsmdpMap;
class St_bprsMap;
 
class StDbTable;
class StMaker;

class StEmcMappingDb : public TObject, public StVirtualEmcMappingDb
{
public:
    StEmcMappingDb(int date=20330101, int time=0);
    virtual ~StEmcMappingDb();
    
    static StEmcMappingDb* instance();
    
    /// St_db_Maker-compatible interface
    void SetDateTime(int date, int time);
    void SetFlavor(const char *flavor, const char *tablename=NULL);
    void SetMaxEntryTime(int date, int time);
    
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

private:
    static StEmcMappingDb* mInstance;
    
    // DB tables provided by St_db_Maker -- prefer to use these
    mutable St_bemcMap  *mBemcTTable;
    mutable St_bprsMap  *mBprsTTable;
    mutable St_bsmdeMap *mSmdeTTable;
    mutable St_bsmdpMap *mSmdpTTable;
    
    // version info from StMaker::GetValidity -- used to expire caches
    mutable Int_t mBemcValidity;
    mutable Int_t mBprsValidity;
    mutable Int_t mSmdeValidity;
    mutable Int_t mSmdpValidity;
    
    // DB tables provided by StDbManager in standalone mode
    StDbTable *mBemcTable;
    StDbTable *mBprsTable;
    StDbTable *mSmdeTable;
    StDbTable *mSmdpTable;
    
    // ensure caches expire if beginTime, flavor, or maxEntryTime changes
    mutable bool mBemcDirty;
    mutable bool mBprsDirty;
    mutable bool mSmdeDirty;
    mutable bool mSmdpDirty;
    
    StMaker *mChain;
    
    TDatime mBeginTime;
    void maybe_reload(StDetectorId) const;
    void reload_dbtable(StDbTable*) const;
    
    bool maybe_reset_cache(StDetectorId det) const;
    void reset_bemc_cache() const;
    void reset_bprs_cache() const;
    void reset_smde_cache() const;
    void reset_smdp_cache() const;
    
    mutable short mCacheCrate[30][160];
    mutable short mCacheDaqId[4800];
    mutable short mCacheTDC[30][160];
    mutable short mCacheBprsRdo[4][4800];
    mutable short mCacheSmdRdo[8][4800];
    
    ClassDef(StEmcMappingDb, 2)
};

inline const bemcMap_st& 
StEmcMappingDb::bemcMap(int softId) const { return bemcMap()[softId-1]; }

inline const bprsMap_st& 
StEmcMappingDb::bprsMap(int softId) const { return bprsMap()[softId-1]; }

inline const bsmdeMap_st& 
StEmcMappingDb::bsmdeMap(int softId) const { return bsmdeMap()[softId-1]; }

inline const bsmdpMap_st& 
StEmcMappingDb::bsmdpMap(int softId) const { return bsmdpMap()[softId-1]; }

#endif

/*****************************************************************************
 * $Log: StEmcMappingDb.h,v $
 * Revision 1.1  2009/01/08 02:16:19  kocolosk
 * move StEmcMappingDb/StEmcDecoder to StEmcUtil/database
 *
 * Revision 2.2  2009/01/02 03:34:34  kocolosk
 * use default date==20330101 like St_db_Maker to suppress spurious error msgs
 *
 * Revision 2.1  2008/12/05 19:05:32  kocolosk
 * new DB-backed implementation of StEmcDecoder
 *
 *****************************************************************************/
