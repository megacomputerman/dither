////////////////////////////////////////////////////////////////////////////////////
//
// Project   : Algoritmo dither
//
// Author    : Wallace Costa
//
// Date      : 23/10/2017
//
// Descript. : Aplica o dither em uma imagem .bmp
//
//
////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

#define BRANCO 255
#define PRETO 0

void log( string strlog ); // Cria o log de execução

int** CriarMatriz   (int x,   int y);         // Cria Matriz dinâmicamente
int** DestruirMatriz(int** M, int x, int y);  // Libera a memória alocada pela matriz
void  ImprimirMatriz(int** M, int x, int y);  // Imprime os valores de uma matriz

int** CriarMatrizDither(int N);
void  GsetLineColum (string line, int* x, int* y);  // Captura as dimensões da matriz
int   GetValue      (string line);                  // Captura o valor da matriz numa posição x,y
int** CarregarMatrizImagem(char* fileName);         // Carrega a matriz imagem de um arquivo txt
int** DitherOrdenado( int** D, int ** I );          // Aplica o algoritmo de dither

int   ReplaceValue   (string line, char* value); // Substitui os valores da matriz O pra o novo arquivo
int   SalvarResultado(char* fileName, int** O);  // Salva em um novo arquivo a matriz resultante do dither ordernado

int linhas, colunas;

string g_strBuffer;
char   g_achBuffer[100];

int main(int argc,char* argv[])
{

  int iSize = 0;

  int** D; // Matriz Dither
  int** I; // Matriz Imagem
  int** O; // Matriz Resultante
 
  log ( "\n------ Nova execução ------\n\n");

  if(argc <= 1)
  {
    printf("\nInforme o nome do arquivo com extensão,\nfaça assim: %s nome_arquivo.extensão\n\n", argv[0] );
    return -1;
  }

  if ( argv[2] != NULL )
  {
    iSize = atoi( argv[2] );
  }
  else
  {
    iSize = 4;
  }

  puts( "Carregando matriz imagem" );
  I = CarregarMatrizImagem( argv[1] );
  if (I == NULL) {
    printf("\nErro, ao carregar matriz imagem\n");
    return -1;
  }
  log( "\nMatriz imagem:\n" );
  ImprimirMatriz( I, linhas, colunas );
  log( "\n--------------------\n" );

  puts( "Criando matriz dither" );
  D = CriarMatrizDither( iSize );
  if (D == NULL) {
    printf("\nErro, ao criar matriz dither\n");
    return -1;
  }

  puts( "Aplicando dither ordenado" );
  O = DitherOrdenado( D, I );
  if (O == NULL) {
    log( "Erro durante a ordenação\n" );
    return -1;
  }
  
  puts( "Salvando o resultado" );
  SalvarResultado( argv[1], O );

  // Libera o espaço em memória alocado pela matriz
  DestruirMatriz(I, linhas, colunas);
  DestruirMatriz(D, linhas, colunas);
  DestruirMatriz(O, linhas, colunas);

  puts( "Terminou com sucesso" );

  return 0;
}


void log( string strLog )
{
  ofstream file( "log_dither.txt", ofstream::out | ofstream::app );
  if (!file)
  {
    perror("Erro ao abrir o arquivo ");
    return;
  }

  file << strLog;

  file.close();
}


int** CriarMatriz(int Linhas,int Colunas)
{ 
  int i,j;
  int **m = (int**)malloc(Linhas * sizeof(int*)); // Aloca um Vetor de Ponteiros

  for (i = 0; i < Linhas; i++){
    
    m[i] = (int*) malloc(Colunas * sizeof(int)); // Aloca um Vetor de Inteiros para cada posição do Vetor de Ponteiros.

    for (j = 0; j < Colunas; j++){
      
      m[i][j] = 0;
      
    }
  }
  return m; //Retorna o Ponteiro para a Matriz Alocada
}


void ImprimirMatriz( int** M, int linha, int coluna )
{
  int x, y;

  if ( (M == NULL) || (linha < 1) || (coluna < 1)) {
    printf("\nErro parâmetros inválidos: DestruirMatriz)\n");
    exit(1);
  }

  for (y = 0 ; y < coluna ; y++) {
    
    printf("\n");
    log( "\n" );
    
    for (x = 0 ; x < linha ; x++) {
    
      sprintf( g_achBuffer, "\(%d,%d\) = %d\n", x, y, M[x][y] );
      printf( g_achBuffer );
      log( g_achBuffer );
    
    }
  }
}


int **DestruirMatriz(int **M, int linha, int coluna ) {
  int x;

  if ((linha < 1) || (coluna < 1)) {

    printf("\nErro, parâmetro inválido (func: DestruirMatriz)\n");
    exit(1);
   
   }

  if (M == NULL) return NULL;
  
  for (x = 0 ; x < linha ; x++) free(M[x]);
    free(M);
   
   return NULL;
}


int** CriarMatrizDither(int N)
{
  int** D  = NULL;
  int** DD = NULL;
  int x=0, y=0, i=0, j=0;

  const unsigned dim = 1 << N;

  // Cria matriz dither
  D = CriarMatriz(dim, dim);
  if (D == NULL) {
    log("\nErro ao criar matriz\n");
    return D;
  }

  sprintf(g_achBuffer, "\nMatriz dither %ux%u:\n\n", dim, dim);
  log( g_achBuffer );
  for(unsigned y=0; y<dim; ++y)
  {
    log( "   " );
    for(unsigned x=0; x<dim; ++x)
    {
      unsigned v = 0, mask = N-1, xc = x ^ y, yc = y;

      for(unsigned bit=0; bit < 2*N; --mask)
      { 
        v |= ((yc >> mask)&1) << bit++;
        v |= ((xc >> mask)&1) << bit++;
      }
      sprintf(g_achBuffer, "%5d", v);
      log( g_achBuffer );
      D[y][x] = v;
      
    }
    log(" |");
    if(y == 0)
    {
      sprintf( g_achBuffer, "1/%u", dim * dim ); 
      //printf(" 1/%u", dim * dim);
      log( g_achBuffer );
    }
    log("\n");
  }
  log( "\n--------------------\n" );

  // Ajusta a matriz dither a dimensão da imagem
  DD = CriarMatriz(linhas, colunas);
  if (DD == NULL) {
    log("Erro ao criar matriz\n");
    return D;
  }
  for (y = 0 ; y < colunas ; y++) {
    if( j == (N-1) ) j = 0;
    for (x = 0 ; x < linhas ; x++) {
      if( i == (N-1) ) i = 0;
      DD[x][y] = D[i][j];
      i++;
    }
    j++;
  }
  DestruirMatriz( D, dim, dim );
  
  return DD;
}


void GetLineColum( string line, int* x, int* y )
{
  string strAux;
  int iPosition = 0;
  
  iPosition = line.find(':') + 2;
  strAux = line.substr( iPosition, ( line.find(',') - iPosition ) );
  *x = atoi( strAux.c_str() );

  iPosition = line.find(',') + 1;
  strAux = line.substr( iPosition, ( line.find(',', iPosition) - iPosition ) );
  *y = atoi( strAux.c_str() );

}


int GetValue( string line )
{
  int iPosition = 0;
  string strAux;

  iPosition = line.find('(') + 1;
  strAux = line.substr( iPosition, (line.find( ',', 5 ) - iPosition) );
  
  return atoi(strAux.c_str());
}


int** CarregarMatrizImagem(char* fileName)
{
  int** I = NULL;
  int   x = 0;
  int   y = 0;

  string strBuff;

  // Abre arquivo para leitura
  ifstream file( fileName );
  if (!file)
  {
    perror("Erro ao abrir o arquivo ");
    return I;
  }

  // Ler o tamanho da matriz
  getline( file, strBuff );
  GetLineColum(strBuff, &linhas, &colunas );
  if ((linhas < 1) || (colunas < 1)) {
    printf("\nErro, linha ou coluna inválidos\n");
    return I;
  }

  I = CriarMatriz(linhas, colunas);
  // testa se a matriz foi alocada corretamente, e se nao foi, aborta a
  // execução do programa.
  if (I == NULL) {
    printf("\nErro, ao criar matriz\n");
    return I;
  }

  puts( "Preenchendo a matriz Imagem\n" );
  // Preenche a matriz Imagem 
  for (y = 0 ; y < colunas ; y++) {
    for (x = 0 ; x < linhas ; x++) {
      getline( file, strBuff );
      I[x][y] = GetValue( strBuff );
    }
  }
  file.close();

  puts( "Carregou matriz Imagem" );
  return I;
}


int** DitherOrdenado( int** D, int** I )
{
  int** O = NULL;
  int   x, y, i, j;
  
  O = CriarMatriz(linhas, colunas);
  if (O == NULL) {
    printf("\nErro, ao criar matriz resultante\n");
    return O;
  }

  log( "\nAplicando algoritmo dither ordenado:\n\n" );
  for (x = 0 ; x < linhas ; x++) {
    for (y = 0 ; y < colunas ; y++) {

      i = x%linhas;
      j = y%linhas;
      
      sprintf( g_achBuffer, "x=%d y=%d i =%d j =%d\n", x,y,i,j );
      log( g_achBuffer );
      sprintf( g_achBuffer, "%d > %d\n", I[x][y], D[i][j] );
      log( g_achBuffer );
      if ( I[x][y] > D[i][j] )
      {
        O[x][y] = BRANCO;
      }
      else
      {
        O[x][y] = PRETO;
      }
    }
  }

  puts( "aplicou dither" );
  return O;
}


int ReplaceValue( string line, char* szValue )
{
  string aux;
  aux = line.substr(0, (line.find( '(' ) + 1) );
  aux = aux + szValue + "," + szValue + "," + szValue;
  aux = aux + line.substr( line.find( ')' ) );
  g_strBuffer = aux + "\n";

  return 0;
}


int SalvarResultado( char* fileName, int** O )
{
  string line;
  char szValue[10];
  int x, y, iValue;
 
  ifstream inFile(fileName);
  if (!inFile)
  {
    perror("Erro ao abrir o arquivo ");
    system("pause");
    return -1;
  }
  
  line = fileName;
  line = line.substr( 0, line.find('.') );
  line += "_new.txt";

  ofstream outFile( line.c_str() );
  if (!outFile)
  {
    perror("Erro ao abrir o arquivo ");
    system("pause");
    return -1;
  }

  getline(inFile, line);

  outFile << line << "\n";

  // Save matrix O on file 
  for (y = 0 ; y < colunas ; y++) {
    for (x = 0 ; x < linhas ; x++) {
      
      sprintf(szValue,"%d", O[x][y] );
      getline(inFile, line);
      ReplaceValue( line, szValue );

      outFile << g_strBuffer;
     }
  }

  inFile.close();
  outFile.close();

  return 0;

}
