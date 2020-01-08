// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef TRANSITMAP_CONFIG_TRANSITMAPCONFIG_H_
#define TRANSITMAP_CONFIG_TRANSITMAPCONFIG_H_

#include <string>

namespace transitmapper {
namespace config {

struct Config {
  double lineWidth;
  double lineSpacing;

  std::string name;
  std::string outputPath;
  std::string dbgPath;

  std::string renderMethod;
  std::string optimMethod;
  std::string glpkMPSOutputPath;
  std::string glpkHOutputPath;
  std::string glpkSolutionOutputPath;

  size_t optimRuns;

  bool splittingOpt;
  bool outOptGraph;

  double outputResolution;
  double inputSmoothing;
  double innerGeometryPrecision;

  double outputPadding;

  double outlineWidth;
  std::string outlineColor;

  bool renderStations;
  bool renderNodeFronts;
  bool renderNodeCircles;
  bool renderNodePolygons;
  bool renderStationNames;
  bool renderEdges;

  bool outputStats;
  bool renderStats;
  bool collapseLinePartners;
  bool renderNodeConnections;
  bool expandFronts;
  bool tightStations;

  bool renderDirMarkers;

  bool createCoreOptimGraph;
  bool untangleGraph;
  bool simpleRenderForTwoEdgeNodes;

  bool useGlpkFeasibilityPump;
  bool useGlpkProximSearch;
  int glpkPSTimeLimit;
  int glpkTimeLimit;
  std::string externalSolver;

  double crossPenMultiSameSeg;
  double crossPenMultiDiffSeg;
  double splitPenWeight;
  double stationCrossWeightSameSeg;
  double stationCrossWeightDiffSeg;
  double stationSplitWeight;

  std::string worldFilePath;
};

}  // namespace config
}  // namespace transitmapper

#endif  // TRANSITMAP_CONFIG_TRANSITMAPCONFIG_H_
