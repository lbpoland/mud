inherit OBJ;

void create() { ::create(); set_name("command_handler"); }

void init() { ::init(); add_action("cmd_move", "go"); add_action("cmd_inventory", "inventory"); add_action("cmd_equip", "equip"); add_action("cmd_draw", "draw"); add_action("cmd_party_loot", "partyloot"); }

int cmd_move(string direction) { object room = environment(this_player()); if (room && room->query_exit(direction)) { this_player()->move_player(direction); return 1; } write("You cannot go that way!\n"); return 0; }
int cmd_inventory() { object *inv = all_inventory(this_player()); string output = "You are carrying:\n"; foreach (object item in inv) { output += "  " + item->query_short() + "\n"; } write(output); return 1; }
int cmd_equip(string item_name) { object item = present(item_name, this_player()); if (!item) return notify_fail("You don’t have that!\n"); if (item->query_property("layer")) { this_player()->set_property("worn_" + item->query_property("layer"), item); write("You equip " + item->query_short() + ".\n"); return 1; } return notify_fail("That cannot be equipped!\n"); }
int cmd_draw(string weapon_name) { object weapon = present(weapon_name, this_player()); if (!weapon || !weapon->query_property("scabbard")) return notify_fail("No sheathed weapon found!\n"); weapon->move(this_player()); write("You draw " + weapon->query_short() + " from its scabbard.\n"); return 1; }
int cmd_party_loot(string str) { object *party = filter(all_inventory(environment(this_object())), (: $1->query_property("party") == $2->query_property("party") && $1 != $2 :), this_object()); if (!sizeof(party)) return notify_fail("No party members found!\n"); mapping loot = this_player()->query_property("party_inventory") || ([]); string output = "Party Loot Chest:\n"; int total_value = 0; foreach (string item_name, int value in loot) { output += "  " + item_name + " (Value: " + value + " gp)\n"; total_value += value; } output += "Total Value: " + total_value + " gp\n"; write(output); return 1; }