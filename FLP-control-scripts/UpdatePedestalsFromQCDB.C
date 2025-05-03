#include <QualityControl/MonitorObject.h>
#include <QualityControl/MonitorObjectCollection.h>
#include "QualityControl/ObjectMetadataKeys.h"
#include "QualityControl/CcdbDatabase.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace o2::quality_control::core;
using namespace o2::quality_control::repository;

std::string mDatabaseUrl;
CcdbDatabase mDatabase;

std::tuple<uint64_t, uint64_t, uint64_t, int> getObjectInfo(const std::string path, const std::map<std::string, std::string>& metadata)
{
  // find the time-stamp of the most recent object matching the current activity
  // if ignoreActivity is true the activity matching criteria are not applied

  auto listing = mDatabase.getListingAsPtree(path, metadata, true);
  if (listing.count("objects") == 0) {
    //std::cout << "Could not get a valid listing from db '" << mDatabaseUrl << "' for latestObjectMetadata '" << path << "'" << std::endl;
    return std::make_tuple<uint64_t, uint64_t, uint64_t, int>(0, 0, 0, 0);
  }
  const auto& objects = listing.get_child("objects");
  if (objects.empty()) {
    //std::cout << "Could not find the file '" << path << "' in the db '"
    //                     << mDatabaseUrl << "' for given Activity settings. Zeroes and empty strings are treated as wildcards." << std::endl;
    return std::make_tuple<uint64_t, uint64_t, uint64_t, int>(0, 0, 0, 0);
  } else if (objects.size() > 1) {
    //std::cout << "Expected just one metadata entry for object '" << path << "'. Trying to continue by using the first." << std::endl;
  }

  // retrieve timestamps
  const auto& latestObjectMetadata = objects.front().second;
  uint64_t creationTime = latestObjectMetadata.get<uint64_t>(metadata_keys::created);
  uint64_t validFrom = latestObjectMetadata.get<uint64_t>(metadata_keys::validFrom);
  uint64_t validUntil = latestObjectMetadata.get<uint64_t>(metadata_keys::validUntil);

  // retrieve 'runNumber' metadata element, or zero if not set
  uint64_t runNumber = latestObjectMetadata.get<int>(metadata_keys::runNumber, 0);

  //TDatime datime;
  //datime.Set(creationTime / 1000);
  //std::cout << std::format("Run {}  created at {} ({})\n", runNumber, datime.AsSQLString(), creationTime);

  return std::make_tuple(validFrom, validUntil, creationTime, runNumber);
}

TH2* getHistogram(std::string path, std::string name)
{
  //Activity activity;
  //activity.mProvenance = "qc";
  //auto mo = mDatabase.retrieveMO(path, name, DatabaseInterface::Timestamp::Current, activity);
  //if (!mo) return nullptr;
  //return dynamic_cast<TH2*>(mo->getObject());

  map<string, string> metadata;
  std::string fullPath = std::string("qc/") + path + "/" + name;
  TObject* obj = mDatabase.retrieveTObject(fullPath, metadata, DatabaseInterface::Timestamp::Current);
  
  return dynamic_cast<TH2*>(obj);
}

void UpdatePedestalsFromQCDB(const char* databaseUrl)
{
  std::map<std::string, std::string> metadata;
  std::string dbPrexif = "qc";
  //mDatabaseUrl = "ali-qcdb.cern.ch:8083";
  mDatabase.connect(databaseUrl, "", "", "");

  std::ofstream pedOut("Pedestals.txt");

  auto* pedHist = getHistogram("MCH/MO/Pedestals", "Pedestals_Elec");
  std::cout << "Pedestals histogram: " << pedHist << std::endl;
  if (!pedHist) return;

  auto* noiseHist = getHistogram("MCH/MO/Pedestals", "Noise_Elec");
  std::cout << "Noise histogram: " << noiseHist << std::endl;
  if (!noiseHist) return;

  auto* statHist = getHistogram("MCH/MO/Pedestals", "Statistics_Elec");
  std::cout << "Statistics histogram: " << statHist << std::endl;
  if (!statHist) return;

  int nbinsx = statHist->GetXaxis()->GetNbins();
  int nbinsy = statHist->GetYaxis()->GetNbins();

  std::cout << std::format("# of bins: x={} y={}", nbinsx, nbinsy) << std::endl;
  
  for (int i = 1; i <= nbinsx; i++) {
    int index = i - 1;
    int dsId = (index % 40);
    int linkId = (index / 40) % 12;
    int feeId = index / (12 * 40);
    //std::cout << std::format("index={}  FEEID={}  LINK={}  DS={}", index, feeId, linkId, dsId) << std::endl;

    int cruId = feeId / 2;
    int dataPath = feeId % 2;
    if (dataPath > 0) linkId += 12;
    //std::cout << std::format("index={}  CRUID={}  LINK={}  DS={}", index, cruId, linkId, dsId) << std::endl;

    for (int j = 1; j <= nbinsy; j++) {
      int chanAddr = j - 1;
      int chipId = chanAddr / 32;
      if (chipId > 0) chanAddr -= 32;

      float ped = pedHist->GetBinContent(i, j);
      int iped = static_cast<int>(std::round(ped));
      float noise = noiseHist->GetBinContent(i, j);
      double stat = statHist->GetBinContent(i, j);
      if (stat < 100) {
	ped = 1000;
      }

      pedOut << std::format("{} {} {} {} {} {} 0x0C 0x{:x} {:0.3f} {}\n", cruId, linkId, dsId, chipId, chanAddr, iped, iped, noise, stat);
    }
  }
}
