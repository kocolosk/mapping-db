TestSorter() {
    gROOT->Macro("loadMuDst.C");
    gSystem->Load("StDbBroker");
    gSystem->Load("St_db_Maker");
    
    // get the table
    St_db_Maker dbMk("StarDb", "MySQL:StarDb");
    dbMk.Init();
    TDataSet *DB = dbMk.GetInputDB("Calibrations/emc/map");
    St_bemcMap* bemc = (St_bemcMap*)DB->Find("bemcMap");
    
    // build the sorter
    TString key="daqID";
    TTableSorter sorter(bemc, key);
    
    int matches = 0;
    int linearFound = 0;
    int sorterFound = 0;
    for(short daq=0; daq<4800; ++daq) {
        // find row by linear search first
        short linearSearchResult = -1;
        for(int i=0; i<4800; ++i) {
            if( ((*bemc)[i]).daqID == daq ) {
                linearSearchResult = i;
                linearFound += 1;
                break;
            }
        }
        
        // now try to find daqID using TTableSorter
        int sorterResult = sorter[daq];
        if(sorterResult > -1) sorterFound += 1;
        
        cout << "daqID " << daq << "\tlinear found row: " << linearSearchResult 
            << "\tsorter found row: " << sorterResult << endl;
        
        if(linearSearchResult == sorterResult) matches += 1;
    }
    
    cout << "Summary: linear found " << linearFound << ", sorter found " << 
        sorterFound << " and we had " << matches << " agreements." << endl;
}