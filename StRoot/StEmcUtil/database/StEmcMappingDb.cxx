// $Id: StEmcMappingDb.cxx,v 1.1 2009/01/08 02:16:18 kocolosk Exp $

#include "StEmcMappingDb.h"

#include "TTableSorter.h"

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
    mBprsDirty(true), mSmdeDirty(true), mSmdpDirty(true), mCacheDaqId(NULL)
{
    mChain = StMaker::GetChain();
    
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

void StEmcMappingDb::Init() {
    if(!mChain) return;
    LOG_DEBUG << "initializing StEmcMappingDb using St_db_Maker" << endm;
    TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
    
    Int_t version;
    
    if((version = mChain->GetValidity(mBemcTTable,NULL)) != mBemcValidity) {
        mBemcValidity = version;
        mBemcTTable = static_cast<St_bemcMap*>(DB->Find("bemcMap"));
    }
    if((version = mChain->GetValidity(mBprsTTable,NULL)) != mBprsValidity) {
        mBprsValidity = version;
        mBprsTTable = static_cast<St_bprsMap*>(DB->Find("bprsMap"));
    }
    if((version = mChain->GetValidity(mSmdeTTable,NULL)) != mSmdeValidity) {
        mSmdeValidity = version;
        mSmdeTTable = static_cast<St_bsmdeMap*>(DB->Find("bsmdeMap"));
    }
    if((version = mChain->GetValidity(mSmdpTTable,NULL)) != mSmdpValidity) {
        mSmdpValidity = version;
        mSmdpTTable = static_cast<St_bsmdpMap*>(DB->Find("bsmdpMap"));
    }
}

void StEmcMappingDb::SetDateTime(int date, int time) {
    if(mChain) {
        LOG_ERROR << "StEmcMappingDb::SetDateTime is illegal in a chain" << endm;
    }
    else {
        mBeginTime.Set(date, time);
        mBemcDirty = true;
        mBprsDirty = true;
        mSmdeDirty = true;
        mSmdpDirty = true;
    }
}

void StEmcMappingDb::SetFlavor(const char *flavor, const char *tablename) {
    if(mChain) {
        LOG_ERROR << "StEmcMappingDb::SetFlavor is illegal in a chain" << endm;
    }
    else {
        if(tablename) {
            if(!strcmp(tablename, "bemcMap"))  {
                mBemcTable->setFlavor(flavor);
                mBemcDirty = true;
            }
            if(!strcmp(tablename, "bprsMap")) {
                mBprsTable->setFlavor(flavor);
                mBprsDirty = true;
            }
            if(!strcmp(tablename, "bsmdeMap")) {
                mSmdeTable->setFlavor(flavor);
                mSmdeDirty = true;
            }
            if(!strcmp(tablename, "bsmdpMap")) {
                mSmdpTable->setFlavor(flavor);
                mSmdpDirty = true;
            }
        }
        else {
            mBemcTable->setFlavor(flavor);
            mBprsTable->setFlavor(flavor);
            mSmdeTable->setFlavor(flavor);
            mSmdpTable->setFlavor(flavor);
            mBemcDirty = true;
            mBprsDirty = true;
            mSmdeDirty = true;
            mSmdpDirty = true;
        }
    }
}

void StEmcMappingDb::SetMaxEntryTime(int date, int time) {
    if(mChain) {
        LOG_ERROR << "StEmcMappingDb::SetMaxEntryTime is illegal in a chain" << endm;
    }
    else {
        unsigned unixMax = TUnixTime::Convert(TDatime(date,time), true);
        mBemcTable->setProdTime(unixMax);
        mBprsTable->setProdTime(unixMax);
        mSmdeTable->setProdTime(unixMax);
        mSmdpTable->setProdTime(unixMax);
        mBemcDirty = true;
        mBprsDirty = true;
        mSmdeDirty = true;
        mSmdpDirty = true;
    }
}

// struct declaration auto-generated at $STAR/include/bemcMap.h
const bemcMap_st* StEmcMappingDb::bemc() const {
    if(mChain) {
        if(!mBemcTTable) {
            TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
            mBemcTTable = DB ? static_cast<St_bemcMap*>(DB->Find("bemcMap")) : 0;
        }
        return mBemcTTable ? mBemcTTable->GetTable() : 0;
    }
    maybe_reload(kBarrelEmcTowerId);
    return (bemcMap_st*)mBemcTable->GetTable();
}

// struct declaration auto-generated at $STAR/include/bprsMap.h
const bprsMap_st* StEmcMappingDb::bprs() const {
    if(mChain) {
        if(!mBprsTTable) {
            TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
            mBprsTTable = DB ? static_cast<St_bprsMap*>(DB->Find("bprsMap")) : 0;
        }
        return mBprsTTable ? mBprsTTable->GetTable() : 0;
    } 
    maybe_reload(kBarrelEmcPreShowerId);
    return (bprsMap_st*)mBprsTable->GetTable();
}

// struct declaration auto-generated at $STAR/include/bsmdeMap.h
const bsmdeMap_st* StEmcMappingDb::bsmde() const {
    if(mChain) {
        if(!mSmdeTTable) {
            TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
            mSmdeTTable = DB ? static_cast<St_bsmdeMap*>(DB->Find("bsmdeMap")) : 0;
        }
        return mSmdeTTable ? mSmdeTTable->GetTable() : 0;
    } 
    maybe_reload(kBarrelSmdEtaStripId);
    return (bsmdeMap_st*)mSmdeTable->GetTable();
}

// struct declaration auto-generated at $STAR/include/bsmdpMap.h
const bsmdpMap_st* StEmcMappingDb::bsmdp() const {
    if(mChain) {
        if(!mSmdpTTable) {
            TDataSet *DB = mChain->GetInputDB("Calibrations/emc/map");
            mSmdpTTable = DB ? static_cast<St_bsmdpMap*>(DB->Find("bsmdpMap")) : 0;
        }
        return mSmdpTTable ? mSmdpTTable->GetTable() : 0;
    } 
    maybe_reload(kBarrelSmdPhiStripId);
    return (bsmdpMap_st*)mSmdpTable->GetTable();
}

int 
StEmcMappingDb::softIdFromMES(StDetectorId det, int m, int e, int s) const {
    switch(det) {
        case kBarrelEmcTowerId:
        const bemcMap_st* tow = bemc();
        for(int i=0; i<4800; i++) {
            if(tow[i].m == m && tow[i].e == e && tow[i].s == s) {
                return i+1;
            }
        }
        break;
        case kBarrelEmcPreShowerId:
        const bprsMap_st* prs = bprs();
        for(int i=0; i<4800; i++) {
            if(prs[i].m == m && prs[i].e == e && prs[i].s == s) {
                return i+1;
            }
        }
        break;
        case kBarrelSmdEtaStripId:
        const bsmdeMap_st* smde = bsmde();
        for(int i=0; i<18000; i++) {
            if(smde[i].m == m && smde[i].e == e && smde[i].s == s) {
                return i+1;
            }
        }
        break;
        case kBarrelSmdPhiStripId:
        const bsmdpMap_st* smdp = bsmdp();
        for(int i=0; i<18000; i++) {
            if(smdp[i].m == m && smdp[i].e == e && smdp[i].s == s) {
                return i+1;
            }
        }
        default: break;
    }
    return 0;
}

int 
StEmcMappingDb::softIdFromCrate(StDetectorId det, int crate, int channel) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemc();
        for(int i=0; i<4800; i++) {
            if(map[i].crate == crate) {
                if(map[i].crateChannel == channel) 
                    return i+1;
            }
        }
    }
    return 0;
}

int 
StEmcMappingDb::softIdFromDaqId(StDetectorId det, int daqId) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemc();
        for(int i=0; i<4800; ++i) {
            if(map[i].daqID == daqId) return i+1;
        }
        //if(!mCacheDaqId) {
        //    TString key("daqID");
        //    mCacheDaqId = new TTableSorter(mBemcTTable, key);
        //}
        //cerr << "this sorter has N keys " << mCacheDaqId->CountKeys() << endl;
        //return (*mCacheDaqId)[daqId] + 1;
    }
    return 0;
}

int 
StEmcMappingDb::softIdFromTDC(StDetectorId det, int TDC, int channel) const {
    if(det == kBarrelEmcTowerId) {
        const bemcMap_st* map = bemc();
        for(int i=0; i<4800; i++) {
            if(map[i].TDC == TDC) {
                if(map[i].crateChannel == channel) return i+1;
            }
        }
    }
    return 0;
}

int 
StEmcMappingDb::softIdFromRDO(StDetectorId det, int rdo, int channel) const {
    switch(det) {
        case kBarrelEmcPreShowerId:
        const bprsMap_st* prs = bprs();
        for(int i=0; i<4800; i++) {
            if(prs[i].rdo == rdo && prs[i].rdoChannel == channel) {
                return i+1;
            }
        }
        break;
        case kBarrelSmdEtaStripId:
        const bsmdeMap_st *smde = bsmde();
        for(int i=0; i<18000; i++) {
            if(smde[i].rdo == rdo && smde[i].rdoChannel == channel) {
                return i+1;
            }
        }
        break;
        case kBarrelSmdPhiStripId:
        const bsmdpMap_st *smdp = bsmdp();
        for(int i=0; i<18000; i++) {
            if(smdp[i].rdo == rdo && smdp[i].rdoChannel == channel) {
                return i+1;
            }
        }
        default: break;
    }
    return 0;
}

/* Private methods used for caching SQL query results */
void StEmcMappingDb::maybe_reload(StDetectorId det) const {
    if(mChain) return;
    switch(det) {
        case kBarrelEmcTowerId:
        if(mBemcDirty) reload_dbtable(mBemcTable);
        mBemcDirty = false;
        break;
        
        case kBarrelEmcPreShowerId:
        if(mBprsDirty) reload_dbtable(mBprsTable);
        mBprsDirty = false;
        break;
        
        case kBarrelSmdEtaStripId:
        if(mSmdeDirty) reload_dbtable(mSmdeTable);
        mSmdeDirty = false;
        break;
        
        case kBarrelSmdPhiStripId:
        if(mSmdpDirty) reload_dbtable(mSmdpTable);
        mSmdpDirty = false;
        default: break;
    }
}

void StEmcMappingDb::reload_dbtable(StDbTable* table) const {
    LOG_DEBUG << "(re)loading mapping table using StDbManager" << endm;
    StDbManager *mgr = StDbManager::Instance();
    mgr->setVerbose(false);
    mgr->setRequestTime(mBeginTime.AsSQLString());
    mgr->fetchDbTable(table);
    delete mCacheDaqId; mCacheDaqId = NULL;
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
