
inherit "/lib/std/object";
#include "combat.h"
#include "attack_messages.h"

#define MAX_STAMINA 100
#define STANCE_OFFENSIVE 1
#define STANCE_DEFENSIVE 2
#define STANCE_BALANCED 3
#define CRIT_CHANCE_BASE 5
#define WIMPY_THRESHOLD 20
#define ATTITUDE_WIMPY -20
#define ATTITUDE_DEFENSIVE -10
#define ATTITUDE_NEUTRAL 0
#define ATTITUDE_OFFENSIVE 10
#define ATTITUDE_INSANE 20
#define MAX_AC 10

struct combat_stats {
    int stamina;
    int stance;
    int damage;
    string last_attack;
    int action_points;
    int wimpy;
    int attitude;
};

struct armor_stats {
    int ac_melee;
    mapping ac_elemental;
};

mapping damage_types = ([
    "slash": 1, "slashing": 1, "pierce": 1, "piercing": 1, "bludgeon": 1, "bludgeoning": 1,
    "fire": 1, "cold": 1, "electric": 1, "acid": 1, "poison": 1, "necrotic": 1,
    "radiant": 1, "psychic": 1, "force": 1, "thunder": 1,
    "holy": 1, "unholy": 1, "chaotic": 1, "lawful": 1
]);

mapping weapon_weights = ([
    "dagger": 1, "short sword": 2, "longsword": 3, "greatsword": 6,
    "mace": 4, "warhammer": 5, "wooden mace": 3, "flail": 4,
    "bow": 2, "crossbow": 3, "staff": 3, "spear": 3,
    "wooden shield": 3, "leather armor": 1
]);

mapping attitude_modifiers = ([
    "wimpy": ATTITUDE_WIMPY, "defensive": ATTITUDE_DEFENSIVE,
    "neutral": ATTITUDE_NEUTRAL, "offensive": ATTITUDE_OFFENSIVE,
    "insane": ATTITUDE_INSANE
]);

mapping elemental_tiers = ([
    0: (["fire": "fails to ignite, the flame sputters out!", "cold": "fails to freeze, the chill fades!",
        "electric": "fails to shock, the spark dies!", "acid": "fails to corrode, the drip evaporates!",
        "poison": "fails to poison, the venom dissipates!", "necrotic": "fails to drain, the shadow lifts!",
        "radiant": "fails to glow, the light dims!", "psychic": "fails to assault, the mind resists!",
        "force": "fails to push, the force collapses!", "thunder": "fails to boom, the sound muffles!",
        "holy": "fails to bless, the grace falters!", "unholy": "fails to curse, the darkness retreats!",
        "chaotic": "fails to disrupt, the chaos settles!", "lawful": "fails to order, the law weakens!"]),
    1: (["fire": "ignites a weak flame, singeing slightly!", "cold": "freezes a faint frost, chilling a bit!",
        "electric": "shocks with a mild spark, tingling faintly!", "acid": "corrodes with a light drip, burning a touch!",
        "poison": "poisons with a faint venom, weakening slightly!", "necrotic": "drains with a dim shadow, tiring a bit!",
        "radiant": "glows with a soft light, warming faintly!", "psychic": "assaults with a mild thought, confusing a touch!",
        "force": "pushes with a light gust, nudging slightly!", "thunder": "booms with a low rumble, startling a bit!",
        "holy": "blesses with a faint grace, uplifting slightly!", "unholy": "curses with a dim shadow, unsettling a bit!",
        "chaotic": "disrupts with a mild chaos, confusing faintly!", "lawful": "orders with a gentle law, calming a touch!"]),
    50: (["fire": "unleashes a modest blaze, burning mildly!", "cold": "freezes with a steady frost, chilling deeply!",
        "electric": "shocks with a moderate spark, stinging sharply!", "acid": "corrodes with a steady drip, searing noticeably!",
        "poison": "poisons with a moderate venom, weakening significantly!", "necrotic": "drains with a dark shadow, fatiguing heavily!",
        "radiant": "glows with a warm light, healing faintly!", "psychic": "assaults with a strong thought, disorienting notably!",
        "force": "pushes with a firm gust, knocking back slightly!", "thunder": "booms with a loud clap, deafening briefly!",
        "holy": "blesses with a warm grace, uplifting notably!", "unholy": "curses with a dark shadow, unsettling heavily!",
        "chaotic": "disrupts with a steady chaos, confusing significantly!", "lawful": "orders with a firm law, calming deeply!"]),
    100: (["fire": "releases a solid inferno, burning fiercely!", "cold": "freezes with a harsh frost, numbing deeply!",
        "electric": "shocks with a strong jolt, searing painfully!", "acid": "corrodes with a steady flow, eating away!",
        "poison": "poisons with a potent venom, debilitating severely!", "necrotic": "drains with a deep shadow, exhausting completely!",
        "radiant": "glows with a bright light, healing noticeably!", "psychic": "assaults with a powerful thought, stunning greatly!",
        "force": "pushes with a heavy gust, toppling slightly!", "thunder": "booms with a thunderous roar, deafening intensely!",
        "holy": "blesses with a bright grace, uplifting greatly!", "unholy": "curses with a deep shadow, unsettling severely!",
        "chaotic": "disrupts with a strong chaos, confusing heavily!", "lawful": "orders with a stern law, calming powerfully!"]),
    250: (["fire": "blasts with a roaring blaze, scorching severely!", "cold": "freezes with an icy storm, paralyzing deeply!",
        "electric": "shocks with a fierce bolt, searing agonizingly!", "acid": "corrodes with a torrential flow, dissolving flesh!",
        "poison": "poisons with a deadly venom, crippling utterly!", "necrotic": "drains with a black shadow, annihilating vitality!",
        "radiant": "glows with a radiant light, healing substantially!", "psychic": "assaults with a dominating thought, shattering focus!",
        "force": "pushes with a mighty gust, hurling backward!", "thunder": "booms with a deafening blast, rupturing eardrums!",
        "holy": "blesses with a radiant grace, uplifting substantially!", "unholy": "curses with a black shadow, unsettling utterly!",
        "chaotic": "disrupts with a fierce chaos, confusing completely!", "lawful": "orders with a mighty law, calming strongly!"]),
    500: (["fire": "erupts with a blazing inferno, incinerating all!", "cold": "freezes with an arctic storm, petrifying utterly!",
        "electric": "shocks with a devastating bolt, frying instantly!", "acid": "corrodes with a deluge, melting armor!",
        "poison": "poisons with a lethal venom, killing slowly!", "necrotic": "drains with a void shadow, erasing life!",
        "radiant": "glows with a holy light, restoring vigor!", "psychic": "assaults with a crushing thought, breaking minds!",
        "force": "pushes with a torrential gust, obliterating resistance!", "thunder": "booms with a cataclysmic roar, shattering bones!",
        "holy": "blesses with a holy grace, uplifting powerfully!", "unholy": "curses with a void shadow, unsettling fatally!",
        "chaotic": "disrupts with a wild chaos, confusing utterly!", "lawful": "orders with a divine law, calming forcefully!"]),
    750: (["fire": "unleashes a fiery cataclysm, reducing to ash!", "cold": "freezes with a glacial tempest, encasing in ice!",
        "electric": "shocks with a thunderous bolt, vaporizing flesh!", "acid": "corrodes with a flood, dissolving everything!",
        "poison": "poisons with a virulent venom, ending existence!", "necrotic": "drains with an abyssal shadow, consuming souls!",
        "radiant": "glows with a divine light, reviving the fallen!", "psychic": "assaults with a mind-shattering force, erasing will!",
        "force": "pushes with a hurricane gust, leveling the land!", "thunder": "booms with an apocalyptic blast, rending the earth!",
        "holy": "blesses with a divine grace, uplifting divinely!", "unholy": "curses with an abyssal shadow, unsettling eternally!",
        "chaotic": "disrupts with a chaotic storm, confusing completely!", "lawful": "orders with a sacred law, calming divinely!"]),
    1000: (["fire": "calls forth a colossal inferno, engulfing the world!", "cold": "freezes with a polar vortex, freezing time!",
        "electric": "shocks with a cataclysmic bolt, annihilating all!", "acid": "corrodes with a tidal wave, erasing matter!",
        "poison": "poisons with a plague, wiping out life!", "necrotic": "drains with a void abyss, unmaking reality!",
        "radiant": "glows with a celestial light, resurrecting the dead!", "psychic": "assaults with a cosmic thought, rewriting minds!",
        "force": "pushes with a galactic gust, shattering dimensions!", "thunder": "booms with a universal roar, ending existence!",
        "holy": "blesses with a celestial grace, uplifting cosmically!", "unholy": "curses with a void abyss, unsettling universally!",
        "chaotic": "disrupts with a cosmic chaos, confusing utterly!", "lawful": "orders with a divine order, calming universally!"]),
    1500: (["fire": "invokes a legendary blaze, consuming the cosmos!", "cold": "freezes with an eternal ice, halting creation!",
        "electric": "shocks with a godly bolt, erasing the universe!", "acid": "corrodes with a primordial flood, dissolving all!",
        "poison": "poisons with a mythic venom, ending all life!", "necrotic": "drains with a cosmic shadow, unmaking existence!",
        "radiant": "glows with a divine aura, restoring the world!", "psychic": "assaults with a transcendent thought, reshaping reality!",
        "force": "pushes with a cosmic gale, collapsing stars!", "thunder": "booms with a divine thunder, shattering eternity!",
        "holy": "blesses with a transcendent grace, uplifting eternally!", "unholy": "curses with a cosmic shadow, unsettling infinitely!",
        "chaotic": "disrupts with a universal chaos, confusing completely!", "lawful": "orders with a cosmic law, calming eternally!"]),
    2000: (["fire": "summons a divine inferno, forging a new realm!", "cold": "freezes with an absolute chill, birthing a new ice age!",
        "electric": "shocks with a celestial bolt, creating a new world!", "acid": "corrodes with a cosmic tide, reshaping matter!",
        "poison": "poisons with a godly venom, purging and renewing!", "necrotic": "drains with a sacred shadow, regenerating life!",
        "radiant": "glows with a holy light, illuminating creation!", "psychic": "assaults with a divine mind, crafting destiny!",
        "force": "pushes with a universal force, birthing galaxies!", "thunder": "booms with a divine chorus, heralding a new era!",
        "holy": "blesses with a divine aura, uplifting creation!", "unholy": "curses with a sacred shadow, unsettling renewal!",
        "chaotic": "disrupts with a divine chaos, confusing destiny!", "lawful": "orders with a universal law, calming creation!"])
]);

mapping armor_types = ([
    "leather armor": (["ac_melee": 2, "ac_elemental": (["fire": 0, "cold": 0, "poison": 1])]),
    "chain mail": (["ac_melee": 5, "ac_elemental": (["fire": 2, "electric": 1])]),
    "plate armor": (["ac_melee": 8, "ac_elemental": (["all": 3])]),
    "mithral armor": (["ac_melee": 10, "ac_elemental": (["all": 5])]),
    "dragonscale armor": (["ac_melee": 9, "ac_elemental": (["fire": 7, "cold": 5])])
]);

struct combat_stats combat;
struct armor_stats player_armor;
struct armor_stats enemy_armor;

void create() {
    ::create();
    set_name("combat_system");
    combat.stamina = MAX_STAMINA;
    combat.stance = STANCE_BALANCED;
    combat.damage = 0;
    combat.last_attack = "none";
    combat.action_points = 10;
    combat.wimpy = WIMPY_THRESHOLD;
    combat.attitude = ATTITUDE_NEUTRAL;
    player_armor = armor_types["leather armor"];
    enemy_armor = armor_types["none"] || (["ac_melee": 0, "ac_elemental": (["all": 0])]);
}

int query_stamina() {
    return combat.stamina;
}

int set_stance(string stance) {
    if (stance == "offensive") combat.stance = STANCE_OFFENSIVE;
    else if (stance == "defensive") combat.stance = STANCE_DEFENSIVE;
    else combat.stance = STANCE_BALANCED;
    write(sprintf("%s adopts a %s stance under %s’s guidance!\n", this_player()->query_name(), stance, this_player()->query_deity()));
    return 1;
}

int set_attitude(string attitude) {
    if (!attitude_modifiers[attitude]) return notify_fail("Invalid attitude! Use wimpy/defensive/neutral/offensive/insane.\n");
    combat.attitude = attitude_modifiers[attitude];
    write(sprintf("%s sets attitude to %s!\n", this_player()->query_name(), attitude));
    return 1;
}

int set_wimpy(int threshold) {
    combat.wimpy = (threshold > 0 && threshold <= 100) ? threshold : WIMPY_THRESHOLD;
    write(sprintf("%s sets wimpy to %d%% HP!\n", this_player()->query_name(), combat.wimpy));
    return 1;
}

int equip_armor(string armor) {
    if (!armor_types[armor]) return notify_fail("Unknown armor type!\n");
    player_armor = armor_types[armor];
    write(sprintf("%s equips %s!\n", this_player()->query_name(), armor));
    return 1;
}

int apply_tactic(string tactic) {
    mapping tactic_messages = (["feint": "You feint deftly!", "charge": "You charge boldly!"]);
    if (!tactic_messages[tactic]) return notify_fail("Unknown tactic!\n");
    if (combat.action_points < 2) return notify_fail("Not enough action points!\n");
    combat.action_points -= 2;
    write(tactic_messages[tactic]);
    return 1;
}

int use_special(string ability) {
    mapping special_abilities = (["rage": "You unleash a furious rage!", "heal": "You mend your wounds!"]);
    if (!special_abilities[ability]) return notify_fail("Unknown ability!\n");
    if (combat.action_points < 5) return notify_fail("Not enough action points!\n");
    combat.action_points -= 5;
    write(special_abilities[ability]);
    return 1;
}

int calculate_to_hit(string weapon, int player_skill, int tactic_skill, int target_defense, int target_tactic, string type) {
    int weapon_weight = weapon_weights[weapon] || 1;
    int base_roll = random(100) + player_skill + tactic_skill - (target_defense + target_tactic) - weapon_weight;
    int attitude_mod = combat.attitude;
    int ac_reduction = (type not in ["slash", "pierce", "bludgeon", "slashing", "piercing", "bludgeoning"]) ?
                       enemy_armor.ac_elemental[type] || enemy_armor.ac_elemental["all"] || 0 : enemy_armor.ac_melee;
    return base_roll + attitude_mod - ac_reduction;
}

string get_attack_message(int diff, string type, string weapon) {
    string deity = this_player()->query_deity() || "Torm";
    if (diff < -20) return sprintf("%s swings %s with %s’s blade but misses completely!\n", this_player()->query_name(), weapon, deity);
    else if (diff < -10) return sprintf("%s nearly misses with %s’s clumsy %s strike!\n", this_player()->query_name(), weapon, deity);
    else if (diff < 0) return sprintf("%s strikes weakly with %s’s guided %s!\n", this_player()->query_name(), weapon, deity);
    else if (diff < 10) return sprintf("%s lands a neutral hit with %s’s %s!\n", this_player()->query_name(), weapon, deity);
    else if (diff < 20) return sprintf("%s scores a solid blow with %s’s mighty %s!\n", this_player()->query_name(), weapon, deity);
    else if (diff < 30) return sprintf("%s nearly masters %s’s %s strike!\n", this_player()->query_name(), weapon, deity);
    else return sprintf("%s delivers a powerful %s strike with %s’s blessing!\n", this_player()->query_name(), weapon, deity);
}

string get_elemental_attack_message(int damage, string element, string weapon) {
    foreach(int threshold, mapping msgs in elemental_tiers) {
        if (damage <= threshold || threshold == 2000) return sprintf("%s %s %s %s!", this_player()->query_name(), msgs[element], weapon, (random(100) < 5) ? "with a mystical surge" : "");
    }
    return sprintf("%s summons a divine %s blast with %s!", this_player()->query_name(), element, weapon);
}

int attack(string str) {
    string target, type, weapon, extra;
    if (sscanf(str, "attack %s with %s %s with %s", target, type, weapon, extra) == 4 ||
        sscanf(str, "attack %s with %s %s", target, type, weapon) == 3 ||
        sscanf(str, "attack %s with %s", target, weapon) == 2) {
    } else {
        return notify_fail("Syntax: attack <target> with <type> <weapon> [with <element>]\n");
    }
    if (!damage_types[type]) type = "slash";

    object enemy = present(target, environment(this_player()));
    if (!enemy) return notify_fail("No target found!\n");
    if (combat.action_points < 1) return notify_fail("No action points left!\n");
    combat.action_points -= 1;

    int player_skill = this_player()->query_skill("fighting." + type) || 10;
    int player_tactic = this_player()->query_skill("tactics") || 5;
    int troll_defense = enemy->query_skill("fighting.parry") || 15;
    int troll_tactic = enemy->query_skill("tactics") || 5;
    int to_hit = calculate_to_hit(weapon, player_skill, player_tactic, troll_defense, troll_tactic, type);

    string attack_msg = get_attack_message(to_hit, type, weapon);
    int crit_chance = CRIT_CHANCE_BASE + (to_hit > 0 ? min((to_hit / 10) * 2, 10) : 0);
    if (to_hit > 0 && random(100) < crit_chance) {
        combat.damage = to_int(combat.damage * 1.5);
        attack_msg += sprintf(" %s lands a critical strike with %s’s divine wrath!\n", this_player()->query_name(), this_player()->query_deity());
    } else if (to_hit <= 0) {
        combat.damage = 0;
    } else {
        combat.damage = to_int(random(2000) + player_skill * 10);
        if (extra && damage_types[extra]) {
            int elemental_damage = to_int(random(1000) + player_skill * 5);
            attack_msg += get_elemental_attack_message(elemental_damage, extra, weapon);
            combat.damage += elemental_damage - (enemy_armor.ac_elemental[extra] || enemy_armor.ac_elemental["all"] || 0);
        }
    }

    write(attack_msg);
    tell_room(environment(this_player()), attack_msg, ({this_player()}));
    tell_object(enemy, attack_msg);

    if (enemy->query_hp() * 100 / enemy->query_max_hp() < combat.wimpy) {
        write(sprintf("%s flees in wimpy panic under %s’s wrath!\n", enemy->query_name(), this_player()->query_deity()));
        enemy->move(random_exit(environment(enemy)));
    }

    enemy->adjust_hp(-combat.damage);
    combat.stamina -= 10;
    combat.last_attack = type;
    call_out("recover_stamina", 10, this_player());
    call_out("recover_action_points", 5, this_player());
    return 1;
}

int calculate_defense(string weapon, int player_skill, int tactic_skill, int enemy_attack, int enemy_tactic, string type) {
    int shield_weight = weapon_weights["wooden shield"] || 3;
    int weapon_weight = weapon_weights[weapon] || 1;
    int base_roll = random(100) + player_skill + tactic_skill - (enemy_attack + enemy_tactic) - (weapon_weight - shield_weight);
    int attitude_mod = combat.attitude * -1;
    int ac_reduction = (type not in ["slash", "pierce", "bludgeon", "slashing", "piercing", "bludgeoning"]) ?
                       player_armor.ac_elemental[type] || player_armor.ac_elemental["all"] || 0 : player_armor.ac_melee;
    return base_roll + attitude_mod - ac_reduction;
}

string get_defense_message(int diff, string action) {
    string deity = this_player()->query_deity() || "Torm";
    string defense_item = "wooden shield";
    if (action == "parry") defense_item = "short sword";
    else if (action == "dodge") defense_item = "leather armor";
    if (diff < -20) return sprintf("%s fails miserably to %s with %s under %s’s gaze!\n", this_player()->query_name(), action, defense_item, deity);
    else if (diff < -10) return sprintf("%s barely holds to %s with %s against %s’s strike!\n", this_player()->query_name(), action, defense_item, deity);
    else if (diff < 0) return sprintf("%s struggles to %s with %s against %s’s blow!\n", this_player()->query_name(), action, defense_item, deity);
    else if (diff < 10) return sprintf("%s %ss evenly with %s against %s’s attack!\n", this_player()->query_name(), action, defense_item, deity);
    else if (diff < 20) return sprintf("%s %ss skillfully with %s against %s’s strike!\n", this_player()->query_name(), action, defense_item, deity);
    else if (diff < 30) return sprintf("%s nearly masters %s with %s against %s’s assault!\n", this_player()->query_name(), action, defense_item, deity);
    else return sprintf("%s perfectly %ss with %s against %s’s onslaught!\n", this_player()->query_name(), action, defense_item, deity);
}

int defend(string str) {
    string target, action;
    if (sscanf(str, "defend %s with %s", target, action) != 2 && sscanf(str, "defend %s", target) != 1)
        return notify_fail("Syntax: defend <target> [with block/parry/dodge]\n");
    action = action || "block";
    object enemy = present(target, environment(this_player()));
    if (!enemy) return notify_fail("No target found!\n");

    int player_skill = this_player()->query_skill("fighting." + action) || 10;
    int player_tactic = this_player()->query_skill("tactics") || 5;
    int troll_attack = enemy->query_skill("fighting.mace") || 15;
    int troll_tactic = enemy->query_skill("tactics") || 5;
    int defense_roll = calculate_defense("wooden mace", player_skill, player_tactic, troll_attack, troll_tactic, "bludgeon");

    string def_msg = get_defense_message(defense_roll, action);
    write(def_msg);
    tell_room(environment(this_player()), def_msg, ({this_player()}));
    if (defense_roll > 0) enemy->adjust_hp(-combat.damage / 2);
    return 1;
}

void recover_stamina(object player) {
    if (combat.stamina < MAX_STAMINA) combat.stamina += 5;
    player->set_property("stamina", combat.stamina);
}

void recover_action_points(object player) {
    if (combat.action_points < 10) combat.action_points += 1;
    player->set_property("action_points", combat.action_points);
}

void consider(string target) {
    object enemy = present(target, environment(this_player()));
    if (!enemy) return notify_fail("No target found!\n");
    int diff = enemy->query_skill("fighting") - this_player()->query_skill("fighting");
    string msg = sprintf("%s considers %s: %s is %s than you!\n",
                         this_player()->query_name(), enemy->query_short(),
                         enemy->query_short(), (diff > 0) ? "stronger" : "weaker");
    write(msg);
    return 1;
}

void pursue(string target) {
    object enemy = present(target, environment(this_player()));
    if (!enemy) return notify_fail("No target found!\n");
    write(sprintf("%s pursues %s with relentless fury under %s’s command!\n", this_player()->query_name(), enemy->query_short(), this_player()->query_deity()));
    enemy->move(environment(this_player()));
    return 1;
}

void init() {
    ::init();
    add_action("set_stance", "stance");
    add_action("set_attitude", "attitude");
    add_action("set_wimpy", "wimpy");
    add_action("equip_armor", "equip");
    add_action("apply_tactic", "tactic");
    add_action("use_special", "special");
    add_action("attack", "attack");
    add_action("defend", "defend");
    add_action("consider", "consider");
    add_action("pursue", "pursue");
}
