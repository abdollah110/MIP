import ROOT
from ROOT import *
path="/home/otsdaq/CMSTimingJarvis/2019_04_April_CMSTiming/Tracks/"
root1=path+"Run14135_CMSTiming_SlowTriggerStream_converted.root"
InFile_Trk=TFile(root1,"open")
Tree=InFile_Trk.Get("CMSTiming/event")
nentry = Tree.GetEntries()
print nentry



#1 unit = 144 ns

void Sync_cpp(){

long long bco=0;
TFile * ff = TFile("2019_04_April_CMSTiming/Tracks/Run14422_CMSTiming_SlowTriggerStream_converted.root")
TTree * tr = (TTree *)ff->Get("CMSTiming")
tr->SetBranchAddress("bco",  &bco);

for( int i; i < tr->GetEntries(); i++){
tr->GetEntry(i);
std::cout<<"bco= "<<bco<<"\n";

}


}


def main(args):
    
    path="2019_04_April_CMSTiming/Tracks/"
    #path="/data/TestBeam/2019_04_April_CMSTiming/CMSTimingConverted/"
    rootFile=path+"Run%s_CMSTiming_%sTriggerStream_converted.root"%(args.runnumber,args.mode)
    InFile_Trk=TFile(rootFile,"open")
    Tree=InFile_Trk.Get(args.treename)
    nentry = Tree.GetEntries()
    
    print 'number of events is ', nentry
    
    #first event
    Tree.GetEntry(0)
    ev0_time=Tree.bco*(144E-9)
    
    #loop over all events
    for entry in xrange(nentry):
        if (entry % 10000 == 0): print "Entry is : ", entry
        Tree.GetEntry(entry)
        print entry, Tree.trigger, Tree.bco, Tree.bco*(144E-9) ,  Tree.bco*(144E-9)-ev0_time




if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--treename', '-t', action='store', dest='treename', default='CMSTiming', help='name of input tree')
    parser.add_argument('--runNumber', '-r', action='store', dest='runnumber', default='14185', help='run number')
    parser.add_argument('--Mode', '-m', action='store', dest='mode', default='Slow', help='Fast or Slow mode')
    
    main(parser.parse_args())


