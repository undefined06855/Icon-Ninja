#pragma once
#include <Geode/loader/Event.hpp>
#include <Geode/binding/PlayerObject.hpp>

namespace ninja {

/*

Usage (probably):

EarlyNewPlayerEvent is called before stuff is done to the playerobject (this is a blank playerobject)
NewPlayerEvent is called after stuff is done (this is a pre-populated playerobject)

If you set `modified` in EarlyNewPlayerEvent, Icon Ninja will not set its own stuff and will trust that the mod has
(though please can you set modified in NewPlayerEvent in case I decide to do something with it later)

EarlyNewPlayerEvent().listen([](PlayerObject* player) {
    if (doTheShit) {
        // do the shit
        // ...

        player->setUserFlag("undefined0.icon_ninja/modified");
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

