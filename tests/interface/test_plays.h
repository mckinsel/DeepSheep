#include "sheepshead/interface/hand.h"

// Store some plays for testing

namespace testplays {

auto pick = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::PICK,
    sheepshead::interface::PickDecision::PICK);

auto pass = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::PICK,
    sheepshead::interface::PickDecision::PASS);

auto go_alone = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::LONER,
    sheepshead::interface::LonerDecision::LONER);

auto get_partner = sheepshead::interface::Play(
    sheepshead::interface::Play::PlayType::LONER,
    sheepshead::interface::LonerDecision::PARTNER);

} // namespace testplays
