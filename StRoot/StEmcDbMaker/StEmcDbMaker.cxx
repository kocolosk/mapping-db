// $Id$

using namespace std;

#include "StEmcDbMaker.h"

#include "TDatime.h"

#include "StMessMgr.h"
#include "tables/St_bemcMap_Table.h"
#include "tables/St_bprsMap_Table.h"
#include "tables/St_bsmdeMap_Table.h"
#include "tables/St_bsmdpMap_Table.h"
#include "tables/St_emcCalib_Table.h"
#include "tables/St_smdCalib_Table.h"
#include "tables/St_emcPed_Table.h"
#include "tables/St_smdPed_Table.h"
#include "tables/St_emcStatus_Table.h"
#include "tables/St_smdStatus_Table.h"
#include "tables/St_emcGain_Table.h"
#include "tables/St_smdGain_Table.h"
#include "tables/St_emcTriggerPed_Table.h"
#include "tables/St_emcTriggerStatus_Table.h"
#include "tables/St_emcTriggerLUT_Table.h"

ClassImp(StEmcDbMaker);

StEmcDbMaker::StEmcDbMaker(const char *name) : StMaker(name), 
    mBemcMap(NULL), mBprsMap(NULL), mSmdeMap(NULL), mSmdpMap(NULL),
    mBemcCalib(NULL), mBprsCalib(NULL), mSmdeCalib(NULL), mSmdpCalib(NULL),
    mBemcPed(NULL), mBprsPed(NULL), mSmdePed(NULL), mSmdpPed(NULL),
    mBemcStatus(NULL), mBprsStatus(NULL), mSmdeStatus(NULL), mSmdpStatus(NULL),
    mBemcGain(NULL), mBprsGain(NULL), mSmdeGain(NULL), mSmdpGain(NULL),
    mTriggerPed(NULL), mTriggerStatus(NULL), mTriggerLUT(NULL)
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
    
    DB = GetInputDB("Calibrations/emc/y3bemc");
    if(DB) {
        mBemcCalib  = static_cast<St_emcCalib*> (DB->Find("bemcCalib"));
        mBemcPed    = static_cast<St_emcPed*>   (DB->Find("bemcPed"));
        mBemcStatus = static_cast<St_emcStatus*>(DB->Find("bemcStatus"));
        mBemcGain   = static_cast<St_emcGain*>  (DB->Find("bemcGain"));
    }
    else {
        LOG_ERROR << "could not find Calibrations/emc/y3bemc DB" << endm;        
    }
    
    DB = GetInputDB("Calibrations/emc/y3bprs");
    if(DB) {
        mBprsCalib  = static_cast<St_emcCalib*> (DB->Find("bprsCalib"));
        mBprsPed    = static_cast<St_emcPed*>   (DB->Find("bprsPed"));
        mBprsStatus = static_cast<St_emcStatus*>(DB->Find("bprsStatus"));
        mBprsGain   = static_cast<St_emcGain*>  (DB->Find("bprsGain"));
    }
    else {
        LOG_ERROR << "could not find Calibrations/emc/y3bprs DB" << endm;        
    }
    
    DB = GetInputDB("Calibrations/emc/y3bsmde");
    if(DB) {
        mSmdeCalib  = static_cast<St_smdCalib*> (DB->Find("bsmdeCalib"));
        mSmdePed    = static_cast<St_smdPed*>   (DB->Find("bsmdePed"));
        mSmdeStatus = static_cast<St_smdStatus*>(DB->Find("bsmdeStatus"));
        mSmdeGain   = static_cast<St_smdGain*>  (DB->Find("bsmdeGain"));
    }
    else {
        LOG_ERROR << "could not find Calibrations/emc/y3bsmde DB" << endm;        
    }
    
    DB = GetInputDB("Calibrations/emc/y3bsmdp");
    if(DB) {
        mSmdpCalib  = static_cast<St_smdCalib*> (DB->Find("bsmdpCalib"));
        mSmdpPed    = static_cast<St_smdPed*>   (DB->Find("bsmdpPed"));
        mSmdpStatus = static_cast<St_smdStatus*>(DB->Find("bsmdpStatus"));
        mSmdpGain   = static_cast<St_smdGain*>  (DB->Find("bsmdpGain"));
    }
    else {
        LOG_ERROR << "could not find Calibrations/emc/y3smdp DB" << endm;        
    }
    
    DB = GetInputDB("Calibrations/emc/trigger");
    if(DB) {
        mTriggerPed = 
            static_cast<St_emcTriggerPed*>(DB->Find("bemcTriggerPed"));
        mTriggerStatus = 
            static_cast<St_emcTriggerStatus*>(DB->Find("bemcTriggerStatus"));
        mTriggerLUT = 
            static_cast<St_emcTriggerLUT*>(DB->Find("bemcTriggerLUT"));
    }
    else {
        LOG_ERROR << "could not find Calibrations/emc/trigger DB" << endm;        
    }
    
    mTableVec.push_back(mBemcCalib);
    mTableVec.push_back(mBemcPed);
    mTableVec.push_back(mBemcStatus);
    mTableVec.push_back(mBemcGain);
    
    mTableVec.push_back(mBprsCalib);
    mTableVec.push_back(mBprsPed);
    mTableVec.push_back(mBprsStatus);
    mTableVec.push_back(mBprsGain);
    
    mTableVec.push_back(mSmdeCalib);
    mTableVec.push_back(mSmdePed);
    mTableVec.push_back(mSmdeStatus);
    mTableVec.push_back(mSmdeGain);
    
    mTableVec.push_back(mSmdpCalib);
    mTableVec.push_back(mSmdpPed);
    mTableVec.push_back(mSmdpStatus);
    mTableVec.push_back(mSmdpGain);
    
    mTableVec.push_back(mTriggerPed);
    mTableVec.push_back(mTriggerStatus);
    mTableVec.push_back(mTriggerLUT);
    
    return StMaker::Init();
}

Int_t StEmcDbMaker::Make() {
    if(new_version(mBemcMap)) reset_bemc_cache();
    if(new_version(mBprsMap)) reset_bprs_cache();
    if(new_version(mSmdeMap)) reset_smde_cache();
    if(new_version(mSmdpMap)) reset_smdp_cache();
    
    vector<TTable*>::const_iterator it;
    for(it = mTableVec.begin(); it != mTableVec.end(); it++) {
        if(*it) new_version(*it);
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

bool StEmcDbMaker::new_version(const TTable *table) const {
    Int_t& version = mVersion[table->GetName()];
    TDatime ts[2];
    Int_t new_version = GetValidity(table, ts);
    string begin(ts[0].AsSQLString()); // copy result from static buffer
    
    if(version != new_version) {
        version = new_version;
        LOG_INFO << Form("new %-17s table with validity %s - %s", 
            table->GetName(), begin.c_str(), ts[1].AsSQLString()) << endm;
        return true;
    }
    return false;
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
