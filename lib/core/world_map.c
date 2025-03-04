
inherit "/lib_new/core/object.c";
#include "/lib_new/domains/include/armoury.h"

mapping world_map;
mapping biome_data;

void create() {
    ::create();
    world_map = ([]);
    biome_data = ([]);
    add_room("/lib_new/domains/world/waterdeep_dock_ward", 0, 0, 0, "city");
    add_room("/lib_new/domains/world/underdark_entry", 10, 0, 0, "underdark");
}

void add_room(string room_path, int x, int y, int z, string biome) {
    object room = new(room_path);
    world_map[x, y, z] = room;
    biome_data[x, y, z] = biome;
    room->set_coordinates(x, y, z);
    update_connections(x, y, z);
    apply_biome_effect(x, y, z);
}

object query_room(int x, int y, int z) {
    return world_map[x, y, z];
}

string query_biome(int x, int y, int z) {
    return biome_data[x, y, z];
}

void update_connections(int x, int y, int z) {
    string *directions = ({"north", "south", "east", "west", "up", "down"});
    mapping exits = ([]);
    foreach(string dir in directions) {
        int dx = (dir == "east" ? 1 : dir == "west" ? -1 : 0);
        int dy = (dir == "north" ? 1 : dir == "south" ? -1 : 0);
        int dz = (dir == "up" ? 1 : dir == "down" ? -1 : 0);
        object neighbor = query_room(x + dx, y + dy, z + dz);
        if (neighbor) {
            exits[dir] = neighbor->query_path();
        }
    }
    world_map[x, y, z]->set_exits(exits);
}

void apply_biome_effect(int x, int y, int z) {
    string biome = query_biome(x, y, z);
    object room = query_room(x, y, z);
    if (biome == "underdark") {
        room->set_property("light", 0);
        room->add_spell("darkvision", "/lib_new/domains/handlers/spells/darkvision", "cast_spell");
    } else if (biome == "city") {
        room->set_property("light", 2);
        room->add_combat_action(50, "crowd_noise", ({"crowd_noise"}));
    }
}

/* Sample Room: Waterdeep Dock Ward */
object waterdeep_dock_ward = new(class room {
    inherit "/lib_new/core/room.c";
    int fish_smell_counter;
    mapping stall_inventory;

    void create() {
        ::create();
        set_short("Waterdeep Dock Ward");
        set_long("Generate an LPC world map system for a Forgotten Realms MUD inheriting /lib_new/core/object.c, with an advanced x,y,z grid supporting infinite layered zones (surface, Underdark), dynamic adjacency-based events (weather, biome effects), portals, elevation changes, biome-specific mechanics (e.g., darkvision in Underdark), and intelligent AI-driven connections, surpassing Skylib and Discworld MUD complexity. Include a sample Waterdeep Dock Ward room.");
        set_items(([
            "stalls": "Weathered oak stalls laden with fish.",
        ]));
        fish_smell_counter = 0;
        stall_inventory = (["herring": 20]);
    }

    string query_path() { return "/lib_new/domains/world/waterdeep_dock_ward"; }
});
