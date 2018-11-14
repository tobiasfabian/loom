// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef TRANSITMAP_OPTIM_OPTGRAPHSCORER_H_
#define TRANSITMAP_OPTIM_OPTGRAPHSCORER_H_

#include <string>
#include <vector>
#include "transitmap/graph/OrderingConfig.h"
#include "transitmap/graph/Penalties.h"
#include "transitmap/optim/OptGraph.h"

using transitmapper::graph::Penalties;
using transitmapper::graph::OrderingConfig;

namespace transitmapper {
namespace optim {

class OptGraphScorer {
 public:
  OptGraphScorer(const Scorer* scorer) : _scorer(scorer) {}

  double getCrossingScore(const std::set<OptNode*>& g,
                  const OptOrderingConfig& c) const;
  double getCrossingScore(OptNode* n, const OptOrderingConfig& c) const;

  double getCrossingScore(OptEdge* e,
                  const OptOrderingConfig& c) const;
  double getSplittingScore(OptEdge* e, const OptOrderingConfig& c) const;

  double getSplittingScore(const std::set<OptNode*>& g,
                  const OptOrderingConfig& c) const;
  double getSplittingScore(OptNode* n, const OptOrderingConfig& c) const;

 private:
  const Scorer* _scorer;

  std::pair<size_t, size_t> getNumCrossings(OptNode* n,
                                            const OptOrderingConfig& c) const;
  size_t getNumSeparations(OptNode* n,
                                            const OptOrderingConfig& c) const;
};
}
}

#endif  // TRANSITMAP_OPTIM_OPTGRAPHSCORER_H_