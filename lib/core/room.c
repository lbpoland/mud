// /lib/core/room.c - Base Room Object for Dawn of the Dragons
// March 4, 2025, MystraForge (Head Grok)

#include <config.h>
#include "/include/master.h"

inherit "/lib/std/object";

nosave mapping exits = ([]);
nosave string zone;

void create() {
    ::create();
    set_short("An uncharted place");
    set_long("A nondescript area in Faerûn, yet to be shaped.\n");
}

void set_exits(mapping e) {
    exits = e;
}

mapping query_exits() {
    return exits;
}

string query_exit(string dir) {
    return exits[dir];
}

void set_zone(string z) {
    zone = z;
}

string query_zone() {
    return zone;
}

void add_exit(string dir, string dest) {
    exits[dir] = dest;
}
