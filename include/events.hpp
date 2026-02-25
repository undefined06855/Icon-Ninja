#pragma once
#include <Geode/loader/Event.hpp>
#include <Geode/binding/PlayerObject.hpp>

namespace ninja {

/*

Usage (probably):

EarlyNewPlayerEvent is called before stuff is done to the playerobject (this is a blank playerobject)
NewPlayerEvent is called after stuff is done (this is a pre-populated playerobject)

If you return ListenerResult:Stop in EarlyNewPlayerEvent, Icon Ninja will skip populating the PlayerObject and assume
that you have.

EarlyNewPlayerEvent().listen([](PlayerObject* player) {
    if (doTheShit) {
        // do the shit
        // ...
        return ListenerResult::Stop;
    } else {
        // dont do the shit
        return ListenerResult::Propagate;
    }
}).leak();

*/

class NewPlayerEvent : public geode::Event<NewPlayerEvent, bool(PlayerObject*)> {
public:
    using Event::Event;
};

class EarlyNewPlayerEvent : public geode::Event<NewPlayerEvent, bool(PlayerObject*)> {
public:
    using Event::Event;
};

}

