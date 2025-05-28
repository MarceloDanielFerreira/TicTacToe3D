#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <tuple>

#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846 // Definir M_PI si no está definido
#endif

// Configuración del juego
const int BOARD_SIZE = 3;
const float SPACING = 2.0f;

// Variables de estado del juego
int board[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE] = { 0 }; // 0=vacío, 1=jugador1, 2=jugador2
int currentPlayer = 1;
bool gameOver = false;
int winner = 0;
bool isDraw = false;  // Nueva variable para detectar empates

// Variables de cámara
float cameraAngleX = 45.0f;
float cameraAngleY = 45.0f;
float cameraDistance = 15.0f;

// Variables de audio
bool soundEnabled = true;
bool musicEnabled = true;

// Variables de animación
float winningAnimationTime = 0.0f;
bool isAnimating = false;
std::vector<std::tuple<int, int, int>> winningPositions;

// Prototipos de funciones
void init();
void display();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void drawBoard();
void drawCube(float x, float y, float z, bool isAxis);
void drawSphere(float x, float y, float z, int player);
bool checkWin();
void highlightWinningLine();
void drawText(const char* text, float x, float y);
void createSimpleTextures();
void playSound(const char* soundFile);
void updateAnimation(int value);
void drawWinningAnimation();
bool checkDraw();

// Texturas simples generadas proceduralmente
GLuint cubeTexture, sphereTexturePlayer1, sphereTexturePlayer2;

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ta-Te-Ti 3D");

    init();
    createSimpleTextures();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutTimerFunc(0, updateAnimation, 0);

    glutMainLoop();
    return 0;
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);

    // Configuración básica de luz
    GLfloat lightPos[4] = { 10.0f, 10.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    // Iniciar música de fondo
    if (musicEnabled) {
        PlaySound(TEXT("background.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
}

void createSimpleTextures() {
    // Crear texturas simples proceduralmente

    // Textura para los cubos (patrón de madera simulado)
    unsigned char cubeTexData[64 * 64 * 3];
    for (int i = 0; i < 64 * 64; i++) {
        int x = i % 64;
        int y = i / 64;
        float value = (sin(x * 0.3f) * cos(y * 0.1f) + 1.0f) * 0.4f + 0.2f;
        cubeTexData[i * 3 + 0] = static_cast<unsigned char>(value * 180); // R
        cubeTexData[i * 3 + 1] = static_cast<unsigned char>(value * 120); // G
        cubeTexData[i * 3 + 2] = static_cast<unsigned char>(value * 60);  // B
    }

    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeTexData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Textura para jugador 1 (rojo con patrón)
    unsigned char player1TexData[64 * 64 * 3];
    for (int i = 0; i < 64 * 64; i++) {
        int x = i % 64;
        int y = i / 64;
        float value = (sin(x * 0.2f) * sin(y * 0.2f) + 1.0f) * 0.5f;
        player1TexData[i * 3 + 0] = static_cast<unsigned char>(value * 255); // R
        player1TexData[i * 3 + 1] = static_cast<unsigned char>(value * 50);  // G
        player1TexData[i * 3 + 2] = static_cast<unsigned char>(value * 50);  // B
    }

    glGenTextures(1, &sphereTexturePlayer1);
    glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, player1TexData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Textura para jugador 2 (azul con patrón)
    unsigned char player2TexData[64 * 64 * 3];
    for (int i = 0; i < 64 * 64; i++) {
        int x = i % 64;
        int y = i / 64;
        float value = (cos(x * 0.15f) * sin(y * 0.15f) + 1.0f) * 0.5f;
        player2TexData[i * 3 + 0] = static_cast<unsigned char>(value * 50);  // R
        player2TexData[i * 3 + 1] = static_cast<unsigned char>(value * 50);  // G
        player2TexData[i * 3 + 2] = static_cast<unsigned char>(value * 255); // B
    }

    glGenTextures(1, &sphereTexturePlayer2);
    glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, player2TexData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posicionar la cámara
    gluLookAt(
        cameraDistance * sin(cameraAngleY * M_PI / 180) * cos(cameraAngleX * M_PI / 180),
        cameraDistance * sin(cameraAngleX * M_PI / 180),
        cameraDistance * cos(cameraAngleY * M_PI / 180) * cos(cameraAngleX * M_PI / 180),
        0.0, 0.0, 0.0,  // Punto al que mira la cámara (centro de la escena)
        0.0, 1.0, 0.0   // Vector up (orientación vertical)
    );

    drawBoard();

    // Mostrar información del jugador actual
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    char playerText[50];
    if (gameOver) {
        if (isDraw) {
            sprintf(playerText, "Empate!");
        }
        else {
            sprintf(playerText, "Jugador %d gana!", winner);
        }
    }
    else {
        sprintf(playerText, "Turno: Jugador %d", currentPlayer);
    }
    drawText(playerText, 10, 580);

    // Agregar instrucciones
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("Controles:", 10, 550);
    drawText("Click: Seleccionar", 10, 530);
    drawText("Flechas: Rotar camara", 10, 510);
    drawText("PgUp/PgDn: Zoom", 10, 490);
    drawText("R: Reiniciar", 10, 470);
    drawText("Q: Salir", 10, 450);

    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    drawWinningAnimation();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !gameOver) {
        // Implementar selección de objetos con picking
        // (código simplificado para el ejemplo)

        // Convertir coordenadas de pantalla a coordenadas del mundo 3D
        GLint viewport[4];
        GLdouble modelview[16], projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;

        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        winX = (float)x;
        winY = (float)viewport[3] - (float)y;
        glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        // Determinar qué cubo o esfera fue seleccionada
        int selectedX = round(posX / SPACING + 1);
        int selectedY = round(posY / SPACING + 1);
        int selectedZ = round(posZ / SPACING + 1);

        // Verificar límites y si la posición está vacía
        if (selectedX >= 0 && selectedX < BOARD_SIZE &&
            selectedY >= 0 && selectedY < BOARD_SIZE &&
            selectedZ >= 0 && selectedZ < BOARD_SIZE &&
            board[selectedX][selectedY][selectedZ] == 0) {

            board[selectedX][selectedY][selectedZ] = currentPlayer;
            playSound("click.wav");

            if (checkWin()) {
                gameOver = true;
                winner = currentPlayer;
                isAnimating = true;
                winningAnimationTime = 0.0f;
                playSound("win.wav");
            }
            else if (checkDraw()) {
                gameOver = true;
                isDraw = true;
                playSound("draw.wav");
            }
            else {
                currentPlayer = (currentPlayer == 1) ? 2 : 1;
                playSound("turn.wav");
            }
        }

        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
    case 'Q':
        exit(0);
        break;
    case 'r':
    case 'R':
        // Reiniciar el juego
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                for (int k = 0; k < BOARD_SIZE; k++) {
                    board[i][j][k] = 0;
                }
            }
        }
        currentPlayer = 1;
        gameOver = false;
        winner = 0;
        glutPostRedisplay();
        break;
    }
}

void specialKeyboard(int key, int x, int y) {
    const float ROTATION_SPEED = 5.0f;
    const float ZOOM_SPEED = 0.5f;
    
    switch (key) {
        case GLUT_KEY_LEFT:
            cameraAngleY -= ROTATION_SPEED;
            break;
        case GLUT_KEY_RIGHT:
            cameraAngleY += ROTATION_SPEED;
            break;
        case GLUT_KEY_UP:
            cameraAngleX += ROTATION_SPEED;
            break;
        case GLUT_KEY_DOWN:
            cameraAngleX -= ROTATION_SPEED;
            break;
        case GLUT_KEY_PAGE_UP:
            cameraDistance -= ZOOM_SPEED;
            if (cameraDistance < 5.0f) cameraDistance = 5.0f;
            break;
        case GLUT_KEY_PAGE_DOWN:
            cameraDistance += ZOOM_SPEED;
            if (cameraDistance > 30.0f) cameraDistance = 30.0f;
            break;
    }
    
    // Limitar ángulos de cámara
    if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
    if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;
    
    glutPostRedisplay();
}

void drawBoard() {
    // Dibujar ejes principales (cubos)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                float x = i * SPACING - SPACING;
                float y = j * SPACING - SPACING;
                float z = k * SPACING - SPACING;
                
                // Determinar si es un eje principal
                bool isAxis = (i == 1 && j == 1) || (i == 1 && k == 1) || (j == 1 && k == 1);
                
                if (board[i][j][k] == 0) {
                    drawCube(x, y, z, isAxis);
                } else {
                    drawSphere(x, y, z, board[i][j][k]);
                }
            }
        }
    }

    // Resaltar la línea ganadora si hay un ganador
    if (gameOver && winner != 0) {
        highlightWinningLine();
    }
}

void drawCube(float x, float y, float z, bool isAxis) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Aplicar textura
    glEnable(GL_TEXTURE_2D);
    if (isAxis) {
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glColor3f(0.8f, 0.6f, 0.2f); // Color amarillo oscuro para ejes
    } else {
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glColor3f(0.7f, 0.7f, 0.7f); // Color gris para cubos normales
    }

    // Dibujar cubo con coordenadas de textura
    glBegin(GL_QUADS);
    // Frente
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    
    // Atrás
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    
    // Arriba
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    
    // Abajo
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    
    // Derecha
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    
    // Izquierda
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawSphere(float x, float y, float z, int player) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Aplicar textura
    glEnable(GL_TEXTURE_2D);
    if (player == 1) {
        glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer1);
        glColor3f(1.0f, 0.3f, 0.3f); // Rojo para jugador 1
    } else {
        glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer2);
        glColor3f(0.3f, 0.3f, 1.0f); // Azul para jugador 2
    }

    // Crear esfera con coordenadas de textura
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluSphere(quadric, 0.4f, 32, 32);
    gluDeleteQuadric(quadric);
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

bool checkWin() {
    // Verificar todas las posibles líneas ganadoras en 3D
    winningPositions.clear();  // Limpiar posiciones ganadoras anteriores

    // Verificar líneas en el eje X
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int z = 0; z < BOARD_SIZE; z++) {
            if (board[0][y][z] != 0 &&
                board[0][y][z] == board[1][y][z] &&
                board[1][y][z] == board[2][y][z]) {
                // Guardar la línea ganadora
                for (int x = 0; x < BOARD_SIZE; x++) {
                    winningPositions.push_back(std::make_tuple(x, y, z));
                }
                return true;
            }
        }
    }

    // Verificar líneas en el eje Y
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int z = 0; z < BOARD_SIZE; z++) {
            if (board[x][0][z] != 0 &&
                board[x][0][z] == board[x][1][z] &&
                board[x][1][z] == board[x][2][z]) {
                // Guardar la línea ganadora
                for (int y = 0; y < BOARD_SIZE; y++) {
                    winningPositions.push_back(std::make_tuple(x, y, z));
                }
                return true;
            }
        }
    }

    // Verificar líneas en el eje Z
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (board[x][y][0] != 0 &&
                board[x][y][0] == board[x][y][1] &&
                board[x][y][1] == board[x][y][2]) {
                // Guardar la línea ganadora
                for (int z = 0; z < BOARD_SIZE; z++) {
                    winningPositions.push_back(std::make_tuple(x, y, z));
                }
                return true;
            }
        }
    }

    // Verificar diagonales en los planos XY
    for (int z = 0; z < BOARD_SIZE; z++) {
        if (board[0][0][z] != 0 &&
            board[0][0][z] == board[1][1][z] &&
            board[1][1][z] == board[2][2][z]) {
            // Guardar la línea ganadora
            for (int x = 0; x < BOARD_SIZE; x++) {
                winningPositions.push_back(std::make_tuple(x, x, z));
            }
            return true;
        }
        if (board[2][0][z] != 0 &&
            board[2][0][z] == board[1][1][z] &&
            board[1][1][z] == board[0][2][z]) {
            // Guardar la línea ganadora
            for (int x = 0; x < BOARD_SIZE; x++) {
                winningPositions.push_back(std::make_tuple(x, BOARD_SIZE - 1 - x, z));
                #include <tuple> // Agregar esta línea al inicio del archivo para incluir std::make_tuple
            }
            return true;
        }
    }

    // Verificar diagonales en los planos XZ
    for (int y = 0; y < BOARD_SIZE; y++) {
        if (board[0][y][0] != 0 &&
            board[0][y][0] == board[1][y][1] &&
            board[1][y][1] == board[2][y][2]) {
            // Guardar la línea ganadora
            for (int x = 0; x < BOARD_SIZE; x++) {
                winningPositions.push_back(std::make_tuple(x, y, x));
            }
            return true;
        }
        if (board[2][y][0] != 0 &&
            board[2][y][0] == board[1][y][1] &&
            board[1][y][1] == board[0][y][2]) {
            // Guardar la línea ganadora
            for (int x = 0; x < BOARD_SIZE; x++) {
                winningPositions.push_back(std::make_tuple(x, y, BOARD_SIZE - 1 - x));
            }
            return true;
        }
    }

    // Verificar diagonales en los planos YZ
    for (int x = 0; x < BOARD_SIZE; x++) {
        if (board[x][0][0] != 0 &&
            board[x][0][0] == board[x][1][1] &&
            board[x][1][1] == board[x][2][2]) {
            // Guardar la línea ganadora
            for (int y = 0; y < BOARD_SIZE; y++) {
                winningPositions.push_back(std::make_tuple(x, y, y));
            }
            return true;
        }
        if (board[x][2][0] != 0 &&
            board[x][2][0] == board[x][1][1] &&
            board[x][1][1] == board[x][0][2]) {
            // Guardar la línea ganadora
            for (int y = 0; y < BOARD_SIZE; y++) {
                winningPositions.push_back(std::make_tuple(x, BOARD_SIZE - 1 - y, y));
            }
            return true;
        }
    }

    // Verificar diagonales espaciales (de esquina a esquina)
    if (board[0][0][0] != 0 &&
        board[0][0][0] == board[1][1][1] &&
        board[1][1][1] == board[2][2][2]) {
        winningPositions.push_back(std::make_tuple(0, 0, 0));
        winningPositions.push_back(std::make_tuple(1, 1, 1));
        winningPositions.push_back(std::make_tuple(2, 2, 2));
        return true;
    }
    if (board[2][0][0] != 0 &&
        board[2][0][0] == board[1][1][1] &&
        board[1][1][1] == board[0][2][2]) {
        winningPositions.push_back(std::make_tuple(2, 0, 0));
        winningPositions.push_back(std::make_tuple(1, 1, 1));
        winningPositions.push_back(std::make_tuple(0, 2, 2));
        return true;
    }
    if (board[0][2][0] != 0 &&
        board[0][2][0] == board[1][1][1] &&
        board[1][1][1] == board[2][0][2]) {
        winningPositions.push_back(std::make_tuple(0, 2, 0));
        winningPositions.push_back(std::make_tuple(1, 1, 1));
        winningPositions.push_back(std::make_tuple(2, 0, 2));
        return true;
    }
    if (board[2][2][0] != 0 &&
        board[2][2][0] == board[1][1][1] &&
        board[1][1][1] == board[0][0][2]) {
        winningPositions.push_back(std::make_tuple(2, 2, 0));
        winningPositions.push_back(std::make_tuple(1, 1, 1));
        winningPositions.push_back(std::make_tuple(0, 0, 2));
        return true;
    }

    return false;
}

bool checkDraw() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                if (board[i][j][k] == 0) {
                    return false;  // Todavía hay espacios vacíos
                }
            }
        }
    }
    return true;  // No hay espacios vacíos
}

void highlightWinningLine() {
    // Implementar para resaltar la línea ganadora
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f); // Amarillo brillante
    glLineWidth(5.0f);

    // Aquí se debería determinar qué línea específica es la ganadora
    // y dibujar una línea que la resalte

    glEnable(GL_LIGHTING);
}

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void playSound(const char* soundFile) {
    if (soundEnabled) {
        std::string fullPath = "sounds/" + std::string(soundFile);
        std::wstring widePath(fullPath.begin(), fullPath.end());
        PlaySound(widePath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    }
}

void updateAnimation(int value) {
    if (isAnimating) {
        winningAnimationTime += 0.1f;
        if (winningAnimationTime > 2.0f) {
            isAnimating = false;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(16, updateAnimation, 0);
}

void drawWinningAnimation() {
    if (!isAnimating) return;

    float scale = 1.0f + 0.2f * sin(winningAnimationTime * 5.0f);
    float rotation = winningAnimationTime * 180.0f;
    float pulse = 0.5f + 0.5f * sin(winningAnimationTime * 10.0f);

    // Dibujar línea conectora
    if (!winningPositions.empty()) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.0f);
        glLineWidth(5.0f);
        glBegin(GL_LINE_STRIP);
        for (const auto& pos : winningPositions) {
            int x, y, z;
            std::tie(x, y, z) = pos;
            glVertex3f(x * SPACING - SPACING, y * SPACING - SPACING, z * SPACING - SPACING);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    // Animar las piezas ganadoras
    for (const auto& pos : winningPositions) {
        int x, y, z;
        std::tie(x, y, z) = pos;
        
        glPushMatrix();
        glTranslatef(x * SPACING - SPACING, y * SPACING - SPACING, z * SPACING - SPACING);
        glRotatef(rotation, 0.0f, 1.0f, 0.0f);
        glScalef(scale, scale, scale);
        
        // Efecto de brillo
        glColor3f(pulse, pulse, pulse);
        
        if (board[x][y][z] == 1) {
            drawSphere(0, 0, 0, 1);
        } else {
            drawSphere(0, 0, 0, 2);
        }
        
        glPopMatrix();
    }
}