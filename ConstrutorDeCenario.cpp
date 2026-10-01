#include "ConstrutorDeCenario.h"
#include "ConstrutorDePista.h"
#include "carro/CarroCaminhao.h"
#include "carro/CarroCaminhonete.h"
#include "carro/CarroConversivel.h"
#include "carro/CarroEsportivo.h"
#include "carro/CarroFusca.h"
#include "carro/CarroHatch.h"
#include "carro/CarroHotRod.h"
#include "carro/CarroJipe.h"
#include "carro/CarroMonstro.h"
#include "carro/CarroPerua.h"
#include "carro/CarroSedan.h"
#include "carro/CarroSUV.h"
#include "carro/CarroVan.h"
#include "carro/Veiculo.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h> // glutSolidCylinder
#include <cmath>

// --------------------------------------
// Ajudantes de desenho
// --------------------------------------

// Caixa alinhada aos eixos, dada pelos limites em cada eixo.
static void cubo(float x0, float x1, float y0, float y1, float z0, float z1) {
    glPushMatrix();
        glTranslatef((x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2);
        glScalef(x1 - x0, y1 - y0, z1 - z0);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// Quad subdividido em nu x nv pedacos, de o ate o + u + v. A normal e v x u.
// A iluminacao do OpenGL e por vertice: sem subdividir, a luz do abajur
// nao aparece no meio de uma parede.
static void grade(float ox, float oy, float oz,
                  float ux, float uy, float uz,
                  float vx, float vy, float vz, int nu, int nv) {
    float nx = vy * uz - vz * uy, ny = vz * ux - vx * uz, nz = vx * uy - vy * ux;
    glNormal3f(nx, ny, nz);
    glBegin(GL_QUADS);
    for (int i = 0; i < nu; i++)
        for (int j = 0; j < nv; j++) {
            float a = (float)i / nu, b = (float)(i + 1) / nu;
            float c = (float)j / nv, d = (float)(j + 1) / nv;
            glVertex3f(ox + ux * a + vx * c, oy + uy * a + vy * c, oz + uz * a + vz * c);
            glVertex3f(ox + ux * a + vx * d, oy + uy * a + vy * d, oz + uz * a + vz * d);
            glVertex3f(ox + ux * b + vx * d, oy + uy * b + vy * d, oz + uz * b + vz * d);
            glVertex3f(ox + ux * b + vx * c, oy + uy * b + vy * c, oz + uz * b + vz * c);
        }
    glEnd();
}

// As formas 2D abaixo ficam no plano xy local, viradas para +z, afastadas dz.
// Usadas em posteres, telas e janela depois de posicionar com glTranslate/glRotate.
static void retangulo(float x0, float y0, float x1, float y1, float dz = 0) {
    glNormal3f(0, 0, 1);
    glBegin(GL_QUADS);
        glVertex3f(x0, y0, dz);
        glVertex3f(x1, y0, dz);
        glVertex3f(x1, y1, dz);
        glVertex3f(x0, y1, dz);
    glEnd();
}

static void degrade(float x0, float y0, float x1, float y1, float dz,
                    float rb, float gb, float bb, float rc, float gc, float bc) {
    glNormal3f(0, 0, 1);
    glBegin(GL_QUADS);
        glColor3f(rb, gb, bb);
        glVertex3f(x0, y0, dz);
        glVertex3f(x1, y0, dz);
        glColor3f(rc, gc, bc);
        glVertex3f(x1, y1, dz);
        glVertex3f(x0, y1, dz);
    glEnd();
}

// Garante a ordem anti-horaria; com GL_LIGHT_MODEL_TWO_SIDE a face de tras fica escura.
static void triangulo(float x0, float y0, float x1, float y1, float x2, float y2, float dz = 0) {
    if ((x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0) < 0) {
        float tx = x1, ty = y1;
        x1 = x2; y1 = y2; x2 = tx; y2 = ty;
    }
    glNormal3f(0, 0, 1);
    glBegin(GL_TRIANGLES);
        glVertex3f(x0, y0, dz);
        glVertex3f(x1, y1, dz);
        glVertex3f(x2, y2, dz);
    glEnd();
}

static void disco(float cx, float cy, float raio, float dz = 0) {
    glNormal3f(0, 0, 1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(cx, cy, dz);
        for (int i = 0; i <= 24; i++) {
            float a = i * 2 * carros::PI_CARRO / 24;
            glVertex3f(cx + raio * cosf(a), cy + raio * sinf(a), dz);
        }
    glEnd();
}

// Texto em linhas da GLUT, centrado em x, sem iluminacao.
static void texto(const char* s, float x, float y, float altura, float dz,
                  float r, float g, float b) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    float e = altura / 119.05f; // altura de uma maiuscula na GLUT_STROKE_ROMAN
    float largura = glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*)s) * e;
    glPushMatrix();
        glTranslatef(x - largura / 2, y, dz);
        glScalef(e, e, e);
        for (const char* c = s; *c; c++)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    glPopMatrix();
    glPopAttrib();
}

static void semLuz()  { glPushAttrib(GL_ENABLE_BIT); glDisable(GL_LIGHTING); }
static void comLuz()  { glPopAttrib(); }

static void madeira()       { glColor3f(0.42f, 0.24f, 0.12f); }
static void madeiraEscura() { glColor3f(0.30f, 0.16f, 0.08f); }

// Fileira de livros em pe ao longo de x, de x0 ate x1.
static void livros(float x0, float x1, float y, float z0, float z1, float alturaMax) {
    static const float cores[][3] = {
        {0.55f, 0.15f, 0.12f}, {0.18f, 0.30f, 0.45f}, {0.25f, 0.40f, 0.25f},
        {0.60f, 0.50f, 0.25f}, {0.35f, 0.20f, 0.35f}, {0.20f, 0.20f, 0.22f}
    };
    int i = 0;
    for (float x = x0; x + 14 <= x1; i++) {
        float w = 14.0f + (i * 7) % 10;
        float h = alturaMax * (0.7f + 0.1f * ((i * 5) % 4));
        glColor3fv(cores[(i * 4) % 6]);
        cubo(x, x + w - 1, y, y + h, z0, z1);
        x += w;
    }
}

// Vaso com folhas; (x, y, z) e o centro da base.
static void planta(float x, float y, float z) {
    glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(0.85f, 0.85f, 0.82f);
        cubo(-16, 16, 0, 28, -16, 16);
        glColor3f(0.25f, 0.55f, 0.20f);
        cubo(-5, 5, 28, 75, -5, 5);
        for (int i = 0; i < 4; i++) {
            glPushMatrix();
                glRotatef(i * 90.0f + 45.0f, 0, 1, 0);
                glTranslatef(0, 28, 0);
                glRotatef(-35, 0, 0, 1);
                cubo(-4, 4, 0, 45, -4, 4);
            glPopMatrix();
        }
    glPopMatrix();
}

// Bonecos de blocos virados para +z; (x, y, z) e o centro da base.
static void creeper(float x, float y, float z) {
    glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(0.30f, 0.65f, 0.30f);
        cubo(-12, 12, 0, 12, -10, 10);   // pes
        cubo(-8, 8, 12, 50, -6, 6);      // corpo
        cubo(-12, 12, 50, 74, -12, 12);  // cabeca
        glColor3f(0.05f, 0.08f, 0.05f);
        cubo(-8, -3, 64, 69, 12, 13);    // olhos
        cubo(3, 8, 64, 69, 12, 13);
        cubo(-3, 3, 54, 63, 12, 13);     // boca
    glPopMatrix();
}

static void boneco(float x, float y, float z) {
    glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(0.20f, 0.25f, 0.55f);
        cubo(-8, 8, 0, 24, -5, 5);       // pernas
        glColor3f(0.15f, 0.55f, 0.60f);
        cubo(-8, 8, 24, 48, -5, 5);      // camisa
        cubo(-14, -8, 26, 48, -4, 4);    // bracos
        cubo(8, 14, 26, 48, -4, 4);
        glColor3f(0.80f, 0.60f, 0.45f);
        cubo(-8, 8, 48, 64, -8, 8);      // cabeca
        glColor3f(0.30f, 0.20f, 0.10f);
        cubo(-8, 8, 60, 65, -8, 8);      // cabelo
    glPopMatrix();
}

// Origem da pista no mundo: em cima do tampo da mesa, centralizada.
static constexpr float PISTA_X = -260.0f, PISTA_Y = 170.0f, PISTA_Z = -280.0f;

//casa
class Chao{
    public:
        void desenha(){
            // Tabuas de 100 de largura alternando dois tons.
            for (int i = 0; i < 20; i++) {
                if (i % 2)
                    glColor3f(0.729f, 0.549f, 0.278f);
                else
                    glColor3f(0.922f, 0.694f, 0.353f);
                grade(-1000.0f, -0.1f, -1000.0f + i * 100,
                      2000, 0, 0, 0, 0, 100, 40, 2);
            }
        }
};

class Paredes{
    public:
        void desenha(){
            // Origem e direcao horizontal de cada parede, com a face para dentro.
            const float paredes[4][6] = {
                {-1000, 0, -1000,     0, 0,  2000},   // esquerda (x = -1000)
                {-1000, 0,  1000,  2000, 0,     0},   // frente   (z =  1000)
                { 1000, 0,  1000,     0, 0, -2000},   // direita  (x =  1000)
                { 1000, 0, -1000, -2000, 0,     0},   // fundo    (z = -1000)
            };
            for (const auto& p : paredes) {
                // Barrado branco ate 300, azul ate o teto.
                glColor3f(0.93f, 0.92f, 0.88f);
                grade(p[0], 0, p[2], p[3], 0, p[5], 0, 300, 0, 40, 6);
                glColor3f(0.24f, 0.52f, 0.62f);
                grade(p[0], 300, p[2], p[3], 0, p[5], 0, 500, 0, 40, 10);
            }

            // Moldura de madeira entre o branco e o azul.
            madeiraEscura();
            cubo(-1000, -992, 295, 310, -1000, 1000);
            cubo(992, 1000, 295, 310, -1000, 1000);
            cubo(-1000, 1000, 295, 310, -1000, -992);
            cubo(-1000, 1000, 295, 310, 992, 1000);
        }
};

class Teto{
    public:
        void desenha(){
            glColor3f(0.612, 0.502, 0);
            grade(-1000, 800, -1000, 0, 0, 2000, 2000, 0, 0, 40, 40);
        }
};

class Mesa {
    private:
        void desenhaCubo(float x, float y, float z,
                         float largura, float altura, float profundidade) {
            glPushMatrix();
                glTranslatef(x, y, z);
                glScalef(largura, altura, profundidade);
                glutSolidCube(1.0f);
            glPopMatrix();
        }

    public:
        void desenha() {
            glColor3f(0.45f, 0.25f, 0.10f);

            // Tampo com a face superior em y = 170.
            desenhaCubo(0.0f, 155.0f, 0.0f, 850.0f, 30.0f, 700.0f);

            // Quatro pes.
            desenhaCubo(-370.0f, 70.0f, -300.0f, 35.0f, 140.0f, 35.0f);
            desenhaCubo( 370.0f, 70.0f, -300.0f, 35.0f, 140.0f, 35.0f);
            desenhaCubo(-370.0f, 70.0f,  300.0f, 35.0f, 140.0f, 35.0f);
            desenhaCubo( 370.0f, 70.0f,  300.0f, 35.0f, 140.0f, 35.0f);
        }
};

class Ventilador {
    public:
        void desenha() {
            static float angulo = 0.0f;
            angulo += 2.0f;
            if (angulo >= 360.0f)
                angulo -= 360.0f;

            glPushMatrix();
                // Posiciona o ventilador abaixo do teto.
                glTranslatef(00.0f, 740.0f, 000.0f);

                // Haste presa ao teto.
                glColor3f(0.25f, 0.25f, 0.25f);
                glPushMatrix();
                    glTranslatef(0.0f, 0.0f, 0.0f);
                    glScalef(8.0f, 120.0f, 8.0f);
                    glutSolidCube(1.0f);
                glPopMatrix();

                // Motor central.
                glPushMatrix();
                    glTranslatef(0.0f, -30.0f, 0.0f);

                    // A esfera funciona como a lampada do ventilador.
                    GLfloat emissao[] = {1.0f, 0.85f, 0.40f, 1.0f};
                    GLfloat semEmissao[] = {0.0f, 0.0f, 0.0f, 1.0f};
                    bool acesa = glIsEnabled(GL_LIGHT0);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,
                                 acesa ? emissao : semEmissao);
                    if (acesa)
                        glColor3f(1.0f, 0.90f, 0.50f);
                    else
                        glColor3f(0.25f, 0.22f, 0.15f);
                    glutSolidSphere(30.0f, 16, 16);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, semEmissao);
                glPopMatrix();
                // As quatro pas giram ao redor do eixo Y.
                glRotatef(angulo, 0.0f, 1.0f, 0.0f);
                glColor3f(0.55f, 0.35f, 0.15f);

                for (int i = 0; i < 4; i++) {
                    glPushMatrix();
                        glRotatef(i * 90.0f, 0.0f, 1.0f, 0.0f);
                        glTranslatef(90.0f, -30.0f, 0.0f);
                        glScalef(300.0f, 6.0f, 35.0f);
                        glutSolidCube(1.0f);
                    glPopMatrix();
                }
            glPopMatrix();
        }
};

// Decoracao do quarto. Paredes: esquerda x = -1000, fundo z = -1000, direita x = 1000.
class Quarto {
    private:
        // Cama: cabeceira no fundo, encostada na parede esquerda.
        void cama() {
            madeiraEscura();
            cubo(-1000, -440, 0, 330, -1000, -960);   // cabeceira
            cubo(-995, -445, 0, 150, -200, -180);     // peseira
            cubo(-990, -450, 40, 120, -960, -200);    // estrado
            cubo(-990, -950, 0, 40, -960, -920);      // pes
            cubo(-490, -450, 0, 40, -960, -920);
            cubo(-990, -950, 0, 40, -240, -200);
            cubo(-490, -450, 0, 40, -240, -200);

            glColor3f(0.92f, 0.92f, 0.95f);
            cubo(-975, -465, 120, 195, -955, -205);   // colchao
            glColor3f(0.12f, 0.22f, 0.50f);
            cubo(-980, -460, 150, 205, -790, -198);   // coberta
            glColor3f(0.85f, 0.86f, 0.92f);
            cubo(-930, -510, 195, 235, -945, -850);   // travesseiro
        }

        void criadoMudo() {
            madeira();
            cubo(-420, -230, 0, 200, -1000, -820);
            madeiraEscura();
            cubo(-405, -245, 110, 185, -821, -815);   // gavetas
            cubo(-405, -245, 25, 100, -821, -815);
            glColor3f(0.75f, 0.70f, 0.50f);
            cubo(-335, -315, 142, 152, -815, -810);   // puxadores
            cubo(-335, -315, 57, 67, -815, -810);

            // Abajur: a cupula acesa e a posicao da GL_LIGHT1.
            glColor3f(0.20f, 0.15f, 0.10f);
            cubo(-360, -310, 200, 215, -935, -885);
            semLuz();
            glColor3f(1.0f, 0.88f, 0.60f);
            cubo(-375, -295, 215, 295, -950, -870);
            comLuz();

            // Despertador com 21:34 em vermelho.
            glColor3f(0.05f, 0.05f, 0.05f);
            cubo(-292, -233, 200, 232, -900, -870);
            glPushMatrix();
                glTranslatef(-262.5f, 210, -870);
                texto("21:34", 0, 0, 11, 0.5f, 1.0f, 0.10f, 0.05f);
            glPopMatrix();
        }

        // Janela no fundo, com predios a noite e persiana meio fechada.
        void janela() {
            glPushMatrix();
                glTranslatef(-330, 360, -999);
                semLuz();
                degrade(0, 0, 460, 290, 0, 0.06f, 0.09f, 0.22f, 0.02f, 0.03f, 0.10f);
                glColor3f(0.015f, 0.02f, 0.05f);
                const float predios[][4] = {
                    {20, 0, 90, 120}, {100, 0, 150, 80}, {170, 0, 250, 170},
                    {270, 0, 320, 100}, {340, 0, 420, 140}, {430, 0, 460, 90}
                };
                for (const auto& p : predios)
                    retangulo(p[0], p[1], p[2], p[3], 0.5f);
                glColor3f(0.95f, 0.80f, 0.35f);
                const float janelas[][2] = {
                    {40, 90}, {62, 60}, {190, 140}, {222, 110}, {200, 70}, {360, 110}, {384, 80}
                };
                for (const auto& j : janelas)
                    retangulo(j[0], j[1], j[0] + 7, j[1] + 9, 1.0f);
                comLuz();
            glPopMatrix();

            madeira();
            cubo(-350, 150, 340, 360, -1000, -965);   // parapeito
            cubo(-350, -330, 360, 650, -1000, -975);  // batentes
            cubo(130, 150, 360, 650, -1000, -975);
            cubo(-110, -90, 360, 650, -1000, -980);   // divisoria
            cubo(-350, 150, 650, 690, -1000, -960);   // caixa da persiana
            for (int i = 0; i < 6; i++) {
                float y = 635 - i * 20;
                cubo(-325, 125, y, y + 11, -985, -972);
            }
        }

        void posteres() {
            // Parede esquerda: rotacao de 90 graus vira o plano local para +x.
            glPushMatrix();
                glTranslatef(-998, 400, -560);
                glRotatef(90, 0, 1, 0);
                glColor3f(0.06f, 0.06f, 0.07f);
                retangulo(0, 0, 190, 250);
                glColor3f(0.75f, 0.75f, 0.78f);                  // lobo
                retangulo(55, 140, 110, 200, 0.5f);
                triangulo(95, 150, 165, 168, 100, 190, 0.5f);
                triangulo(95, 150, 150, 148, 100, 128, 0.5f);
                triangulo(68, 198, 85, 238, 104, 200, 0.5f);
                triangulo(55, 140, 40, 175, 60, 200, 0.5f);
                glColor3f(0.06f, 0.06f, 0.07f);
                retangulo(98, 178, 106, 184, 1.0f);              // olho
                texto("WINTER", 95, 70, 20, 1.0f, 0.92f, 0.92f, 0.92f);
                texto("IS COMING", 95, 40, 16, 1.0f, 0.92f, 0.92f, 0.92f);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(-998, 400, -280);
                glRotatef(90, 0, 1, 0);
                degrade(0, 0, 180, 240, 0, 0.85f, 0.35f, 0.20f, 0.35f, 0.08f, 0.10f);
                glColor3f(0.95f, 0.55f, 0.35f);
                disco(90, 150, 35, 0.3f);                        // sol
                glColor3f(0.40f, 0.10f, 0.14f);
                triangulo(0, 70, 60, 150, 130, 70, 0.5f);        // montanhas
                triangulo(80, 70, 150, 140, 180, 70, 0.5f);
                glColor3f(0.15f, 0.05f, 0.08f);
                retangulo(0, 0, 180, 70, 0.7f);
                triangulo(30, 70, 90, 130, 150, 70, 0.7f);
                retangulo(86, 128, 94, 152, 0.9f);               // viajante no pico
                disco(90, 157, 5, 0.9f);
            glPopMatrix();

            // Parede do fundo, a direita da mesa do computador.
            glPushMatrix();
                glTranslatef(520, 380, -998);
                glColor3f(0.06f, 0.06f, 0.07f);
                retangulo(0, 0, 140, 240);
                texto("CORINTHIANS", 70, 205, 14, 0.5f, 0.95f, 0.95f, 0.95f);
                glColor3f(0.75f, 0.10f, 0.10f);                  // remos cruzados
                for (int lado = -1; lado <= 1; lado += 2) {
                    glPushMatrix();
                        glTranslatef(70, 110, 0);
                        glRotatef(lado * 35.0f, 0, 0, 1);
                        retangulo(-5, -75, 5, 75, 0.3f);
                    glPopMatrix();
                }
                glColor3f(0.95f, 0.95f, 0.95f);
                disco(70, 110, 38, 0.5f);
                glColor3f(0.06f, 0.06f, 0.07f);
                disco(70, 110, 32, 0.7f);
                texto("SCCP", 70, 104, 13, 0.9f, 0.95f, 0.95f, 0.95f);
            glPopMatrix();
        }

        void escrivaninha() {
            madeira();
            cubo(-40, 500, 290, 310, -1000, -760);    // tampo
            cubo(-30, 110, 0, 290, -990, -770);       // gaveteiro
            cubo(460, 490, 0, 290, -990, -960);       // pes
            cubo(460, 490, 0, 290, -800, -770);
            madeiraEscura();
            cubo(-20, 100, 200, 275, -771, -765);
            cubo(-20, 100, 110, 185, -771, -765);
            cubo(-20, 100, 20, 95, -771, -765);

            // Monitor; a tela acesa ilumina a mesa pela GL_LIGHT2.
            glColor3f(0.05f, 0.05f, 0.06f);
            cubo(110, 200, 310, 318, -950, -900);
            cubo(145, 165, 318, 350, -935, -920);
            cubo(60, 250, 345, 470, -935, -925);
            glPushMatrix();
                glTranslatef(68, 353, -924.5f);
                semLuz();
                degrade(0, 0, 174, 109, 0, 0.35f, 0.65f, 1.0f, 0.12f, 0.30f, 0.80f);
                glColor3f(0.10f, 0.22f, 0.55f);
                triangulo(0, 0, 60, 55, 120, 0, 0.3f);
                triangulo(70, 0, 130, 45, 174, 0, 0.3f);
                comLuz();
            glPopMatrix();

            glColor3f(0.08f, 0.08f, 0.09f);
            cubo(90, 230, 310, 317, -885, -845);      // teclado
            cubo(250, 268, 310, 318, -870, -845);     // mouse
            glColor3f(0.45f, 0.45f, 0.48f);
            cubo(15, 40, 310, 345, -960, -935);       // porta-lapis
            glColor3f(0.85f, 0.65f, 0.15f);
            cubo(22, 25, 345, 365, -950, -947);
            glColor3f(0.20f, 0.35f, 0.75f);
            cubo(30, 33, 345, 362, -945, -942);

            // Gabinete com tres ventoinhas vermelhas na frente.
            glColor3f(0.05f, 0.05f, 0.06f);
            cubo(300, 390, 310, 500, -980, -880);
            glPushMatrix();
                glTranslatef(345, 0, -879.5f);
                semLuz();
                for (int i = 0; i < 3; i++) {
                    glColor3f(1.0f, 0.12f, 0.05f);
                    disco(0, 465 - i * 60, 22, 0);
                    glColor3f(0.10f, 0.01f, 0.01f);
                    disco(0, 465 - i * 60, 14, 0.3f);
                }
                comLuz();
            glPopMatrix();
        }

        void cadeira() {
            glColor3f(0.08f, 0.08f, 0.09f);
            cubo(60, 230, 175, 200, -760, -600);      // assento
            cubo(70, 220, 200, 430, -600, -580);      // encosto
            cubo(55, 70, 200, 260, -740, -620);       // bracos
            cubo(220, 235, 200, 260, -740, -620);
            cubo(138, 152, 35, 175, -687, -673);      // haste
            for (int i = 0; i < 5; i++) {             // base estrela com rodinhas
                glPushMatrix();
                    glTranslatef(145, 25, -680);
                    glRotatef(i * 72.0f, 0, 1, 0);
                    cubo(0, 85, -6, 6, -7, 7);
                    cubo(75, 95, -25, -6, -9, 9);
                glPopMatrix();
            }
        }

        void prateleiras() {
            madeira();
            cubo(160, 400, 610, 622, -1000, -940);
            cubo(260, 470, 520, 532, -1000, -940);
            planta(190, 622, -970);
            livros(250, 395, 622, -995, -950, 80);
            glColor3f(0.55f, 0.15f, 0.12f);           // livros deitados
            cubo(270, 340, 532, 545, -995, -955);
            glColor3f(0.25f, 0.40f, 0.25f);
            cubo(275, 335, 545, 556, -993, -958);
            boneco(420, 532, -965);
        }

        void estante() {
            madeira();
            cubo(700, 990, 0, 700, -1000, -990);      // fundo
            cubo(700, 718, 0, 700, -1000, -850);      // laterais
            cubo(972, 990, 0, 700, -1000, -850);
            const float alturas[] = {0, 160, 300, 440, 580, 685};
            for (float y : alturas)
                cubo(700, 990, y, y + 15, -1000, -850);

            glColor3f(0.18f, 0.18f, 0.20f);           // caixas organizadoras
            cubo(722, 840, 15, 120, -985, -860);
            cubo(852, 968, 15, 120, -985, -860);
            livros(722, 900, 175, -985, -870, 110);
            creeper(935, 175, -915);
            livros(722, 968, 315, -985, -870, 110);
            livros(722, 860, 455, -985, -870, 100);
            boneco(915, 455, -915);
            livros(722, 830, 595, -985, -870, 80);
            planta(930, 700, -925);
            madeiraEscura();
            cubo(730, 840, 700, 740, -980, -900);     // caixa em cima
        }

        // Rack na parede direita com TV, videogame e controle.
        void rack() {
            madeira();
            cubo(790, 990, 175, 195, -450, 250);      // tampo
            cubo(790, 990, 40, 55, -450, 250);        // prateleira de baixo
            cubo(790, 990, 0, 175, -450, -430);       // laterais
            cubo(790, 990, 0, 175, 230, 250);
            cubo(790, 990, 0, 175, -110, -90);        // divisoria
            glColor3f(0.06f, 0.06f, 0.07f);
            cubo(830, 950, 55, 85, -330, -150);       // videogame
            cubo(840, 870, 195, 202, 60, 110);        // controle

            cubo(860, 930, 195, 205, -170, -30);      // pe da TV
            cubo(900, 915, 205, 240, -110, -90);
            cubo(905, 925, 230, 500, -330, 130);      // TV

            // Tela: rotacao de -90 vira o plano local para -x (para dentro do quarto).
            glPushMatrix();
                glTranslatef(904.5f, 240, -320);
                glRotatef(-90, 0, 1, 0);
                semLuz();
                degrade(0, 100, 440, 250, 0, 0.95f, 0.55f, 0.45f, 0.25f, 0.20f, 0.55f);
                glColor3f(1.0f, 0.95f, 0.60f);
                retangulo(200, 150, 240, 190, 0.2f);      // sol
                glColor3f(0.18f, 0.40f, 0.20f);
                retangulo(0, 0, 440, 105, 0.2f);          // grama
                glColor3f(0.30f, 0.45f, 0.85f);
                triangulo(180, 0, 260, 0, 220, 105, 0.4f); // rio
                glColor3f(0.08f, 0.22f, 0.12f);
                for (int i = 0; i < 9; i++) {             // pinheiros
                    float x = 15 + i * 50 + (i % 2) * 12;
                    if (x > 165 && x < 270)
                        continue;
                    float h = 70 + (i % 3) * 25;
                    triangulo(x - 18, 60, x + 18, 60, x, 60 + h, 0.6f);
                }
                comLuz();
            glPopMatrix();

            planta(890, 195, 195);
        }

        void tapete() {
            glColor3f(0.20f, 0.20f, 0.22f);
            grade(-600, 1, -520, 1200, 0, 0, 0, 0, 1040, 24, 20);
            glColor3f(0.38f, 0.38f, 0.40f);
            grade(-540, 2, -460, 1080, 0, 0, 0, 0, 920, 22, 18);
            glColor3f(0.24f, 0.24f, 0.26f);
            grade(-500, 3, -420, 1000, 0, 0, 0, 0, 840, 20, 16);
        }

        // No canto da frente a direita do tampo, por fora da curva da pista.
        void coisasNaMesa() {
            glColor3f(0.20f, 0.42f, 0.20f);
            cubo(352, 412, 170, 184, 292, 342);
            glColor3f(0.85f, 0.85f, 0.80f);
            cubo(356, 408, 184, 194, 295, 339);
            glColor3f(0.15f, 0.32f, 0.18f);
            cubo(350, 414, 194, 206, 293, 341);

            glColor3f(0.35f, 0.35f, 0.38f);           // caneca
            cubo(390, 415, 170, 205, 255, 282);
            cubo(415, 421, 178, 198, 263, 274);
        }

    public:
        void desenha() {
            tapete();
            cama();
            criadoMudo();
            janela();
            posteres();
            escrivaninha();
            cadeira();
            prateleiras();
            estante();
            rack();
            coisasNaMesa();
        }
};

static void luzPontual(GLenum luz, float x, float y, float z,
                       float r, float g, float b, float linear, float quadratica) {
    GLfloat posicao[] = {x, y, z, 1.0f};
    GLfloat cor[] = {r, g, b, 1.0f};
    GLfloat nada[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(luz, GL_POSITION, posicao);
    glLightfv(luz, GL_DIFFUSE, cor);
    glLightfv(luz, GL_SPECULAR, cor);
    glLightfv(luz, GL_AMBIENT, nada);
    glLightf(luz, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(luz, GL_LINEAR_ATTENUATION, linear);
    glLightf(luz, GL_QUADRATIC_ATTENUATION, quadratica);
    glEnable(luz);
}

// Maquete em volta da pista, em coordenadas da pista (y = 0 e o tampo da mesa).
class DecoracaoDaPista {
    private:
        // Medidas pensadas no mundo (comentario) e passadas para coordenadas da pista.
        // Gramado: tampo menos 25 de borda (mundo x = +-400, z = +-335).
        static constexpr float X0 = -400 - PISTA_X, X1 = 400 - PISTA_X;
        static constexpr float Z0 = -335 - PISTA_Z, Z1 = 335 - PISTA_Z;
        // Montanha no miolo do circuito (mundo 0, -115), poco da cachoeira na frente.
        static constexpr float MONTANHA_X = 0 - PISTA_X, MONTANHA_Z = -115 - PISTA_Z, MONTANHA_R = 85;
        static constexpr float LAGO_X = MONTANHA_X, LAGO_Z = MONTANHA_Z + 120, LAGO_R = 28;
        // Tunel sobre a reta da esquerda (mundo x = -340, z de -60 a 20).
        static constexpr float TUNEL_X0 = -392 - PISTA_X, TUNEL_X1 = -288 - PISTA_X;
        static constexpr float TUNEL_Z0 = -60 - PISTA_Z, TUNEL_Z1 = 20 - PISTA_Z;
        // Barranco: a cada 1 de altura da pista, avanca 1.1 para o lado.
        static constexpr float BARRANCO = 1.1f;

        struct Item { float x, z, tamanho; };
        std::vector<ConstrutorDePista::Ponto> linha = ConstrutorDePista::linhaCentral(10);
        std::vector<Item> arvores, pedras, postes;

        // Distancia ate a pista, descontando o barranco onde ela e elevada.
        float distanciaDaPista(float x, float z) const {
            float menor = 1e9f;
            for (const auto& p : linha)
                menor = fminf(menor, hypotf(x - p.x, z - p.z) - p.y * BARRANCO);
            return menor;
        }

        bool livre(float x, float z, float folga) const {
            return x > X0 + 15 && x < X1 - 15 && z > Z0 + 15 && z < Z1 - 15
                && distanciaDaPista(x, z) > ConstrutorDePista::MEIA_LARGURA + folga
                && hypotf(x - MONTANHA_X, z - MONTANHA_Z) > MONTANHA_R + 10
                && hypotf(x - LAGO_X, z - LAGO_Z) > LAGO_R + 12
                && !(x > TUNEL_X0 - 20 && x < TUNEL_X1 + 20 && z > TUNEL_Z0 - 20 && z < TUNEL_Z1 + 20)
                && !(x > 330 - PISTA_X && z > 235 - PISTA_Z); // livros e caneca (Quarto::coisasNaMesa)
        }

        // Pseudoaleatorio fixo: a maquete sai igual em toda execucao.
        unsigned semente = 12345;
        float sorteia(float a, float b) {
            semente = semente * 1103515245u + 12345u;
            return a + (b - a) * ((semente >> 16) & 0x7fff) / 32767.0f;
        }

        static void cone(float x, float y, float z, float raio, float altura, int lados) {
            glPushMatrix();
                glTranslatef(x, y, z);
                glRotatef(-90, 1, 0, 0);               // a GLUT cria o cone em +z
                glutSolidCone(raio, altura, lados, 1);
            glPopMatrix();
        }

        static void pinheiro(float x, float y, float z, float t) {
            glColor3f(0.35f, 0.22f, 0.10f);
            cubo(x - 2 * t, x + 2 * t, y, y + 10 * t, z - 2 * t, z + 2 * t);
            glColor3f(0.12f, 0.38f, 0.18f);
            cone(x, y + 8 * t, z, 13 * t, 18 * t, 7);
            glColor3f(0.15f, 0.45f, 0.20f);
            cone(x, y + 18 * t, z, 10 * t, 16 * t, 7);
            cone(x, y + 27 * t, z, 7 * t, 14 * t, 7);
        }

        void montanha() {
            const float x = MONTANHA_X, z = MONTANHA_Z;
            glColor3f(0.45f, 0.45f, 0.48f);
            cone(x, 0, z, MONTANHA_R, 110, 7);
            glColor3f(0.38f, 0.38f, 0.42f);
            cone(x - 35, 0, z - 25, 60, 150, 6);
            glColor3f(0.50f, 0.49f, 0.50f);
            cone(x + 40, 0, z - 15, 55, 125, 6);

            // Platos de grama nas encostas.
            glColor3f(0.30f, 0.52f, 0.20f);
            const float platos[][4] = {     // dx, y, dz, raio
                {-40, 35, 25, 28}, {45, 30, 30, 26}, {-30, 90, -20, 18}, {35, 70, -10, 20}
            };
            for (const auto& p : platos) {
                glPushMatrix();
                    glTranslatef(x + p[0], p[1], z + p[2]);
                    glRotatef(-90, 1, 0, 0);
                    glutSolidCylinder(p[3], 5, 8, 1);
                glPopMatrix();
                // Arvore na beirada de fora do plato, para nao ficar enterrada no cone.
                float dist = hypotf(p[0], p[2]), fora = (dist + p[3] * 0.7f) / dist;
                pinheiro(x + p[0] * fora, p[1] + 5, z + p[2] * fora, 0.7f);
            }

            // Cachoeira descendo a frente do cone principal ate o lago.
            semLuz();
            glNormal3f(0, 0.65f, 0.76f);
            glBegin(GL_QUADS);
                glColor3f(0.75f, 0.92f, 1.0f);
                glVertex3f(x - 9, 82, z + 26);
                glVertex3f(x + 9, 82, z + 26);
                glColor3f(0.30f, 0.62f, 0.92f);
                glVertex3f(x + 14, 0.8f, z + MONTANHA_R + 3);
                glVertex3f(x - 14, 0.8f, z + MONTANHA_R + 3);
            glEnd();
            comLuz();
        }

        void lago() {
            glPushMatrix();
                glTranslatef(LAGO_X, 0, LAGO_Z);
                glRotatef(-90, 1, 0, 0);              // plano xy local vira o chao
                glColor3f(0.40f, 0.40f, 0.42f);
                disco(0, 0, LAGO_R + 6, 0.8f);        // borda de pedra
                semLuz();
                glColor3f(0.25f, 0.58f, 0.85f);
                disco(0, 0, LAGO_R, 1.2f);
                glColor3f(0.85f, 0.95f, 1.0f);
                disco(0, LAGO_R - 12, 7, 1.5f);       // espuma onde a agua cai
                comLuz();
            glPopMatrix();
        }

        void tunel() {
            glColor3f(0.42f, 0.42f, 0.45f);
            cubo(TUNEL_X0, TUNEL_X0 + 10, 0, 45, TUNEL_Z0, TUNEL_Z1);
            cubo(TUNEL_X1 - 10, TUNEL_X1, 0, 45, TUNEL_Z0, TUNEL_Z1);
            cubo(TUNEL_X0, TUNEL_X1, 38, 52, TUNEL_Z0, TUNEL_Z1);
            glColor3f(0.30f, 0.30f, 0.33f);           // arcos das bocas
            for (float zb : {TUNEL_Z0 - 3, TUNEL_Z1 + 3}) {
                cubo(TUNEL_X0 - 3, TUNEL_X0 + 12, 0, 48, zb - 3, zb + 3);
                cubo(TUNEL_X1 - 12, TUNEL_X1 + 3, 0, 48, zb - 3, zb + 3);
                cubo(TUNEL_X0 - 3, TUNEL_X1 + 3, 36, 54, zb - 3, zb + 3);
            }
            glColor3f(0.30f, 0.52f, 0.20f);
            cubo(TUNEL_X0 - 2, TUNEL_X1 + 2, 52, 58, TUNEL_Z0 + 5, TUNEL_Z1 - 5);
            pinheiro(TUNEL_X0 + 18, 58, TUNEL_Z0 + 25, 0.7f);
            pinheiro(TUNEL_X1 - 15, 58, TUNEL_Z1 - 20, 0.6f);
        }

        // Portico quadriculado sobre a largada (inicio da pista, x = 30).
        void largada() {
            const float x = 30, z = ConstrutorDePista::MEIA_LARGURA + 10;
            glColor3f(0.10f, 0.10f, 0.10f);
            cubo(x - 2, x + 2, 0, 62, -z - 2, -z + 2);
            cubo(x - 2, x + 2, 0, 62, z - 2, z + 2);
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 2; j++) {
                    if ((i + j) % 2)
                        glColor3f(0.95f, 0.95f, 0.95f);
                    else
                        glColor3f(0.05f, 0.05f, 0.05f);
                    float za = -z + i * (2 * z / 8);
                    cubo(x - 1, x + 1, 46 + j * 7, 53 + j * 7, za, za + 2 * z / 8);
                }
        }

        void cercas() {
            glColor3f(0.55f, 0.38f, 0.20f);
            const float e = 6;                        // recuo da borda
            const float lados[][4] = {
                {X0 + e, Z0 + e, X1 - e, Z0 + e}, {X1 - e, Z0 + e, X1 - e, Z1 - e},
                {X1 - e, Z1 - e, X0 + e, Z1 - e}, {X0 + e, Z1 - e, X0 + e, Z0 + e}
            };
            for (const auto& l : lados) {
                float comp = hypotf(l[2] - l[0], l[3] - l[1]);
                float dx = (l[2] - l[0]) / comp, dz = (l[3] - l[1]) / comp;
                for (float s = 0; s <= comp; s += 30) {
                    float px = l[0] + dx * s, pz = l[1] + dz * s;
                    cubo(px - 1.5f, px + 1.5f, 0, 15, pz - 1.5f, pz + 1.5f);
                }
                for (float y : {6.0f, 12.0f})
                    cubo(fminf(l[0], l[2]) - 1, fmaxf(l[0], l[2]) + 1, y, y + 2,
                         fminf(l[1], l[3]) - 1, fmaxf(l[1], l[3]) + 1);
            }
        }

        // Quad com a face da frente virada para o lado de 'para' (iluminacao dos dois lados).
        static void quadVirado(ConstrutorDePista::Ponto a, ConstrutorDePista::Ponto b,
                               ConstrutorDePista::Ponto c, ConstrutorDePista::Ponto d,
                               float px, float py, float pz) {
            float ux = b.x - a.x, uy = b.y - a.y, uz = b.z - a.z;
            float vx = d.x - a.x, vy = d.y - a.y, vz = d.z - a.z;
            float nx = uy * vz - uz * vy, ny = uz * vx - ux * vz, nz = ux * vy - uy * vx;
            if (nx * px + ny * py + nz * pz < 0) {
                std::swap(b, d);
                nx = -nx; ny = -ny; nz = -nz;
            }
            glNormal3f(nx, ny, nz);
            glBegin(GL_QUADS);
                glVertex3f(a.x, a.y, a.z);
                glVertex3f(b.x, b.y, b.z);
                glVertex3f(c.x, c.y, c.z);
                glVertex3f(d.x, d.y, d.z);
            glEnd();
        }

        // Embaixo das partes elevadas: rampa de grama do lado de dentro do
        // circuito (direita de quem anda) e muro de pedra do lado de fora.
        void barranco() {
            const float borda = ConstrutorDePista::MEIA_LARGURA + 1;
            size_t n = linha.size();
            for (size_t i = 0; i < n; i++) {
                const auto& a = linha[i];
                const auto& b = linha[(i + 1) % n];
                if (a.y < 0.5f && b.y < 0.5f)
                    continue;
                float tx = b.x - a.x, tz = b.z - a.z, t = hypotf(tx, tz);
                float rx = -tz / t, rz = tx / t;      // direita

                float pa = borda + a.y * BARRANCO, pb = borda + b.y * BARRANCO;
                glColor3f(0.30f, 0.50f, 0.18f);
                quadVirado({a.x + rx * borda, a.y, a.z + rz * borda},
                           {b.x + rx * borda, b.y, b.z + rz * borda},
                           {b.x + rx * pb, 0.6f, b.z + rz * pb},
                           {a.x + rx * pa, 0.6f, a.z + rz * pa}, 0, 1, 0);

                glColor3f(0.42f, 0.40f, 0.38f);
                quadVirado({a.x - rx * borda, a.y, a.z - rz * borda},
                           {b.x - rx * borda, b.y, b.z - rz * borda},
                           {b.x - rx * borda, 0.6f, b.z - rz * borda},
                           {a.x - rx * borda, 0.6f, a.z - rz * borda}, -rx, 0, -rz);
            }
        }

        static void poste(float x, float z) {
            glColor3f(0.15f, 0.15f, 0.17f);
            cubo(x - 1.5f, x + 1.5f, 0, 48, z - 1.5f, z + 1.5f);
            cubo(x - 4, x + 4, 48, 51, z - 4, z + 4);
            semLuz();
            glColor3f(1.0f, 0.85f, 0.50f);
            cubo(x - 3, x + 3, 42, 48, z - 3, z + 3);
            comLuz();
        }

    public:
        DecoracaoDaPista() {
            for (int i = 0; i < 90; i++) {
                float x = sorteia(X0, X1), z = sorteia(Z0, Z1);
                if (livre(x, z, 22))
                    arvores.push_back({x, z, sorteia(0.7f, 1.3f)});
            }
            for (int i = 0; i < 40; i++) {
                float x = sorteia(X0, X1), z = sorteia(Z0, Z1);
                if (livre(x, z, 8))
                    pedras.push_back({x, z, sorteia(4, 9)});
            }
            // Postes a cada ~130 de pista, do lado que tiver espaco.
            for (size_t i = 6; i + 1 < linha.size(); i += 13) {
                if (linha[i].y > 1)                   // nada de poste no barranco
                    continue;
                float dx = linha[i + 1].x - linha[i - 1].x;
                float dz = linha[i + 1].z - linha[i - 1].z;
                float n = hypotf(dx, dz), off = ConstrutorDePista::MEIA_LARGURA + 10;
                for (int lado = 1; lado >= -1; lado -= 2) {
                    float px = linha[i].x - dz / n * off * lado;
                    float pz = linha[i].z + dx / n * off * lado;
                    if (livre(px, pz, 5)) {
                        postes.push_back({px, pz, 0});
                        break;
                    }
                }
            }
        }

        void desenha() {
            glColor3f(0.30f, 0.50f, 0.18f);
            cubo(X0, X1, 0, 0.6f, Z0, Z1);

            barranco();
            montanha();
            lago();
            tunel();
            largada();
            cercas();
            for (const auto& a : arvores)
                pinheiro(a.x, 0.6f, a.z, a.tamanho);
            glColor3f(0.48f, 0.47f, 0.47f);
            for (const auto& p : pedras) {
                glPushMatrix();
                    glTranslatef(p.x, p.tamanho * 0.4f, p.z);
                    glRotatef(p.x * 7, 0, 1, 0);
                    glRotatef(20, 1, 0, 1);
                    glScalef(p.tamanho, p.tamanho * 0.8f, p.tamanho);
                    glutSolidCube(1.0f);
                glPopMatrix();
            }
            for (const auto& p : postes)
                poste(p.x, p.z);
        }
};

// Vitrine na parede esquerda: os modelos antigos girando dentro de cupulas de vidro.
class Vitrine {
    private:
        static constexpr float ESCALA = 10.0f;
        static constexpr float X0 = -1000, X1 = -840, Z0 = 50, Z1 = 850;
        static constexpr float ALTO = 740;
        static constexpr float CENTRO_X = -915;
        // Cupula redonda do tamanho do pedestal; cabe o caminhao, o maior.
        static constexpr float RAIO = 46, CUPULA_A = 50;
        static constexpr float PEDESTAL = 12;
        static constexpr int TOTAL = 13;

        carros::CarroSedan sedan;
        carros::CarroSUV suv;
        carros::CarroVan van;
        carros::CarroCaminhao caminhao;
        carros::CarroEsportivo esportivo;
        carros::CarroCaminhonete caminhonete;
        carros::CarroFusca fusca;
        carros::CarroHatch hatch;
        carros::CarroConversivel conversivel;
        carros::CarroJipe jipe;
        carros::CarroMonstro monstro;
        carros::CarroPerua perua;
        carros::CarroHotRod hotRod;

        carros::ModeloCarro* modelos[TOTAL] = {
            &sedan, &suv, &van, &caminhao, &esportivo, &caminhonete, &fusca,
            &hatch, &conversivel, &jipe, &monstro, &perua, &hotRod
        };
        float giro = 0;

        // Andar de baixo com 5 carros, os dois de cima com 4.
        // Devolve a altura do tampo da prateleira e o z do centro do pedestal.
        static void lugar(int i, float& y, float& z) {
            const float andares[] = {80, 300, 520};
            int andar = i < 5 ? 0 : (i - 5) / 4 + 1;
            int coluna = i < 5 ? i : (i - 5) % 4;
            int n = andar == 0 ? 5 : 4;
            y = andares[andar];
            z = (Z0 + Z1) / 2 + (coluna - (n - 1) / 2.0f) * 152;
        }

        void movel() {
            madeiraEscura();
            cubo(X0, X1, 0, 80, Z0, Z1);              // base
            cubo(X0, X1, 285, 300, Z0, Z1);           // prateleiras
            cubo(X0, X1, 505, 520, Z0, Z1);
            cubo(X0, X1, ALTO - 15, ALTO, Z0, Z1);    // tampo
            cubo(X0, X1, 0, ALTO, Z0, Z0 + 15);       // laterais
            cubo(X0, X1, 0, ALTO, Z1 - 15, Z1);
            glColor3f(0.10f, 0.10f, 0.14f);           // fundo escuro, destaca os carros
            cubo(X0, X0 + 8, 80, ALTO - 15, Z0 + 15, Z1 - 15);

            // Fita de LED embaixo de cada prateleira, na beirada da frente.
            semLuz();
            glColor3f(0.85f, 0.92f, 1.0f);
            for (float y : {283.0f, 503.0f, ALTO - 17})
                cubo(X1 - 14, X1 - 6, y - 3, y, Z0 + 15, Z1 - 15);
            comLuz();
        }

    public:
        void atualiza(float dt) {
            giro += 30 * dt;                          // graus por segundo
            if (giro >= 360)
                giro -= 360;
        }

        void desenha() {
            movel();
            for (int i = 0; i < TOTAL; i++) {
                float y, z;
                lugar(i, y, z);

                glColor3f(0.08f, 0.08f, 0.09f);       // pedestal fixo
                glPushMatrix();
                    glTranslatef(CENTRO_X, y, z);
                    glRotatef(-90, 1, 0, 0);
                    glutSolidCylinder(RAIO, PEDESTAL, 32, 1);
                glPopMatrix();


                carros::Veiculo carro(*modelos[i], 0, 0, giro + i * 25);
                carro.estado.luzesLigadas = false;
                glPushMatrix();
                    glTranslatef(CENTRO_X, y + PEDESTAL, z);
                    glScalef(ESCALA, ESCALA, ESCALA);
                    carro.desenhar();
                glPopMatrix();
            }
        }

        // Cupulas transparentes: chamar depois de tudo que e opaco.
        void desenhaVidros() {
            glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            for (int i = 0; i < TOTAL; i++) {
                float y, z;
                lugar(i, y, z);
                glPushMatrix();
                    glTranslatef(CENTRO_X, y + PEDESTAL, z);
                    glRotatef(-90, 1, 0, 0);          // cilindro da GLUT cresce em +z
                    glColor4f(0.70f, 0.85f, 1.0f, 0.15f);
                    glutSolidCylinder(RAIO, CUPULA_A, 32, 1);
                    glDisable(GL_LIGHTING);           // aros marcam onde o vidro esta
                    glColor4f(0.85f, 0.95f, 1.0f, 0.6f);
                    for (float h : {0.5f, CUPULA_A}) {
                        glBegin(GL_LINE_LOOP);
                        for (int k = 0; k < 32; k++) {
                            float a = k * 2 * carros::PI_CARRO / 32;
                            glVertex3f(RAIO * cosf(a), RAIO * sinf(a), h);
                        }
                        glEnd();
                    }
                    glEnable(GL_LIGHTING);
                glPopMatrix();
            }
            glPopAttrib();
        }

        carros::ModeloCarro& modelo(int i) { return *modelos[i]; }

        // Indice do carro mais perto do clique (coordenadas da GLUT), ou -1.
        int carroEm(int mouseX, int mouseY) {
            GLdouble vista[16], projecao[16];
            GLint viewport[4];
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
                glLoadIdentity();
                posicionaCamera();
                glGetDoublev(GL_MODELVIEW_MATRIX, vista);
            glPopMatrix();
            glGetDoublev(GL_PROJECTION_MATRIX, projecao);
            glGetIntegerv(GL_VIEWPORT, viewport);

            int escolhido = -1;
            double menor = 70; // pixels
            for (int i = 0; i < TOTAL; i++) {
                float y, z;
                lugar(i, y, z);
                GLdouble sx, sy, sz;
                gluProject(CENTRO_X, y + PEDESTAL + CUPULA_A / 2, z,
                           vista, projecao, viewport, &sx, &sy, &sz);
                double d = hypot(sx - mouseX, sy - (viewport[3] - mouseY));
                if (d < menor) {
                    menor = d;
                    escolhido = i;
                }
            }
            return escolhido;
        }

        static void posicionaCamera() {
            gluLookAt(-390, 600, (Z0 + Z1) / 2,
                      X0, 330, (Z0 + Z1) / 2,
                      0, 1, 0);
        }
};

Vitrine& vitrine() {
    static Vitrine v;
    return v;
}

static constexpr float ESCALA_CARRO = 8.0f;

// Desenha um Veiculo cujas coordenadas estao em unidades do carro, com origem
// no inicio da pista, em cima do piso (que tem 1 de espessura).
static void desenhaNaPista(carros::Veiculo& v) {
    glPushMatrix();
        glTranslatef(PISTA_X, PISTA_Y + 1, PISTA_Z);
        glScalef(ESCALA_CARRO, ESCALA_CARRO, ESCALA_CARRO);
        v.desenhar();
    glPopMatrix();
}

// Inclinacao (graus) de quem anda na direcao 'angulo' sobre o trecho da pista
// entre a e b: sobe de frente, desce de re.
static float inclinacaoNaPista(const ConstrutorDePista::Ponto& a,
                               const ConstrutorDePista::Ponto& b, float angulo) {
    float tx = b.x - a.x, tz = b.z - a.z, th = hypotf(tx, tz);
    float rad = angulo * carros::PI_CARRO / 180.0f;
    float alinhado = (cosf(rad) * tx - sinf(rad) * tz) / th; // 1 = de frente pro trecho
    return atanf((b.y - a.y) / th * alinhado) * 180.0f / carros::PI_CARRO;
}

class CarroDaCena {
    private:
        carros::CarroSedan modelo; // declarado antes do veiculo, que guarda ponteiro pra ele
        carros::Veiculo veiculo;
        bool primeiraPessoa = false;
        // Passo curto: a distancia ate o ponto mais proximo fica quase a distancia real.
        std::vector<ConstrutorDePista::Ponto> linhaDaPista = ConstrutorDePista::linhaCentral(2);

    public:
        // Comeca no meio da primeira reta, apontando para +x.
        CarroDaCena() : veiculo(modelo, 25.0f / ESCALA_CARRO, 0) {
            veiculo.estado.luzesLigadas = false;
        }

        void desenha() {
            desenhaNaPista(veiculo);
        }

        void tecla(unsigned char tecla, bool apertada) {
            if ((tecla == 'c' || tecla == 'C') && apertada)
                primeiraPessoa = !primeiraPessoa;

            // O carro nao possui mais controle de luzes.
            if (tecla != 'l' && tecla != 'L')
                veiculo.tecla(tecla, apertada);
        }

        void atualiza(float dt) {
            veiculo.atualizar(dt);
            seguePista();
        }

        // Acompanha a altura e a inclinacao do piso e faz a colisao com as
        // muretas: se o centro do carro passa do limite, volta para a borda e
        // perde velocidade, deslizando encostado na mureta.
        void seguePista() {
            // ponytail: meia largura fixa (a do sedan); monster e caminhao raspam um pouco na mureta
            const float limite = ConstrutorDePista::MEIA_LARGURA - 8.0f;
            float x = ESCALA_CARRO * veiculo.x;
            float z = ESCALA_CARRO * veiculo.z;

            size_t n = linhaDaPista.size(), j = 0;
            float menor = 1e9f;
            for (size_t i = 0; i < n; i++) {
                float d = hypotf(x - linhaDaPista[i].x, z - linhaDaPista[i].z);
                if (d < menor) {
                    menor = d;
                    j = i;
                }
            }
            const auto& p = linhaDaPista[j];
            veiculo.y = p.y / ESCALA_CARRO;
            veiculo.inclinacao = inclinacaoNaPista(linhaDaPista[(j + n - 1) % n],
                                                   linhaDaPista[(j + 1) % n], veiculo.angulo);
            if (menor <= limite)
                return;

            veiculo.x = (p.x + (x - p.x) * limite / menor) / ESCALA_CARRO;
            veiculo.z = (p.z + (z - p.z) * limite / menor) / ESCALA_CARRO;
            veiculo.velocidade *= 0.85f;
        }

        void trocaModelo(carros::ModeloCarro& novo) {
            veiculo.modelo = &novo;
        }

        void posicionaCamera() {
            float rad = veiculo.angulo * carros::PI_CARRO / 180.0f;
            float frenteX = cosf(rad);
            float frenteZ = -sinf(rad);
            float carroX = PISTA_X + ESCALA_CARRO * veiculo.x;
            float carroY = PISTA_Y + 1 + ESCALA_CARRO * veiculo.y;
            float carroZ = PISTA_Z + ESCALA_CARRO * veiculo.z;

            if (primeiraPessoa) {
                // Olho do motorista no SRM do carro: cabeca encostada no banco
                // esquerdo (encosto em x = -0.20..-0.10, topo y = 1.20), na altura
                // do topo do volante (y = 1.15), olhando reto. Gira junto com o carro.
                const float olhoX = -0.08f, olhoY = 1.15f, olhoZ = -0.40f;
                float olhoMundoX = carroX + ESCALA_CARRO * (olhoX * frenteX + olhoZ * sinf(rad));
                float olhoMundoZ = carroZ + ESCALA_CARRO * (olhoX * frenteZ + olhoZ * cosf(rad));
                float olhoMundoY = carroY + ESCALA_CARRO * olhoY;
                // Olha junto com a inclinacao da rampa.
                float subida = tanf(veiculo.inclinacao * carros::PI_CARRO / 180.0f);

                gluLookAt(
                    olhoMundoX, olhoMundoY, olhoMundoZ,
                    olhoMundoX + frenteX, olhoMundoY - 0.03f + subida, olhoMundoZ + frenteZ,
                    0.0f, 1.0f, 0.0f
                );
                return;
            }

            gluLookAt(
                carroX - frenteX * 90.0f,
                carroY + 54.0f,
                carroZ - frenteZ * 90.0f,
                carroX + frenteX * 25.0f,
                carroY + 11.0f,
                carroZ + frenteZ * 25.0f,
                0.0f, 1.0f, 0.0f
            );
        }
};

// Carros que andam sozinhos, cada um na sua faixa, seguindo a linha central.
class CarrosAutomaticos {
    private:
        static constexpr float PASSO = 2.0f;
        struct Automatico {
            carros::Veiculo veiculo;
            float s;           // distancia percorrida na volta
            float faixa;       // deslocamento para a direita do meio da pista
            float velocidade;  // unidades da cena por segundo
        };
        std::vector<ConstrutorDePista::Ponto> linha = ConstrutorDePista::linhaCentral(PASSO);
        std::vector<Automatico> carros;

    public:
        // Usa os mesmos modelos da vitrine: esportivo, fusca e hot rod.
        CarrosAutomaticos() {
            const float faixa = ConstrutorDePista::MEIA_LARGURA * 0.55f;
            carros.push_back({carros::Veiculo(vitrine().modelo(4)), 500, -faixa, 75});
            carros.push_back({carros::Veiculo(vitrine().modelo(6)), 900, 0, 55});
            carros.push_back({carros::Veiculo(vitrine().modelo(12)), 1500, faixa, 65});
            for (auto& c : carros)
                c.veiculo.estado.luzesLigadas = false;
            atualiza(0);
        }

        void atualiza(float dt) {
            size_t n = linha.size();
            for (auto& c : carros) {
                c.s = fmodf(c.s + c.velocidade * dt, n * PASSO);
                size_t i = (size_t)(c.s / PASSO) % n, j = (i + 1) % n;
                float f = c.s / PASSO - (int)(c.s / PASSO);
                const auto& a = linha[i];
                const auto& b = linha[j];

                float tx = b.x - a.x, tz = b.z - a.z, th = hypotf(tx, tz);
                float x = a.x + tx * f - tz / th * c.faixa;   // direita = (-tz, tx)
                float z = a.z + tz * f + tx / th * c.faixa;
                auto& v = c.veiculo;
                v.angulo = atan2f(-tz, tx) * 180.0f / carros::PI_CARRO;
                v.x = x / ESCALA_CARRO;
                v.z = z / ESCALA_CARRO;
                v.y = (a.y + (b.y - a.y) * f) / ESCALA_CARRO;
                v.inclinacao = inclinacaoNaPista(a, b, v.angulo);
                v.estado.distancia += c.velocidade * dt / ESCALA_CARRO; // gira as rodas
            }
        }

        void desenha() {
            for (auto& c : carros)
                desenhaNaPista(c.veiculo);
        }
};

CarrosAutomaticos& automaticos() {
    static CarrosAutomaticos a;
    return a;
}

CarroDaCena& carroDaCena() {
    static CarroDaCena carro;
    return carro;
}

void ConstrutorDeCenario::desenha(){
    Chao chao;
    chao.desenha();

    Mesa mesa;
    mesa.desenha();

    // Centraliza a pista e a coloca sobre o tampo.
    ConstrutorDePista pista;
    glPushMatrix();
        glTranslatef(PISTA_X, PISTA_Y, PISTA_Z);
        pista.desenha();
        static DecoracaoDaPista decoracao; // sorteia arvores e postes uma vez so
        decoracao.desenha();
    glPopMatrix();

    Paredes paredes;
    paredes.desenha();
    Teto teto;
    teto.desenha();
    Ventilador ventilador;
    ventilador.desenha();
    Quarto quarto;
    quarto.desenha();
    vitrine().desenha();

    // Vidros misturam com o que ja foi desenhado: carros e cupulas por ultimo.
    automaticos().desenha();
    carroDaCena().desenha();
    vitrine().desenhaVidros();

    glFlush();
}

void ConstrutorDeCenario::luzes() {
    // Noite: pouca luz ambiente, o resto vem das luzes do quarto.
    GLfloat ambiente[] = {0.08f, 0.08f, 0.11f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente);

    luzPontual(GL_LIGHT1, -335, 255, -910, 1.00f, 0.72f, 0.38f, 0.0010f, 0.000006f); // abajur
    luzPontual(GL_LIGHT2,  155, 410, -880, 0.25f, 0.40f, 0.95f, 0.0020f, 0.000020f); // monitor
    luzPontual(GL_LIGHT3,  850, 365, -100, 0.35f, 0.40f, 0.95f, 0.0015f, 0.000010f); // TV
    luzPontual(GL_LIGHT4,  345, 405, -850, 0.70f, 0.06f, 0.04f, 0.0040f, 0.000050f); // gabinete
    luzPontual(GL_LIGHT5, -100, 520, -950, 0.10f, 0.14f, 0.30f, 0.0005f, 0.0f);      // luar pela janela
    luzPontual(GL_LIGHT6, -700, 650, 450, 0.80f, 0.85f, 0.90f, 0.0010f, 0.000002f); // vitrine
}

void ConstrutorDeCenario::tecla(unsigned char tecla, bool apertada) {
    carroDaCena().tecla(tecla, apertada);
}

void ConstrutorDeCenario::atualiza(float dt) {
    carroDaCena().atualiza(dt);
    automaticos().atualiza(dt);
    vitrine().atualiza(dt);
}

void ConstrutorDeCenario::posicionaCamera() {
    carroDaCena().posicionaCamera();
}

void ConstrutorDeCenario::posicionaCameraVitrine() {
    Vitrine::posicionaCamera();
}

void ConstrutorDeCenario::cliqueNaVitrine(int x, int y) {
    int i = vitrine().carroEm(x, y);
    if (i >= 0)
        carroDaCena().trocaModelo(vitrine().modelo(i));
}
