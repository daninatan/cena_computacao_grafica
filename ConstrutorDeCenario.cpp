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
#include "carro/CarroSUV.h"
#include "carro/CarroSedan.h"
#include "carro/CarroVan.h"
#include "carro/Veiculo.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <random>

//casa
class Chao{
    public:
        void desenha(){
            glBegin(GL_QUADS);
                bool change = false;
                for (int z = -1000; z < 1000; z += 100) {
                    if (change)
                        glColor3f(0.729f, 0.549f, 0.278f);
                    else
                        glColor3f(0.922f, 0.694f, 0.353f);

                    glNormal3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(-1000.0f, -0.1f, z);
                    glVertex3f(-1000.0f, -0.1f, z + 100);
                    glVertex3f( 1000.0f, -0.1f, z + 100);
                    glVertex3f( 1000.0f, -0.1f, z);

                    change = !change;
                }
            glEnd();
        }
};

class Paredes{
    public:
        void desenha(){
            glColor3f(0.969, 0.969, 0.969);
            glBegin(GL_QUADS);
                glNormal3f(1, 0, 0);
                glVertex3f(-1000.0f, -0.1f, -1000);
                glVertex3f( -1000.0f, 300.0f, -1000);
                glVertex3f( -1000.0f, 300.0f, 1000);
                glVertex3f(-1000.0f, -0.1f, 1000);

                glNormal3f(0, 0, -1);
                glVertex3f(-1000.0f, -0.1f, 1000);
                glVertex3f( -1000.0f, 300.0f, 1000);
                glVertex3f( 1000.0f, 300.0f, 1000);
                glVertex3f(1000.0f, -0.1f, 1000);

                glNormal3f(-1, 0, 0);
                glVertex3f(1000.0f, -0.1f, 1000);
                glVertex3f( 1000.0f, 300.0f, 1000);
                glVertex3f( 1000.0f, 300.0f, -1000);
                glVertex3f(1000.0f, -0.1f, -1000);

                glNormal3f(0, 0, 1);
                glVertex3f(1000.0f, -0.1f, -1000);
                glVertex3f( 1000.0f, 300.0f, -1000);
                glVertex3f( -1000.0f, 300.0f, -1000);
                glVertex3f(-1000.0f, -0.1f, -1000);

                glColor3f(0.373, 0.722, 0.961);

                //paredes azuis
                glNormal3f(1, 0, 0);
                glVertex3f(-1000.0f, 300.0, -1000);
                glVertex3f( -1000.0f, 800.0f, -1000);
                glVertex3f( -1000.0f, 800.0f, 1000);
                glVertex3f(-1000.0f, 300.0f, 1000);

                glNormal3f(0, 0, -1);
                glVertex3f(-1000.0f, 300.0, 1000);
                glVertex3f( -1000.0f, 800.0f, 1000);
                glVertex3f( 1000.0f, 800.0f, 1000);
                glVertex3f(1000.0f, 300.0f, 1000);

                glNormal3f(-1, 0, 0);
                glVertex3f(1000.0f, 300.0, 1000);
                glVertex3f( 1000.0f, 800.0f, 1000);
                glVertex3f( 1000.0f, 800.0f, -1000);
                glVertex3f(1000.0f, 300.0f, -1000);

                glNormal3f(0, 0, 1);
                glVertex3f(1000.0f,300.0f, -1000);
                glVertex3f( 1000.0f, 800.0f, -1000);
                glVertex3f( -1000.0f, 800.0f, -1000);
                glVertex3f(-1000.0f, 300.0f, -1000);
            glEnd();
        }
};

class Teto{
    public:
        void desenha(){
          glBegin(GL_QUADS);
            glColor3f(0.612, 0.502, 0);

            glNormal3f(0, -1, 0);
            glVertex3f(1000.0f,800.0f, 1000);
            glVertex3f(-1000.0f, 800.0f, 1000);
            glVertex3f(-1000.0f, 800.0f, -1000);
            glVertex3f(1000.0f, 800.0f, -1000);
          glEnd();  
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

class CarroAleatorio {
    private:
        static constexpr float ESCALA = 8.0f;
        static constexpr float POSICAO_X = -55.0f;
        static constexpr float POSICAO_Y = 171.0f;
        static constexpr float POSICAO_Z = -295.0f;

        carros::Veiculo veiculo;

        static carros::ModeloCarro& sorteiaModelo() {
            static carros::CarroCaminhao caminhao;
            static carros::CarroCaminhonete caminhonete;
            static carros::CarroConversivel conversivel;
            static carros::CarroEsportivo esportivo;
            static carros::CarroFusca fusca;
            static carros::CarroHatch hatch;
            static carros::CarroHotRod hotRod;
            static carros::CarroJipe jipe;
            static carros::CarroMonstro monstro;
            static carros::CarroPerua perua;
            static carros::CarroSUV suv;
            static carros::CarroSedan sedan;
            static carros::CarroVan van;

            static carros::ModeloCarro* modelos[] = {
                &caminhao, &caminhonete, &conversivel, &esportivo,
                &fusca, &hatch, &hotRod, &jipe, &monstro, &perua,
                &suv, &sedan, &van
            };

            // O sorteio acontece somente na primeira vez que a cena e desenhada.
            static std::mt19937 gerador(std::random_device{}());
            static std::uniform_int_distribution<int> sorteio(0, 12);
            static int escolhido = sorteio(gerador);

            return *modelos[escolhido];
        }

    public:
        CarroAleatorio() : veiculo(sorteiaModelo()) {
            veiculo.estado.luzesLigadas = false;
        }

        void desenha() {
            glPushMatrix();
                // Primeira reta da pista, com o carro apontando para +x.
                glTranslatef(POSICAO_X, POSICAO_Y, POSICAO_Z);
                glScalef(ESCALA, ESCALA, ESCALA);
                veiculo.desenhar();
            glPopMatrix();
        }

        void tecla(unsigned char tecla, bool apertada) {
            // O carro nao possui mais controle de luzes.
            if (tecla != 'l' && tecla != 'L')
                veiculo.tecla(tecla, apertada);
        }

        void atualiza(float dt) {
            veiculo.atualizar(dt);
        }

        void posicionaCamera() {
            float rad = veiculo.angulo * carros::PI_CARRO / 180.0f;
            float frenteX = cosf(rad);
            float frenteZ = -sinf(rad);
            float carroX = POSICAO_X + ESCALA * veiculo.x;
            float carroZ = POSICAO_Z + ESCALA * veiculo.z;

            gluLookAt(
                carroX - frenteX * 90.0f,
                POSICAO_Y + 55.0f,
                carroZ - frenteZ * 90.0f,
                carroX + frenteX * 25.0f,
                POSICAO_Y + 12.0f,
                carroZ + frenteZ * 25.0f,
                0.0f, 1.0f, 0.0f
            );
        }
};

CarroAleatorio& carroDaCena() {
    static CarroAleatorio carro;
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
        glTranslatef(-80.0f, 170.0f, -295.0f);
        pista.desenha();
    glPopMatrix();

    Paredes paredes;
    paredes.desenha();
    Teto teto;
    teto.desenha();
    Ventilador ventilador;
    ventilador.desenha();

    carroDaCena().desenha();

    glFlush();
}

void ConstrutorDeCenario::tecla(unsigned char tecla, bool apertada) {
    carroDaCena().tecla(tecla, apertada);
}

void ConstrutorDeCenario::atualiza(float dt) {
    carroDaCena().atualiza(dt);
}

void ConstrutorDeCenario::posicionaCamera() {
    carroDaCena().posicionaCamera();
}
