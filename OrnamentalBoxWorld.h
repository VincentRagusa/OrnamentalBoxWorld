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
#include "../../Utilities/Random.h"

#include <cstdlib>
#include <thread>
#include <vector>

class OrnamentalBoxWorld : public AbstractWorld {

public:
  static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
  
  static std::shared_ptr<ParameterLink<int>> visualQualiaPL;
  static std::shared_ptr<ParameterLink<int>> touchQualiaPL;

  int
  visualQualia, touchQualia, targetBox;

  bool
  boxFacing;

  std::vector<std::vector<int>>
  grid, boxes;

  OrnamentalBoxWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~OrnamentalBoxWorld() = default;

  void
  print_grid();

  void
  randomize_boxes();

  void
  evaluateDuo(std::shared_ptr<Organism> sender, std::shared_ptr<Organism> receiver, int analyze, int visualize, int debug);
  
  void
  evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug);

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() override;
};

class AgentAvatar {
  public:
    int
    x,y, heading, target_box;

    bool
    sender; //false if receiver

    std::vector<std::vector<int>>& grid, boxes;

    AgentAvatar(std::vector<std::vector<int>>& grid_, std::vector<std::vector<int>>& boxes_, int target_box_, bool sender_) :
      grid(grid_), boxes(boxes_), target_box(target_box_), sender(sender_) {}

    void
    walk(int movement_ID);

    void
    respawn();

    std::vector<int>
    get_vision_vector();

};