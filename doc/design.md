DeepSheep
=========

# Problem

DeepSheep learns how to play the card game Sheepshead using
reinforcement learning.

# Requirements

1. Should allow for learning using different rule variations.
    a. Number of players: 3, 4, or 5.
    b. Ways of selection partner: called ace, jack of diamonds
    c. Trump suit.
    d. Card order (the Spitz, for example)
    e. No picker behavior: leaster, doubler, forced pick
    f. Stakes doubling calls
    g. Who knows what else

However, it is not a requirement that every known rule variation be
implementable. Some are a little too Wisconsin to be reduced to
software.

2. Should support a variety of learning strategies. For example,
a simple table-lookup Q-learning approach to deep nets trained using
experience replay. Changes in learning strategy should not require
significant redesign.

3. Should have an emphasis on concurrency throughout the design.
It should be possible to train efficiently on some highly-parallel
hardware architecture. Consider an EC2 gc.2xlarge or c3.4xlarge as
target hardware.

4. An individual player should be able to interact with a game of
Sheepshead without sharing any state with any other player.

# Design

## Subsystems

DeepSheep is broken into three main subsystems:

1. *sheepshead*

    Responsible for the rules and structure of the card game
    Sheepshead.

3. *learning*

    Responsible for algorithms that learn to play Sheepshead
    better.

2. *actors*

    Responsible for actually doing stuff: playing a game,
    presenting decisions and results to the learning subsystem,
    etc.


### Interaction between subsystems

The *sheepshead* subsystem presents an interface to the *actors* subsystem
that allows for the creation, play, and evaluation of Sheepshead games or
parts of games.

The *actors* subsystem uses the interface from the *sheepshead* subsystem
to create and execute games in a way that allows for learning. It
presents possible plays and their outcomes to the *learning* subsystem.
It may use the learning subsystem to select plays during games.

The *learning* subsystem appropriately abstracts the games presented to
it by the *actors* subsystem and applies learning algorithms. At the
request of classes in the actors subsystem it provides a recommended
play for a particular state of a game.

## The *sheepshead* subsystem

The *sheepshead* subsystem consists of 2 modules:

1. *proto*

    Responsible for the low-level representation of a Sheepshead game. The
    interface presented by this module may not make sense in terms of
    the card game's rules and actions. The goal of this module is a
    small, fast representation that can independently recapitulate an
    entire game of Sheepshead without using any other information. This
    helps with concurrent learning tasks, replays, etc.

2. *interface*
    
    Responsible for wrapping *proto* with an interface cognizable as
    Sheepshead rules and actions.

The *proto* module is currently implemented using protocol buffers. 
Protocol buffers make a Sheepshead easy to serialize and pass around.
The main class is sheesphead::model::Hand. The Hand is the basic unit
of Sheepshead gameplay. It contains zero or more sheepshead::model::Tricks
and may also contain a sheepshead::model::PickingRound. Note that this
design store the full history of a hand at any point in the game, but it
discards previous hands. This makes in impossible to learn things like
styles of play for different players, but that is okay for now. Should
that be a future requirement, the Game class can be extended by just
changing a "required" to a "repeated" in the proto definition.

The *proto* module also has a sheepshead::model::RuleVariation class
responsible for storing information about the particular variant of
Sheepshead being played.

Finally, the *proto* module has a class sheepshead::model::Card that
represents a playing card. Most card properties are rule-dependent, but
the Card does has an "unknown" attribute.

The *interface* hides everything about *proto*. It implements classes
that contain private pointers to types from *proto*. Each class defines
a set of cohesive methods that form an interface to a Hand or a Hand
component.

The idea that underlies the prosecution of a Hand is that a Hand is always
in one of three states: Playable, Arbitrable, or Complete. When a Hand is
Playable, it needs a decision from a player to advance. So, it needs a
player to choose whether to pick or pass, to crack or not, which card to
play, etc. When a Hand is Arbitrable, it needs the rules of Sheepshead to
be applied. For example, all players have laid a card for a
trick, so the winner must be determined so the leader of the next trick
can play. When a Hand is Complete, there's nothing left to do. It's a
record of a complete hand of Sheepshead, and you can assign chips to
each player.

The following interfaces are in the namespace sheepshead::interface:

+ *Hand* - The base interface. Responsible for creating a new hand
or parsing it from a serialized form. Gives access to other interfaces
and allows for serialization. Sketch of public interface:
    1. Construct a hand, either anew from a Rules object or from a
       serialized form.
    2. Serialize a hand.
    1. Is the hand Playable, Arbitrable, or Complete?
    2. Get Player interface.
    3. Get Arbiter interface.
    4. Get Rules interface.
    5. Get Chronicle interface.

+ *Player* - One of the two interfaces that can change a Hand. This
provides the basic interface needed to advance a Playable Hand.
Sketch of public interface:
    1. What plays can the player make? Either empty if it's not the
    player's turn or a list of one or more things the player can do.
    Could be pick/pass, loner/partner, which card to play, etc.
        a. What is the current Turn? Could be a Trick, PickingRound, or
        Notification.
        b. Which player is supposed to play next?
    2. Make a play, chaning the hand.

+ *Arbiter* - The second of the two interfaces that can change a Hand.
Provides interface needed to advance an Arbitrable Hand.
    1. Arbitrate.

+ *Chronicle* - Access to the history of the hand. Cannot alter the hand,
but gives information what's happened so far.
    1. What is the current turn?
    2. Get the PickingRound interface.
    3. Get the nth Trick interface.
    4. Get chips for nth player.

+ *Trick*
    1. Is it finished?
    2. Who won?
    3. Who led?
    4. What was the nth played card?
    5. What card was played by PlayerId

+ *PickingRound*
    1. Who picked? Nobody?
    2. Did the picker go alone?
    3. What card did the 

+ *Rules* - Rules is the only interface that can exist independent of a
Hand.
    1. Number of players
    2. Number of tricks
    3. Picker style, Ace, JD, or forced?
    4. Various cracking options.


### Iterators in the sheepshead iterface
    1. PlayerItr. Dereferences to PlayerId&. Is "circular", so iterating
       past the end just brings you back to the first PlayerId.
    2. CardItr. Deferences to Card.
    3. TrickItr. Deferences to Trick.
    4. PickDecisionItr. Dereferences to PickDecision

### Transitions in the picking round

This gets complicated because the rules of sheepshead are kind of complicated
and underspecified. In general, we'll make players "choose" to make compelled
plays but not plays that make no sense. So, a player may have to pick, but a
player won't choose "null" partner or something because partners aren't
allowed.

The picking round starts with each player deciding to pick or pass. Each player
is free to do either when given the chance, except when it's the last player to
decide and the "forced pick" rule is in effect. Then that player can only
pick.

There are two possible outcomes after players decide to pick or pass: one
player picks or nobody picks. If nobody picks, then what happens depends on the
rules. If the rules say doubler, then the hand ends and the next one is worth
double. If the rules say leasters, then the picking round ends, and the hand is
played using leasters rules.

That leaves the case when somebody does pick. If the rules do not permit a
partner, then the next step is for the picker to discard and the picking round
is finished. Similarly, if the rules do permit a partner, partner selection is
by jack of diamonds, and the picker has the jack of diamonds, the picker must
go alone. This isn't a rule endorsed by any guide I've seen, but it avoids some
problems the guides have not considered.

If the rules do permit a partner, then the next step is for the
picker to decide whether to pick a partner or to go alone.

If the picker decides to go alone, then the next step is for the picker to
discard and the picking round is finished. If the picker decides to get a
partner, then what happens depends on the rules for choosing a partner.

If the rules say the player with the jack of diamonds is partner, then the picker
discards and the picking rounds is finished. If the rules say partner is by
called ace, then the next step is for the picker to call a partner card.

The rules for calling a partner card are trickly. The relevant question is
whether the picker ends up calling an ace of a suit for which he has no fail
cards. If that is false, then the next is discard, and the picking round is
finished. If that is true, then the next step is for the picker to designate
one of his cards as "unknown". Then, finally that longest path through the
picking round is over, and the picker can discard.
