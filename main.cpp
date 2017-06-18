/*
        Departamento de Computação
        Computação Gráfica 2017.1
        Prof: Laurindo de Sousa
        Antonio Fabricio Almeida e Silva
        Editor Gráfico - Trabalho 04
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Biblioteca com funcoes matematicas
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Variaveis Globais

//clicks do mouse necessarios para criar as formas
bool click1 = false, click2 = false, click3 = false;
//booleano de cada forma geometrica usado para saber qual deseja desenhar
bool linha = false, quadrilatero = false, triangulo = false,poligono = false, circulo = false;
//booleanos para saber quando fechar o poligono e o raio do circulo
bool fechaPoligono = false, pegaRaio = false;

double x_1,y_1,x_2,y_2,x_3,y_3;
double raio;

double escalaX = 1, escalaY = 1, translacaoX, translacaoY, fatorCisalhamento = 1, anguloRotacao;
int width = 512, height = 512; //Largura e altura da janela

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto{
    int x;
    int y;
    ponto * prox;
};

//Estrutura para armazenar todos os pontos, para fazer operações
struct coordenadasArmazenadas{
	int x1;
	int y1;
	int x2;
	int y2;
	coordenadasArmazenadas *prox;
};

// Lista encadeada de pontos
// indica o primeiro elemento da lista
ponto * pontos = NULL;
ponto * temp = NULL;

//Lista de coordenadas
coordenadasArmazenadas *listaCoord;

coordenadasArmazenadas * storeCoordenada(int x1, int y1, int x2, int y2){
	coordenadasArmazenadas *coordArmaz;
	coordenadasArmazenadas *coordAux;
	coordArmaz = new coordenadasArmazenadas;
	int ym;
	if(y1<y2){
        ym = y1;
	}else{
        ym = y2;
	}
	int yp;


	coordArmaz->x1 = x1;
	coordArmaz->y1 = y1;
	coordArmaz->x2 = x2;
	coordArmaz->y2 = y2;

	if (listaCoord != NULL){
        if(listaCoord->y1 < listaCoord->y2){
            yp = listaCoord->y1;
        }else{
            yp = listaCoord->y2;
        }
	}

	if ((listaCoord == NULL) || (ym<=yp)) {
		coordArmaz->prox = listaCoord;
		listaCoord = coordArmaz;
		return coordArmaz;
	} else {
		if (listaCoord->prox == NULL) {
			coordArmaz->prox = NULL;
			listaCoord->prox = coordArmaz;
			return listaCoord;
		} else {
			coordAux = listaCoord;
			if(coordAux->prox->y1 < coordAux->prox->y2){
                yp = coordArmaz->prox->y1;
			}else{
                yp = coordAux->prox->y2;
			}

			while ((yp < ym) && (coordAux->prox != NULL)) {
				coordAux = coordAux->prox;
				if (coordAux->prox != NULL){
                    if(coordAux->prox->y1 < coordAux->prox->y2){
                        yp = coordAux->prox->y1;
                    }else{
                        yp = coordAux->prox->y2;
                    }
				}

			}
			coordArmaz->prox = coordAux->prox;
			coordAux->prox = coordArmaz;
			return listaCoord;
		}
	}
}


// Funcao para armazenar um ponto na lista
// Armazena como uma Pilha (empilha)
ponto * pushPonto(int x, int y){
	ponto * pnt;
	pnt = new ponto;
	pnt->x = x;
	pnt->y = y;
	pnt->prox = pontos;
	pontos = pnt;
	return pnt;
}

// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
ponto * popPonto(){
	ponto * pnt;
	pnt = NULL;
	if(pontos != NULL){
		pnt = pontos->prox;
		delete pontos;
		pontos = pnt;
	}
	return pnt;
}

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void menuOpcoes(int value);
void infos();
void limpaTela();

// Funcao que implementa o Algoritmo Imediato para rasterizacao de retas
void retaImediata(double x1,double y1,double x2,double y2);
void bresenham(double x1,double y1,double x2,double y2);
void desenhaQuadrilatero(double x1,double y1,double x2,double y2);
void desenhaTriangulo(double x1,double y1,double x2,double y2,double x3,double y3);
void desenhaCirculo(double x1,double y1, double r);
void escala();
void cisalhamentoEmX();
void cisalhamentoEmY();

// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();

// Funcao Principal do C
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Paint CG"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutDisplayFunc(display); //funcao callback de desenho

    glutCreateMenu(menuOpcoes);
    glutAddMenuEntry("Desenhar Linha", 0);
    glutAddMenuEntry("Desenhar Quadrilatero", 1);
    glutAddMenuEntry("Desenhar Triangulo", 2);
    glutAddMenuEntry("Desenhar Poligono", 3);
    glutAddMenuEntry("Desenhar Circunferencia", 4);
    glutAddMenuEntry("Escala", 5);
    glutAddMenuEntry("Translação", 6);
    glutAddMenuEntry("Rotação", 7);
    glutAddMenuEntry("Cisalhamento em X", 8);
    glutAddMenuEntry("Cisalhamento em Y", 9);
    glutAddMenuEntry("Limpar a tela", 10);
    glutAddMenuEntry("Sair", 11);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop(); // executa o loop do OpenGL
    return 0; // retorna 0 para o tipo inteiro da funcao main();
}


// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
    infos();
}

void reshape(int w, int h){
	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);

	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);

   // muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
   // quando estivermos a desenhar na tela)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    click1 = true; //Para redesenhar os pixels selecionados
    click2 = true;
    if (triangulo)
    	click3 = true;
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;

        //recebe os pontos do poligono
        case 102: //ao apertar a tecla F, o poligono e fechado ligando o ultimo ponto ao ponto inicial
            if (poligono && fechaPoligono) {
				    fechaPoligono = false;
   	   	   	   	    x_2 = x_1;
					y_2 = y_1;
					x_1 = x_3;
	                y_1 = y_3;
	                //printf("x y (%.0f,%.0f)\n",x_1,y_1);
	                glutPostRedisplay();
			}
        break;
    }
}

//Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y){
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
            if (triangulo == true || linha == true || quadrilatero == true){
				if(triangulo && click2) {
					click3 = true;
	                x_3 = x;
	                y_3 = height - y;
	                //printf("x3y3(%.0f,%.0f)\n",x_3,y_3);
	                glutPostRedisplay();
				} else if(click1){
	                click2 = true;
	                x_2 = x;
	                y_2 = height - y;
	                //printf("x2y2(%.0f,%.0f)\n",x_2,y_2);
	                if (triangulo == false){
						glutPostRedisplay();
					}
	            }else{
	                click1 = true;
	                x_1 = x;
	                y_1 = height - y;
	                //printf("x1y1(%.0f,%.0f)\n",x_1,y_1);
	            }
			} else if (poligono) { //Letra E)
				if (fechaPoligono) {
					x_2 = x_1;
					y_2 = y_1;
					x_1 = x;
	                y_1 = height - y;
	                //printf("x y (%.0f,%.0f)\n",x_1,y_1);
	                glutPostRedisplay();
				} else {
					fechaPoligono = true;
					x_1 = x;
	                y_1 = height - y;
	                x_3 = x_1;
	                y_3 = y_1;
	                //printf("x1y1(%.0f,%.0f)\n",x_1,y_1);
				}
			} else if (circulo) {
			    if(click1){
                    double xAux, yAux;
                    pegaRaio = true;
                    x_2 = x;
                    y_2 = height - y;
                    //printf("x2y2(%.0f,%.0f)\n", x_2, y_2);
                    xAux = abs(x_2) - abs(x_1);
                    yAux = abs(y_2) - abs(y_1);
                    raio = sqrt(pow(xAux,2) + pow(yAux,2));
                    //printf("\nraio %.0f\n", raio);
                    glutPostRedisplay();
			    }else{
                    click1 = true;
                    x_1 = x;
                    y_1 = height - y;
                    //printf("x1y1(%.0f,%.0f)\n",x_1,y_1);
			    }
			}
         }
         break;
/*
      case GLUT_MIDDLE_BUTTON:
         if (state == GLUT_DOWN) {
            glutPostRedisplay();
         }
         break;
*/
      default:
         break;
   }
}


// Funcao usada na funcao callback para desenhar na tela
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o Buffer de Cores
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto


    if (triangulo == true || linha == true || quadrilatero == true){
		if(triangulo){
			if(click1 && click2 && click3) {
				desenhaTriangulo(x_1,y_1,x_2,y_2,x_3,y_3);
				drawPontos();
	        	click1 = false;
	        	click2 = false;
	        	click3 = false;
			}
		}else if(click1 && click2){
	        //retaImediata(x_1,y_1,x_2,y_2);
	        if (linha)
				bresenham(x_1,y_1,x_2,y_2);
	        else
				desenhaQuadrilatero(x_1,y_1,x_2,y_2);
			drawPontos();
	        //listaCoord(); //retirar depois
			click1 = false;
	        click2 = false;
	    }
	} else if (poligono) { //Letra E)
		bresenham(x_1,y_1,x_2,y_2);
		drawPontos();
		if (!fechaPoligono) {
			bresenham(x_1,y_1,x_3,y_3);
			drawPontos();
		}
	} else if (circulo && pegaRaio) {
		desenhaCirculo(x_1, y_1, raio);
		drawPontos();
		pegaRaio = false;
		click1 = false;
		click2 = false;
	}
    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas

}

//Funcao que desenha os pontos contidos em uma lista de pontos
void drawPontos(){
    ponto * pnt;
    pnt = pontos;
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
        while(pnt != NULL){
            glVertex2i(pnt->x,pnt->y);
            pnt = pnt->prox;
        }
    glEnd();  // indica o fim do desenho
}



//Funcao callback para inicializar a forma geometrica escolhida no menu
//os cicks e as formas restantes sao inicializadas como false em cada escolha
void menuOpcoes(int value){
    switch(value){
        case 0: //desenhar linha
            linha = true;
			triangulo = false;
			quadrilatero = false;
			poligono = false;
			circulo = false;
			click1 = false;
			click2 = false;
        break;

        case 1://desenhar quadrilatero
            linha = false;
            quadrilatero = true;
			triangulo = false;
			poligono = false;
			circulo = false;
			click1 = false;
			click2 = false;
		break;

        case 2://desenhar triangulo
            linha = false;
            quadrilatero = false;
			triangulo = true;
			poligono = false;
			circulo = false;
			click1 = false;
			click2 = false;
			click3 = false;
		break;

		case 3://desenhar poligono
            linha = false;
            quadrilatero = false;
			triangulo = false;
			poligono = true;
			circulo = false;
			click1 = false;
			click2 = false;
			click3 = false;
		break;

		case 4://desenhar circunferencia
            triangulo = false;
			linha = false;
			quadrilatero = false;
			poligono = false;
			circulo = true;
			click1 = false;
			click2 = false;
        break;

        case 5: //Escala
            triangulo = false;
			linha = false;
			quadrilatero = false;
			poligono = false;
			circulo = false;
            system("cls");
            printf("\n Digite o valor de escala em X: ");
            scanf("%lf", &escalaX);
            printf("\n Digite o valor de escala em Y: ");
            scanf("%lf", &escalaY);
            escala();
            glutPostRedisplay();
        break;

        case 6://Translacao
            triangulo = false;
			linha = false;
			quadrilatero = false;
			poligono = false;
			circulo = false;
            system("cls");
            printf("\n Digite o valor de Translacao em X: ");
            scanf("%lf", &translacaoX);
            printf("\n Digite o valor de Translacao em Y: ");
            scanf("%lf", &translacaoY);
            glutPostRedisplay();
        break;

        case 8://cisalhamento em X
            triangulo = false;
			linha = false;
			quadrilatero = false;
			poligono = false;
			circulo = false;
            system("cls");
            printf("\n Digite o valor de cisalhamento: ");
            scanf("%lf", &fatorCisalhamento);
            cisalhamentoEmX();
            glutPostRedisplay();
        break;

        case 9://cisalhamento em Y
            triangulo = false;
			linha = false;
			quadrilatero = false;
			poligono = false;
			circulo = false;
            system("cls");
            printf("\n Digite o valor de cisalhamento: ");
            scanf("%lf", &fatorCisalhamento);
            cisalhamentoEmY();
            glutPostRedisplay();
        break;

        case 10:
            limpaTela();
		break;

		case 11:
		    exit(0);
        break;
    }

    /* Manda o redesenhar a tela quando o menu for desativado */
    glutPostRedisplay();
}

void infos() {
	system("cls");
	printf("Infos adicionais\nPressione o botao direito na tela para escolher as opcoes de desenho!\n");
}


//Função para limpar a tela a qualquer momento caso o usuario desejar
void limpaTela(){
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    while(pontos != NULL){
        temp = pontos;
        pontos = temp->prox;
        free(pontos);
    }
}


//Algoritmo de Bresenham para rasterizacao de retas(Letra 'A')
void bresenham(double x1,double y1,double x2,double y2){
	double aux1, aux2;
	double d, incE, incNE;
	double dx, dy; //delta X e delta Y
	bool sim = false, dec = false;

	listaCoord = storeCoordenada((int) x1, (int) y1, (int) x2, (int) y2);

	dx = x2 - x1;
	dy = y2 - y1;
	pontos = pushPonto((int)x1,(int)y1);
	pontos = pushPonto((int)x2,(int)y2);

    //Algoritmo para reducao ao primeiro octanto para rasterizacao de retas
    //necessarios para que possa desenhar as formas geometricas por toda a tela
    //Letra 'B'
	if ((dx * dy)<0) {
		y1 = y1*(-1);
		y2 = y2*(-1);
		dy = y2 - y1;
		sim = true;
	}
	if (abs(dx)<abs(dy)) {
		aux1 = y1;
		y1 = x1;
		x1 = aux1;

		aux2 = y2;
		y2 = x2;
		x2 = aux2;

		dx = x2 - x1;
		dy = y2 - y1;

		dec = true;
	}
	if (x1 > x2) {
		aux1 = x1;
		x1 = x2;
		x2 = aux1;

		aux2 = y1;
		y1 = y2;
		y2 = aux2;

		dx = x2 - x1;
		dy = y2 - y1;
	}

    //Continuacao do algoritmo de bresenham apos a reducao
	d = (2 * dy) - dx;
	incE = 2 * dy;
	incNE = 2 * (dy - dx);


	while (x1 < x2) {
		if (d <= 0) {
			aux1 = x1 + 1;
			aux2 = y1;
			d += incE;
		} else {
			aux1 = x1 + 1;
			aux2 = y1 + 1;
			d += incNE;
		}

		x1 = aux1;
		y1 = aux2;

		if (dec) {
			aux1 = y1;
			aux2 = x1;
		}
		if (sim) {
			aux2 = aux2 * (-1);
		}
		pontos = pushPonto((int)aux1,(int)aux2);
	}

}


//Ao receber dois pontos, utiliza o algoritmo de bresenham para desenhar o quadrilatero
//Letra 'C'
void desenhaQuadrilatero(double x1,double y1,double x2,double y2) {
	bresenham(x1, y1, x1, y2);
	bresenham(x1, y2, x2, y2);
	bresenham(x2, y2, x2, y1);
	bresenham(x2, y1, x1, y1);
}

//Ao receber 3 coordenadas na tela, um triangulo e desenhado, utilizando o algoritmo de bresenham
//Letra 'D'
void desenhaTriangulo(double x1,double y1,double x2,double y2,double x3,double y3) {
	bresenham(x1, y1, x2, y2);
	bresenham(x2, y2, x3, y3);
	bresenham(x3, y3, x1, y1);
}

//Algoritmo para desenhar um circulo a partir do raio dado
//Letra 'G'
void desenhaCirculo(double x,double y, double r){
	double d = 1 - r;
	double deltaE = 3.0;
	double deltaSE = (-2*r) + 5;
	double x_aux = 0;
	double y_aux = r;

	//insere em struct de Coordenadas para operações posteriores
	//coord = insCoord((int) x1, (int) y1, 0, 0, (int) r);

	pontos = pushPonto((int)(x),(int)(r + y)); //ponto superior da circunferencia (positivo)
	pontos = pushPonto((int)(x),(int)(-r + y)); //ponto inferior da circunferencia (negativo)
	pontos = pushPonto((int)(r + x),(int)(y));  //ponto extremo direito da circunferencia (positivo)
	pontos = pushPonto((int)(-r + x),(int)(y)); ////ponto extremo esquerdo da circunferencia (negativo)

	x_aux += 1;
	while(y_aux > x_aux) {
		if (d < 0) {
			d += deltaE;
			deltaE += 2;
			deltaSE += 2;

		} else if(d >= 0){
			d += deltaSE;
			deltaE += 2;
			deltaSE = deltaSE + 4;
			y_aux--;
		}

		//marcando os 8 pixeis da circunferencia coorespondente as coordenadas
		pontos = pushPonto((int)(x_aux + x),(int)(y_aux + y));
		pontos = pushPonto((int)(-x_aux + x),(int)(y_aux + y));
		pontos = pushPonto((int)(-x_aux + x),(int)(-y_aux + y));
		pontos = pushPonto((int)(x_aux + x),(int)(-y_aux + y));
		pontos = pushPonto((int)(y_aux + x),(int)(x_aux + y));
		pontos = pushPonto((int)(-y_aux + x),(int)(x_aux + y));
		pontos = pushPonto((int)(-y_aux + x),(int)(-x_aux + y));
		pontos = pushPonto((int)(y_aux + x),(int)(-x_aux + y));
		x_aux++;
	}
}

void escala() {
	coordenadasArmazenadas *coordArmazenadas;
	coordArmazenadas = listaCoord;
	listaCoord = NULL;

	while(coordArmazenadas != NULL) {
		coordArmazenadas->x1 *= escalaX;
		coordArmazenadas->y1 *= escalaY;
		coordArmazenadas->x2 *= escalaX;
		coordArmazenadas->y2 *= escalaY;

		bresenham(coordArmazenadas->x1, coordArmazenadas->y1, coordArmazenadas->x2, coordArmazenadas->y2);
		drawPontos();


		coordArmazenadas = coordArmazenadas->prox;
	}
	escalaX = 1;
	escalaY = 1;
}

void cisalhamentoEmX() {
	coordenadasArmazenadas *coordArmazenadas;
	coordArmazenadas = listaCoord;
	listaCoord = NULL;

	while(coordArmazenadas != NULL) {
        coordArmazenadas->x1 += (fatorCisalhamento * coordArmazenadas->y1);
        coordArmazenadas->x2 += (fatorCisalhamento * coordArmazenadas->y2);

		bresenham(coordArmazenadas->x1, coordArmazenadas->y1, coordArmazenadas->x2, coordArmazenadas->y2);

		drawPontos();

		coordArmazenadas = coordArmazenadas->prox;
	}
	fatorCisalhamento = 1;
}

void cisalhamentoEmY() {
	coordenadasArmazenadas *coordArmazenadas;
	coordArmazenadas = listaCoord;
	listaCoord = NULL;

	while(coordArmazenadas != NULL) {
        coordArmazenadas->y1 += (fatorCisalhamento * coordArmazenadas->x1);
        coordArmazenadas->y2 += (fatorCisalhamento * coordArmazenadas->x2);

		bresenham(coordArmazenadas->x1, coordArmazenadas->y1, coordArmazenadas->x2, coordArmazenadas->y2);

		drawPontos();

		coordArmazenadas = coordArmazenadas->prox;
	}

	fatorCisalhamento = 1;
}


