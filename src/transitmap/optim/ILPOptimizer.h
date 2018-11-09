// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#ifndef TRANSITMAP_OPTIM_ILPOPTIMIZER_H_
#define TRANSITMAP_OPTIM_ILPOPTIMIZER_H_

#include <glpk.h>
#include "transitmap/config/TransitMapConfig.h"
#include "transitmap/graph/OrderingConfig.h"
#include "transitmap/graph/Route.h"
#include "transitmap/graph/TransitGraph.h"
#include "transitmap/optim/OptGraph.h"
#include "transitmap/optim/Optimizer.h"
#include "transitmap/optim/Scorer.h"

using std::exception;
using std::string;

namespace transitmapper {
namespace optim {

using namespace graph;


struct VariableMatrix {
  std::vector<int> rowNum;
  std::vector<int> colNum;
  std::vector<double> vals;

  void addVar(int row, int col, double val);
  void getGLPKArrs(int** ia, int** ja, double** r) const;
  size_t getNumVars() const { return vals.size(); }
};

class ILPOptimizer : public Optimizer {
 public:
  ILPOptimizer(const config::Config* cfg, const Scorer* scorer)
      : _cfg(cfg), _scorer(scorer){};

  int optimize(TransitGraph* tg) const;
  int optimize(const std::set<OptNode*>& g, HierarchOrderingConfig* c) const;

 protected:
  const config::Config* _cfg;
  const Scorer* _scorer;


  virtual glp_prob* createProblem(const std::set<OptNode*>& g) const;

  void solveProblem(glp_prob* lp) const;
  void preSolveCoinCbc(glp_prob* lp) const;

  virtual void getConfigurationFromSolution(glp_prob* lp, HierarchOrderingConfig* c,
                                            const std::set<OptNode*>& g) const;

  std::string getILPVarName(OptEdge* e, const Route* r, size_t p) const;

  void writeSameSegConstraints(const std::set<OptNode*>& g, VariableMatrix* vm,
                               glp_prob* lp) const;

  void writeDiffSegConstraints(const std::set<OptNode*>& g, VariableMatrix* vm,
                               glp_prob* lp) const;

  bool printHumanReadable(glp_prob* lp, const std::string& path) const;
  double getConstraintCoeff(glp_prob* lp, int constraint, int col) const;

  std::vector<PosComPair> getPositionCombinations(OptEdge* a, OptEdge* b) const;
  std::vector<PosCom> getPositionCombinations(OptEdge* a) const;

  int getCrossingPenaltySameSeg(const OptNode* n) const;
  int getCrossingPenaltyDiffSeg(const OptNode* n) const;
  int getSplittingPenalty(const OptNode* n) const;

};
}  // namespace optim
}  // namespace transitmapper

#endif  // TRANSITMAP_OPTIM_ILPOPTIMIZER_H_
