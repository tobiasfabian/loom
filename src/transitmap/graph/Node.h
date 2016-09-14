// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosip@informatik.uni-freiburg.de>

#ifndef TRANSITMAP_GRAPH_NODE_H_
#define TRANSITMAP_GRAPH_NODE_H_

#include <set>
#include "gtfsparser/gtfs/Stop.h"
#include "gtfsparser/gtfs/Route.h"
#include "../geo/PolyLine.h"
#include "../util/Geo.h"

using namespace gtfsparser;

namespace transitmapper {
namespace graph {

// forward declarations
class Edge;
class Node;
class EdgeTripGeom;
struct TripOccurance;


typedef std::vector<size_t> Ordering;
typedef std::map<EdgeTripGeom*, Ordering> Configuration;

// forward declaration of TransitGraph
class TransitGraph;

struct NodeFront {
  NodeFront(Edge* e, Node* n, const EdgeTripGeom* refEtg) : n(n), refEtg(refEtg) {
    addEdge(e);
  }

  Node* n; // pointer to node here also

  std::vector<Edge*> edges;

  util::geo::Point getTripOccPos(const gtfs::Route*) const;
  util::geo::Point getTripOccPos(const gtfs::Route* r, const Configuration& c) const;
  util::geo::Point getTripOccPosUnder(const gtfs::Route* r, const graph::Configuration& c,
    const EdgeTripGeom* g, const std::vector<size_t>* order) const;

  const EdgeTripGeom* refEtg;

  geo::PolyLine geom;
  void setGeom(const geo::PolyLine& g) { geom = g; };
  void addEdge(Edge* e) { edges.push_back(e); }
};

struct Partner {
  const NodeFront* front;
  const Edge* edge;
  const EdgeTripGeom* etg;
  const gtfs::Route* route;
};

struct InnerGeometry {
  InnerGeometry(geo::PolyLine g, const gtfs::Route* r, const EdgeTripGeom* etg)
  : geom(g), route(r), etg(etg) {};
  geo::PolyLine geom;
  const gtfs::Route* route;
  const EdgeTripGeom* etg;
};

class Node {

 public:
  explicit Node(util::geo::Point pos);
  Node(double x, double y);
  Node(util::geo::Point pos, gtfs::Stop* stop);
  Node(double x, double y, gtfs::Stop* stop);

  ~Node();

  const std::set<gtfs::Stop*>& getStops() const;
  void addStop(gtfs::Stop* s);
  const util::geo::Point& getPos() const;
  void setPos(const util::geo::Point& p);

  const std::set<Edge*>& getAdjListOut() const {
    return _adjListOut;
  }
  const std::set<Edge*>& getAdjListIn() const {
    return _adjListIn;
  }
  const std::vector<NodeFront>& getMainDirs() const {
    return _mainDirs;
  }
  std::vector<NodeFront>& getMainDirs() {
    return _mainDirs;
  }

  void addMainDir(NodeFront f);

  const NodeFront* getNodeFrontFor(const Edge* e) const;
  double getScore() const;
  double getScore(const graph::Configuration& c) const;
  double getScoreUnder(const graph::Configuration& c, const EdgeTripGeom* g, const graph::Ordering* order) const;
  double getAreaScore(const Configuration& c, const EdgeTripGeom* g, const graph::Ordering* order) const;
  double getAreaScore(const Configuration& c) const;
  std::vector<Partner> getPartner(const NodeFront* f, const gtfs::Route* r) const;

  std::vector<InnerGeometry> getInnerGeometries(const graph::Configuration& c) const;
  std::vector<InnerGeometry> getInnerGeometries(bool bezier) const;
  std::vector<InnerGeometry> getInnerGeometriesUnder(const graph::Configuration& c, const EdgeTripGeom* g,
    const std::vector<size_t>* order) const;

  util::geo::Polygon getConvexFrontHull(double d) const;

 protected:
  // add edge to this node's adjacency lists
  void addEdge(Edge* e);

  // remove edge from this node's adjacency lists
  void removeEdge(Edge* e);

 private:
  std::set<Edge*> _adjListIn;
  std::set<Edge*> _adjListOut;
  util::geo::Point _pos;

  std::vector<NodeFront> _mainDirs;

  std::set<gtfs::Stop*> _stops;

  geo::PolyLine getInnerBezier(const NodeFront& nf,
      const TripOccurance& tripOcc, const graph::Partner& partner) const;

  geo::PolyLine getInnerStraightLine(const NodeFront& nf,
      const TripOccurance& tripOcc, const graph::Partner& partner) const;

  friend class TransitGraph;
};
}}

#endif  // TRANSITMAP_GRAPH_NODE_H_
