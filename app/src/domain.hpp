#ifndef DOMAIN_HPP_
#define DOMAIN_HPP_

#include <string>
#include <QVariant>
#include "qdom.hpp"

class Building
{
public:
    Building() = default;

    Building(const std::string& _name)
    {
        name = _name;
    }

    enum Type {
        Good,
        Evil
    };

    QVariant data(const std::string& role) const
    {
        if (role == "x_name")
            return QVariant(name.c_str());

        return QVariant();
    }

    std::vector<std::string> roles {
        "name"
    };

    std::string name;
    int type  = Good;
    int level = 1;

    int costForNext(){}
};

class Spell {
public:
    QVariant data(const std::string& role) const
    {
        if (role == "x_name")
            return QVariant(name.c_str());

        return QVariant();
    }

    std::vector<std::string> roles {
        "name"
    };

    std::string name;
    int mana_cost;
    bool active;
    int time_left;
    int duration;
    int auto_level;
    int faction;
};

class Upgrade {
public:
    std::string name;
    int cost;
    bool acquired;
    bool can_acquire;
};

class Trophy {
public:
    Trophy()
    {

    }

    Trophy(const std::string& _name, int _type, int _levels) {
        name = _name;
        attained_levels = _levels;
        type = _type;
    }

    enum Type{
        Allegiance,
        Misc
    };

    QVariant data(const std::string& role) const
    {
        if (role == "x_done")
            return attained_levels == total_levels;

        return QVariant();
    }

    std::vector<std::string> roles {
        "done"
    };

    std::string name;
    int type = Misc;
    int attained_levels;
    int total_levels = 5;
};

class User {
public:

    dom::property<int> gold;
    dom::property<int> gold_per_sec;
    dom::property<int> gems;
    dom::property<int> rubies;
    dom::property<int> assistants;
    dom::property<int> faction;
    dom::property<int> pending_gems;
};

struct Domain {
    dom::object<User>    user;
    dom::array<Building> buildings;
    dom::array<Trophy>   trophy;
    dom::array<Spell>    spells;
    dom::array<Upgrade>  upgrades;
    dom::array<Trophy>   trophies;

};

extern Domain domain;

#endif // DOMAIN_HPP_
