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

class AgentAvatar {
  public:
    int
    r,c, heading, target_box, v, t;

    bool
    sender; //false if receiver

    std::vector<std::vector<int>>& grid, boxes;

    AgentAvatar(std::vector<std::vector<int>>& grid_, std::vector<std::vector<int>>& boxes_, int target_box_, bool sender_, int v_, int t_) :
      grid(grid_), boxes(boxes_), target_box(target_box_), sender(sender_), v(v_), t(t_) {}

    void
    walk(int movement_ID);

    void
    respawn();

    std::vector<int>
    get_sensor_vector();

    void
    vision_helper(std::vector<int>& vision,int& i, int ro, int co);

    void
    touch_helper(std::vector<int>& vision, int& i, int ro, int co);

    int
    get_touch_box();
};

class OrnamentalBoxWorld : public AbstractWorld {

public:
  static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
  
  static std::shared_ptr<ParameterLink<int>> visualQualiaPL;
  static std::shared_ptr<ParameterLink<int>> touchQualiaPL;
  static std::shared_ptr<ParameterLink<int>> messageSizePL;
  static std::shared_ptr<ParameterLink<int>> lifetimePL;
  static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
  static std::shared_ptr<ParameterLink<int>> brainUpdatesPL;
  static std::shared_ptr<ParameterLink<bool>> messageAlwaysGivenPL;
  static std::shared_ptr<ParameterLink<bool>> makeUniqueBoxesPL; 

  int
  visualQualia, touchQualia, targetBox, messageSize, input_size, output_size, lifetime, evaluationsPerGeneration, brainUpdates;

  bool
  boxFacing, messageAlwaysGiven, makeUniqueBoxes;

  std::vector<std::vector<int>>
  grid, boxes;

  OrnamentalBoxWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~OrnamentalBoxWorld() = default;

  void
  print_grid();

  void
  print_grid(AgentAvatar agent);

  void
  randomize_boxes(bool makeUnique);

  void
  evaluateDuo(std::shared_ptr<Organism> sender, std::shared_ptr<Organism> receiver, int analyze, int visualize, int debug);
  
  void
  evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug);

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() override;
};

