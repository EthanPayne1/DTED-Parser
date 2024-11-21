#include <iostream>
#include <limits>

#include "cpl_conv.h"
#include "gdal_priv.h"

class DTEDHighPoint {
 public:
  DTEDHighPoint()
      : maxElevation(std::numeric_limits<double>::lowest()),
        latitude(0),
        longitude(0) {}

  bool findHighestPoint(const char* dtedFilename) {
    GDALAllRegister();

    // Open the DTED dataset
    GDALDataset* poDataset = (GDALDataset*)GDALOpen(dtedFilename, GA_ReadOnly);
    if (!poDataset) {
      std::cerr << "Failed to open DTED file: " << dtedFilename << std::endl;
      return false;
    }

    // Get the raster band (elevation data)
    GDALRasterBand* poBand = poDataset->GetRasterBand(1);
    if (!poBand) {
      std::cerr << "Failed to get raster band" << std::endl;
      GDALClose(poDataset);
      return false;
    }

    // Get raster dimensions
    int width = poBand->GetXSize();
    int height = poBand->GetYSize();
    std::cout << "width: " << width << '\n' << "height: " << height << std::endl;
    // 3601x3601 pixels

    // Get geotransform
    double adfGeoTransform[6];
    // std::cout << adfGeoTransform << std::endl;
    if (poDataset->GetGeoTransform(adfGeoTransform) != CE_None) {
      std::cerr << "Failed to get geotransform" << std::endl;
      GDALClose(poDataset);
      return false;
    }

    // Scan the elevation data
    int maxX = 0, maxY = 0;
    GDALDataType dataType = poBand->GetRasterDataType();
    // std::cout << "scan the elevation data: " << dataType << std::endl; 3?

    // Use Int16 buffer since DTED typically uses 16-bit integers
    GInt16* scanline = (GInt16*)CPLMalloc(sizeof(GInt16) * width);

    for (int y = 0; y < height; y++) {
      if (poBand->RasterIO(GF_Read, 0, y, width, 1, scanline, width, 1,
                           GDT_Int16, 0, 0) != CE_None) {
        continue;
      }

      for (int x = 0; x < width; x++) {
        if (scanline[x] > maxElevation) {
          maxElevation = scanline[x];
          // std::cout << "elevation: " << maxElevation << std::endl;
          maxX = x;
          maxY = y;
        }
      }
    }

    // Convert raster coordinates to geographic coordinates
    longitude = adfGeoTransform[0] + maxX * adfGeoTransform[1];
    latitude = adfGeoTransform[3] + maxY * adfGeoTransform[5];
    std::cout << "Longitude: " << longitude << '\n' << "Latitude: " << latitude << std::endl;
    CPLFree(scanline);
    GDALClose(poDataset);
    return true;
  }

  double getMaxElevation() const { return maxElevation; }
  double getLatitude() const { return latitude; }
  double getLongitude() const { return longitude; }

 private:
  double maxElevation;
  double latitude;
  double longitude;
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <dted_filename>" << std::endl;
    return 1;
  }

  DTEDHighPoint finder;
  if (finder.findHighestPoint(argv[1])) {
    std::cout << "Highest point:" << std::endl
              << "Elevation: " << finder.getMaxElevation() << " meters"
              << std::endl
              << "Location: " << finder.getLatitude() << "°N, "
              << finder.getLongitude() << "°E" << std::endl;
    return 0;
  }

  return 1;
}