// $Id: StEmcMappingDb.cxx,v 1.1 2009/01/08 02:16:18 kocolosk Exp $

#include "StEmcMappingDb.h"

#include "TUnixTime.h"
#include "StMessMgr.h"
#include "StMaker.h"

#include "StDbLib/StDbManager.hh"
#include "StDbLib/StDbConfigNode.hh"
#include "StDbLib/StDbTable.h"

#include "tables/St_bemcMap_Table.h"
#include "tables/St_bprsMap_Table.h"
#include "tables/St_bsmdeMap_Table.h"
#include "tables/St_bsmdpMap_Table.h"

ClassImp(StEmcMappingDb)

StEmcMappingDb::StEmcMappingDb(int date, int time) : mBemcTTable(NULL),
    mBprsTTable(NULL), mSmdeTTable(NULL), mSmdpTTable(NULL), mBemcValidity(-2),
    mBprsValidity(-2), mSmdeValidity(-2), mSmdpValidity(-2), mBemcTable(NULL),
    mBprsTable(NULL), mSmdeTable(NULL), mSmdpTable(NULL), mBemcDirty(true),
    mBprsDirty(true), mSmdeDirty(true), mSmdpDirty(true)
{
    mChain = StMaker::GetChain();
    
    reset_bemc_cache();
    reset_bprs_cache();
    reset_smde_cache();
    reset_smdp_cache();
    
    if(!mChain) {
        StDbManager *mgr = StDbManager::Instance();
        StDbConfigNode *db = mgr->initConfig("Calibrations_emc");
        mBemcTable = db->addDbTable("bemcMap");
        mSmdeTable = db->addDbTable("bsmdeMap");
        mSmdpTable = db->addDbTable("bsmdpMap");
        mBprsTable = db->addDbTable("bprsMap");
        
        int elements[18000]; for(int i=0; i<18000; i++) elements[i] = i+1;
        
        mBemcTable->resizeElementID(4800);
        mBemcTable->setElementID(elements, 4800);
        
        mBprsTable->resizeElementID(4800);
        mBprsTable->setElementID(elements, 4800);
        
        mSmdeTable->resizeElementID(18000);
        mSmdeTable->setElementID(elements, 18000);
        
        mSmdpTable->resizeElementID(18000);
        mSmdpTable->setElementID(elements, 18000);
        
        mBemcTTable = new St_bemcMap();
        mBemcTTable->Adopt(4800, mBemcTable->GetTable());
        
        mBprsTTable = new St_bprsMap();
        mBprsTTable->Adopt(4800, mBprsTable->GetTable());
        
        mSmdeTTable = new St_bsmdeMap();
        mSmdeTTable->Adopt(18000, mSmdeTable->GetTable());
        
        mSmdpTTable = new St_bsmdpMap();
        mSmdpTTable->Adopt(18000, mSmdpTable->GetTable());
        
        SetDateTime(date, time);
    }
}

StEmcMappingDb::~StEmcMappingDb() {
    // I'm intentionally not deleting the TTables created on the heap above.  If
    // I tried to delete them I'd end up with a double-delete of the mapping 
    // memory, since the StDbTables already own it.  I don't want to copy the
    // data because it's ~6 MB or so.  Each TTable only has 2 Long_ts and an
    // Int_t; I think this should be OK.
}

void StEmcMappingDb::SetDateTime(int date, int time) {
    if(mChain) {
        LOG_DEBUG << "StEmcMappingDb::SetDateTime is illegal in a chain" << endm;
    }
    else {
        mBeginTime.Set(date, time);
        unsigned unix = TUnixTime::Convert(mBeginTime, true);
        if( !(mBemcTable->getBeginTime() < unix < mBemcTable->getEndTime()) ) {
            mBemcDirty = true;
            reset_bemc_cache();
        }
        if( !(mBprsTable->getBeginTime() < unix < mBprsTable->getEndTime()) ) {
            mBprsDirty = true;
            reset_bprs_cache();
        }
        if( !(mSmdeTable->getBeginTime() < unix < mSmdeTable->getEndTime()) ) {
            mSmdeDirty = true;
            reset_smde_cache();
        }
        if( !(mSmdpTable->getBeginTime() < unix < mSmdpTable->getEndTime()) ) {
            mSmdpDirty = true;
            reset_smdp_cache();
        }
    }
}

void StEmcMappingDb::SetFlavor(const char *flavor, const char *tablename) {
    if(mChain) {
        LOG_DEBUG << "StEmcMappingDb::SetFlavor is illegal in a chain" << endm;
    }
    else {
        if(!tablename || !strcmp(tablename, "bemcMap"))  {
            if(strcmp(mBemcTable->getFlavor(), flavor)) {
                mBemcTable->setFlavor(flavor);
                mBemcDirty = true;
                reset_bemc_cache();
            }
        }
        if(!tablename || !strcmp(tablename, "bprsMap")) {
            if(strcmp(mBprsTable->getFlavor(), flavor)) {
                mBprsTable->setFlavor(flavor);
                mBprsDirty = true;
                reset_bprs_cache();
            }
        }
        if(!tablename || !strcmp(tablename, "bsmdeMap")) {
            if(strcmp(mSmdeTable->getFlavor(), flavor)) {
                mSmdeTable->setFlavor(flavor);
                mSmdeDirty = true;
                reset_smde_cache();
            }
        }
        if(!tablename || !strcmp(tablename, "bsmdpMap")) {
            if(strcmp(mSmdpTable->getFlavor(), flavor)) {
                mSmdpTable->setFlavor(flavor);
                mSmdpDirty = true;
                reset_smdp_cache();
            }
        }
    }
}

void StEmcMappingDb::SetMaxEntryTime(int date, int time) {
    if(mChain) {
        LOG_DEBUG << "StEmcMappingDb::SetMaxEntryTime is illegal in a chain" << endm;
    }
    else {
        unsigned unixMax = TUnixTime::Convert(TDatime(date,time), true);
        if(mBemcTable->getProdTime() != unixMax) {
            mBemcTable->setProdTime(unixMax);
            mBemcDirty = true;
            reset_bemc_cache();
        }
        if(mBprsTable->getProdTime() != unixMax) {
            mBprsTable->setProdTime(unixMax);
            mBprsDirty = true;
            reset_bprs_cache();
        }
        if(mSmdeTable->getProdTime() != unixMax) {
            mSmdeTable->setProdTime(unixMax);
            mSmdeDirty = true;
            reset_smde_cache();
        }
        if(mSmdpTable->getProdTime() != unixMax) {
            mSmdpTable->setProdTime(unixMax);
            mSmdpDirty = true;
            reset_smdp_cache();
        }        
    }
}

// struct declaration auto-generated at $STAR/include/bemcMap.h
const bemcMap_st* StEmcMappingDb::bemc() const {
    maybe_reload(kBarrelEmcTowerId);
    return mBemcTTable->GetTable();
}

// struct declaration auto-generated at $STAR/include/bprsMap.h
const bprsMap_st* StEmcMappingDb::bprs() const {
    maybe_reload(kBarrelEmcPreShowerId);
    return mBprsTTable->GetTable();
}

// struct declaration auto-generated at $STAR/include/bsmdeMap.h
const bsmdeMap_st* StEmcMappingDb::bsmde() const {
    maybe_reload(kBarrelSmdEtaStripId);
    return mSmdeTTable->GetTable();
}

// struct declaration auto-generated at $STAR/include/bsmdpMap.h
const bsmdpMap_st* StEmcMappingDb::bsmdp() const {
    maybe_reload(kBarrelSmdPhiStripId);
    return mSmdpTTable->GetTable();
}

int 
StEmcMappingDb::softIdFromMES(StDetectorId det, int m, int e, int s) const {
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
StEmcMappingDb::softIdFromCrate(StDetectorId det, int crate, int channel) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemc();
        if(!mCacheCrate[crate-1][channel]) {
            for(int i=0; i<4800; ++i) {
                mCacheCrate[map[i].crate-1][map[i].crateChannel] = i+1;
            }
        }
        
        // confirm that cache is still valid
        short id = mCacheCrate[crate-1][channel];
        if(map[id-1].crate == crate && map[id-1].crateChannel == channel) {
            return id;
        } else {
            reset_bemc_cache();
            return softIdFromCrate(det, crate, channel);
        }
    }
    return 0;
}

int 
StEmcMappingDb::softIdFromDaqId(StDetectorId det, int daqID) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemc();
        if(!mCacheDaqId[daqID]) {
            for(int i=0; i<4800; ++i) {
                mCacheDaqId[map[i].daqID] = i+1;
            }
        }
        
        // confirm that cache is still valid
        if(map[mCacheDaqId[daqID]-1].daqID == daqID) {
            return mCacheDaqId[daqID];
        } else {
            reset_bemc_cache();
            return softIdFromDaqId(det, daqID);
        }
    }
    return 0;
}

int 
StEmcMappingDb::softIdFromTDC(StDetectorId det, int TDC, int channel) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemc();
        if(!mCacheTDC[TDC][channel]) {
            for(int i=0; i<4800; ++i) {
                mCacheTDC[map[i].TDC][map[i].crateChannel] = i+1;
            }
        }
        
        // confirm that cache is still valid
        short id = mCacheTDC[TDC][channel];
        if(map[id-1].TDC == TDC && map[id-1].crateChannel == channel) {
            return id;
        } else {
            reset_bemc_cache();
            return softIdFromTDC(det, TDC, channel);
        }
    }
    return 0;
}

int 
StEmcMappingDb::softIdFromRDO(StDetectorId det, int rdo, int channel) const {
    short id;
    switch(det) {
        case kBarrelEmcPreShowerId:
        const bprsMap_st* prs = bprs();
        if(mCacheBprsRdo[rdo][channel] == -1) {
            for(int i=0; i<4800; i++) {
                if(prs[i].rdo == rdo && prs[i].rdoChannel == channel) {
                    mCacheBprsRdo[rdo][channel] = i+1;
                    return i+1;
                }
            }
        }

        switch((id = mCacheBprsRdo[rdo][channel])) {
            // didn't find it on a linear search, must be empty channel
            case -1:
            mCacheBprsRdo[rdo][channel] = 0;
            return 0;
            
            // check cache validity before accepting an empty channel
            case 0:
            if(maybe_reset_cache(kBarrelEmcPreShowerId)) 
                return softIdFromRDO(det, rdo, channel);
            else return 0;
            
            default:
            if(prs[id-1].rdo == rdo && prs[id-1].rdoChannel == channel) {
                return id;
            } else {
                reset_bprs_cache();
                return softIdFromRDO(det, rdo, channel);
            }
        }
        
        case kBarrelSmdEtaStripId: case kBarrelSmdPhiStripId:
        const bsmdeMap_st *smde = bsmde();
        const bsmdpMap_st *smdp = bsmdp();
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
        
        switch((id = mCacheSmdRdo[rdo][channel])) {
            // didn't find it on a linear search, must be empty channel
            case -1:
            mCacheSmdRdo[rdo][channel] = 0;
            return 0;
            
            // check cache validity before accepting an empty channel
            case 0:
            if(maybe_reset_cache(det)) 
                return softIdFromRDO(det, rdo, channel);
            else return 0;
            
            // confirm result
            default:
            if(smde[id-1].rdo == rdo && smde[id-1].rdoChannel == channel) {
                if(det == kBarrelSmdEtaStripId) return id;
                return 0;
            }
            else if(smdp[id-1].rdo == rdo && smdp[id-1].rdoChannel == channel) {
                if(det == kBarrelSmdPhiStripId) return id;
                return 0;
            } 
            else {
                reset_smde_cache(); // also resets smdp cache
                return softIdFromRDO(det, rdo, channel);
            }
        }
        
        default: break;
    }
    return 0;
}

/* Private methods used for caching SQL query results */
void StEmcMappingDb::maybe_reload(StDetectorId det) const {
    switch(det) {
        case kBarrelEmcTowerId:
        if(mChain) {
            if(!mBemcTTable) {
                TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
                if(DB) mBemcTTable = static_cast<St_bemcMap*>(DB->Find("bemcMap"));
            }
        } else {
            if(mBemcDirty) reload_dbtable(mBemcTable);
            mBemcDirty = false;
        }
        break;
        
        case kBarrelEmcPreShowerId:
        if(mChain) {
            if(!mBprsTTable) {
                TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
                if(DB) mBprsTTable = static_cast<St_bprsMap*>(DB->Find("bprsMap"));
            }
        } else {
            if(mBprsDirty) reload_dbtable(mBprsTable);
            mBprsDirty = false;
        }
        break;
        
        case kBarrelSmdEtaStripId:
        if(mChain) {
            if(!mSmdeTTable) {
                TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
                if(DB) mSmdeTTable = static_cast<St_bsmdeMap*>(DB->Find("bsmdeMap"));
            }
        } else {
            if(mSmdeDirty) reload_dbtable(mSmdeTable);
            mSmdeDirty = false;
        }
        break;
        
        case kBarrelSmdPhiStripId:
        if(mChain) {
            if(!mSmdpTTable) {
                TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
                if(DB) mSmdpTTable = static_cast<St_bsmdpMap*>(DB->Find("bsmdpMap"));
            }
        } else {
            if(mSmdpDirty) reload_dbtable(mSmdpTable);
            mSmdpDirty = false;
        }
        default: break;
    }
}

void StEmcMappingDb::reload_dbtable(StDbTable* table) const {
    LOG_DEBUG << "(re)loading mapping table using StDbManager" << endm;
    StDbManager *mgr = StDbManager::Instance();
    mgr->setVerbose(false);
    mgr->setRequestTime(mBeginTime.AsSQLString());
    mgr->fetchDbTable(table);
}

bool StEmcMappingDb::maybe_reset_cache(StDetectorId det) const {
    // caches are proactively reset when flipping dirty flags, so ...
    if(!mChain) return false;
    
    Int_t version;
    switch(det) {
        case kBarrelEmcTowerId:
        if((version = mChain->GetValidity(mBemcTTable,NULL)) != mBemcValidity) {
            mBemcValidity = version;
            reset_bemc_cache();
            return true;
        }
        break;
        
        case kBarrelEmcPreShowerId:
        if((version = mChain->GetValidity(mBprsTTable,NULL)) != mBprsValidity) {
            mBprsValidity = version;
            reset_bprs_cache();
            return true;
        }
        break;
        
        case kBarrelSmdEtaStripId:
        if((version = mChain->GetValidity(mSmdeTTable,NULL)) != mSmdeValidity) {
            mSmdeValidity = version;
            reset_smde_cache();
            return true;
        }
        break;
        
        case kBarrelSmdPhiStripId:
        if((version = mChain->GetValidity(mSmdpTTable,NULL)) != mSmdpValidity) {
            mSmdpValidity = version;
            reset_smdp_cache();
            return true;
        }
        default: break;
    }
    return false;
}

void StEmcMappingDb::reset_bemc_cache() const {
    memset(mCacheCrate, 0, sizeof(mCacheCrate));
    memset(mCacheDaqId, 0, sizeof(mCacheDaqId));
    memset(mCacheTDC, 0, sizeof(mCacheTDC));
}

void StEmcMappingDb::reset_bprs_cache() const {
    memset(mCacheBprsRdo, -1, sizeof(mCacheBprsRdo));
}

void StEmcMappingDb::reset_smde_cache() const {
    memset(mCacheSmdRdo, -1, sizeof(mCacheSmdRdo));
}

void StEmcMappingDb::reset_smdp_cache() const {
    memset(mCacheSmdRdo, -1, sizeof(mCacheSmdRdo));
}

/*****************************************************************************
 * $Log: StEmcMappingDb.cxx,v $
 * Revision 1.1  2009/01/08 02:16:18  kocolosk
 * move StEmcMappingDb/StEmcDecoder to StEmcUtil/database
 *
 * Revision 2.1  2008/12/05 19:05:32  kocolosk
 * new DB-backed implementation of StEmcDecoder
 *
 *****************************************************************************/
