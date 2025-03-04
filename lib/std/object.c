/* object.c - Base object for ArchaonMUD */
void create() {
    /* Minimal object setup */
}

int id(string str) {
    return str == "object";
}

string short() {
    return "an object";
}

string long() {
    return "A generic object.";
}
