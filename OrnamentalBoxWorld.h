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
  static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
  
  static std::shared_ptr<ParameterLink<int>> visualQualiaPL;
  static std::shared_ptr<ParameterLink<int>> touchQualiaPL;
  static std::shared_ptr<ParameterLink<bool>> boxFacingPL;

  OrnamentalBoxWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~OrnamentalBoxWorld() = default;
  
  std::vector<std::vector<int>> grid;

  void
  print_grid();

  void
  evaluateDuo(std::shared_ptr<Organism> sender, std::shared_ptr<Organism> receiver, int analyze, int visualize, int debug);
  
  void
  evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug);

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() override;
};

