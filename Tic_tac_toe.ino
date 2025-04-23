#include "U8glib.h"

// OLED Display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);

// Spielfeld und Variablen
char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
int cursorX = 0, cursorY = 0;
bool isPlayer1 = true; // Spieler 1 beginnt (X)
bool gameRunning = false;

void u8g_prepare() {
    u8g.setFont(u8g_font_6x10);  // Schriftart vorbereiten
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
}

// **Spielfeld zurücksetzen**
void resetBoard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' '; // Alle Felder leeren
        }
    }
}

// **Hauptmenü anzeigen**
void drawMenu() {
    u8g_prepare();
    u8g.drawStr(20, 20, "Tic Tac Toe");           // Titel
    u8g.drawStr(10, 40, "Press Button to Start"); // Anweisung
}

// **Spielfeld zeichnen**
void drawBoard() {
    u8g_prepare();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int x = j * 40;
            int y = i * 20;

            // Kästchen zeichnen
            u8g.drawFrame(x, y, 40, 20);

            // X oder O zentriert zeichnen
            if (board[i][j] != ' ') {
                char symbol[2] = {board[i][j], '\0'}; // Zeichen konvertieren
                u8g.drawStr(x + 15, y + 7, symbol);  // Zentrierung
            }
        }
    }

    // Cursor anzeigen
    int cursorXPos = cursorX * 40;
    int cursorYPos = cursorY * 20;
    u8g.drawFrame(cursorXPos + 1, cursorYPos + 1, 38, 18);
}

// **Siegprüfung**
bool checkWin(char player) {
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) || // Horizontal
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) { // Vertikal
            return true;
        }
    }
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) || // Diagonal
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return true;
    }
    return false;
}

// **Draw-Logik: Prüfen, ob alle Felder gefüllt sind**
bool checkDraw() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return false; // Es gibt noch freie Felder
            }
        }
    }
    return true; // Alle Felder sind gefüllt
}

// **Spielende anzeigen**
void drawGameOver(char winner) {
    u8g_prepare();
    if (winner == ' ') {
        u8g.drawStr(20, 20, "It's a Draw!");
    } else {
        u8g.drawStr(20, 20, "Player ");
        u8g.drawStr(60, 20, winner == 'X' ? "1 Wins!" : "2 Wins!");
    }
}

// **Setup-Funktion**
void setup() {
    pinMode(A0, INPUT);      // Joystick X
    pinMode(A1, INPUT);      // Joystick Y
    pinMode(2, INPUT_PULLUP);// Joystick-Taster
}

// **Loop-Funktion**
void loop() {
    if (!gameRunning) {
        // Hauptmenü anzeigen
        u8g.firstPage();
        do {
            drawMenu();
        } while (u8g.nextPage());

        // Spiel starten, wenn der Button gedrückt wird
        if (digitalRead(2) == LOW) {
            resetBoard();    // Spielfeld zurücksetzen
            gameRunning = true;
            delay(500); // Entprellung
        }
        return;
    }

    // Joystick-Eingaben
    int x = analogRead(A0);
    int y = analogRead(A1);
    int button = digitalRead(2);

    // Cursor-Bewegung
    if (x < 400) cursorX = max(0, cursorX - 1);
    if (x > 600) cursorX = min(2, cursorX + 1);
    if (y < 400) cursorY = max(0, cursorY - 1);
    if (y > 600) cursorY = min(2, cursorY + 1);

    // Symbol platzieren
    if (button == LOW) { 
        if (board[cursorY][cursorX] == ' ') { // Wenn das Feld leer ist
            board[cursorY][cursorX] = isPlayer1 ? 'X' : 'O'; // X oder O platzieren

            if (checkWin(isPlayer1 ? 'X' : 'O')) {
                gameRunning = false; // Spielende bei Sieg
                delay(500);
                u8g.firstPage(); 
                do {
                    drawGameOver(isPlayer1 ? 'X' : 'O');
                } while (u8g.nextPage());
                delay(2000); // Verzögerung für Anzeige des Gewinner-Bildschirms
                return;
            }

            if (checkDraw()) {
                gameRunning = false; // Spielende bei Unentschieden
                delay(500);
                u8g.firstPage();
                do {
                    drawGameOver(' '); // ' ' zeigt ein Draw an
                } while (u8g.nextPage());
                delay(2000); // Verzögerung für Anzeige des Unentschieden-Bildschirms
                return;
            }

            isPlayer1 = !isPlayer1; // Spielerwechsel
        }
        delay(500); // Entprellung
    }

    // Spielfeld zeichnen
    u8g.firstPage();
    do {
        drawBoard();
    } while (u8g.nextPage());
}
