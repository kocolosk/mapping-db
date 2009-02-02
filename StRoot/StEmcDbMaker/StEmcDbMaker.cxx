// $Id$

#include "StEmcDbMaker.h"

#include "TDatime.h"

#include "StMessMgr.h"
#include "tables/St_bemcMap_Table.h"
#include "tables/St_bprsMap_Table.h"
#include "tables/St_bsmdeMap_Table.h"
#include "tables/St_bsmdpMap_Table.h"

ClassImp(StEmcDbMaker);

StEmcDbMaker::StEmcDbMaker(const char *name) : 
    StMaker(name), mBemcMap(NULL), mBprsMap(NULL), mSmdeMap(NULL),
    mSmdpMap(NULL), mBemcVersion(-2), mBprsVersion(-2), mSmdeVersion(-2),
    mSmdpVersion(-2)
{
    reset_bemc_cache();
    reset_bprs_cache();
    reset_smde_cache();
    reset_smdp_cache();
}

Int_t StEmcDbMaker::Init() {
    TDataSet *DB = GetInputDB("Calibrations/emc/map");
    if(DB) {
        mBemcMap = static_cast<St_bemcMap*>(DB->Find("bemcMap"));
        mBprsMap = static_cast<St_bprsMap*>(DB->Find("bprsMap"));
        mSmdeMap = static_cast<St_bsmdeMap*>(DB->Find("bsmdeMap"));
        mSmdpMap = static_cast<St_bsmdpMap*>(DB->Find("bsmdpMap"));
    }
    else {
        LOG_ERROR << "could not find Calibrations/emc/map DB" << endm;
    }
    return StMaker::Init();
}

Int_t StEmcDbMaker::Make() {
    Int_t version;
    TDatime ts;

    if((version = GetValidity(mBemcMap,&ts)) != mBemcVersion) {
        mBemcVersion = version;
        reset_bemc_cache();
        LOG_INFO << "loaded new bemcMap  with beginTime " << ts.AsSQLString() 
            << endm;
    }
    
    if((version = GetValidity(mBprsMap,&ts)) != mBprsVersion) {
        mBprsVersion = version;
        reset_bprs_cache();
        LOG_INFO << "loaded new bprsMap  with beginTime " << ts.AsSQLString() 
            << endm;
    }
    
    if((version = GetValidity(mSmdeMap,&ts)) != mSmdeVersion) {
        mSmdeVersion = version;
        reset_smde_cache();
        LOG_INFO << "loaded new bsmdeMap with beginTime " << ts.AsSQLString() 
            << endm;
    }
    
    if((version = GetValidity(mSmdpMap,&ts)) != mSmdpVersion) {
        mSmdpVersion = version;
        reset_smdp_cache();
        LOG_INFO << "loaded new bsmdpMap with beginTime " << ts.AsSQLString() 
            << endm;
    }
    
    return kStOk;
}

// struct declaration auto-generated at $STAR/include/bemcMap.h
const bemcMap_st* StEmcDbMaker::bemcMap() const {
    return mBemcMap->GetTable();
}

// struct declaration auto-generated at $STAR/include/bprsMap.h
const bprsMap_st* StEmcDbMaker::bprsMap() const {
    return mBprsMap->GetTable();
}

// struct declaration auto-generated at $STAR/include/bsmdeMap.h
const bsmdeMap_st* StEmcDbMaker::bsmdeMap() const {
    return mSmdeMap->GetTable();
}

// struct declaration auto-generated at $STAR/include/bsmdpMap.h
const bsmdpMap_st* StEmcDbMaker::bsmdpMap() const {
    return mSmdpMap->GetTable();
}

int 
StEmcDbMaker::softIdFromMES(StDetectorId det, int m, int e, int s) const {
    switch(det) {
        case kBarrelEmcTowerId:
        case kBarrelEmcPreShowerId:
        return 40*(m-1) + 20*(s-1) + e;
        
        case kBarrelSmdEtaStripId:
        return 150*(m-1) + 150*(s-1) + e;
        
        case kBarrelSmdPhiStripId:
        return 150*(m-1) + 10*(s-1) + e;
        
        default: break;
    }
    return 0;
}

int 
StEmcDbMaker::softIdFromCrate(StDetectorId det, int crate, int channel) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemcMap();
        if(!mCacheCrate[crate-1][channel]) {
            for(int i=0; i<4800; ++i) {
                mCacheCrate[map[i].crate-1][map[i].crateChannel] = i+1;
            }
        }
        
        return mCacheCrate[crate-1][channel];
    }
    return 0;
}

int 
StEmcDbMaker::softIdFromDaqId(StDetectorId det, int daqID) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemcMap();
        if(!mCacheDaqId[daqID]) {
            for(int i=0; i<4800; ++i) {
                mCacheDaqId[map[i].daqID] = i+1;
            }
        }
        
        return mCacheDaqId[daqID];
    }
    return 0;
}

int 
StEmcDbMaker::softIdFromTDC(StDetectorId det, int TDC, int channel) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemcMap();
        if(!mCacheTDC[TDC][channel]) {
            for(int i=0; i<4800; ++i) {
                mCacheTDC[map[i].TDC][map[i].crateChannel] = i+1;
            }
        }
        
        return mCacheTDC[TDC][channel];
    }
    return 0;
}

int 
StEmcDbMaker::softIdFromRDO(StDetectorId det, int rdo, int channel) const {
    switch(det) {
        case kBarrelEmcPreShowerId:
        const bprsMap_st* prs = bprsMap();
        if(mCacheBprsRdo[rdo][channel] == -1) {
            for(int i=0; i<4800; i++) {
                if(prs[i].rdo == rdo && prs[i].rdoChannel == channel) {
                    mCacheBprsRdo[rdo][channel] = i+1;
                }
            }
        }
        if(mCacheBprsRdo[rdo][channel] == -1) {
            // didn't find it on a linear search, must be empty channel
            mCacheBprsRdo[rdo][channel] = 0;
        }
        return mCacheBprsRdo[rdo][channel];
        
        case kBarrelSmdEtaStripId: case kBarrelSmdPhiStripId:
        const bsmdeMap_st *smde = bsmdeMap();
        const bsmdpMap_st *smdp = bsmdpMap();
        if(mCacheSmdRdo[rdo][channel] == -1) {
            for(int i=0; i<18000; i++) {
                if(smde[i].rdo == rdo && smde[i].rdoChannel == channel) {
                    mCacheSmdRdo[rdo][channel] = i+1;
                    if(det == kBarrelSmdEtaStripId) return i+1;
                }
                else if(smdp[i].rdo == rdo && smdp[i].rdoChannel == channel) {
                    mCacheSmdRdo[rdo][channel] = i+1;
                    if(det == kBarrelSmdPhiStripId) return i+1;
                }
            }
        }
        short id;
        switch((id = mCacheSmdRdo[rdo][channel])) {
            // didn't find it on a linear search, must be empty channel
            case -1:
            mCacheSmdRdo[rdo][channel] = 0;
            
            case 0:
            return 0;
            
            // confirm detector
            default:
            if(smde[id-1].rdo == rdo && smde[id-1].rdoChannel == channel) {
                if(det == kBarrelSmdEtaStripId) return id;
                return 0;
            }
            else if(smdp[id-1].rdo == rdo && smdp[id-1].rdoChannel == channel) {
                if(det == kBarrelSmdPhiStripId) return id;
                return 0;
            } 
        }
        
        default: break;
    }
    return 0;
}

void StEmcDbMaker::reset_bemc_cache() const {
    memset(mCacheCrate, 0, sizeof(mCacheCrate));
    memset(mCacheDaqId, 0, sizeof(mCacheDaqId));
    memset(mCacheTDC, 0, sizeof(mCacheTDC));
}

void StEmcDbMaker::reset_bprs_cache() const {
    memset(mCacheBprsRdo, -1, sizeof(mCacheBprsRdo));
}

void StEmcDbMaker::reset_smde_cache() const {
    memset(mCacheSmdRdo, -1, sizeof(mCacheSmdRdo));
}

void StEmcDbMaker::reset_smdp_cache() const {
    memset(mCacheSmdRdo, -1, sizeof(mCacheSmdRdo));
}

/*****************************************************************************
 * $Log$
 *****************************************************************************/
