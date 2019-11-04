//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include "../AbstractWorld.h"

#include <cstdlib>
#include <thread>
#include <vector>

class OrnamentalBoxWorld : public AbstractWorld {

public:
  static std::shared_ptr<ParameterLink<std::string>> 
  groupNamePL;

  static std::shared_ptr<ParameterLink<std::string>> 
  brainNamePL;

  OrnamentalBoxWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~OrnamentalBoxWorld() = default;

  // void
  // evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug);
  
  void
  evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug);

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() override;
};

