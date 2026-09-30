
#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cstdlib> // Adicione no início do arquivo

#include "ConstrutorDeCenario.h"

// Proporção da janela
GLfloat fAspect = 1.0f;
bool cameraGeral = false;

ConstrutorDeCenario construtor;


// --------------------------------------
// Configuração da câmera
// --------------------------------------
void EspecificaParametrosVisualizacao(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70, fAspect, 1.0, 5000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (cameraGeral) {
        // Visao em perspectiva de toda a cena.
        gluLookAt(
            850.0f, 700.0f, 850.0f,
            0.0f, 200.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        );
    } else {
        construtor.posicionaCamera();
    }
}


// --------------------------------------
// Desenho da cena
// --------------------------------------
void Desenha(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configura a câmera
    EspecificaParametrosVisualizacao();

    // A luz pontual fica no centro da esfera do ventilador.
    GLfloat posicaoLuz[] = {
        0.0f, 710.0f, 0.0f, 1.0f
    };

    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    // Desenha a pista com iluminação
    glEnable(GL_LIGHTING);

    construtor.desenha();

    // Desenha os eixos sem iluminação
    glDisable(GL_LIGHTING);

    glLineWidth(2.0f);

    glBegin(GL_LINES);

        // Eixo X - vermelho
        glColor3f(1, 0, 0);
        glVertex3f(-10000, 0, 0);
        glVertex3f(10000, 0, 0);

        // Eixo Y - verde
        glColor3f(0, 1, 0);
        glVertex3f(0, -10000, 0);
        glVertex3f(0, 1000, 0);

        // Eixo Z - azul
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, -10000);
        glVertex3f(0, 0, 10000);

    glEnd();

    glutSwapBuffers();
}


// --------------------------------------
// Inicialização
// --------------------------------------
void Inicializa(void)
{
    // Fundo branco
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0);

    // Suavização da iluminação
    glShadeModel(GL_SMOOTH);

    // Mantem corretas as normais dos objetos que usam glScalef.
    glEnable(GL_NORMALIZE);

    // Iluminação
    GLfloat luzAmbiente[] = {
        0.22f, 0.20f, 0.16f, 1.0f
    };

    GLfloat luzDifusa[] = {
        1.0f, 0.90f, 0.68f, 1.0f
    };

    GLfloat luzEspecular[] = {
        1.0f, 0.95f, 0.80f, 1.0f
    };

    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0005f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0000005f);

    // Material
    GLfloat especularMaterial[] = {
        0.8f, 0.8f, 0.8f, 1.0f
    };

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_SPECULAR,
        especularMaterial
    );

    glMaterialf(
        GL_FRONT_AND_BACK,
        GL_SHININESS,
        60.0f
    );

    // Permite usar glColor3f com iluminação
    glColorMaterial(
        GL_FRONT_AND_BACK,
        GL_AMBIENT_AND_DIFFUSE
    );

    glEnable(GL_COLOR_MATERIAL);

    // Iluminação nos dois lados das faces
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
}


// --------------------------------------
// Controles do carro
// --------------------------------------
void keyboard(unsigned char key, int, int)
{
    if (key == 27)
        std::exit(0);

    if (key == 'v' || key == 'V') {
        cameraGeral = !cameraGeral;
        glutPostRedisplay();
        return;
    }

    if (key == 'n' || key == 'N'){
        if(glIsEnabled(GL_LIGHT0)){
            glDisable(GL_LIGHT0);
        }else {
            glEnable(GL_LIGHT0);
        }
    }

    construtor.tecla(key, true);
}

void keyboardUp(unsigned char key, int, int)
{
    construtor.tecla(key, false);
}


// --------------------------------------
// Redimensionamento da janela
// --------------------------------------
void AlteraTamanhoJanela(int w, int h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w / (GLfloat)h;

    glutPostRedisplay();
}

// Mantem a animacao do ventilador em aproximadamente 60 FPS.
void AtualizaCena(int)
{
    construtor.atualiza(0.016f);
    glutPostRedisplay();
    glutTimerFunc(16, AtualizaCena, 0);
}


// --------------------------------------
// Main
// --------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH
    );

    glutInitWindowPosition(300, 0);
    glutInitWindowSize(1200, 1000);

    glutCreateWindow("Pista Hot Wheels");

    Inicializa();

    // Registra os callbacks
    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutIgnoreKeyRepeat(1);
    glutTimerFunc(16, AtualizaCena, 0);

    glutMainLoop();

    return 0;
}
