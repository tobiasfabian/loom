// Copyright 2016
// University of Freiburg - Chair of Algorithms and Datastructures
// Author: Patrick Brosi

#define _ELPP_NO_DEFAULT_LOG_FILE
#define _ELPP_DISABLE_LOG_FILE_FROM_ARG

#include <unistd.h>
#include <iostream>
#include <string>
#include <set>
#include <stdio.h>
#include "easylogging/easylogging.h"
#include "gtfsparser/parser.h"
#include "graph/transitgraph.h"
#include "graph/node.h"
#include "util/XmlWriter.cpp"
#include "output/svgoutput.h"
#include "geo/PolyLine.h"

INITIALIZE_EASYLOGGINGPP;

using namespace transitmapper;
using std::string;

// _____________________________________________________________________________
int main(int argc, char** argv) {
  // disable output buffering for standard output
  setbuf(stdout, NULL);

  // initialize randomness
  srand(time(NULL) + rand());

  el::Loggers::reconfigureAllLoggers(
    el::ConfigurationType::Format, "[%datetime] %level: %msg");

  el::Loggers::reconfigureAllLoggers(
    el::ConfigurationType::ToFile, "false");

  // initialize easylogging lib
  START_EASYLOGGINGPP(argc, argv);

  // parse an example feed
  gtfsparser::Parser parser;

  gtfsparser::gtfs::Feed feed;

  if (argc > 1) {
    try {
      LOG(INFO) << "reading feed at " << argv[1];
      parser.parse(&feed, argv[1]);

      graph::TransitGraph g("shinygraph");

      g.addEdge(g.addNode(new graph::Node(50, 30)), g.addNode(new graph::Node(100, 80)));

      graph::Node* a = g.addNode(new graph::Node(244, 600));
      g.addEdge(a, a);

      std::ofstream o;
      o.open("/home/patrick/test.svg");
      output::SvgOutput svgOut(&o);
      svgOut.print(g);
    } catch (gtfsparser::ParserException &e) {
      LOG(ERROR) << "Feed could not be parsed. " << e.what();
    }
  } else {

    // just testing parallel drawing...
    transitmapper::geo::PolyLine p;
    transitmapper::geo::PolyLine a;
    transitmapper::geo::PolyLine b;

    for (double i = 0; i < 2000; i += 20) {
      // sin
      a << util::geo::Point(i, 1000 + sin(0.006 * i) * 500);

      // cos
      b << util::geo::Point(i, 1000 + cos(0.003 * i) * 500);

      // linear
      // a << util::geo::Point(i, 1000);

      // triangle
      if (i < 1000) {
        p << util::geo::Point(i, 1000 - (i/2));
      } else {
        p << util::geo::Point(i, (i/2));
      }
    }

    std::ofstream o;
    o.open("/home/patrick/test.svg");
    output::SvgOutput svgOut(&o);

    o << "<svg width=\"2000\" height=\"2000\">";

    //svgOut.printLine(p, "fill:none;stroke:black;stroke-width:10");

    std::vector<const transitmapper::geo::PolyLine*> ls;
    ls.push_back(&a);
    ls.push_back(&p);
    ls.push_back(&b);
    transitmapper::geo::PolyLine avg = transitmapper::geo::PolyLine::average(ls);

    svgOut.printLine(avg, "fill:none;stroke:#8844AA;stroke-width:15");

    svgOut.printLine(a, "fill:none;stroke:black;stroke-width:5");
    svgOut.printLine(p, "fill:none;stroke:red;stroke-width:5");
    svgOut.printLine(b, "fill:none;stroke:red;stroke-width:5");

    for (size_t i = 1; i < 5; i++) {
      transitmapper::geo::PolyLine pl = avg;

      pl.offsetPerp(18 * i);
      if (i % 2) svgOut.printLine(pl, "fill:none;stroke:green;stroke-width:15");
      else svgOut.printLine(pl, "fill:none;stroke:red;stroke-width:15");
    }

    for (int i = 1; i < 5; i++) {
      transitmapper::geo::PolyLine pl = avg;

      pl.offsetPerp(-18 * i);
      if (i % 2) svgOut.printLine(pl, "fill:none;stroke:blue;stroke-width:15");
      else svgOut.printLine(pl, "fill:none;stroke:orange;stroke-width:15");
    }
    o << "</svg>";

  }


  return(0);
}
