#pragma once
// =====================================================================================
//  PRIMITIVAS BÁSICAS: todo SRM de carro é montado só com isso.
// =====================================================================================
#include <GL/freeglut.h>
#include <cmath>

namespace carros {

const float PI_CARRO = 3.14159265f;

// Transparência precisa de 2 passadas: opacos primeiro, vidros depois.
// Cada primitiva olha isso e só desenha na passada dela. Quem controla é o Veiculo.
inline bool passadaDoVidro = false;

inline void cor(float r, float g, float b) { glColor3f(r, g, b); }

// Caixa de x0 a x1, y0 a y1, z0 a z1 (cubo da GLUT transladado e esticado).
inline void caixa(float x0, float x1, float y0, float y1, float z0, float z1) {
    if (passadaDoVidro) return;                                  // opaca: só na 1ª passada
    glPushMatrix();
    glTranslatef((x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2);
    glScalef(fabsf(x1 - x0), fabsf(y1 - y0), fabsf(z1 - z0));
    glutSolidCube(1);
    glPopMatrix();
}

// Barra inclinada ligando (x0, y0) a (x1, y1) no plano z.
inline void barra(float x0, float y0, float x1, float y1, float z, float espessura) {
    if (passadaDoVidro) return;
    float dx = x1 - x0, dy = y1 - y0;
    float comprimento = sqrtf(dx * dx + dy * dy);
    float angulo = atan2f(dy, dx) * 180 / PI_CARRO;
    glPushMatrix();
    glTranslatef((x0 + x1) / 2, (y0 + y1) / 2, z);
    glRotatef(angulo, 0, 0, 1);
    glScalef(comprimento, espessura, espessura);
    glutSolidCube(1);
    glPopMatrix();
}

// Normal de uma face: produto vetorial de duas arestas (a->b) x (a->c).
inline void normalDaFace(const float a[3], const float b[3], const float c[3]) {
    float u[3] = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
    float v[3] = {c[0] - a[0], c[1] - a[1], c[2] - a[2]};
    glNormal3f(u[1] * v[2] - u[2] * v[1],
               u[2] * v[0] - u[0] * v[2],
               u[0] * v[1] - u[1] * v[0]);
}

// Vidro: quadrilátero azulado e semitransparente (só na 2ª passada).
inline void vidro(const float a[3], const float b[3], const float c[3], const float d[3]) {
    if (!passadaDoVidro) return;
    glColor4f(0.35f, 0.55f, 0.70f, 0.45f);
    normalDaFace(a, b, c);
    glBegin(GL_QUADS);
    glVertex3fv(a); glVertex3fv(b); glVertex3fv(c); glVertex3fv(d);
    glEnd();
}

// QUARTO DE ELIPSE esticado em z (capô arredondado): superfície curva + tampas laterais.
// Origem na base da borda ALTA; a curva desce pra frente até (comp, 0):
//   x = comp*sen(a), y = alt*cos(a), a de 0 a 90 graus.
// Pra curvar pra trás (-x), gire 180° em y no SRU.
// tampas = false: só a casca curva, oca por dentro (ex.: paralama por cima da roda).
inline void quartoElipse(float comp, float alt, float larg, bool tampas = true) {
    if (passadaDoVidro) return;
    const int N = 16;
    float z = larg / 2;

    // SUPERFÍCIE CURVA: -z primeiro, depois +z -> face da frente aponta pra fora
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N; i++) {
        float a = i * (PI_CARRO / 2) / N;
        glNormal3f(sinf(a) / comp, cosf(a) / alt, 0);
        glVertex3f(comp * sinf(a), alt * cosf(a), -z);
        glVertex3f(comp * sinf(a), alt * cosf(a),  z);
    }
    glEnd();
    if (!tampas) return;

    // TAMPAS laterais: no lado +z percorre a curva ao contrário pra ficar anti-horário
    for (int lado = -1; lado <= 1; lado += 2) {
        glNormal3f(0, 0, lado);
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, lado * z);
        for (int i = 0; i <= N; i++) {
            int k = (lado > 0) ? N - i : i;
            float a = k * (PI_CARRO / 2) / N;
            glVertex3f(comp * sinf(a), alt * cosf(a), lado * z);
        }
        glEnd();
    }
}

// PERFIL EXTRUDADO: a SILHUETA LATERAL de uma peça (vista de lado), esticada em z.
// Entre x0 e x1 (x0 < x1), cima(x) e baixo(x) dão a altura da borda de cima e de baixo.
// Como baixo(x) é livre, dá pra recortar caixa de roda (arco) no meio da peça.
// larg = largura total em z, centrada na origem. Fecha tudo: cima, baixo, laterais e pontas.
template <class Cima, class Baixo>
void perfilExtrudado(float x0, float x1, float larg, Cima cima, Baixo baixo, int n = 48) {
    if (passadaDoVidro) return;
    float z = larg / 2, e = (x1 - x0) / n;
    auto X = [&](int i) { return x0 + (x1 - x0) * i / n; };

    // CIMA: normal perpendicular à inclinação (-dy, dx). -z depois +z -> aponta pra fora
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) {
        float x = X(i);
        glNormal3f(-(cima(x + e) - cima(x - e)), 2 * e, 0);
        glVertex3f(x, cima(x), -z);
        glVertex3f(x, cima(x),  z);
    }
    glEnd();
    // BAIXO: normal pra baixo; +z depois -z
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) {
        float x = X(i);
        glNormal3f(baixo(x + e) - baixo(x - e), -2 * e, 0);
        glVertex3f(x, baixo(x),  z);
        glVertex3f(x, baixo(x), -z);
    }
    glEnd();
    // LATERAIS: faixa entre a borda de baixo e a de cima (ordem invertida em cada lado)
    glNormal3f(0, 0, 1);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) { glVertex3f(X(i), cima(X(i)), z);   glVertex3f(X(i), baixo(X(i)), z); }
    glEnd();
    glNormal3f(0, 0, -1);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) { glVertex3f(X(i), baixo(X(i)), -z); glVertex3f(X(i), cima(X(i)), -z); }
    glEnd();
    // PONTAS: trás (x0) e frente (x1)
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(x0, baixo(x0), -z); glVertex3f(x0, baixo(x0), z); glVertex3f(x0, cima(x0), z); glVertex3f(x0, cima(x0), -z);
    glNormal3f(1, 0, 0);
    glVertex3f(x1, baixo(x1), z); glVertex3f(x1, baixo(x1), -z); glVertex3f(x1, cima(x1), -z); glVertex3f(x1, cima(x1), z);
    glEnd();
}

// Altura numa tabela de pontos {x, y} com x crescente (interpolação linear).
// Serve pra descrever uma silhueta por alguns pontos-chave.
inline float interpola(float x, const float t[][2], int n) {
    if (x <= t[0][0]) return t[0][1];
    for (int i = 1; i < n; i++)
        if (x <= t[i][0]) {
            float f = (x - t[i - 1][0]) / (t[i][0] - t[i - 1][0]);
            return t[i - 1][1] + f * (t[i][1] - t[i - 1][1]);
        }
    return t[n - 1][1];
}

// Caixa que BRILHA por EMISSÃO quando acesa (farol = branco, senão lanterna vermelha).
// A emissão TEM que voltar a zero depois, senão tudo desenhado depois brilharia também.
inline void lampada(bool acesa, bool farol, float x0, float x1, float y0, float y1, float z0, float z1) {
    GLfloat branca[]   = {1, 1, 0.85f, 1};
    GLfloat vermelha[] = {0.9f, 0, 0, 1};
    GLfloat apagada[]  = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, !acesa ? apagada : farol ? branca : vermelha);
    if (farol) cor(0.9f, 0.9f, 0.8f); else cor(0.8f, 0.05f, 0.05f);
    caixa(x0, x1, y0, y1, z0, z1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, apagada);
}

// =====================================================================================
//  COMPONENTES COMUNS (qualquer carro usa)
// =====================================================================================

inline GLUquadric* quadrica() { static GLUquadric* q = gluNewQuadric(); return q; }

// RODA - SRM: origem no centro. Pneu (toro, raio externo 0.35) em volta do eixo z e
// calota (disco) um pouco para +z, que é o lado de FORA no modelo.
inline void rodaSRM() {
    if (passadaDoVidro) return;
    cor(0.08f, 0.08f, 0.08f);
    glutSolidTorus(0.10, 0.25, 16, 32);                    // tubo 0.10, anel 0.25 -> raio 0.35
    glPushMatrix();
    glTranslatef(0, 0, 0.06f);
    cor(0.75f, 0.75f, 0.78f);
    gluDisk(quadrica(), 0, 0.17, 24, 1);
    cor(0.2f, 0.2f, 0.22f);                                // cruz na calota: sem ela o giro não aparece
    caixa(-0.16f, 0.16f, -0.025f, 0.025f, 0, 0.015f);
    caixa(-0.025f, 0.025f, -0.16f, 0.16f, 0, 0.015f);
    glPopMatrix();
}

// RODA - SRU: centro na altura do raio (pneu encosta no chão), esterçada pela direção
// e girando conforme a distância que o carro andou (rolar sem escorregar: ângulo = d / raio).
// No lado esquerdo (z < 0) gira 180° pra calota ficar pra fora. Roda maior = escala.
inline void rodaSRU(float x, float z, float direcao, float distancia, float escala = 1) {
    float raio = 0.35f * escala;
    glPushMatrix();
    glTranslatef(x, raio, z);                                         // 5) leva até o lugar
    glRotatef(direcao, 0, 1, 0);                                      // 4) esterça em volta do eixo vertical
    glRotatef(-distancia / raio * 180 / PI_CARRO, 0, 0, 1);          // 3) rola em volta do eixo (z): pra frente = sentido horário
    if (z < 0) glRotatef(180, 0, 1, 0);                               // 2) espelha pro lado esquerdo
    glScalef(escala, escala, escala);                                 // 1) tamanho
    rodaSRM();
    glPopMatrix();
}

// Spots dos faróis (GL_LIGHT1/2, brancos, pra frente e pra baixo) e da lanterna
// (GL_LIGHT3, vermelho, fraco). Posições com w = 1, no SRM do carro.
// ponytail: índices fixos -> só um carro por vez ilumina a cena; parametrizar se precisar de mais
inline void spotsDoCarro(bool ligadas, float xFrente, float xTras, float y, float zFarol) {
    GLfloat branca[]   = {1, 1, 0.85f, 1};
    GLfloat vermelha[] = {0.7f, 0, 0, 1};
    GLfloat paraFrente[] = {1, -0.25f, 0};
    GLenum farois[2] = {GL_LIGHT1, GL_LIGHT2};
    for (int i = 0; i < 2; i++) {
        GLfloat pos[] = {xFrente, y, i == 0 ? -zFarol : zFarol, 1};
        glLightfv(farois[i], GL_POSITION, pos);
        glLightfv(farois[i], GL_SPOT_DIRECTION, paraFrente);
        glLightfv(farois[i], GL_DIFFUSE, branca);
        glLightfv(farois[i], GL_SPECULAR, branca);
        glLightf(farois[i], GL_SPOT_CUTOFF, 30);            // abertura do cone
        glLightf(farois[i], GL_SPOT_EXPONENT, 10);          // mais forte no centro
    }
    GLfloat trasPos[] = {xTras, y, 0, 1};
    GLfloat paraTras[] = {-1, -0.35f, 0};
    glLightfv(GL_LIGHT3, GL_POSITION, trasPos);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, paraTras);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, vermelha);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 50);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 5);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.6f);

    GLenum todas[3] = {GL_LIGHT1, GL_LIGHT2, GL_LIGHT3};
    for (GLenum l : todas) ligadas ? glEnable(l) : glDisable(l);
}

} // namespace carros
