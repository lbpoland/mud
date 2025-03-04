
inherit "/lib/std/object";
#include "armor_craft.h"

#define MAX_CRAFT_SKILL 100
#define BASE_CRAFT_DIFFICULTY 50

struct material {
    string name;
    int durability;
    mapping ac_modifiers;
};

struct recipe {
    string name;
    string[] materials;
    int difficulty;
    string tool;
    mapping stats;
};

mapping materials = ([
    "leather": (["name": "leather", "durability": 20, "ac_modifiers": (["melee": 2, "fire": 0, "cold": 0, "poison": 1])]),
    "iron": (["name": "iron", "durability": 50, "ac_modifiers": (["melee": 5, "fire": 2, "electric": 1])]),
    "mithral": (["name": "mithral", "durability": 80, "ac_modifiers": (["melee": 8, "all": 3])]),
    "adamantine": (["name": "adamantine", "durability": 100, "ac_modifiers": (["melee": 10, "all": 5])]),
    "dragon scale": (["name": "dragon scale", "durability": 120, "ac_modifiers": (["melee": 9, "fire": 7, "cold": 5])]),
    "elven silk": (["name": "elven silk", "durability": 60, "ac_modifiers": (["melee": 3, "psychic": 4, "radiant": 2])])
]);

mapping recipes = ([
    "leather armor": (["name": "leather armor", "materials": ({"leather"}), "difficulty": 30, "tool": "needle",
                      "stats": (["ac_melee": 2, "ac_elemental": (["fire": 0, "cold": 0, "poison": 1])])]),
    "chain mail": (["name": "chain mail", "materials": ({"iron", "iron"}), "difficulty": 50, "tool": "hammer",
                   "stats": (["ac_melee": 5, "ac_elemental": (["fire": 2, "electric": 1])])]),
    "plate armor": (["name": "plate armor", "materials": ({"iron", "iron", "mithral"}), "difficulty": 70, "tool": "anvil",
                    "stats": (["ac_melee": 8, "ac_elemental": (["all": 3])])]),
    "mithral armor": (["name": "mithral armor", "materials": ({"mithral", "adamantine"}), "difficulty": 90, "tool": "forge",
                      "stats": (["ac_melee": 10, "ac_elemental": (["all": 5])])]),
    "dragonscale armor": (["name": "dragonscale armor", "materials": ({"dragon scale", "mithral"}), "difficulty": 95, "tool": "enchanted anvil",
                          "stats": (["ac_melee": 9, "ac_elemental": (["fire": 7, "cold": 5])])])
]);

void create() {
    ::create();
    set_name("armor_craft_system");
}

int calculate_craft_success(int skill, int difficulty) {
    int base_roll = random(100) + skill - difficulty;
    int attitude_mod = this_player()->query_property("attitude") || 0;
    return base_roll + attitude_mod;
}

string get_craft_message(int diff, string recipe_name) {
    string deity = this_player()->query_deity() || "Moradin";
    if (diff < -20) return sprintf("%s fails miserably to craft %s under %s’s gaze! The materials crumble!\n", this_player()->query_name(), recipe_name, deity);
    else if (diff < -10) return sprintf("%s nearly fails to craft %s, the work falters!\n", this_player()->query_name(), recipe_name);
    else if (diff < 0) return sprintf("%s struggles to craft %s, barely holding together!\n", this_player()->query_name(), recipe_name);
    else if (diff < 10) return sprintf("%s crafts %s with neutral skill under %s’s guidance!\n", this_player()->query_name(), recipe_name, deity);
    else if (diff < 20) return sprintf("%s crafts %s with solid effort!\n", this_player()->query_name(), recipe_name);
    else if (diff < 30) return sprintf("%s nearly masters %s with %s’s blessing!\n", this_player()->query_name(), recipe_name, deity);
    else return sprintf("%s crafts a masterful %s with %s’s divine forge!\n", this_player()->query_name(), recipe_name, deity);
}

int craft_armor(string recipe_name) {
    if (!recipes[recipe_name]) return notify_fail("Unknown recipe! Use: leather armor, chain mail, plate armor, mithral armor, dragonscale armor.\n");
    recipe recipe = recipes[recipe_name];
    int player_skill = this_player()->query_skill("crafting.smithing") || 10;
    int craft_roll = calculate_craft_success(player_skill, recipe.difficulty);

    string tool = recipe.tool;
    if (!present(tool, this_player())) return notify_fail(sprintf("You need a %s to craft %s!\n", tool, recipe_name));

    string *materials = recipe.materials;
    foreach(string mat in materials) {
        if (!present(mat, this_player())) return notify_fail(sprintf("You need %s to craft %s!\n", mat, recipe_name));
    }

    string craft_msg = get_craft_message(craft_roll, recipe_name);
    write(craft_msg);
    tell_room(environment(this_player()), craft_msg, ({this_player()}));

    if (craft_roll > 0) {
        object armor = clone_object("/lib/items/armor");
        armor->set_name(recipe_name);
        armor->set_short(sprintf("%s crafted by %s", recipe_name, this_player()->query_name()));
        armor->set_long(sprintf("A %s forged with %s’s blessing.", recipe_name, this_player()->query_deity()));
        armor->set_property("ac_melee", recipe.stats.ac_melee);
        armor->set_property("ac_elemental", recipe.stats.ac_elemental);
        armor->move(this_player());
        foreach(string mat in materials) destruct(present(mat, this_player()));
    }
    return 1;
}

void init() {
    ::init();
    add_action("craft_armor", "craft");
}
