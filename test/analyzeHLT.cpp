#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TSystem.h>

#include <iostream>
#include <string>

// needed for trigger studies
#include "DataFormats/Common/interface/TriggerResults.h"
// needed for event-id info
#include "DataFormats/Provenance/interface/EventAuxiliary.h"
//
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "HLTrigger/Timer/test/AnalyzeTriggerResults.h"

using std::cout; using std::endl; using std::string;

#define PRINT_EVTRUN_NO 0

// Christos Leonidopoulos, July 2006


/* usage: analyzeHLT <ROOT file> <process name>
   where, 
   <ROOT file>: file with HLT information to be analyzed (default: hlt.root)
   <process name>: name for cmsRun job in config file (default: "PRODHLT")
*/
int main(int argc, char ** argv)
{
  // load libraries
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  // default arguments
  string filename = "hlt.root";
  string process_name = "PRODHLT";
  // get parameters from command line
  if(argc >= 2) filename = argv[1];
  if(argc >= 3) process_name = argv[2];

  // open file
  TFile file(filename.c_str());
  if(file.IsZombie()) 
    {
      cout << " *** Error opening file " << filename << endl;
      exit(-1);
    }
  TTree * events = dynamic_cast<TTree *>(file.Get("Events") );
  assert(events);

  TBranch * TBtrigRes = 0;
  TBranch * TBevtAux = events->GetBranch("EventAuxiliary");
  assert(TBevtAux);
  //  std::cout << " TBevtAux = " << int(TBevtAux) << std::endl;

  // structure holding the trigger decision
  edm::TriggerResults trigRes;

#if PRINT_EVTRUN_NO
  // structure holding event information
  edm::EventAuxiliary * evtAux = new edm::EventAuxiliary;

  TBevtAux->SetAddress((void *)&evtAux);
#endif

  AnalyzeTriggerResults * tr = 0;

  char tmp_name[1024];
  sprintf(tmp_name, "edmTriggerResults_TriggerResults__%s.obj", 
	  process_name.c_str());
  TBtrigRes = events->GetBranch(tmp_name);
  assert(TBtrigRes);
  TBtrigRes->SetAddress((void*)&trigRes);
  tr = new AnalyzeTriggerResults();

  int n_evts = events->GetEntries();
  
  for(int i = 0; i != n_evts; ++i)
    {

#if PRINT_EVTRUN_NO
      TBevtAux->GetEntry(i);
      cout << " Run # = " << evtAux->id().run() 
	   << " event # = " << evtAux->id().event() 
	   << " entry # = " << i << "\n";
#endif

      TBtrigRes->GetEntry(i);
      tr->analyze(trigRes);

    } // loop over all events


  // get results, do cleanup
  tr->getResults();
  delete tr;

#if PRINT_EVTRUN_NO
  delete evtAux;
#endif
 
  return 0;
}
