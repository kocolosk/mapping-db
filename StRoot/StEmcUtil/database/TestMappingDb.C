TestMappingDb(bool useMaker=false, bool useJprof=true) {
    gROOT->Macro("loadMuDst.C");
    StVirtualEmcMappingDb *mapping = NULL;
    if(useJprof) {
        gSystem->Setenv("JPROF_FLAGS", "JP_START JP_PERIOD=0.001");
        gSystem->Load("libJprof");
    }
    StChain *chain = NULL;
    if(useMaker) {
        chain = new StChain;
        gSystem->Load("StDbBroker");
        gSystem->Load("St_db_Maker");
        gSystem->Load("StEmcDbMaker");
        St_db_Maker *dbMk = new St_db_Maker("StarDb", "MySQL:StarDb");
        StEmcDbMaker *emcDb = new StEmcDbMaker;
        dbMk->SetDateTime(20090101,0);
        chain->Init();
        // mapping = emcDb;
    }
    if(!mapping) mapping = StEmcMappingDb::instance();
    TStopwatch timer;
    
    if(useMaker) chain->Make();
    else {
        mapping->bemcMap(1);
        mapping->bsmdeMap(1);
        mapping->bsmdpMap(1);
    }
    
    cerr << "init: " << timer.CpuTime() << '\t' << timer.RealTime() << endl;
    timer.Start();
    
    for(int counter = 0; counter < 20; ++counter) {
        short id;
        // for(id=1; id<=4800; ++id) {
        //     short id2 = mapping->softIdFromDaqId(kBarrelEmcTowerId, 
        //         mapping->bemcMap(id).daqID);
        //     assert(id == id2);
        // }
        
        // for(id=1; id<=18000; ++id) {
        //     short id2 = mapping->softIdFromRDO(kBarrelSmdEtaStripId, 
        //         mapping->bsmdeMap(id).rdo, mapping->bsmdeMap(id).rdoChannel);
        //     if(id != id2) cerr << id << '\t' << id2 << endl;
        //     assert(id == id2);
        // }
        
        for(short rdo=0; rdo<8; ++rdo) {
            for(short channel=0; channel<4800; ++channel) {
                short id = mapping->softIdFromRDO(kBarrelSmdPhiStripId, rdo, channel);
                // cerr << rdo << '\t' << rdoChannel << endl;
                if(id > 0) {
                    assert(mapping->bsmdpMap(id).rdo == rdo);
                    assert(mapping->bsmdpMap(id).rdoChannel == channel);
                }
            }
        }
        
        for(id=1; id<=4800; ++id) {
            short id2 = mapping->softIdFromCrate(kBarrelEmcTowerId, 
                mapping->bemcMap(id).crate, mapping->bemcMap(id).crateChannel);
            short id3 = mapping->softIdFromTDC(kBarrelEmcTowerId, 
                mapping->bemcMap(id).TDC, mapping->bemcMap(id).crateChannel);
            short id4 = mapping->softIdFromMES(kBarrelEmcTowerId, 
                mapping->bemcMap(id).m, mapping->bemcMap(id).e, mapping->bemcMap(id).s);
            if(id != id2) cerr << id << '\t' << id2 << endl;
            assert(id == id2);
            if(id != id3) cerr << id << '\t' << id2 << endl;
            assert(id == id3);
            if(id != id4) cerr << id << '\t' << id2 << endl;
            assert(id == id4);
        }
        
        if(counter == 0) {
            cerr << "first pass: " << timer.CpuTime() << '\t' << 
                timer.RealTime() << endl;
            timer.Start();
        }
    }
    
    cerr << "loop: " << timer.CpuTime() << '\t' << timer.RealTime() << endl;
    return 0;
}
