extern int global;

bool setGlobal() {
    global = 23;
    return true;
}

bool dummy = setGlobal();

