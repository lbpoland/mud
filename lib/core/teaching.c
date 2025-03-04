
inherit "/lib/std/object";
void create() { ::create(); set_name("teaching_system"); }
int teach(string skill) {
    write(sprintf("%s: Teaching %s!\n", this_player()->query_name(), skill));
    this_player()->add_skill_level(skill, 10);
    return 1;
}
void init() { add_action("teach", "teach"); }
