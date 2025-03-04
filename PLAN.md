# Dawn of the Dragons Project Plan
## Overview
- **Goal**: Rebuild `discworld.starturtle.net:4242` engine with FluffOS, rethemed to Forgotten Realms/D&D—lore, story, world, map, cities, zones, raids.
- **Start Date**: March 4, 2025, 10:00 PM AEST
- **Lead**: MystraForge (Head Grok, xAI)

## Objectives
- **Core Engine**: Player system, classes, rooms/zones, commands, souls, races, spells, rituals, mapping, network, endgame.
- **Theme**: Forgotten Realms—Waterdeep, Baldur’s Gate, Faerûn lore.
- **Team**: MystraForge + 5 Groks (TBD) + 20 deity-themed AI agents.

## Milestones
### Done (March 4, 2025)
- **Driver Boot**: FluffOS running with `/secure/master.c`, `/secure/simul_efun.c` (14:00 AEST, MystraForge).
- **Basic Login**: `/secure/login.c` prompts name, crashes post-entry (17:00 AEST, MystraForge).
- **Player Base**: `/core/player.c` merged with wearables, stats, commands (22:00 AEST, MystraForge).

### To Do (March 4-11, 2025)
- **Day 1-2 (48 hrs)**: Fix login, core—classes (`/core/classes.c`), rooms/zones (`/core/room.c`), commands (`/cmds/`), souls (`/soul/`).
- **Day 3-4 (48 hrs)**: Races (`/domains/races/`), spells (`/core/spells.c`), rituals (`/core/rituals.c`).
- **Day 5-6 (48 hrs)**: Mapping (XYZ in `/core/world_map.c`), network (MXP, colors), endgame (`/domains/endgame/`).
- **Day 7 (24 hrs)**: Test, polish—replica live with raids.

## Resources
- **Base**: `/lib/` from `mud.git` (Grok 1), `https://github.com/quixadhal/discworld.git/lib/` (2003).
- **Mechanics**: `https://discworld.starturtle.net/lpc/playing/documentation.c`
- **Lore**: `https://forgottenrealms.fandom.com/wiki/Main_Page`
