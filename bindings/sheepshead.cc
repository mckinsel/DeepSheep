#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "sheepshead/interface/arbiter.h"
#include "sheepshead/interface/deck.h"
#include "sheepshead/interface/hand.h"
#include "sheepshead/interface/history.h"
#include "sheepshead/interface/pickinground.h"
#include "sheepshead/interface/playerid.h"
#include "sheepshead/interface/playmaker.h"
#include "sheepshead/interface/seat.h"
#include "sheepshead/interface/trick.h"

#include <iostream>
#include <vector>

namespace py = pybind11;

PYBIND11_PLUGIN(sheepshead) {
    py::module m("sheepshead", "Bindings to the DeepSheep sheepshead interface.");
    
    /* deck.h */ 
    py::class_<sheepshead::interface::Card> card(m, "Card");
    
    py::enum_<sheepshead::interface::Card::Suit>(card, "Suit")
      .value("DIAMONDS", sheepshead::interface::Card::Suit::DIAMONDS)
      .value("HEARTS", sheepshead::interface::Card::Suit::HEARTS)
      .value("CLUBS", sheepshead::interface::Card::Suit::CLUBS)
      .value("SPADES", sheepshead::interface::Card::Suit::SPADES)
      .value("TRUMP", sheepshead::interface::Card::Suit::TRUMP)
      .value("UNKNOWN", sheepshead::interface::Card::Suit::UNKNOWN)
      .export_values();

    py::enum_<sheepshead::interface::Card::Rank>(card, "Rank")
      .value("ACE", sheepshead::interface::Card::Rank::ACE)
      .value("TEN", sheepshead::interface::Card::Rank::TEN)
      .value("KING", sheepshead::interface::Card::Rank::KING)
      .value("QUEEN", sheepshead::interface::Card::Rank::QUEEN)
      .value("JACK", sheepshead::interface::Card::Rank::JACK)
      .value("NINE", sheepshead::interface::Card::Rank::NINE)
      .value("EIGHT", sheepshead::interface::Card::Rank::EIGHT)
      .value("SEVEN", sheepshead::interface::Card::Rank::SEVEN)
      .value("UNKNOWN", sheepshead::interface::Card::Rank::UNKNOWN)
      .export_values();

    card.def("suit", &sheepshead::interface::Card::suit);
    card.def("rank", &sheepshead::interface::Card::rank);
    card.def("true_suit", &sheepshead::interface::Card::true_suit);
    card.def("true_rank", &sheepshead::interface::Card::true_rank);
    card.def("is_trump", &sheepshead::interface::Card::is_trump);
    card.def("is_unknown", &sheepshead::interface::Card::is_unknown);
    card.def("point_value", &sheepshead::interface::Card::point_value);
    card.def("debug_string", &sheepshead::interface::Card::debug_string);
    card.def("is_null", &sheepshead::interface::Card::is_null);

    py::class_<sheepshead::interface::CardItr>(m, "CardItr")
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_null", &sheepshead::interface::CardItr::is_null);

    /* playerid.h */
    py::class_<sheepshead::interface::PlayerId>(m, "PlayerId")
      .def("is_null", &sheepshead::interface::PlayerId::is_null)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def("debug_string", &sheepshead::interface::PlayerId::debug_string);

    py::class_<sheepshead::interface::PlayerItr>(m, "PlayerItr")
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_null", &sheepshead::interface::PlayerItr::is_null);
    
    /* seat.h */
    py::class_<sheepshead::interface::Seat>(m, "Seat")
      .def("is_null", &sheepshead::interface::Seat::is_null)
      .def("debug_string", &sheepshead::interface::Seat::debug_string)
      .def("held_cards_begin", &sheepshead::interface::Seat::held_cards_begin)
      .def("held_cards_end", &sheepshead::interface::Seat::held_cards_end)
      .def("number_of_held_cards", &sheepshead::interface::Seat::number_of_held_cards);
    
    /* trick.h */
    using MutableTrick = sheepshead::interface::Trick<sheepshead::interface::MutableHandHandle>;
    py::class_<MutableTrick>(m,  "Trick")
      .def("is_null", &MutableTrick::is_null)
      .def("is_started", &MutableTrick::is_started)
      .def("is_finished", &MutableTrick::is_finished)
      .def("leader", &MutableTrick::leader)
      .def("laid_cards_begin", &MutableTrick::laid_cards_begin)
      .def("laid_cards_end", &MutableTrick::laid_cards_end)
      .def("number_of_laid_cards", &MutableTrick::number_of_laid_cards)
      .def("winner", &MutableTrick::winner)
      .def("card_played_by", &MutableTrick::card_played_by)
      .def("point_value", &MutableTrick::point_value)
      .def("debug_string", &MutableTrick::debug_string);

    /* pickinground.h */
    py::enum_<sheepshead::interface::PickDecision>(m, "PickDecision")
      .value("PICK", sheepshead::interface::PickDecision::PICK)
      .value("PASS", sheepshead::interface::PickDecision::PASS)
      .value("UNASKED", sheepshead::interface::PickDecision::UNASKED);

    py::enum_<sheepshead::interface::LonerDecision>(m, "LonerDecision")
      .value("PARTNER", sheepshead::interface::LonerDecision::PARTNER)
      .value("LONER", sheepshead::interface::LonerDecision::LONER)
      .value("NONE", sheepshead::interface::LonerDecision::NONE);

    using MutablePR = sheepshead::interface::PickingRound<sheepshead::interface::MutableHandHandle>;
    py::class_<MutablePR>(m, "PickingRound")
      .def(py::init<>())
      .def("picker", &MutablePR::picker)
      .def("pick_decisions_begin", &MutablePR::pick_decisions_begin)
      .def("pick_decisions_end", &MutablePR::pick_decisions_end)
      .def("leader", &MutablePR::leader)
      .def("loner_decision", &MutablePR::loner_decision)
      .def("partner_card", &MutablePR::partner_card)
      .def("unknown_decision_has_been_made", &MutablePR::unknown_decision_has_been_made)
      .def("blinds", &MutablePR::blinds)
      .def("discarded_cards", &MutablePR::discarded_cards)
      .def("is_null", &MutablePR::is_null)
      .def("is_started", &MutablePR::is_started)
      .def("is_finished", &MutablePR::is_finished)
      .def("pick_decision_by", &MutablePR::pick_decision_by)
      .def("debug_string", &MutablePR::debug_string);

    using MutablePickDecisionItr = sheepshead::interface::PickDecisionItr<sheepshead::interface::MutableHandHandle>;
    py::class_<MutablePickDecisionItr>(m, "PickDecisionItr")
      .def(py::self == py::self)
      .def(py::self != py::self);

    /* history.h */
    py::class_<sheepshead::interface::History>(m, "History")
      .def("tricks_begin", &sheepshead::interface::History::tricks_begin)
      .def("tricks_end", &sheepshead::interface::History::tricks_end)
      .def("number_of_started_tricks", &sheepshead::interface::History::number_of_started_tricks)
      .def("number_of_finished_tricks", &sheepshead::interface::History::number_of_finished_tricks)
      .def("latest_trick", &sheepshead::interface::History::latest_trick)
      .def("partner", &sheepshead::interface::History::partner)
      .def("debug_string", &sheepshead::interface::History::debug_string);
    
    /* playmaker.h */
    py::class_<sheepshead::interface::Play> play(m, "Play");

    py::enum_<sheepshead::interface::Play::PlayType>(play, "PlayType")
      .value("PICK", sheepshead::interface::Play::PlayType::PICK)
      .value("LONER", sheepshead::interface::Play::PlayType::LONER)
      .value("PARTNER", sheepshead::interface::Play::PlayType::PARTNER)
      .value("UNKNOWN", sheepshead::interface::Play::PlayType::UNKNOWN)
      .value("DISCARD", sheepshead::interface::Play::PlayType::DISCARD)
      .value("TRICK_CARD", sheepshead::interface::Play::PlayType::TRICK_CARD)
      .export_values();

    play.def("play_type", &sheepshead::interface::Play::play_type);
    play.def("pick_decision", &sheepshead::interface::Play::pick_decision);
    play.def("loner_decision", &sheepshead::interface::Play::loner_decision);
    play.def("partner_decision", &sheepshead::interface::Play::partner_decision);
    play.def("trick_card_decision", &sheepshead::interface::Play::trick_card_decision);
    //play.def("unknown_decision", &sheepshead::interface::Play::unknown_decision);
    play.def("discard_decision", &sheepshead::interface::Play::discard_decision);
    play.def("debug_string", &sheepshead::interface::Play::debug_string);

    py::class_<sheepshead::interface::Playmaker>(m, "Playmaker")
      .def("make_play", &sheepshead::interface::Playmaker::make_play);

    /* arbiter.h */
    py::class_<sheepshead::interface::Arbiter>(m, "Arbiter")
      .def("arbitrate", &sheepshead::interface::Arbiter::arbitrate);


    /* hand.h */
    py::class_<sheepshead::interface::Hand>(m, "Hand")
      .def(py::init<unsigned long>())
      .def("is_playable", &sheepshead::interface::Hand::is_playable)
      .def("is_arbitrable", &sheepshead::interface::Hand::is_arbitrable)
      .def("is_finished", &sheepshead::interface::Hand::is_finished)
      .def("debug_string", &sheepshead::interface::Hand::debug_string)
      .def("arbiter", &sheepshead::interface::Hand::arbiter)
      .def("available_plays", &sheepshead::interface::Hand::available_plays)
      .def("current_player", &sheepshead::interface::Hand::current_player)
      .def("history", &sheepshead::interface::Hand::history)
      .def("reward", &sheepshead::interface::Hand::reward)
      .def("seat", &sheepshead::interface::Hand::seat)
      .def("playmaker", &sheepshead::interface::Hand::playmaker);
    
    /* serialization functions */ 
    m.def("serialize",
      [](const sheepshead::interface::Hand& h) {
        std::string s;
        h.serialize(&s);
        py::bytes b = py::cast(s);
        return b;
        });

    m.def("deserialize",
      [](const py::str& b) {
        std::string s = b;
        sheepshead::interface::Hand h(s);
        return h;
        });

    return m.ptr();
}
