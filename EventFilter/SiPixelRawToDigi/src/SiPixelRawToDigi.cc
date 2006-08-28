#include "EventFilter/SiPixelRawToDigi/interface/SiPixelRawToDigi.h"

#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigiCollection.h"

#include "DataFormats/DetId/interface/DetId.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"


#include "CondFormats/SiPixelObjects/interface/SiPixelFedCablingMap.h"
#include "CondFormats/DataRecord/interface/SiPixelFedCablingMapRcd.h"

#include "EventFilter/SiPixelRawToDigi/interface/PixelDataFormatter.h"
#include "CondFormats/SiPixelObjects/interface/PixelFEDCabling.h"
using namespace std;


// -----------------------------------------------------------------------------
SiPixelRawToDigi::SiPixelRawToDigi( const edm::ParameterSet& conf ) 
  : eventCounter_(0), fedCablingMap_(0)
{
  edm::LogInfo("SiPixelRawToDigi")<< " HERE ** constructor!" << endl;
  produces< edm::DetSetVector<PixelDigi> >();
}


// -----------------------------------------------------------------------------
SiPixelRawToDigi::~SiPixelRawToDigi() {
  edm::LogInfo("SiPixelRawToDigi")  << " HERE ** SiPixelRawToDigi destructor!";
}


// -----------------------------------------------------------------------------
void SiPixelRawToDigi::beginJob(const edm::EventSetup& c) 
{
}

// -----------------------------------------------------------------------------
void SiPixelRawToDigi::produce( edm::Event& ev,
                              const edm::EventSetup& es) 
{
  PixelDataFormatter formatter;

  cout << " *** HERE1" << endl;
  edm::ESHandle<SiPixelFedCablingMap> map;
  es.get<SiPixelFedCablingMapRcd>().get( map );
  cout << map->version() << endl;
  cout << " *** HERE2" << endl;



  edm::Handle<FEDRawDataCollection> buffers;
//  ev.getByLabel("PixelDaqRawData", rawdata);
  ev.getByType(buffers);

  // create product (digis)
  std::auto_ptr< edm::DetSetVector<PixelDigi> > collection( new edm::DetSetVector<PixelDigi> );

  const vector<PixelFEDCabling> & cabling = map->cabling();
  typedef vector<PixelFEDCabling>::const_iterator FI;
  for (FI it = cabling.begin(); it != cabling.end(); it++) {
     PixelDataFormatter::Digis digis;
     LogDebug("SiPixelRawToDigi")<< " PRODUCE DIGI FOR FED: " <<  (*it).id() << endl;
     
     const FEDRawData& fedRawData = buffers->FEDData( (*it).id() );
     LogDebug("SiPixelRawToDigi")<< "sizeof data buffer: " << fedRawData.size() << endl;
     formatter.interpretRawData( *it, fedRawData, digis);

     typedef PixelDataFormatter::Digis::iterator ID;
     for (ID it = digis.begin(); it != digis.end(); it++) {
       uint32_t detid = it->first;
       edm::DetSet<PixelDigi>& detSet = collection->find_or_insert(detid);
       detSet.data = it->second;
     } 
  }

  ev.put( collection );
}

