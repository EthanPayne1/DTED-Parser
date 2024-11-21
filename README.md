# DTED (Digital Terrain Elevation Data) Handler in GDAL

## Overview
DTED is a military specification for storing terrain elevation data used by the U.S. Department of Defense and Geographic Information Systems. This implementation provides functionality to read and process DTED files through GDAL.

## DTED Levels
DTED comes in three resolution levels:

* Level 0: Lowest resolution (~1km)
* Level 1: Medium resolution (~100m)
* Level 2: Highest resolution (~30m)

## File Format
DTED Grid Example (3601x3601 pixels for Level 2):
```
   [1000m][1200m][900m]...
   [800m] [750m] [600m]...
   [500m] [450m] [400m]...
```
How This Code Works
Core Features
Opens DTED files using GDAL library
Reads elevation data row by row
Finds highest elevation point
Converts grid coordinates to latitude/longitude
Dependencies
GDAL (Geospatial Data Abstraction Library)
C++ compiler
Installation
# macOS (using Homebrew)
brew install gdal

### Building
```g++ -o find_highest_point find_highest_point.cpp $(gdal-config --cflags) $(gdal-config --libs)```

### Usage
```./find_highest_point path/to/file.dt2```

## Code Structure
## Main Components

### DTEDHighPoint Class
- Handles file reading
- Tracks maximum elevation  
- Converts coordinates

### Data Processing
- Reads elevation data in 16-bit integers
- Scans entire grid for highest point 
- Uses geotransform for coordinate conversion

### Output Example
Highest point:
```
Elevation: 2500 meters
Location: 35.6789°N, 138.7654°E
```

## Technical Details

### Resolution
- DTED-2 (.dt2) files: 3601x3601 grid
- Each pixel represents ~30m on Earth's surface
- Elevation precision: 1 meter

### Memory Usage
- Efficient row-by-row reading
- Only one scanline in memory at a time
- 16-bit integer storage per elevation point

### Geographic Transformation
[x,y] grid coordinates → [latitude,longitude] using:
- Origin point (top-left)
- Pixel spacing
- Rotation parameters


### DTEDDataset
Main class handling DTED file operations:
```cpp
class DTEDDataset {
  char *pszFilename;
  DTED *psDTED;
  bool bVerifyChecksum;
  // ... other members
};
```

### DTEDHighPoint
Class for finding elevation extremes:
```cpp
class DTEDHighPoint {
  double maxElevation;
  double latitude;
  double longitude;
};
```

## Features

### 1. File Operations
* Reading DTED files
* Accessing elevation data
* Metadata handling
* Coordinate transformations

### 2. Metadata Support
```
DTED_VerticalAccuracy
DTED_HorizontalAccuracy
DTED_VerticalDatum
DTED_HorizontalDatum
```

### 3. Configuration Options
```
GDAL_DTED_SINGLE_BLOCK=TRUE/FALSE  // Read mode
DTED_VERIFY_CHECKSUM=YES/NO        // Data validation
```

## Usage Example
```cpp
// Initialize DTED file handler
DTEDHighPoint finder;

// Process DTED file
if (finder.findHighestPoint("elevation.dt1")) {
  double elevation = finder.getMaxElevation();
  double lat = finder.getLatitude();
  double lon = finder.getLongitude();
}
```

## File Structure

### Header Information
* Origin coordinates
* Resolution data
* Security classifications
* Reference system info
* Data compilation details

### Data Organization
* Column-major storage
* North-south profile alignment
* Elevation values in meters
* Special handling for no-data values

## Dependencies
* GDAL core library
* C++ Standard Library
* Common Portability Library (CPL)

## Technical Documentation
For detailed API documentation and implementation specifics, see:

* [GDAL DTED Documentation](https://gdal.org/drivers/raster/dted.html)
* DTED Format Specification

## Notes
* Default horizontal datum: WGS84
* Elevation referenced to mean sea level
* Preserves security codes and partial cell indicators
* Supports both reading and limited writing capabilities

## Building
```bash
cmake -DGDAL_ENABLE_DRIVER_DTED=ON ..
make
```
For development and testing, refer to CMakeLists.txt.
