#include "sheepshead/interface/hand.h"

#include <chrono>
#include <iostream>
#include <random>

#include <assert.h>

/*
 * Attempt to take a turn by finding player that has some available plays
 * and randomly choosing one of them.
 */
bool try_to_play(sheepshead::interface::Hand* hand, std::default_random_engine& generator)
{
  std::cerr << "\n*******\nTrying to play " << std::endl;
  auto leader_itr = hand->history().picking_round().leader();
  auto player_itr = leader_itr;


  do {
    auto available_plays = hand->playmaker(*player_itr).available_plays();


    if(available_plays.size() > 0) {
      std::cerr<< hand->debug_string() << std::endl;
      std::cerr << player_itr->debug_string();
      std::cerr << " has " << available_plays.size() << " available plays." << std::endl;

      for(auto& available_play : available_plays) {
        std::cerr << available_play.debug_string() << "; ";
      }
      std::cerr << std::endl;

      std::uniform_int_distribution<int>
        distribution(0, available_plays.size() - 1);

      unsigned int chosen_play = distribution(generator);

      std::cerr << "Chosen play is " << available_plays[chosen_play].debug_string() << std::endl;

      hand->playmaker(*player_itr).make_play(available_plays[chosen_play]);
      return true;
    }

    ++player_itr;
  } while(player_itr != leader_itr);

  return false;
}

int main(int argc, char* argv[])
{

  unsigned long seed = 0;
  if(argc > 1) {
    seed = strtoul(argv[1], NULL, 0);
  } else {
    seed = std::chrono::system_clock::now().time_since_epoch().count();
  }
  auto hand = sheepshead::interface::Hand(seed);

  std::default_random_engine generator(seed);
  std::uniform_int_distribution<int> distribution(0, 2);
  distribution(generator);

  while(!hand.is_finished()) {
    if(hand.is_arbitrable()) {
      std::cerr << "Arbitrating" << std::endl;
      hand.arbiter().arbitrate();
      std::cerr << "Done arbitrating." << std::endl;
    }

    if(hand.is_playable()) {
      bool result = try_to_play(&hand, generator);
      if(result) std::cerr  << "Successfully made a play" << std::endl;
    }
  }

  auto leader_itr = hand.history().picking_round().leader();
  auto player_itr = leader_itr;
  int total_reward = 0;
  do {
    std::cerr << player_itr->debug_string() << " reward ";
    std::cerr << hand.reward(*player_itr) << std::endl;
    total_reward += hand.reward(*player_itr);
    ++player_itr;
  } while(player_itr != leader_itr);
  assert(total_reward == 0);

}
