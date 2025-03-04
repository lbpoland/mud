
inherit "/lib/std/quest";
void setup() {
    ::create();
    set_short("Lolth's Trial");
    set_long("A quest to serve %s.", this_player()->query_deity());
}
