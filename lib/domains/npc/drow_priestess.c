inherit OBJ;

void create() { ::create(); set_name("drow_priestess"); set_short("a sinister drow priestess"); set_long("A figure cloaked in {red}Lolth’s{/red} grace, whispering dark secrets."); set_property("alignment", -50); set_property("hp", 40); }

void init() { ::init(); add_action("talk", "talk"); add_action("attack", "attack"); }

int talk(string str) { if (str == "priestess") { write("The drow priestess hisses: Serve Lolth, or perish! Accept my quest? (yes/no)\n"); input_to("accept_quest"); return 1; } return 0; }

void accept_quest(string choice) { if (lower_case(choice) == "yes") { this_player()->set_property("quest_active", "Serve Lolth"); write("Quest accepted: Serve Lolth in Tlindhet!\n"); } else { write("The priestess scorns you.\n"); } }
void offer_quest(string quest) { this_player()->set_property("quest_active", quest); write("A quest is offered: " + quest + "\n"); }

int attack(string target_name) { object target = present(target_name, environment(this_object())); if (!target) return notify_fail("No target found!\n"); int damage = random(15) + 5; target->adjust_hp(-damage); write("The drow priestess strikes " + target->query_name() + " for " + damage + " damage!\n"); return 1; }