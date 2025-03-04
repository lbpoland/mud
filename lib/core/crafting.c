
inherit "/lib/std/object";
void create() { ::create(); set_name("crafting_system"); }
int craft(string item) {
    write(sprintf("%s: Crafting %s!\n", this_player()->query_name(), item));
    return 1;
}
void init() { add_action("craft", "craft"); }
