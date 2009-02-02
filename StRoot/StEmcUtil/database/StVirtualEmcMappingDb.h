#ifndef STAR_StVirtualEmcMappingDb
#define STAR_StVirtualEmcMappingDb

// $Id$

/*****************************************************************************
 * @class StVirtualEmcMappingDb
 * @author Adam Kocoloski
 *
 * Description of class.
 *****************************************************************************/

#include "TObject.h"

#include "StEvent/StEnumerations.h"

#include "bemcMap.h"
#include "bprsMap.h"
#include "bsmdeMap.h"
#include "bsmdpMap.h"

class StVirtualEmcMappingDb
{
public:
    virtual const bemcMap_st* bemcMap() const                           = 0;
    virtual const bemcMap_st& bemcMap(int softId) const                 = 0;
    
    virtual const bprsMap_st* bprsMap() const                           = 0;
    virtual const bprsMap_st& bprsMap(int softId) const                 = 0;
    
    virtual const bsmdeMap_st* bsmdeMap() const                         = 0;
    virtual const bsmdeMap_st& bsmdeMap(int softId) const               = 0;

    virtual const bsmdpMap_st* bsmdpMap() const                         = 0;
    virtual const bsmdpMap_st& bsmdpMap(int softId) const               = 0;
    
    virtual int softIdFromMES(StDetectorId, int m, int e, int s) const  = 0;
    
    // these are for towers
    virtual int softIdFromCrate(StDetectorId, int crate, int ch) const  = 0;
    virtual int softIdFromDaqId(StDetectorId, int daqId) const          = 0;
    virtual int softIdFromTDC(StDetectorId, int TDC, int channel) const = 0;
    
    // this is for SMD/PRS
    virtual int softIdFromRDO(StDetectorId, int rdo, int channel) const = 0;
    
    ClassDef(StVirtualEmcMappingDb, 1);
};

#endif

/*****************************************************************************
 * $Log$
 *****************************************************************************/
