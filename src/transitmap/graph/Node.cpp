// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosip@informatik.uni-freiburg.de>

#include <cassert>
#include "./Node.h"
#include "./Edge.h"
#include "./TransitGraph.h"
#include "gtfsparser/gtfs/Stop.h"
#include "../graph/EdgeTripGeom.h"
#include "../graph/OrderingConfiguration.h"

using namespace transitmapper;
using namespace graph;
using namespace gtfsparser;

// _____________________________________________________________________________
util::geo::Point NodeFront::getTripOccPos(const gtfs::Route* r) const {
  for (auto e : edges) {
    for (auto& etg : *e->getEdgeTripGeoms()) {
      TripOccWithPos to = etg.getTripsForRoute(r);
      if (to.first) {
        double p = 0;
        //if (etg.getGeomDir() != n) {
        //  p = (etg.getWidth() + etg.getSpacing()) * to.second + etg.getWidth()/2;
        //} else {
          p = (etg.getWidth() + etg.getSpacing()) * (etg.getTripsUnordered().size() - 1 - to.second) + etg.getWidth()/2;
        //}

        double pp = p / geom.getLength();

        return geom.getPointAt(pp).p;
      }
    }
  }
};

// _____________________________________________________________________________
util::geo::Point NodeFront::getTripOccPos(const gtfs::Route* r, const Configuration& c) const {
  for (auto e : edges) {
    for (auto& etg : *e->getEdgeTripGeoms()) {
      TripOccWithPos to = etg.getTripsForRouteUnder(r, c.find(&etg)->second);
      if (to.first) {
        double p = 0;
        //if (etg.getGeomDir() != n) {
        //  p = (etg.getWidth() + etg.getSpacing()) * to.second + etg.getWidth()/2;
        //} else {
          p = (etg.getWidth() + etg.getSpacing()) * (etg.getTripsUnordered().size() - 1 - to.second) + etg.getWidth()/2;
        //}

        double pp = p / geom.getLength();

        return geom.getPointAt(pp).p;
      }
    }
  }
};

// _____________________________________________________________________________
util::geo::Point NodeFront::getTripOccPosUnder(const gtfs::Route* r,
    const graph::Configuration& c, const EdgeTripGeom& g, const Ordering& order) const {
  for (auto e : edges) {
    for (auto& etg : *e->getEdgeTripGeoms()) {
      TripOccWithPos to;

      if (&etg == &g) {
        to = etg.getTripsForRouteUnder(r, order);
      } else {
        to = etg.getTripsForRouteUnder(r, c.find(&etg)->second);
      }

      if (to.first) {
        double p = 0;
        //if (etg.getGeomDir() != n) {
        //  p = (etg.getWidth() + etg.getSpacing()) * to.second + etg.getWidth()/2;
        //} else {
          p = (etg.getWidth() + etg.getSpacing()) * (etg.getTripsUnordered().size() - 1 - to.second) + etg.getWidth()/2;
        //}

        double pp = p / geom.getLength();

        return geom.getPointAt(pp).p;
      }
    }
  }
};

// _____________________________________________________________________________
Node::Node(util::geo::Point pos) : _pos(pos) {
}

// _____________________________________________________________________________
Node::Node(double x, double y) : _pos(x, y) {
}

// _____________________________________________________________________________
Node::Node(util::geo::Point pos, gtfs::Stop* s) : _pos(pos) {
  if (s) _stops.insert(s);
}

// _____________________________________________________________________________
Node::Node(double x, double y, gtfs::Stop* s) : _pos(x, y) {
  if (s) _stops.insert(s);
}

// _____________________________________________________________________________
Node::~Node() {
  for (Edge* e : _adjListOut) {
    e->getFrom()->removeEdge(e);
    e->getTo()->removeEdge(e);
    _adjListIn.erase(e); // catch edge to itself case
    delete e;
  }

  for (Edge* e : _adjListIn) {
    e->getFrom()->removeEdge(e);
    e->getTo()->removeEdge(e);
    delete e;
  }
}

// _____________________________________________________________________________
void Node::addStop(gtfs::Stop* s) {
  _stops.insert(s);
}

// _____________________________________________________________________________
const std::set<gtfs::Stop*>& Node::getStops() const {
  return _stops;
}

// _____________________________________________________________________________
void Node::addEdge(Edge* e) {
  if (e->getFrom() == this) _adjListOut.insert(e);
  if (e->getTo() == this) _adjListIn.insert(e);
}

// _____________________________________________________________________________
void Node::removeEdge(Edge* e) {
  if (e->getFrom() == this) _adjListOut.erase(e);
  if (e->getTo() == this) _adjListIn.erase(e);
}

// _____________________________________________________________________________
const util::geo::Point& Node::getPos() const {
  return _pos;
}

// _____________________________________________________________________________
void Node::setPos(const util::geo::Point& p) {
  _pos = p;
}

// _____________________________________________________________________________
void Node::addMainDir(NodeFront f) {
  _mainDirs.push_back(f);
}

// _____________________________________________________________________________
const NodeFront* Node::getNodeFrontFor(const Edge* e) const {
  for (auto& nf : getMainDirs()) {
    if (std::find(nf.edges.begin(), nf.edges.end(), e) != nf.edges.end()) {
      return &nf;
    }
  }

  return 0;
}

// _____________________________________________________________________________
double Node::getScore() const {
  std::vector<InnerGeometry> igs = getInnerGeometries();

  double score = 0;

  for (size_t i = 0; i < igs.size(); i++) {
    for (size_t j = 0; j < igs.size(); j++) {
      if (j == i) continue;  // don't check against itself

      if (igs[j].geom.distTo(igs[i].geom) < 1) {
        score += .5;
      }
    }
  }

  return score / sqrt(_adjListIn.size() + _adjListOut.size());
}

// _____________________________________________________________________________
double Node::getScore(const Configuration& c) const {
  std::vector<InnerGeometry> igs = getInnerGeometries(c);

  double score = 0;

  for (size_t i = 0; i < igs.size(); i++) {
    for (size_t j = 0; j < igs.size(); j++) {
      if (j == i) continue;  // don't check against itself

      if (igs[j].geom.distTo(igs[i].geom) < 1) {
        score += .5;
      }
    }
  }

  return score / sqrt(_adjListIn.size() + _adjListOut.size());
}

// _____________________________________________________________________________
double Node::getScoreUnder(const graph::Configuration& c, const EdgeTripGeom& g,
    const std::vector<size_t>& order) const {


  std::vector<InnerGeometry> igs = getInnerGeometriesUnder(c, g, order);

  double score = 0;

  for (size_t i = 0; i < igs.size(); i++) {
    for (size_t j = 0; j < igs.size(); j++) {
      if (j == i) continue;  // don't check against itself

      if (igs[j].geom.distTo(igs[i].geom) < 1) {
        score += .5;
      }
    }
  }

  return score / sqrt(_adjListIn.size() + _adjListOut.size());
}

// _____________________________________________________________________________
double Node::getAreaScore(const Configuration& c, const EdgeTripGeom& g,
  const Ordering& order)
const {
  double ret = getScoreUnder(c, g, order);

  for (auto e : _adjListIn) {
    ret += e->getFrom()->getScoreUnder(c, g, order);
  }

  for (auto e : _adjListOut) {
    ret += e->getTo()->getScoreUnder(c, g, order);
  }

  return ret;
}

// _____________________________________________________________________________
double Node::getAreaScore(const Configuration& c) const {
  double ret = getScore(c);

  for (auto e : _adjListIn) {
    ret += e->getFrom()->getScore(c);
  }

  for (auto e : _adjListOut) {
    ret += e->getTo()->getScore(c);
  }

  return ret;
}

// _____________________________________________________________________________
std::vector<Partner> Node::getPartner(const NodeFront* f, const gtfs::Route* r)
const {
  std::vector<Partner> ret;
  for (const auto& nf : getMainDirs()) {
    if (&nf == f) continue;

    for (const auto e : nf.edges) {
      for (const auto& etg : *e->getEdgeTripGeoms()) {
        // TODO: unordered access is fine here!!!
        for (size_t i: etg.getTripOrdering()) {
          const TripOccurance& to = etg.getTripsUnordered()[i];
          if (to.route == r) {
            Partner p;
            p.front = &nf;
            p.edge = e;
            p.etg = &etg;
            p.route = to.route;
            ret.push_back(p);
          }
        }
      }
    }
  }
  return ret;
}

// _____________________________________________________________________________
std::vector<InnerGeometry> Node::getInnerGeometries() const {
  std::vector<InnerGeometry> ret;

  std::set<const gtfs::Route*> processed;
  for (size_t i = 0; i < getMainDirs().size(); i++) {
    const graph::NodeFront& nf = getMainDirs()[i];
    for (auto e : nf.edges) {
      for (auto etgIt = e->getEdgeTripGeoms()->begin();
            etgIt != e->getEdgeTripGeoms()->end(); etgIt++) {

        for (size_t i : etgIt->getTripOrdering()) {
          const TripOccurance& tripOcc = etgIt->getTripsUnordered()[i];
          if (!processed.insert(tripOcc.route).second) continue;
          util::geo::Point p = nf.getTripOccPos(tripOcc.route);
          std::vector<graph::Partner> partners = getPartner(&nf, tripOcc.route);

          if (partners.size() == 0) continue;

          util::geo::Point pp = partners[0].front->getTripOccPos(partners[0].route);

          geo::PolyLine line(p, pp);
          ret.push_back(InnerGeometry(line, partners[0].route, &*etgIt));
        }
      }
    }
  }

  return ret;
}

// _____________________________________________________________________________
std::vector<InnerGeometry> Node::getInnerGeometries(const graph::Configuration& c)
const {
  std::vector<InnerGeometry> ret;

  std::set<const gtfs::Route*> processed;
  for (size_t i = 0; i < getMainDirs().size(); i++) {
    const graph::NodeFront& nf = getMainDirs()[i];
    for (auto e : nf.edges) {
      for (auto etgIt = e->getEdgeTripGeoms()->begin();
            etgIt != e->getEdgeTripGeoms()->end(); etgIt++) {

        const Ordering& ordering = c.find(&*etgIt)->second;

        for (size_t i : ordering) {
          const TripOccurance& tripOcc = etgIt->getTripsUnordered()[i];
          if (!processed.insert(tripOcc.route).second) continue;
          util::geo::Point p = nf.getTripOccPos(tripOcc.route, c);
          std::vector<graph::Partner> partners = getPartner(&nf, tripOcc.route);

          if (partners.size() == 0) continue;

          util::geo::Point pp = partners[0].front->getTripOccPos(partners[0].route, c);

          geo::PolyLine line(p, pp);
          ret.push_back(InnerGeometry(line, partners[0].route, &*etgIt));
        }
      }
    }
  }

  return ret;
}

// _____________________________________________________________________________
std::vector<InnerGeometry> Node::getInnerGeometriesUnder(const graph::Configuration& c,
    const EdgeTripGeom& g,
    const graph::Ordering& order) const {

  // only for testing
  graph::Configuration a = c;
  a[const_cast<EdgeTripGeom*>(&g)] = order;
  return getInnerGeometries(a);

  std::vector<InnerGeometry> ret;

  std::set<const gtfs::Route*> processed;
  for (size_t i = 0; i < getMainDirs().size(); i++) {
    const graph::NodeFront& nf = getMainDirs()[i];
    for (auto e : nf.edges) {
      for (auto etgIt = e->getEdgeTripGeoms()->begin();
            etgIt != e->getEdgeTripGeoms()->end(); etgIt++) {

        const std::vector<size_t>* ordering = 0;

        if (&*etgIt == &g) {
          ordering = &order;
        } else {
          ordering = &c.find(&*etgIt)->second;
        }

        for (size_t i : *ordering) {
          const TripOccurance& tripOcc = etgIt->getTripsUnordered()[i];
          if (!processed.insert(tripOcc.route).second) continue;
          util::geo::Point p = nf.getTripOccPosUnder(tripOcc.route, c, g, *ordering);
          std::vector<graph::Partner> partners = getPartner(&nf, tripOcc.route);

          if (partners.size() == 0) continue;

          util::geo::Point pp = partners[0].front->getTripOccPosUnder(partners[0].route, c, g, *ordering);

          geo::PolyLine line(p, pp);
          ret.push_back(InnerGeometry(line, partners[0].route, &*etgIt));
        }
      }
    }
  }

  return ret;
}
