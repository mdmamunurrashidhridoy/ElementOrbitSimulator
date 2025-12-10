#ifdef _WIN32
#include <windows.h>    // must be included before GL headers on Windows
#endif

#include <GL/glut.h>    // or <GL/freeglut.h> if you're using freeglut
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdio>       // for sprintf

// ----------------- Constants -----------------
const float PI = 3.14159265358979323846f;

// ----------------- Modes -----------------
enum Mode { MODE_TABLE, MODE_ATOM };
Mode currentMode = MODE_TABLE;

// ----------------- Camera (3D view) -----------------
float camAngleY = 30.0f;   // horizontal angle
float camAngleX = 20.0f;   // vertical angle
float camDist   = 35.0f;   // distance from center

bool  isPaused = false;

// ----------------- Element data -----------------
struct ElementInfo {
    int  Z;                  // atomic number
    const char* symbol;
    const char* name;
    int  period;             // 1..7
    int  group;              // 1..18 (0 for f-block)
    int  blockRow;           // 0 = main table, 1 = lanthanides row, 2 = actinides row
    int  blockCol;           // 0..14 index in lanth/act row (for f-block)
};

// Full 118 elements with period, group, and f-block row info
ElementInfo elements[] = {
    //   Z  Sym  Name            period group blockRow blockCol
    {  1, "H",  "Hydrogen",          1,   1,   0,      0 },
    {  2, "He", "Helium",            1,  18,   0,      0 },

    {  3, "Li", "Lithium",           2,   1,   0,      0 },
    {  4, "Be", "Beryllium",         2,   2,   0,      0 },
    {  5, "B",  "Boron",             2,  13,   0,      0 },
    {  6, "C",  "Carbon",            2,  14,   0,      0 },
    {  7, "N",  "Nitrogen",          2,  15,   0,      0 },
    {  8, "O",  "Oxygen",            2,  16,   0,      0 },
    {  9, "F",  "Fluorine",          2,  17,   0,      0 },
    { 10, "Ne", "Neon",              2,  18,   0,      0 },

    { 11, "Na", "Sodium",            3,   1,   0,      0 },
    { 12, "Mg", "Magnesium",         3,   2,   0,      0 },
    { 13, "Al", "Aluminium",         3,  13,   0,      0 },
    { 14, "Si", "Silicon",           3,  14,   0,      0 },
    { 15, "P",  "Phosphorus",        3,  15,   0,      0 },
    { 16, "S",  "Sulfur",            3,  16,   0,      0 },
    { 17, "Cl", "Chlorine",          3,  17,   0,      0 },
    { 18, "Ar", "Argon",             3,  18,   0,      0 },

    { 19, "K",  "Potassium",         4,   1,   0,      0 },
    { 20, "Ca", "Calcium",           4,   2,   0,      0 },
    { 21, "Sc", "Scandium",          4,   3,   0,      0 },
    { 22, "Ti", "Titanium",          4,   4,   0,      0 },
    { 23, "V",  "Vanadium",          4,   5,   0,      0 },
    { 24, "Cr", "Chromium",          4,   6,   0,      0 },
    { 25, "Mn", "Manganese",         4,   7,   0,      0 },
    { 26, "Fe", "Iron",              4,   8,   0,      0 },
    { 27, "Co", "Cobalt",            4,   9,   0,      0 },
    { 28, "Ni", "Nickel",            4,  10,   0,      0 },
    { 29, "Cu", "Copper",            4,  11,   0,      0 },
    { 30, "Zn", "Zinc",              4,  12,   0,      0 },
    { 31, "Ga", "Gallium",           4,  13,   0,      0 },
    { 32, "Ge", "Germanium",         4,  14,   0,      0 },
    { 33, "As", "Arsenic",           4,  15,   0,      0 },
    { 34, "Se", "Selenium",          4,  16,   0,      0 },
    { 35, "Br", "Bromine",           4,  17,   0,      0 },
    { 36, "Kr", "Krypton",           4,  18,   0,      0 },

    { 37, "Rb", "Rubidium",          5,   1,   0,      0 },
    { 38, "Sr", "Strontium",         5,   2,   0,      0 },
    { 39, "Y",  "Yttrium",           5,   3,   0,      0 },
    { 40, "Zr", "Zirconium",         5,   4,   0,      0 },
    { 41, "Nb", "Niobium",           5,   5,   0,      0 },
    { 42, "Mo", "Molybdenum",        5,   6,   0,      0 },
    { 43, "Tc", "Technetium",        5,   7,   0,      0 },
    { 44, "Ru", "Ruthenium",         5,   8,   0,      0 },
    { 45, "Rh", "Rhodium",           5,   9,   0,      0 },
    { 46, "Pd", "Palladium",         5,  10,   0,      0 },
    { 47, "Ag", "Silver",            5,  11,   0,      0 },
    { 48, "Cd", "Cadmium",           5,  12,   0,      0 },
    { 49, "In", "Indium",            5,  13,   0,      0 },
    { 50, "Sn", "Tin",               5,  14,   0,      0 },
    { 51, "Sb", "Antimony",          5,  15,   0,      0 },
    { 52, "Te", "Tellurium",         5,  16,   0,      0 },
    { 53, "I",  "Iodine",            5,  17,   0,      0 },
    { 54, "Xe", "Xenon",             5,  18,   0,      0 },

    { 55, "Cs", "Caesium",           6,   1,   0,      0 },
    { 56, "Ba", "Barium",            6,   2,   0,      0 },

    // Lanthanides (separate row, under group 3)
    { 57, "La", "Lanthanum",         6,   3,   1,      0 },
    { 58, "Ce", "Cerium",            6,   0,   1,      1 },
    { 59, "Pr", "Praseodymium",      6,   0,   1,      2 },
    { 60, "Nd", "Neodymium",         6,   0,   1,      3 },
    { 61, "Pm", "Promethium",        6,   0,   1,      4 },
    { 62, "Sm", "Samarium",          6,   0,   1,      5 },
    { 63, "Eu", "Europium",          6,   0,   1,      6 },
    { 64, "Gd", "Gadolinium",        6,   0,   1,      7 },
    { 65, "Tb", "Terbium",           6,   0,   1,      8 },
    { 66, "Dy", "Dysprosium",        6,   0,   1,      9 },
    { 67, "Ho", "Holmium",           6,   0,   1,     10 },
    { 68, "Er", "Erbium",            6,   0,   1,     11 },
    { 69, "Tm", "Thulium",           6,   0,   1,     12 },
    { 70, "Yb", "Ytterbium",         6,   0,   1,     13 },
    { 71, "Lu", "Lutetium",          6,   3,   1,     14 },

    // Back to main table, period 6, groups 4..18
    { 72, "Hf", "Hafnium",           6,   4,   0,      0 },
    { 73, "Ta", "Tantalum",          6,   5,   0,      0 },
    { 74, "W",  "Tungsten",          6,   6,   0,      0 },
    { 75, "Re", "Rhenium",           6,   7,   0,      0 },
    { 76, "Os", "Osmium",            6,   8,   0,      0 },
    { 77, "Ir", "Iridium",           6,   9,   0,      0 },
    { 78, "Pt", "Platinum",          6,  10,   0,      0 },
    { 79, "Au", "Gold",              6,  11,   0,      0 },
    { 80, "Hg", "Mercury",           6,  12,   0,      0 },
    { 81, "Tl", "Thallium",          6,  13,   0,      0 },
    { 82, "Pb", "Lead",              6,  14,   0,      0 },
    { 83, "Bi", "Bismuth",           6,  15,   0,      0 },
    { 84, "Po", "Polonium",          6,  16,   0,      0 },
    { 85, "At", "Astatine",          6,  17,   0,      0 },
    { 86, "Rn", "Radon",             6,  18,   0,      0 },

    { 87, "Fr", "Francium",          7,   1,   0,      0 },
    { 88, "Ra", "Radium",            7,   2,   0,      0 },

    // Actinides (separate row, under group 3)
    { 89, "Ac", "Actinium",          7,   3,   2,      0 },
    { 90, "Th", "Thorium",           7,   0,   2,      1 },
    { 91, "Pa", "Protactinium",      7,   0,   2,      2 },
    { 92, "U",  "Uranium",           7,   0,   2,      3 },
    { 93, "Np", "Neptunium",         7,   0,   2,      4 },
    { 94, "Pu", "Plutonium",         7,   0,   2,      5 },
    { 95, "Am", "Americium",         7,   0,   2,      6 },
    { 96, "Cm", "Curium",            7,   0,   2,      7 },
    { 97, "Bk", "Berkelium",         7,   0,   2,      8 },
    { 98, "Cf", "Californium",       7,   0,   2,      9 },
    { 99, "Es", "Einsteinium",       7,   0,   2,     10 },
    {100, "Fm", "Fermium",           7,   0,   2,     11 },
    {101, "Md", "Mendelevium",       7,   0,   2,     12 },
    {102, "No", "Nobelium",          7,   0,   2,     13 },
    {103, "Lr", "Lawrencium",        7,   3,   2,     14 },

    // Back to main table, period 7, groups 4..18
    {104, "Rf", "Rutherfordium",     7,   4,   0,      0 },
    {105, "Db", "Dubnium",           7,   5,   0,      0 },
    {106, "Sg", "Seaborgium",        7,   6,   0,      0 },
    {107, "Bh", "Bohrium",           7,   7,   0,      0 },
    {108, "Hs", "Hassium",           7,   8,   0,      0 },
    {109, "Mt", "Meitnerium",        7,   9,   0,      0 },
    {110, "Ds", "Darmstadtium",      7,  10,   0,      0 },
    {111, "Rg", "Roentgenium",       7,  11,   0,      0 },
    {112, "Cn", "Copernicium",       7,  12,   0,      0 },
    {113, "Nh", "Nihonium",          7,  13,   0,      0 },
    {114, "Fl", "Flerovium",         7,  14,   0,      0 },
    {115, "Mc", "Moscovium",         7,  15,   0,      0 },
    {116, "Lv", "Livermorium",       7,  16,   0,      0 },
    {117, "Ts", "Tennessine",        7,  17,   0,      0 },
    {118, "Og", "Oganesson",         7,  18,   0,      0 }
};

const int numElements = sizeof(elements) / sizeof(elements[0]);
int selectedIndex = 0;

// ----------------- 2D table cell data -----------------
struct CellRect {
    float x, y, w, h;   // position in 2D (0..100)
    int elementIndex;
};
CellRect cellRects[200];
int cellCount = 0;

// ----------------- Electrons for 3D atom -----------------
struct Electron {
    float radius;      // orbit radius
    float angle;       // current angle in degrees
    float speed;       // degrees per frame
    float tiltX;       // tilt angles for orbit
    float tiltY;
};

const int MAX_ELECTRONS = 120;  // up to Z=118
Electron electrons[MAX_ELECTRONS];
int numElectrons = 0;

// Global rotation for whole atom
float globalRotation = 0.0f;

// ----------------- Function declarations -----------------
void init();
void setCamera3D();
void setupElectronsFromElement(const ElementInfo& e);

void drawText2D(float x, float y, const char* text, void* font);
void drawPeriodicTable();
void drawNucleus(const ElementInfo& e);
void drawOrbit(const Electron &e);
void drawElectron(const Electron &e);
void drawAtomScene();

void display();
void reshape(int w, int h);
void timer(int value);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);

// --------------------------------------------------------
// Initialization
// --------------------------------------------------------
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Lighting for 3D mode
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 50.0f, 50.0f, 50.0f, 1.0f };
    GLfloat ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);

    glClearColor(0.02f, 0.02f, 0.08f, 1.0f); // dark background

    setupElectronsFromElement(elements[selectedIndex]);
}

// Setup 3D camera
void setCamera3D() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float radY = camAngleY * PI / 180.0f;
    float radX = camAngleX * PI / 180.0f;

    float eyeX = camDist * cosf(radX) * sinf(radY);
    float eyeY = camDist * sinf(radX);
    float eyeZ = camDist * cosf(radX) * cosf(radY);

    gluLookAt(eyeX, eyeY, eyeZ,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);
}

// Build electrons from element info (automatic shells)
void setupElectronsFromElement(const ElementInfo& e) {
    numElectrons = 0;

    // Simple Bohr-style capacities for 7 shells, total ~118
    int shellCap[7] = { 2, 8, 18, 32, 32, 18, 8 };

    int remaining = e.Z;
    float baseRadius = 6.0f;      // innermost shell radius
    float radiusStep = 3.0f;      // distance between shells
    float baseSpeed  = 1.5f;      // base angular speed

    for (int shell = 0; shell < 7 && remaining > 0; ++shell) {
        int maxInShell = shellCap[shell];
        int count = (remaining > maxInShell) ? maxInShell : remaining;
        remaining -= count;

        float radius = baseRadius + shell * radiusStep;

        for (int i = 0; i < count && numElectrons < MAX_ELECTRONS; ++i) {
            Electron &el = electrons[numElectrons++];
            el.radius = radius;
            el.angle  = (360.0f * i) / count;  // evenly spaced
            el.speed  = baseSpeed + 0.15f * shell + 0.02f * i;

            // Different tilt for shells for 3D effect
            el.tiltX  = (shell % 2 == 0) ? 25.0f + 5.0f * shell : -30.0f + 5.0f * shell;
            el.tiltY  = (shell % 3 == 0) ? 0.0f  : 20.0f - 5.0f * shell;
        }
    }
}

// --------------------------------------------------------
// Drawing helpers
// --------------------------------------------------------
void drawText2D(float x, float y, const char* text, void* font) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; ++i) {
        glutBitmapCharacter(font, text[i]);
    }
}

// ---------- DDA line drawing helpers ----------
void putPixel(float x, float y) {
    glVertex2f(x, y);
}

void drawLineDDA(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;

    float steps = fabsf(dx) > fabsf(dy) ? fabsf(dx) : fabsf(dy);
    if (steps == 0) steps = 1.0f;

    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= (int)steps; ++i) {
        putPixel(x, y);
        x += xInc;
        y += yInc;
    }
    glEnd();
}

void drawRectDDA(float x, float y, float w, float h) {
    float x2 = x + w;
    float y2 = y + h;

    // bottom, top, left, right edges
    drawLineDDA(x,  y,  x2, y);
    drawLineDDA(x,  y2, x2, y2);
    drawLineDDA(x,  y,  x,  y2);
    drawLineDDA(x2, y,  x2, y2);
}

// Periodic table with real group positions, lanth/act rows separate
void drawPeriodicTable() {
    glDisable(GL_LIGHTING);  // flat 2D for UI

    cellCount = 0;

    // Background
    glColor3f(0.05f, 0.05f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(100.0f, 0.0f);
    glVertex2f(100.0f, 100.0f);
    glVertex2f(0.0f, 100.0f);
    glEnd();

    // Title
    glColor3f(1.0f, 1.0f, 0.8f);
    drawText2D(20.0f, 94.0f, "Interactive Periodic Table (118 Elements, Lanthanides & Actinides Separate)",
               GLUT_BITMAP_HELVETICA_12);

    float startX = 5.0f;
    float cellW  = 3.5f;
    float rowH   = 6.5f;

    // Main periods 1–7
    for (int i = 0; i < numElements; ++i) {
        ElementInfo &e = elements[i];

        float x, y;

        if (e.blockRow == 0) {
            // main table as group/period
            if (e.group <= 0) continue; // should not happen
            x = startX + (e.group - 1) * cellW;
            float baseY = 88.0f;
            y = baseY - (e.period - 1) * rowH;
        } else if (e.blockRow == 1) {
            // lanthanides row
            float lanthStartX = startX + (3 - 1) * cellW;
            x = lanthStartX + e.blockCol * cellW;
            y = 25.0f;
        } else { // blockRow == 2
            // actinides row
            float actStartX = startX + (3 - 1) * cellW;
            x = actStartX + e.blockCol * cellW;
            y = 18.0f;
        }

        float cellH = rowH - 1.0f;

        cellRects[cellCount].x = x;
        cellRects[cellCount].y = y;
        cellRects[cellCount].w = cellW;
        cellRects[cellCount].h = cellH;
        cellRects[cellCount].elementIndex = i;
        cellCount++;

        // Fill color
        if (i == selectedIndex)
            glColor3f(1.0f, 0.8f, 0.2f);  // selected = yellow
        else if (e.blockRow == 1 || e.blockRow == 2)
            glColor3f(0.3f, 0.5f, 0.9f);  // f-block slightly different
        else
            glColor3f(0.2f, 0.4f, 0.8f);  // normal

        glBegin(GL_QUADS);
        glVertex2f(x,         y);
        glVertex2f(x + cellW, y);
        glVertex2f(x + cellW, y + cellH);
        glVertex2f(x,         y + cellH);
        glEnd();

        // DDA border for selected element
        if (i == selectedIndex) {
            glColor3f(1.0f, 1.0f, 1.0f); // white border
            drawRectDDA(x, y, cellW, cellH);
        }

        // Symbol text
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText2D(x + 0.6f, y + 2.3f, e.symbol, GLUT_BITMAP_HELVETICA_10);
    }

    // Info for selected element
    ElementInfo &sel = elements[selectedIndex];

    char info[256];
        char groupTxt[32];
    if (sel.group > 0) {
        sprintf(groupTxt, "%d", sel.group);
    } else {
        sprintf(groupTxt, "f-block");
    }

    sprintf(info, "Selected: %s (%s), Z = %d, Period = %d, Group = %s",
            sel.name, sel.symbol, sel.Z, sel.period, groupTxt);


    glColor3f(0.8f, 1.0f, 0.8f);
    drawText2D(5.0f, 10.0f, info, GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.8f, 0.8f, 0.8f);
    drawText2D(5.0f, 5.0f,
               "LEFT/RIGHT: change element  |  Mouse click: select  |  'A': Atom View",
               GLUT_BITMAP_HELVETICA_10);

    glEnable(GL_LIGHTING);
}

// Nucleus drawing (simple cluster)
void drawNucleus(const ElementInfo& e) {
    int Z = e.Z;

    int neutrons;

    // Rough neutron/proton ratio: light ~1.0, medium ~1.3, heavy ~1.5
    if (Z == 1) {
        neutrons = 0;
    } else {
    float nOverZ;
    if (Z <= 20)       nOverZ = 1.0f;
    else if (Z <= 40)  nOverZ = 1.2f;
    else if (Z <= 82)  nOverZ = 1.4f;
    else               nOverZ = 1.5f;

    neutrons = (int)(nOverZ * Z);
    }
    int totalNucleons = Z + neutrons;

    // We don't want to draw 200 tiny balls for heavy elements,
    // so we cap the number and scale proportionally.
    const int MAX_NUCLEONS_DRAW = 60;
    int drawProtons  = Z;
    int drawNeutrons = neutrons;

    if (totalNucleons > MAX_NUCLEONS_DRAW) {
        float factor = (float)MAX_NUCLEONS_DRAW / (float)totalNucleons;
        drawProtons  = (int)(Z * factor);
        drawNeutrons = MAX_NUCLEONS_DRAW - drawProtons;
    }

    // Overall cluster radius (slightly grows with Z)
    float clusterRadius = 3.0f + 0.01f * Z;
    float sphereRadius  = 0.4f;  // radius of each proton/neutron sphere

    // Make the random pattern deterministic per element:
    // same element => same nucleus layout every time.
    srand(e.Z);

    // --- Draw protons (red) ---
    glColor3f(1.0f, 0.2f, 0.2f);
    for (int i = 0; i < drawProtons; ++i) {
        float u = (float)rand() / (float)RAND_MAX;
        float v = (float)rand() / (float)RAND_MAX;
        float w = (float)rand() / (float)RAND_MAX;

        float theta = 2.0f * PI * u;
        float phi   = acosf(2.0f * v - 1.0f);
        float r     = cbrtf(w);  // cube root for uniform distribution in volume

        float x = r * sinf(phi) * cosf(theta);
        float y = r * sinf(phi) * sinf(theta);
        float z = r * cosf(phi);

        x *= clusterRadius;
        y *= clusterRadius;
        z *= clusterRadius;

        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(sphereRadius, 16, 16);
        glPopMatrix();
    }

    // --- Draw neutrons (blue) ---
    glColor3f(0.2f, 0.4f, 1.0f);
    for (int i = 0; i < drawNeutrons; ++i) {
        float u = (float)rand() / (float)RAND_MAX;
        float v = (float)rand() / (float)RAND_MAX;
        float w = (float)rand() / (float)RAND_MAX;

        float theta = 2.0f * PI * u;
        float phi   = acosf(2.0f * v - 1.0f);
        float r     = cbrtf(w);

        float x = r * sinf(phi) * cosf(theta);
        float y = r * sinf(phi) * sinf(theta);
        float z = r * cosf(phi);

        x *= clusterRadius;
        y *= clusterRadius;
        z *= clusterRadius;

        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(sphereRadius, 16, 16);
        glPopMatrix();
    }
}


void drawOrbit(const Electron &e) {
    glDisable(GL_LIGHTING);
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glRotatef(e.tiltX, 1.0f, 0.0f, 0.0f);
    glRotatef(e.tiltY, 0.0f, 1.0f, 0.0f);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; ++i) {
        float ang = 2.0f * PI * i / 100.0f;
        float x = e.radius * cosf(ang);
        float z = e.radius * sinf(ang);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawElectron(const Electron &e) {
    glColor3f(1.0f, 0.9f, 0.2f); // yellow-ish

    glPushMatrix();
    glRotatef(e.tiltX, 1.0f, 0.0f, 0.0f);
    glRotatef(e.tiltY, 0.0f, 1.0f, 0.0f);

    glRotatef(e.angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(e.radius, 0.0f, 0.0f);

    glutSolidSphere(1.0f, 20, 20);
    glPopMatrix();
}

void drawAtomScene() {
    setCamera3D();

    globalRotation += 0.02f;
    glRotatef(globalRotation, 0.0f, 1.0f, 0.0f);

    ElementInfo &sel = elements[selectedIndex];

    // Draw nucleus based on this element
    drawNucleus(sel);

    // Draw orbits & electrons
    for (int i = 0; i < numElectrons; ++i) {
        drawOrbit(electrons[i]);
        drawElectron(electrons[i]);
    }

    // ---- 2D overlay using the SAME 'sel' ----
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    char groupTxt[32];
    if (sel.group > 0) {
        sprintf(groupTxt, "%d", sel.group);
    } else {
        sprintf(groupTxt, "f-block");
    }

    char info[256];
    sprintf(info, "%s (%s), Z = %d, Period = %d, Group = %s",
            sel.name, sel.symbol, sel.Z, sel.period, groupTxt);

    glColor3f(0.9f, 1.0f, 0.9f);
    drawText2D(5.0f, 95.0f, info, GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.8f, 0.8f, 0.8f);
    drawText2D(5.0f, 90.0f,
               "Arrow keys = rotate  |  +/- = zoom  |  SPACE = pause  |  'T' = Table View",
               GLUT_BITMAP_HELVETICA_10);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
}


// --------------------------------------------------------
// GLUT callbacks
// --------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;

    if (currentMode == MODE_TABLE) {
        // ---------- 2D VIEW FOR PERIODIC TABLE ----------
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 100, 0, 100);   // our 0..100 coordinate system

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawPeriodicTable();
    } else {
        // ---------- 3D VIEW FOR ATOM ----------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, aspect, 1.0, 200.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawAtomScene();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

// Timer callback – animate electrons
void timer(int value) {
    if (!isPaused && currentMode == MODE_ATOM) {
        for (int i = 0; i < numElectrons; ++i) {
            electrons[i].angle += electrons[i].speed;
            if (electrons[i].angle > 360.0f)
                electrons[i].angle -= 360.0f;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);  // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC
            std::exit(0);
            break;
        case ' ':
            isPaused = !isPaused;
            break;
        case '+':
            camDist -= 1.0f;
            if (camDist < 10.0f) camDist = 10.0f;
            break;
        case '-':
            camDist += 1.0f;
            if (camDist > 120.0f) camDist = 120.0f;
            break;
        case 't':
        case 'T':
            currentMode = MODE_TABLE;
            break;
        case 'a':
        case 'A':
            currentMode = MODE_ATOM;
            setupElectronsFromElement(elements[selectedIndex]);
            break;
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (currentMode == MODE_TABLE) {
        // Change selected element by index
        if (key == GLUT_KEY_LEFT) {
            selectedIndex--;
            if (selectedIndex < 0) selectedIndex = numElements - 1;
        }
        else if (key == GLUT_KEY_RIGHT) {
            selectedIndex++;
            if (selectedIndex >= numElements) selectedIndex = 0;
        }
    } else {
        // Atom view: rotate camera
        switch (key) {
            case GLUT_KEY_LEFT:
                camAngleY -= 3.0f;
                break;
            case GLUT_KEY_RIGHT:
                camAngleY += 3.0f;
                break;
            case GLUT_KEY_UP:
                camAngleX += 3.0f;
                if (camAngleX > 89.0f) camAngleX = 89.0f;
                break;
            case GLUT_KEY_DOWN:
                camAngleX -= 3.0f;
                if (camAngleX < -89.0f) camAngleX = -89.0f;
                break;
        }
    }
    glutPostRedisplay();
}

// Mouse click to select element on table
void mouse(int button, int state, int x, int y) {
    if (currentMode != MODE_TABLE) return;
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

    int width  = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    // Convert window coords -> [0,100] space
    float fx = (float)x * 100.0f / width;
    float fy = (float)(height - y) * 100.0f / height;

    for (int i = 0; i < cellCount; ++i) {
        CellRect &c = cellRects[i];
        if (fx >= c.x && fx <= c.x + c.w &&
            fy >= c.y && fy <= c.y + c.h) {
            selectedIndex = c.elementIndex;
            break;
        }
    }

    glutPostRedisplay();
}

// --------------------------------------------------------
// Main
// --------------------------------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1100, 720);
    glutCreateWindow("Interactive 3D Atom + Full Periodic Table (Lanthanides & Actinides Separate)");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
