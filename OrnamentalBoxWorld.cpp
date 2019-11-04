//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "OrnamentalBoxWorld.h"

std::shared_ptr<ParameterLink<std::string>> OrnamentalBoxWorld::groupNamePL = Parameters::register_parameter("WORLD_ORNAMENTALBOX_NAMES-groupNameSpace", (std::string) "root::", "namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> OrnamentalBoxWorld::brainNamePL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX_NAMES-brainNameSpace", (std::string) "root::", "namespace for parameters used to define brain");

std::shared_ptr<ParameterLink<int>> OrnamentalBoxWorld::visualQualiaPL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-visualQualia", (int) 3, "number of visual cues");
std::shared_ptr<ParameterLink<int>> OrnamentalBoxWorld::touchQualiaPL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-touchQualia", (int) 1, "number of tactile cues");
std::shared_ptr<ParameterLink<bool>> OrnamentalBoxWorld::boxFacingPL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-boxFacing", (bool) 0, "do boxes indicate NSEW facing?");

void
OrnamentalBoxWorld::print_grid(){
  for (auto & row:grid){
    for (auto& col:row){
      std::cout << col;
    }
    std::cout << std::endl;
  }
}

OrnamentalBoxWorld::OrnamentalBoxWorld(std::shared_ptr<ParametersTable> PT_): AbstractWorld(PT_) {
  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("score_sender");
  popFileColumns.push_back("score_receiver");

  // build grid
  grid = std::vector<std::vector<int>>(15, std::vector<int>(15, 0));
  // 15x15 grid with boxes placed with 3 spaces between them
  for (int r = 1; r <= 3; r++){
    for (int c = 1; c <= 3; c++){
      grid[(4*r)-1][(4*c)-1] = (r-1)*3+c; //TODO replace 1 with box value (binary features)
    }
  }

  print_grid();
}

// void
// OrnamentalBoxWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug) {
//   auto brain = org->brains[brainNamePL->get(PT)];
//   for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
//     brain->resetBrain();
//     brain->setInput(0, 1);
//     brain->update();
//     double score = 0.0;
//     for (int i = 0; i < brain->nrOutputValues; i++) {
//       if (modePL->get(PT) == 0)
//         score += Bit(brain->readOutput(i));
//       else
//         score += brain->readOutput(i);
//     }
//     if (score < 0.0)
//       score = 0.0;
//     org->dataMap.append("score", score);
//     if (visualize)
//       std::cout << "organism with ID " << org->ID << " scored " << score << std::endl;
//   }
// }

void
OrnamentalBoxWorld::evaluateDuo(std::shared_ptr<Organism> sender, std::shared_ptr<Organism> receiver, int analyze, int visualize, int debug){

  sender->dataMap.append("score_sender", 1); //TODO hardcoded score value
  receiver->dataMap.append("score_receiver", 1); //TODO hardcoded score value
}

void
OrnamentalBoxWorld::evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug) {
  int popSize = groups[groupNamePL->get(PT)]->population.size();
  for (int i = 0; i < popSize; i++) {
    evaluateDuo(groups[groupNamePL->get(PT)]->population[i], groups[groupNamePL->get(PT)]->population[(i+1)%popSize], analyze, visualize, debug);
  }
}

std::unordered_map<std::string, std::unordered_set<std::string>>
OrnamentalBoxWorld::requiredGroups() {
  return {{groupNamePL->get(PT), {"B:" + brainNamePL->get(PT) + ",1," + "1"}}};
}



