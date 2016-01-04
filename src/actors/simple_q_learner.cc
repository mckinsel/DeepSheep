#include <random>
#include "sheepshead/interface/hand.h"
#include "learning/q_function.h"

bool try_to_play(sheepshead::interface::Hand* hand, std::default_random_engine& generator)
{
  auto current_player_id = hand->current_player();
  auto available_plays = hand->available_plays(current_player_id);

  std::uniform_int_distribution<int>
    distribution(0, available_plays.size() - 1);

  unsigned int chosen_play = distribution(generator);
  hand->playmaker(current_player_id).make_play(available_plays[chosen_play]);
  return true;
}

void play_to_end(sheepshead::interface::Hand* hand, std::default_random_engine& generator)
{
  while(!hand->is_finished()) {
    if(hand->is_arbitrable()) {
      hand->arbiter().arbitrate();
    }
    if(hand->is_playable()) {
      try_to_play(hand, generator);
    }
  }
}


int main(int argc, char* argv[])
{

  unsigned long seed = 0;
  if(argc != 2) {
    std::cerr << "Usage: simple_q_learner <game_seed>" << std::endl;
    exit(1);
  } else {
    seed = strtoul(argv[1], NULL, 0);
  }

  auto q_function = learning::QFunction();
  std::default_random_engine generator(seed);
  std::uniform_int_distribution<int> distribution(0, 2);
  std::uniform_real_distribution<float> real_distribution(0, 1);
  distribution(generator);
  
  for(int iter=0; iter<500000; iter++) {
    auto hand = sheepshead::interface::Hand(seed + iter);
    hand.arbiter().arbitrate();
    
    std::string serialized_start;
    hand.serialize(&serialized_start);

  
    // The first player can pick or pass
    auto current_player_id = hand.current_player();
    auto available_plays = hand.available_plays(current_player_id);
    float q_0 = q_function.evaluate(hand, available_plays[0]);
    float q_1 = q_function.evaluate(hand, available_plays[1]);
    float be_greedy = real_distribution(generator);
    if(be_greedy < .10) { // Choose a random action
      if(q_0 <= q_1) {
        hand.playmaker(current_player_id).make_play(available_plays[0]);
      } else {
        hand.playmaker(current_player_id).make_play(available_plays[1]);
      }
    } else {
      if(q_0 <= q_1) {
        hand.playmaker(current_player_id).make_play(available_plays[1]);
      } else {
        hand.playmaker(current_player_id).make_play(available_plays[0]);
      }
    }

    play_to_end(&hand, generator);
    auto start_hand = sheepshead::interface::Hand(serialized_start);
    auto experience = learning::Experience(start_hand, hand);
    q_function.learn(experience);
    if(iter % 10000 == 0) {
      std::cout << q_function.debug_string() << std::endl;
    }
  }

  std::cout << q_function.debug_string() << std::endl;
  google::protobuf::ShutdownProtobufLibrary();
}
