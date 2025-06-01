#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <tuple>

#include <cmath>
#include <windows.h>
#ifndef M_PI  
constexpr double M_PI = 3.14159265358979323846;  
#endif
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846 // Definir M_PI si no está definido
#endif

// Configuración del juego
constexpr int BOARD_SIZE = 3;
constexpr float SPACING = 2.0f;

// Variables de estado del juego
int tablero[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE] = { 0 }; // 0=vacío, 1=jugador1, 2=jugador2
int jugadorActual = 1;
bool juegoTerminado = false;
int ganador = 0;
bool esEmpate = false;
bool texturasHabilitadas = true;

// Variables de cámara
float anguloCamaraX = 45.0f;
float anguloCamaraY = 45.0f;
float distanciaCamara = 15.0f;

// Variables de audio
bool musicaHabilitada = true;
bool musicaReproduciendo = false;

// Variables de animación
float tiempoAnimacionGanadora = 0.0f;
bool estaAnimando = false;
std::vector<std::tuple<int, int, int>> posicionesGanadoras;

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
    glEnable(GL_NORMALIZE);  // Para mejor iluminación

    // Configuración mejorada de luz
    const GLfloat lightPos[4] = { 10.0f, 10.0f, 10.0f, 1.0f };
    const GLfloat lightAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };  // Luz ambiental
    const GLfloat lightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };  // Luz difusa
    const GLfloat lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Luz especular

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Configuración de materiales
    const GLfloat matSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    constexpr GLfloat matShininess = 50.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    // Iniciar música de fondo
    if (musicaHabilitada) {
        std::cout << "Intentando iniciar música de fondo..." << std::endl;
        playSound("background.wav");
    }
}

void createSimpleTextures() {
    // Crear texturas mejoradas proceduralmente

    // Textura para los cubos (patrón de madera mejorado)
    unsigned char cubeTexData[128 * 128 * 3];
    for (int i = 0; i < 128 * 128; i++) {
        int x = i % 128;
        int y = i / 128;
        const float value = (sin(x * 0.2f) * cos(y * 0.1f) + 1.0f) * 0.4f + 0.2f;
        // Añadir variación de color para simular vetas de madera
        float woodGrain = sin(x * 0.1f + y * 0.05f) * 0.1f;
        cubeTexData[i * 3 + 0] = static_cast<unsigned char>((value + woodGrain) * 180); // R
        cubeTexData[i * 3 + 1] = static_cast<unsigned char>((value - woodGrain) * 120); // G
        cubeTexData[i * 3 + 2] = static_cast<unsigned char>((value + woodGrain) * 60);  // B
    }

    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeTexData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, cubeTexData);

    // Textura para jugador 1 (rojo con patrón mejorado)
    unsigned char player1TexData[128 * 128 * 3];
    for (int i = 0; i < 128 * 128; i++) {
        int x = i % 128;
        int y = i / 128;
        float value = (sin(x * 0.15f) * sin(y * 0.15f) + 1.0f) * 0.5f;
        // Añadir variación de color para más profundidad
        float depth = sin(x * 0.1f + y * 0.1f) * 0.2f;
        player1TexData[i * 3 + 0] = static_cast<unsigned char>((value + depth) * 255); // R
        player1TexData[i * 3 + 1] = static_cast<unsigned char>((value - depth) * 50);  // G
        player1TexData[i * 3 + 2] = static_cast<unsigned char>((value - depth) * 50);  // B
    }

    glGenTextures(1, &sphereTexturePlayer1);
    glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, player1TexData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, player1TexData);

    // Textura para jugador 2 (azul con patrón mejorado)
    unsigned char player2TexData[128 * 128 * 3];
    for (int i = 0; i < 128 * 128; i++) {
        int x = i % 128;
        int y = i / 128;
        float value = (cos(x * 0.15f) * sin(y * 0.15f) + 1.0f) * 0.5f;
        // Añadir variación de color para más profundidad
        float depth = sin(x * 0.1f + y * 0.1f) * 0.2f;
        player2TexData[i * 3 + 0] = static_cast<unsigned char>((value - depth) * 50);  // R
        player2TexData[i * 3 + 1] = static_cast<unsigned char>((value - depth) * 50);  // G
        player2TexData[i * 3 + 2] = static_cast<unsigned char>((value + depth) * 255); // B
    }

    glGenTextures(1, &sphereTexturePlayer2);
    glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, player2TexData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, player2TexData);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posicionar la cámara
    gluLookAt(
        distanciaCamara * sin(anguloCamaraY * M_PI / 180) * cos(anguloCamaraX * M_PI / 180),
        distanciaCamara * sin(anguloCamaraX * M_PI / 180),
        distanciaCamara * cos(anguloCamaraY * M_PI / 180) * cos(anguloCamaraX * M_PI / 180),
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
    if (juegoTerminado) {
        if (esEmpate) {
            sprintf(playerText, "Empate!");
        }
        else {
            sprintf(playerText, "Jugador %d gana!", ganador);
        }
    }
    else {
        sprintf(playerText, "Turno: Jugador %d", jugadorActual);
    }
    drawText(playerText, 10, 580);

    // Agregar instrucciones
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("Controles:", 10, 550);
    drawText("Click: Seleccionar", 10, 530);
    drawText("Flechas: Rotar camara", 10, 510);
    drawText("+/-: Zoom", 10, 490);
    drawText("Espacio: Texturas On/Off", 10, 470);
    drawText("R: Reiniciar", 10, 450);
    drawText("M: Musica On/Off", 10, 430);
    drawText("Q: Salir", 10, 410);

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
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !juegoTerminado) {
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
            tablero[selectedX][selectedY][selectedZ] == 0) {

            tablero[selectedX][selectedY][selectedZ] = jugadorActual;
            
            if (checkWin()) {
                juegoTerminado = true;
                ganador = jugadorActual;
                estaAnimando = true;
                tiempoAnimacionGanadora = 0.0f;
            }
            else if (checkDraw()) {
                juegoTerminado = true;
                esEmpate = true;
            }
            else {
                jugadorActual = (jugadorActual == 1) ? 2 : 1;
            }
        }

        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    const float ZOOM_SPEED = 0.5f;
    
    switch (key) {
    case 'q':
    case 'Q':
        // Detener la música antes de salir
        if (musicaReproduciendo) {
            PlaySound(NULL, NULL, 0);
        }
        exit(0);
        break;
    case 'r':
    case 'R':
        // Reiniciar el juego
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                for (int k = 0; k < BOARD_SIZE; k++) {
                    tablero[i][j][k] = 0;
                }
            }
        }
        jugadorActual = 1;
        juegoTerminado = false;
        ganador = 0;
        esEmpate = false;
        glutPostRedisplay();
        break;
    case 'm':
    case 'M':
        // Toggle música
        musicaHabilitada = !musicaHabilitada;
        if (musicaHabilitada) {
            musicaReproduciendo = false; // Resetear el estado para permitir que la música inicie
            playSound("background.wav");
        } else {
            PlaySound(NULL, NULL, 0);
            musicaReproduciendo = false;
        }
        break;
    case '+':
    case '=':  // También funciona con la tecla = (que es la misma que + sin shift)
        // Acercar
        distanciaCamara -= ZOOM_SPEED;
        if (distanciaCamara < 5.0f) distanciaCamara = 5.0f;
        glutPostRedisplay();
        break;
    case '-':
    case '_':  // También funciona con la tecla - (que es la misma que _ sin shift)
        // Alejar
        distanciaCamara += ZOOM_SPEED;
        if (distanciaCamara > 30.0f) distanciaCamara = 30.0f;
        glutPostRedisplay();
        break;
    case ' ':  // Barra espaciadora
        // Toggle texturas
        texturasHabilitadas = !texturasHabilitadas;
        glutPostRedisplay();
        break;
    }
}

void specialKeyboard(int key, int x, int y) {
    const float ROTATION_SPEED = 5.0f;
    
    switch (key) {
        case GLUT_KEY_LEFT:
            anguloCamaraY -= ROTATION_SPEED;
            break;
        case GLUT_KEY_RIGHT:
            anguloCamaraY += ROTATION_SPEED;
            break;
        case GLUT_KEY_UP:
            anguloCamaraX += ROTATION_SPEED;
            break;
        case GLUT_KEY_DOWN:
            anguloCamaraX -= ROTATION_SPEED;
            break;
    }
    
    // Limitar ángulos de cámara
    if (anguloCamaraX > 89.0f) anguloCamaraX = 89.0f;
    if (anguloCamaraX < -89.0f) anguloCamaraX = -89.0f;
    
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
                
                if (tablero[i][j][k] == 0) {
                    drawCube(x, y, z, isAxis);
                } else {
                    drawSphere(x, y, z, tablero[i][j][k]);
                }
            }
        }
    }

    // Resaltar la línea ganadora si hay un ganador
    if (juegoTerminado && ganador != 0) {
        highlightWinningLine();
    }
}

// Función auxiliar para verificar una línea ganadora
bool verificarLinea(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3) {
    if (tablero[x1][y1][z1] != 0 &&
        tablero[x1][y1][z1] == tablero[x2][y2][z2] &&
        tablero[x2][y2][z2] == tablero[x3][y3][z3]) {
        
        // Guardar la línea ganadora
        posicionesGanadoras.push_back(std::make_tuple(x1, y1, z1));
        posicionesGanadoras.push_back(std::make_tuple(x2, y2, z2));
        posicionesGanadoras.push_back(std::make_tuple(x3, y3, z3));
        return true;
    }
    return false;
}

bool checkWin() {
    posicionesGanadoras.clear();

    // Verificar líneas horizontales
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int z = 0; z < BOARD_SIZE; z++) {
            if (verificarLinea(0, y, z, 1, y, z, 2, y, z)) return true;
        }
    }

    // Verificar líneas verticales
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int z = 0; z < BOARD_SIZE; z++) {
            if (verificarLinea(x, 0, z, x, 1, z, x, 2, z)) return true;
        }
    }

    // Verificar líneas en profundidad
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (verificarLinea(x, y, 0, x, y, 1, x, y, 2)) return true;
        }
    }

    // Verificar diagonales en planos XY
    for (int z = 0; z < BOARD_SIZE; z++) {
        if (verificarLinea(0, 0, z, 1, 1, z, 2, 2, z)) return true;
        if (verificarLinea(2, 0, z, 1, 1, z, 0, 2, z)) return true;
    }

    // Verificar diagonales en planos XZ
    for (int y = 0; y < BOARD_SIZE; y++) {
        if (verificarLinea(0, y, 0, 1, y, 1, 2, y, 2)) return true;
        if (verificarLinea(2, y, 0, 1, y, 1, 0, y, 2)) return true;
    }

    // Verificar diagonales en planos YZ
    for (int x = 0; x < BOARD_SIZE; x++) {
        if (verificarLinea(x, 0, 0, x, 1, 1, x, 2, 2)) return true;
        if (verificarLinea(x, 2, 0, x, 1, 1, x, 0, 2)) return true;
    }

    // Verificar diagonales espaciales
    if (verificarLinea(0, 0, 0, 1, 1, 1, 2, 2, 2)) return true;
    if (verificarLinea(2, 0, 0, 1, 1, 1, 0, 2, 2)) return true;
    if (verificarLinea(0, 2, 0, 1, 1, 1, 2, 0, 2)) return true;
    if (verificarLinea(2, 2, 0, 1, 1, 1, 0, 0, 2)) return true;

    return false;
}

// Función auxiliar para dibujar una cara del cubo
void dibujarCaraCubo(float vertices[4][3], float normales[3], float texCoords[4][2]) {
    glNormal3fv(normales);
    for (int i = 0; i < 4; i++) {
        glTexCoord2fv(texCoords[i]);
        glVertex3fv(vertices[i]);
    }
}

void drawCube(float x, float y, float z, bool isAxis) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    if (texturasHabilitadas) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glColor3f(isAxis ? 0.8f : 0.7f, isAxis ? 0.6f : 0.7f, isAxis ? 0.2f : 0.7f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(isAxis ? 0.8f : 0.7f, isAxis ? 0.6f : 0.7f, isAxis ? 0.2f : 0.7f);
    }

    // Definir vértices para cada cara
    float vertices[6][4][3] = {
        // Frente
        {{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}},
        // Atrás
        {{-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}},
        // Arriba
        {{-0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, -0.5f}},
        // Abajo
        {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}},
        // Derecha
        {{0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}},
        // Izquierda
        {{-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, -0.5f}}
    };

    // Definir normales para cada cara
    float normales[6][3] = {
        {0.0f, 0.0f, 1.0f},  // Frente
        {0.0f, 0.0f, -1.0f}, // Atrás
        {0.0f, 1.0f, 0.0f},  // Arriba
        {0.0f, -1.0f, 0.0f}, // Abajo
        {1.0f, 0.0f, 0.0f},  // Derecha
        {-1.0f, 0.0f, 0.0f}  // Izquierda
    };

    // Definir coordenadas de textura para cada cara
    float texCoords[4][2] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
    };

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; i++) {
        dibujarCaraCubo(vertices[i], normales[i], texCoords);
    }
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawSphere(float x, float y, float z, int player) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Aplicar textura solo si están habilitadas
    if (texturasHabilitadas) {
        glEnable(GL_TEXTURE_2D);
        if (player == 1) {
            glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer1);
            glColor3f(1.0f, 0.3f, 0.3f); // Rojo para jugador 1
        } else {
            glBindTexture(GL_TEXTURE_2D, sphereTexturePlayer2);
            glColor3f(0.3f, 0.3f, 1.0f); // Azul para jugador 2
        }
    } else {
        glDisable(GL_TEXTURE_2D);
        if (player == 1) {
            glColor3f(1.0f, 0.3f, 0.3f); // Rojo para jugador 1
        } else {
            glColor3f(0.3f, 0.3f, 1.0f); // Azul para jugador 2
        }
    }

    // Crear esfera con mejor calidad
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluSphere(quadric, 0.4f, 32, 32);
    gluDeleteQuadric(quadric);
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

bool checkDraw() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                if (tablero[i][j][k] == 0) {
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
    // Solo manejar la música de fondo
    if (strcmp(soundFile, "background.wav") == 0) {
        if (musicaHabilitada && !musicaReproduciendo) {
            std::string fullPath = "sounds/" + std::string(soundFile);
            std::wstring widePath(fullPath.begin(), fullPath.end());
            
            if (PlaySound(widePath.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP)) {
                musicaReproduciendo = true;
                std::cout << "Música de fondo iniciada" << std::endl;
            } else {
                std::cout << "Error al reproducir música de fondo" << std::endl;
            }
        }
    }
}

void updateAnimation(int value) {
    if (estaAnimando) {
        tiempoAnimacionGanadora += 0.1f;
        if (tiempoAnimacionGanadora > 2.0f) {
            estaAnimando = false;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(16, updateAnimation, 0);
}

void drawWinningAnimation() {
    if (!estaAnimando) return;

    float escala = 1.0f + 0.2f * sin(tiempoAnimacionGanadora * 5.0f);
    float rotacion = tiempoAnimacionGanadora * 180.0f;
    float pulso = 0.5f + 0.5f * sin(tiempoAnimacionGanadora * 10.0f);

    // Dibuja linea conectora entre las piezas ganadoras
    if (!posicionesGanadoras.empty()) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.0f);
        glLineWidth(5.0f);
        glBegin(GL_LINE_STRIP);
        for (const auto& pos : posicionesGanadoras) {
            int x, y, z;
            std::tie(x, y, z) = pos;
            glVertex3f(x * SPACING - SPACING, y * SPACING - SPACING, z * SPACING - SPACING);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    // Anima las piezas ganadoras con efectos de rotacion y escala
    for (const auto& pos : posicionesGanadoras) {
        int x, y, z;
        std::tie(x, y, z) = pos;
        
        glPushMatrix();
        glTranslatef(x * SPACING - SPACING, y * SPACING - SPACING, z * SPACING - SPACING);
        glRotatef(rotacion, 0.0f, 1.0f, 0.0f);
        glScalef(escala, escala, escala);
        
        // Efecto de brillo pulsante
        glColor3f(pulso, pulso, pulso);
        
        if (tablero[x][y][z] == 1) {
            drawSphere(0, 0, 0, 1);
        } else {
            drawSphere(0, 0, 0, 2);
        }
        
        glPopMatrix();
    }
}