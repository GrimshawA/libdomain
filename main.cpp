#include "dom.hpp"

class Building {
public:
	Building(const std::string& _name)
	{
		name = _name;
	}

    enum Type {
        Good,
        Evil
    };

    std::string name;
    int type  = Good;
    int level = 1;

    int costForNext(){}
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

    std::vector<Building*> users; // this needs to be an automatic link in the schema


};

dom::schema schema = dom::schema{"Upgrade"}
		.field("name", &Upgrade::name)
		.field("cost", &Upgrade::cost)
		.field("acquired", &Upgrade::acquired)
		.field("can_acquire", &Upgrade::can_acquire);

namespace dom {
	template<>
	schema& get_schema_for<Upgrade>(){
		return ::schema;
	}
}

class Trophy {
public:
    Trophy(const std::string& _name, int _type, int _levels) {
        name = _name;
        attained_levels = _levels;
        type = _type;
    }

    enum Type{
        Allegiance,
        Misc
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

} domain;

class view_model
{
public:

    view_model() {
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

        //total_trophies = domain.trophies.size();

        allegiance_unlocked = domain.trophies.reduce([](Trophy& t) {
            return t.type == Trophy::Allegiance ? t.attained_levels : 0;
        });

        allegiance_total = domain.trophies.reduce([](Trophy& t) {
            return t.type == Trophy::Allegiance;
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

        // We need to setup a watch so that when a trophy "level" changes, we can get a callback to emit a popup
        //domain.trophies.watch(&Trophy::attained_levels, []())

        // Maybe a diff_watcher that will track changes on the target variable, so that we can then query it

        // static_watch could be just to invoke it manually and watch the dynamic ??
    }

    //dom::composite_watch trophies_earned_watch;

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

		buildings_model = domain.buildings.select([](Building& b) {
			return b.type == Building::Good;
		});

        // Binds the property so that when roto changes, we compute the expression and commit back to the others
        //gold_per_sec.bind(roto + 1);

        // Considering bind on expression, bind on safe expression(variables can die and its still valid),
        // bind on predicate with manual .eval() and bind on predicate with automatic eval on a watch list (pass a second arg with them)


        // The warden trophy level property must be bound to the prison Building object, therefore we need to observe changes in a specific Building object

        /*warden = dom::observe(prison, [](Building&){
            // something about prison has changed
            warden.level = prison->level / 250;
            // a notify data changed is automatically dispatched for the OBSERVER
        });*/
    }

    void tick()
    {
        static int counter = 0;

        gold.set(*gold + *gold_per_sec);

        if (++counter > 8)
        {
            counter = 0;

            auto* b = buildings_model.at(0);
            b->level++;
            buildings_model.notifyDataChanged(b);

            gold_per_sec.set(1 + b->level * 2);
        }
    }

    //dom::object<Trophy> warden;
    //dom::object<Building> prison; // used in trophy

	dom::array<Building> buildings_model;
    dom::property<int> gold;
    dom::property<int> gold_per_sec;
};

void printGame(view_model& vm) {
    // Always observe through vm to simulate the GUI
    std::cout << "Gold: " << *vm.gold << std::endl;
    std::cout << "Gold per sec: " << *vm.gold_per_sec << std::endl;

	std::cout << "Buildings: " << std::endl;
	for (auto& bld : vm.buildings_model)
	{
		std::cout << "Building: " << bld->name << "(" << bld->level << ")" << std::endl;
	}

    std::cout << "Misc trophies: " << *vm.misc_unlocked << "/" << *vm.misc_total << std::endl;
    for (auto& miscTrophy : vm.misc_trophies)
    {
        std::cout << "Trophy name: " << miscTrophy->name << std::endl;
    }

    std::cout << "Allegiance trophies: " << std::endl;
    for (auto& alleTrophy : vm.allegiance_trophies)
    {
        std::cout << "Trophy name: " << alleTrophy->name << std::endl;
    }
}

void initData() {
    domain.trophies.push(new Trophy("Building master 01", Trophy::Misc, 1));
    domain.trophies.push(new Trophy("Warden", Trophy::Misc, 3));
    domain.trophies.push(new Trophy("Things", Trophy::Allegiance, 4));
    domain.trophies.push(new Trophy("Less things", Trophy::Allegiance, 4));

    domain.buildings.push(new Building("Prison"));
}

int main(int argc, char *argv[])
{
	auto& sch = dom::get_schema_for<Upgrade>();
	auto& sch2 = dom::get_schema_for<User>();

	std::cout << "sch1 " << sch.typeName << std::endl;
	sch.debug();
	std::cout << "sch2 " << sch2.typeName << std::endl;

    // While the view model lives, all the data is ready for a view to consume

    // Init some
    domain.user->gold.set(10);
    domain.user->gold_per_sec.set(1);

    initData();

    view_model vm;

    // Simulate loop
    while(true) {
        gold_controller c;
        c.tick();
        printGame(vm);

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
