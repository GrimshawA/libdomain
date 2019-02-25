#ifndef GOLD_CONTROLLER_HPP_
#define GOLD_CONTROLLER_HPP_

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

#endif // GOLD_CONTROLLER_HPP_
