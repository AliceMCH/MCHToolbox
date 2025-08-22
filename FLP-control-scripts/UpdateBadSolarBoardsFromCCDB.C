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

void UpdateBadSolarBoardsFromCCDB(const char* ccdbUrl = "http://ccdb-test.cern.ch:8080")
{
  //ccdbUrl = "http://ccdb-test.cern.ch:8080";
  std::map<std::string, std::string> metadata;

  o2::ccdb::CcdbApi api;
  api.init(ccdbUrl);

  auto badSolarBoards = api.retrieveFromTFileAny<TObjString>("MCH/Calib/BadSOLAR", metadata, DatabaseInterface::Timestamp::Current);

  //std::cout << badSolarBoards->String().Data() << std::endl;
  std::stringstream sstr(badSolarBoards->String().Data());
  std::vector<int> badSolarIds;
  std::string header;
  sstr >> header;

  std::ofstream outfile("bad_solar_ids.txt");
  while (sstr) {
    int sid;
    sstr >> sid;
    if (!sstr) break;
    outfile << sid << " ";
  }
  //std::cout << std::endl;
}
