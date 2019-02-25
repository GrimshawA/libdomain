#ifndef GAME_VIEW_MODEL_HPP_
#define GAME_VIEW_MODEL_HPP_

#include <QObject>
#include "qdom.hpp"

#include "domain.hpp"
#include "gold_controller.hpp"

class GameViewModel : public QObject
{
    Q_OBJECT

    using ThisClass = GameViewModel;

public:

    DOM_PROPERTY(int, gold)
    DOM_PROPERTY(int, gold_per_sec)
    DOM_PROPERTY(int, gems)
    DOM_PROPERTY(int, rubies)
    DOM_PROPERTY(int, assistants)
    DOM_PROPERTY(int, faction)
    DOM_PROPERTY(int, allegiance_unlocked)
    DOM_PROPERTY(int, allegiance_total)
    DOM_PROPERTY(int, misc_unlocked)
    DOM_PROPERTY(int, misc_total)
    DOM_PROPERTY(int, trophies_unlocked)
    DOM_PROPERTY(int, total_trophies)

    DOM_MODEL(Building, buildings_model)
    DOM_MODEL(Spell,    spells_model)
    DOM_MODEL(Trophy,   allegiance_trophies)
    DOM_MODEL(Trophy,   misc_trophies)

public:

    GameViewModel()
    {
        domain.user->gold.set(10);
        domain.user->gold_per_sec.set(1);

        domain.trophies.push(new Trophy("Building master 01", Trophy::Misc, 1));
        domain.trophies.push(new Trophy("Warden", Trophy::Misc, 3));
        domain.trophies.push(new Trophy("Things", Trophy::Allegiance, 4));
        domain.trophies.push(new Trophy("Less things", Trophy::Allegiance, 4));
        domain.trophies.push(new Trophy("Interesting", Trophy::Allegiance, 5));

        domain.buildings.push(new Building("Prison"));
        domain.buildings.push(new Building("Sanctuary"));

        buildings_model = domain.buildings.select([](Building& b){
            return b.type == Building::Good;
        });

        spells_model = domain.spells.select([this](Spell& s){
            return faction == s.faction;
        });

        purchased_upgrades = domain.upgrades.select([](Upgrade& s) {
            return s.acquired == true;
        });

        unpurchased_upgrades = domain.upgrades.select([](Upgrade& s) {
            return s.acquired == false && s.can_acquire;
        });

        gold = domain.user->gold;

        gold_per_sec = domain.user->gold_per_sec;

        gems = domain.user->gems;
        rubies = domain.user->rubies;
        assistants = domain.user->assistants;

        faction = domain.user->faction;

        // This reduce performs summation of all trophies levels attained
        trophies_unlocked = domain.trophies.reduce([](Trophy& t){
            return t.attained_levels;
        });

        total_trophies = domain.trophies.reduce([](Trophy& t){
            return t.total_levels;
        });

        allegiance_unlocked = domain.trophies.reduce([](Trophy& t) {
            return t.type == Trophy::Allegiance ? t.attained_levels : 0;
        });

        allegiance_total = domain.trophies.reduce([](Trophy& t) {
            return t.type == Trophy::Allegiance ? t.total_levels : 0;
        });

        misc_unlocked = domain.trophies.reduce([](Trophy& t) {
            return t.type == Trophy::Misc ? t.attained_levels : 0;
        });

        misc_total = domain.trophies.reduce([](Trophy& t) {
            return t.type == Trophy::Misc ? t.total_levels : 0;
        });

        allegiance_trophies = domain.trophies.select([](Trophy& t){
            return t.type == Trophy::Allegiance;
        });

        misc_trophies = domain.trophies.select([](Trophy& t){
            return t.type == Trophy::Misc;
        });

        startTimer(1000);
    }

    void timerEvent(QTimerEvent *event)
    {
        goldController.tick();
    }

public slots:

    void click(int x, int y)
    {
        gold.set(*gold + 1);

        clicked(x, y);
    }

signals:
    void clicked(int x,int y);

public:

    gold_controller goldController;

    dom::array<Upgrade>  unpurchased_upgrades;
    dom::array<Upgrade>  purchased_upgrades;

    dom::property<int>   mana;
    dom::property<int>   max_mana;
    dom::property<float> mana_regen;





};

#endif // GAME_VIEW_MODEL_HPP_
