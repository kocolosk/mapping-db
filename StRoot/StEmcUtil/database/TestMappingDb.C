TestMappingDb(bool useMaker=false, bool useJprof=true) {
    gROOT->Macro("loadMuDst.C");
    if(useJprof) {
        gSystem->Setenv("JPROF_FLAGS", "JP_START JP_PERIOD=0.001");
        gSystem->Load("libJprof");
    }
    if(useMaker) {
        gSystem->Load("StDbBroker");
        gSystem->Load("St_db_Maker");
        St_db_Maker dbMk("StarDb", "MySQL:StarDb");
        dbMk.Init();
    }
    TStopwatch timer;
    
    StEmcMappingDb mapping;
    //if(useMaker) mapping.Init();
    mapping.bemc(1);
    mapping.bsmde(1);
    
    cerr << "init: " << timer.CpuTime() << '\t' << timer.RealTime() << endl;
    timer.Start();
    
    for(int counter = 0; counter < 5; ++counter) {
        for(int id=1; id<=4800; ++id) {
            int id2 = mapping.softIdFromDaqId(kBarrelEmcTowerId, 
                mapping.bemc(id).daqID);
            assert(id == id2);
        }
        
        for(int id=1; id<=18000; ++id) {
            int id2 = mapping.softIdFromRDO(kBarrelSmdEtaStripId, 
                mapping.bsmde(id).rdo, mapping.bsmde(id).rdoChannel);
            assert(id == id2);
        }
    }
    
    cerr << "loop: " << timer.CpuTime() << '\t' << timer.RealTime() << endl;
    return 0;
}
