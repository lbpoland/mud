
inherit "/lib/std/room";
void setup() {
    ::create();
    set_short("Tlindhet");
    set_long("Dark Drow city under %s's gaze.", this_player()->query_deity());
}
