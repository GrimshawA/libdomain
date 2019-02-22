#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <vector>

namespace dom {

template<typename T>
class property
{
public: //API

    void set(T newValue)
    {
        _val = newValue;

        dispatchNewValue(newValue, nullptr);

        if (source)
        {
            source->dispatchNewValue(newValue, this);
        }
    }

public:

    void dispatchNewValue(T value, property<T>* ignore)
    {
        _val = value;

        for (auto& s : subscribers)
        {
            if (s != ignore)
            {
                s->_val = value;
            }
        }
    }

    void operator=(property<T>& value)
    {
        // Create the channel
        source = &value;
        value.subscribers.push_back(this);

        _val = source->_val;
    }

    void operator=(T value)
    {
       // Delete me
    }

    bool operator==(T other)
    {
        return _val == other;
    }

    T& operator*() {
        return _val;
    }

    T _val;
    property<T>* source = nullptr;
    std::vector<property<T>*> subscribers;
};

template<typename T>
class array
{
public:

    int size()
    {

    }

    array<T> select(std::function<bool(T&)> pred)
    {
        return {};
    }

    int reduce(std::function<bool(T&)> pred)
    {
        return {};
    }
};

template<typename T>
class object
{
public:

    T* operator->()
    {
        return _val;
    }

    T* _val = new T();
};

}


//////////////////////////////////////////////////////////////////////////////// Usage

class Building {
public:
    enum Type {
        Good,
        Evil
    };

    std::string name;
    int type;
    int level;

    int costForNext();
};

class Spell {
public:
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
    enum Type{
        Allegiance,
        Misc
    };

    int type;
    int attained_levels;
};

class User {
public:

    dom::property<int> gold;
    dom::property<int> gold_per_sec;
    int gems;
    int rubies;
    int assistants;
    int faction;
    int pending_gems;
};

struct Domain {
    dom::object<User>    user;
    dom::array<Building> buildings;
    dom::array<Trophy>   trophy;
    dom::array<Spell>    spells;
    dom::array<Upgrade>  upgrades;
    dom::array<Trophy>   trophies;

} domain;

class view_model
{
public:

    view_model() {
        buildings_model = domain.buildings.select([](auto& b){
            return b.type == Building::Good;
        });

        spells_model = domain.spells.select([this](auto& s){
            return faction == s.faction;
        });

        purchased_upgrades = domain.upgrades.select([](auto& s) {
            return s.acquired == true;
        });

        unpurchased_upgrades = domain.upgrades.select([](auto& s) {
            return s.acquired == false && s.can_acquire;
        });

        gold = domain.user->gold;

        gold_per_sec = domain.user->gold_per_sec;

        gems = domain.user->gems;
        rubies = domain.user->rubies;
        assistants = domain.user->assistants;

        faction = domain.user->faction;

        // This reduce performs summation of all trophies levels attained
        trophies_unlocked = domain.trophies.reduce([](auto& t){
            return t.attained_levels;
        });

        total_trophies = domain.trophies.size();

        allegiance_unlocked = domain.trophies.reduce([](auto& t) {
            return t.type == Trophy::Allegiance ? t.attained_levels : 0;
        });

        allegiance_total = domain.trophies.reduce([](auto& t) {
            return t.type == Trophy::Allegiance;
        });

        misc_unlocked = domain.trophies.reduce([](auto& t) {
            return t.type == Trophy::Misc ? t.attained_levels : 0;
        });

        misc_total = domain.trophies.reduce([](auto& t) {
            return t.type == Trophy::Misc;
        });

        allegiance_trophies = domain.trophies.select([](auto& t){
            return t.type == Trophy::Allegiance;
        });

        misc_trophies = domain.trophies.select([](auto& t){
            return t.type == Trophy::Misc;
        });
    }

    dom::property<int>   gold;
    dom::property<int>   gold_per_sec;
    dom::property<int>   gems;
    dom::property<int>   rubies;
    dom::property<int>   assistants;
    dom::property<int>   faction;

    dom::array<Building> buildings_model;
    dom::array<Spell>    spells_model;
    dom::array<Upgrade>  unpurchased_upgrades;
    dom::array<Upgrade>  purchased_upgrades;

    dom::property<int>   mana;
    dom::property<int>   max_mana;
    dom::property<float> mana_regen;

    dom::property<int>   trophies_unlocked;
    dom::property<int>   total_trophies;

    dom::property<int>   allegiance_unlocked, allegiance_total;
    dom::property<int>   misc_unlocked, misc_total;
    dom::array<Trophy>   allegiance_trophies;
    dom::array<Trophy>   misc_trophies;
};

class gold_controller
{
public:
    gold_controller()
    {
        gold = domain.user->gold;

        // Creates a two way bridge with the central variable
        gold_per_sec = domain.user->gold_per_sec;

        // Binds the property so that when roto changes, we compute the expression and commit back to the others
        //gold_per_sec.bind(roto + 1);

        // Considering bind on expression, bind on safe expression(variables can die and its still valid),
        // bind on predicate with manual .eval() and bind on predicate with automatic eval on a watch list (pass a second arg with them)

    }

    void tick()
    {
        static int counter = 0;

        gold.set(*gold + *gold_per_sec);

        if (++counter > 8)
        {
            gold_per_sec.set(*gold_per_sec + 1);
            counter = 0;
        }
    }

    dom::property<int> gold;
    dom::property<int> gold_per_sec;
};

view_model vm;

void printGame() {
    // Always observe through vm to simulate the GUI
    std::cout << "Gold: " << *vm.gold << std::endl;
    std::cout << "Gold per sec: " << *vm.gold_per_sec << std::endl;
}

int main(int argc, char *argv[])
{
    // While the view model lives, all the data is ready for a view to consume

    // Init some
    domain.user->gold.set(10);
    domain.user->gold_per_sec.set(1);

    // Simulate loop
    while(true) {
        gold_controller c;
        c.tick();
        printGame();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2s);
    }

    return 0;
}
