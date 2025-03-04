
inherit "/lib/std/object";
void create() { ::create(); set_name("mount_system"); }
int ride() {
    write(sprintf("%s: Riding your mount!\n", this_player()->query_name()));
    return 1;
}
void init() { add_action("ride", "ride"); }
