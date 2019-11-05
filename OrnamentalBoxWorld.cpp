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

void
OrnamentalBoxWorld::print_grid(){
  for (auto& row:grid){
    for (auto& col:row){
      std::cout << col;
    }
    std::cout << std::endl;
  }
}

void
OrnamentalBoxWorld::randomize_boxes(){
  for (auto& box:boxes){
    for (auto& qualia:box){
      qualia = Random::getInt(1);
    }
  }
  targetBox = Random::getInt(8) + 1;
}

OrnamentalBoxWorld::OrnamentalBoxWorld(std::shared_ptr<ParametersTable> PT_): AbstractWorld(PT_) {
  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("score_sender");
  popFileColumns.push_back("score_receiver");

  //grab parameters from PLs
  visualQualia = visualQualiaPL->get(PT);
  touchQualia = touchQualiaPL->get(PT);

  // build grid 15x15 grid with boxes placed with 3 spaces between them
  grid = std::vector<std::vector<int>>(15, std::vector<int>(15, 0));
  for (int r = 1; r <= 3; r++){
    for (int c = 1; c <= 3; c++){
      grid[(4 * r) - 1][(4 * c) - 1] = ((r - 1) * 3) + c;
    }
  }
  print_grid();

  //init boxes
  boxes = std::vector<std::vector<int>>(9, std::vector<int>(visualQualia + touchQualia, 0));
  //randomize box properties in evaluateDuo 
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
AgentAvatar::walk(int movement_ID){
  switch(movement_ID){
    case 3:
      // move ahead 1
      switch(heading){
        case 0:
          //N
          r = r-1 < 0 ? 0 : r-1;
          break;
        case 1:
          //E
          c = c+1 == grid.size() ? c : c+1;
          break;
        case 2:
          //S
          r = r+1 == grid.size() ? r : r+1;
          break;
        case 3:
          //W
          c = c-1 < 0 ? 0 : c-1;
          break;
      }
      break;
    case 1:
      //turn left
      heading = (heading - 1) % 4;
      break;
    case 2:
      heading = (heading + 1) % 4;
      //turn right
      break;
    default:
      break;
  }
}

void
AgentAvatar::respawn(){
  heading = Random::getInt(3);
  auto edge_offset = Random::getInt(grid.size() - 1);
  switch (heading) {
    case 0:
      //Facing N, spawn on S wall
      r = grid.size() - 1;
      c = edge_offset;
      break;
    case 1:
      //Facing E, spawn on W wall
      r = edge_offset;
      c = 0;
      break;
    case 2:
      //Facing S, spawn on N wall
      r = 0;
      c = edge_offset;
      break;
    case 3:
      c = grid.size() - 1;
      r = edge_offset;
      //Facing W, spawn on E wall
      break;
  }
}

void
AgentAvatar::vision_helper(std::vector<int>& vision, int& i, int ro, int co){
  if (ro == 0 && co == 0) return;
    if (r+ro > 0 && r+ro < grid.size() && c+co > 0 && c+co < grid.size()){
      if (grid[r+ro][c+co] == 0){
        //in bounds but empty
        for (int j = 0; j < v+1; j++) vision[i++] = 0;
      }
      else{
        //sees a box
        vision[i++] = 1; //indicates a box (otherwise all 0s looks empty)
        for (int j = 0; j < v; j++) vision[i++] = boxes[grid[r+ro][c+co]][j];
      }
    }
    else{
      // all out of bounds vision reports "empty"
      for (int j = 0; j < v+1; j++) vision[i++] = 0; 
    }
}

void
AgentAvatar::touch_helper(std::vector<int>& vision, int& i, int ro, int co){
  if (r+ro > 0 && r+ro < grid.size() && c+co > 0 && c+co < grid.size()){
    if (grid[r+ro][c+co] == 0){
      //in bounds but empty
      for (int j = 0; j < t+1; j++) vision[i++] = 0;
    }
    else{
      //sees a box
      vision[i++] = (int) (grid[r+ro][c+co] == target_box) && sender; //indicates box has secret item (only sender can see it)
      for (int j = 0; j < t; j++) vision[i++] = boxes[grid[r+ro][c+co]][v+j]; // offset by v to skip to touch qualia
    }
  }
  else{
    // all out of bounds vision reports "empty"
    for (int j = 0; j < t+1; j++) vision[i++] = 0; 
  } 
}

std::vector<int>
AgentAvatar::get_sensor_vector(){
  std::vector<int> vision = std::vector<int> ((v+1)*14 + (t+1)*1);
  auto i = 0;
  switch(heading){
    case 0:
      //N
      //vision
      for (int ro = -2; ro <= 0; ro++){
        for (int co = -2; co <= 2; co++){
          vision_helper(vision,i,ro,co);
        }
      }
      //touch
      touch_helper(vision,i,-1,0);
      break;
    case 1:
      //E
      //vision
      for (int co = 2; co >= 0; co--){
        for (int ro = -2; ro <= 2; ro++){
          vision_helper(vision,i,ro,co);
        }
      }
      //touch
      touch_helper(vision,i,0,1);
      break;
    case 2:
      //S
      //vision
      for (int ro = 2; ro >= 0 ; ro--){
        for (int co = 2; co >= -2; co--){
          vision_helper(vision,i,ro,co);
        }
      }
      //touch
      touch_helper(vision,i,1,0);
      break;
    case 3:
      //W
      //vision
      for (int co = -2; co <= 0; co++){
        for (int ro = 2; ro >= -2; ro--){
          vision_helper(vision,i,ro,co);
        }
      }
      //touch
      touch_helper(vision,i,0,-1);
      break;
  }
}

void
OrnamentalBoxWorld::evaluateDuo(std::shared_ptr<Organism> sender, std::shared_ptr<Organism> receiver, int analyze, int visualize, int debug){
  auto sender_brain = sender->brains[brainNamePL->get(PT)];
  auto receiver_brain = receiver->brains[brainNamePL->get(PT)];

  sender_brain->resetBrain();
  receiver_brain->resetBrain();

  //sender phase...


  //receiver phase...



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



