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
std::shared_ptr<ParameterLink<int>> OrnamentalBoxWorld::messageSizePL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-messageSize", (int) 4, "number of bits an agent can send to their partner");
std::shared_ptr<ParameterLink<int>> OrnamentalBoxWorld::lifetimePL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-lifetime", (int) 512, "agent evaluation timeout");
std::shared_ptr<ParameterLink<int>> OrnamentalBoxWorld::evaluationsPerGenerationPL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-evaluationsPerGeneration", (int) 16, "agent re-evaluation count");
std::shared_ptr<ParameterLink<int>> OrnamentalBoxWorld::brainUpdatesPL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-brainUpdates", (int) 1, "number of brain updates allowed between input and output");
std::shared_ptr<ParameterLink<bool>> OrnamentalBoxWorld::messageAlwaysGivenPL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-messageAlwaysGiven", (bool) 0, "does the receiver always know the message");
std::shared_ptr<ParameterLink<bool>> OrnamentalBoxWorld::makeUniqueBoxesPL = Parameters::register_parameter( "WORLD_ORNAMENTALBOX-makeUniqueBoxes", (bool) 0, "are all of the boxes unique");


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
OrnamentalBoxWorld::print_grid(AgentAvatar agent){
  for (int r = 0; r < grid.size(); r++){
    for (int c = 0; c < grid.size(); c++){
      if (agent.r == r and agent.c == c){
        switch (agent.heading){
          case 0:
            std::cout << "^";
            break;
          case 1:
            std::cout << ">";
            break;
          case 2:
            std::cout << "v";
            break;
          case 3:
            std::cout << "<";
            break;
        }
      }
      else {
        std::cout << grid[r][c];
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void
OrnamentalBoxWorld::randomize_boxes(bool makeUnique){
  if (makeUnique){
    if (visualQualia + touchQualia < 4){
      std::cout << "(OrnamentalBoxWorld) WARNING: Cannot make boxes unique! need more qualia." << std::endl;
      std::cout << "Spawning using alternate method..." << std::endl;
      randomize_boxes(false);
      return;
    }

    int box = 0;
    std::vector<int> temp_set = std::vector<int>(visualQualia+touchQualia, 0);
    bool check, boxcheck;
    while(box < boxes.size()){
      for (int q = 0; q < visualQualia + touchQualia; q++){
        temp_set[q] = Random::getInt(1);
      } 
      check = true;
      for (int cbox = 0; cbox < box; cbox++){
        boxcheck = false;
        for (int bit = 0; bit < visualQualia + touchQualia; bit++){
          boxcheck = boxcheck || (boxes[cbox][bit] != temp_set[bit]);
        }
        check = check && boxcheck;
      }
      if (check){
        for (int q = 0; q < visualQualia + touchQualia; q++){
          boxes[box][q] = temp_set[q];
        }
        box++;
      }
    }
    targetBox = Random::getInt(8) + 1;
  }
  else{
    for (auto& box:boxes){
      for (auto& qualia:box){
        qualia = Random::getInt(1);
      }
    }
    targetBox = Random::getInt(8) + 1;
  }
}



OrnamentalBoxWorld::OrnamentalBoxWorld(std::shared_ptr<ParametersTable> PT_): AbstractWorld(PT_) {
  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("receiver_attempts");
  popFileColumns.push_back("receiver_found_item");
  popFileColumns.push_back("receiver_walking");
  // popFileColumns.push_back("score_sender");
  popFileColumns.push_back("score_receiver");

  //grab parameters from PLs
  visualQualia = visualQualiaPL->get(PT);
  touchQualia = touchQualiaPL->get(PT);
  messageSize = messageSizePL->get(PT);
  lifetime = lifetimePL->get(PT);
  evaluationsPerGeneration = evaluationsPerGenerationPL->get(PT);
  brainUpdates = brainUpdatesPL->get(PT);
  messageAlwaysGiven = messageAlwaysGivenPL->get(PT);
  makeUniqueBoxes = makeUniqueBoxesPL->get(PT);

  input_size = 1 + (visualQualia + 1)*14 + touchQualia + 1 + messageSize; //context bit, all vision, all touch, secret item sensor, message buffer
  output_size = 2 + messageSize + 1; //movement, message buffer, send/select

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
      heading--;
      while(heading < 0) heading += 4;
      break;
    case 2:
      heading++;
      while(heading > 3) heading -= 4;
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
AgentAvatar::vision_helper(std::vector<int>& senses, int& i, int ro, int co){
  if (ro == 0 && co == 0) return;
    if (r+ro >= 0 && r+ro < grid.size() && c+co >= 0 && c+co < grid.size()){
      if (grid[r+ro][c+co] == 0){
        //in bounds but empty
        for (int j = 0; j < v+1; j++) senses[i++] = 0;
      }
      else{
        //sees a box
        senses[i++] = 1; //indicates a box (otherwise all 0s looks empty)
        for (int j = 0; j < v; j++) senses[i++] = boxes[ grid[r+ro][c+co]-1 ][j];
      }
    }
    else{
      // all out of bounds vision reports "empty"
      for (int j = 0; j < v+1; j++) senses[i++] = 0; 
    }
}

void
AgentAvatar::touch_helper(std::vector<int>& senses, int& i, int ro, int co){
  if (r+ro >= 0 && r+ro < grid.size() && c+co >= 0 && c+co < grid.size()){
    if (grid[r+ro][c+co] == 0){
      //in bounds but empty
      for (int j = 0; j < t+1; j++) senses[i++] = 0;
    }
    else{
      //sees a box
      senses[i++] = (int) (grid[r+ro][c+co] == target_box) && sender; //indicates box has secret item (only sender can see it)
      for (int j = 0; j < t; j++) senses[i++] = boxes[ grid[r+ro][c+co]-1 ][v+j]; // offset by v to skip to touch qualia
    }
  }
  else{
    // all out of bounds vision reports "empty"
    for (int j = 0; j < t+1; j++) senses[i++] = 0; 
  } 
}

std::vector<int>
AgentAvatar::get_sensor_vector(){
  std::vector<int> senses = std::vector<int> ((v+1)*14 + (t+1)*1);
  auto i = 0;
  switch(heading){
    case 0:
      //N
      //vision
      for (int ro = -2; ro <= 0; ro++){
        for (int co = -2; co <= 2; co++){
          vision_helper(senses,i,ro,co);
        }
      }
      //touch
      touch_helper(senses,i,-1,0);
      break;
    case 1:
      //E
      //vision
      for (int co = 2; co >= 0; co--){
        for (int ro = -2; ro <= 2; ro++){
          vision_helper(senses,i,ro,co);
        }
      }
      //touch
      touch_helper(senses,i,0,1);
      break;
    case 2:
      //S
      //vision
      for (int ro = 2; ro >= 0 ; ro--){
        for (int co = 2; co >= -2; co--){
          vision_helper(senses,i,ro,co);
        }
      }
      //touch
      touch_helper(senses,i,1,0);
      break;
    case 3:
      //W
      //vision
      for (int co = -2; co <= 0; co++){
        for (int ro = 2; ro >= -2; ro--){
          vision_helper(senses,i,ro,co);
        }
      }
      //touch
      touch_helper(senses,i,0,-1);
      break;
  }
  return senses;
}

int
AgentAvatar::get_touch_box(){
  switch(heading){
    case 0:
      //N
      return r-1 >= 0 ? grid[r-1][c] : 0;
    case 1:
      //E
      return c+1 < grid.size() ? grid[r][c+1] : 0;
      break;
    case 2:
      //S
      return r+1 < grid.size() ? grid[r+1][c] : 0;
    case 3:
      //W
      return c-1 >= 0 ? grid[r][c-1] : 0;
  }
}

void
OrnamentalBoxWorld::evaluateDuo(std::shared_ptr<Organism> sender, std::shared_ptr<Organism> receiver, int analyze, int visualize, int debug){
  auto sender_brain = sender->brains[brainNamePL->get(PT)];
  auto receiver_brain = receiver->brains[brainNamePL->get(PT)];

  for (int eval = 0; eval < evaluationsPerGeneration; eval++){

    sender_brain->resetBrain();
    receiver_brain->resetBrain();

    randomize_boxes(makeUniqueBoxes);
    std::vector<int> temp_senses, message;
    message = std::vector<int> (messageSize, 0);
    int in, out;

    // //sender phase...
    // AgentAvatar sender_avatar = AgentAvatar(grid,boxes,targetBox,true,visualQualia,touchQualia);
    // sender_avatar.respawn();
    // int sender_walk_count = 0;
    // double sender_score = 0;

    // for (int t = 0; t< lifetime; t++){
    //   if (debug || visualize) print_grid(sender_avatar);
    //   // set input
    //   temp_senses = sender_avatar.get_sensor_vector();
    //   in = 0;
    //   sender_brain->setInput(in++, 0); //context bit is 0 for sender
    //   for (auto& s:temp_senses) sender_brain->setInput(in++, s);
    //   for (int m = 0; m < messageSize; m++) sender_brain->setInput(in++, 0);
    //   // brain update
    //   for (int bu = 0; bu < brainUpdates; bu++) sender_brain->update();
    //   // update world
    //   out = 0;
    //   auto move_ID = Bit(sender_brain->readOutput(out++))*2 + Bit(sender_brain->readOutput(out++));
    //   sender_avatar.walk(move_ID);
    //   // if (move_ID == 3) sender_score += (1/std::pow(2,++sender_walk_count));
    //   if (Bit(sender_brain->readOutput(out++))){
    //     for (int m = 0; m < messageSize; m++) message[m] = sender_brain->readOutput(out++);
    //     break;
    //   }
    // }

    //HACKY HACK DEBUG HACK
    for (int m = 0; m < messageSize; m++) message[m] = boxes[targetBox-1][m];

    // //receiver phase...
    AgentAvatar receiver_avatar = AgentAvatar(grid,boxes,targetBox,false,visualQualia,touchQualia); 
    receiver_avatar.respawn();
    int receiver_walk_count = 0;
    double receiver_score = 0;
    bool first_eval = true;
    int receiver_attempts = 0;
    int receiver_found_item = 0;
    double receiver_walking = 0.0;

    for (int t = 0; t< lifetime; t++){
      // set input
      // if (debug || visualize) print_grid(receiver_avatar);
      temp_senses = receiver_avatar.get_sensor_vector();
      in = 0;
      receiver_brain->setInput(in++, 1); //context bit is 1 for for receiver
      for (auto& s:temp_senses) receiver_brain->setInput(in++, s);
      if (messageAlwaysGiven || first_eval){
        for (int m = 0; m < messageSize; m++) receiver_brain->setInput(in++, message[m]);
        first_eval = false;
      }
      else{
        for (int m = 0; m < messageSize; m++) receiver_brain->setInput(in++, 0);
      }
      // brain update
      for (int bu = 0; bu < brainUpdates; bu++) receiver_brain->update();
      // update world
      out = 0;
      auto move_ID = Bit(receiver_brain->readOutput(out++))*2 + Bit(receiver_brain->readOutput(out++));
      receiver_avatar.walk(move_ID);
      if (move_ID == 3) receiver_walking += (1/std::pow(2,++receiver_walk_count));
      if (Bit(receiver_brain->readOutput(out++))){
        if (receiver_avatar.get_touch_box() == targetBox){
          receiver_attempts++;
          auto score_delta = 10-receiver_attempts > 0 ? 10-receiver_attempts : 0;
          receiver_found_item = 1;
          // sender_score += score_delta;
          receiver_score += score_delta;
          break;
        }
        else if (receiver_avatar.get_touch_box() != 0){
          receiver_attempts++;
          if (receiver_attempts > 9){
            break;
          }
        }
      }
    }
    //max score for an organism should be (9 + 1 + 1) + (9 + 1) = 21
    // sender->dataMap.append("score_sender", sender_score);
    receiver->dataMap.append("score_receiver", receiver_score);
    receiver->dataMap.append("receiver_found_item", receiver_found_item);
    receiver->dataMap.append("receiver_attempts", receiver_attempts);
    receiver->dataMap.append("receiver_walking", receiver_walking);
  }
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
  auto bla = "B:" + brainNamePL->get(PT) + "," + std::to_string(input_size) + "," + std::to_string(output_size);
  std::cout << bla << std::endl;
  return {{groupNamePL->get(PT), {bla}}};
}



