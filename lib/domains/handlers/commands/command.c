
inherit "/lib_new/core/object.c";

mapping command_sets = ([
    "combat": ({"attack", "defend", "parry"}),
    "stealth": ({"steal", "sneak", "ambush"}),
    "crafting": ({"forge", "repair", "tinker"}),
    "faith": ({"pray", "ritual", "bless"})
]);

void create() {
    ::create();
}

void list_commands(object player) {
    string *known = player->query_known_commands();
    foreach(string category, string *cmds in command_sets) {
        string output = capitalize(category) + ":\n";
        foreach(string cmd in cmds) {
            if (member_array(cmd, known) != -1) {
                output += "  " + cmd + " (Bonus: " + player->query_bonus(cmd) + ")\n";
            }
        }
        if (sizeof(filter_array(cmds, (: member_array($1, known) != -1 :)))) {
            player->message(output);
        }
    }
}

int query_bonus(string cmd) {
    return this_player()->query_skill("fighting.melee") / 10; // Simplified example
}
