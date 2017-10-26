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
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;


int** CriarMatriz   (int x,   int y);         // Cria Matriz dinâmicamente
int** DestruirMatriz(int **M, int x, int y);  // Libera a memória alocada pela matriz
void  ImprimirMatriz(int** M, int x, int y);  // Imprime os valores de uma matriz

int** CriarMatrizDither(int N);
void  GetLineColum  ( string line, int* x, int* y );
int   GetValue      ( string line );
int   SaveResult    (char* fileName);           // Gera novo arquivo já aplicado dither
int   ReplaceValue  (string line, char* value); // Substitui os valores da matriz O pra o novo arquivo

int **O;
int linhas, colunas;

string g_strBuffer;
string g_strFile;

int main(int argc,char* argv[])
{

  int   x, y, i, j;
  int   iValue = 0;
  char  chNo = 0;
  
  int** matriz;
  int** D;
  
  string strBuff;

  if(argc <= 1)
  {
    printf("\nInforme o nome do arquivo com extensão,\nfaça assim: %s nome_arquivo.extensão\n\n", argv[0] );
    return -1;
  }

  ifstream arq( argv[1] );
  if (!arq)
  {
    perror("Erro ao abrir o arquivo ");
    system("pause");
    return -1;
  }

  // Ler o tamanho da matriz
  getline( arq, strBuff );
  GetLineColum(strBuff, &linhas, &colunas );

  if ((linhas < 1) || (colunas < 1)) {
    printf("\nErro, linha ou coluna inválidos\n");
    exit(1);
  }

  matriz = CriarMatriz(linhas, colunas);
  // testa se a matriz foi alocada corretamente, e se nao foi, aborta a
  // execução do programa.
  if (matriz == NULL) {
    printf("\nErro, ao criar matriz\n");
    exit(1);
  }

  if ( argv[2] != NULL )
  {
    x = atoi( argv[2] );
  }
  else
  {
    x = 4;
  }
  
  D = CriarMatrizDither( x );
  if (D == NULL) {
    printf("\nErro, ao criar matriz\n");
    exit(1);
  }
  
  ImprimirMatriz( D, x*x, x*x );

  O = CriarMatriz(linhas, colunas);
  if (O == NULL) {
    printf("\nErro, ao criar matriz\n");
    exit(1);
  }

  puts( "Preenchendo a matriz Imagem\n" );

  // Preenche a matriz Imagem 
  for (y = 0 ; y < colunas ; y++) {
    for (x = 0 ; x < linhas ; x++) {
      
      getline( arq, strBuff );
      matriz[x][y] = GetValue( strBuff );
    }
  }

  arq.close();

  puts( "aplicando dither ordenado\n" );
  // aplicando dither ordenado
  for (x = 0 ; x < linhas ; x++) {
    for (y = 0 ; y < colunas ; y++) {

      i = x%linhas;
      j = y%linhas;
      
      printf( "x=%d y=%d i = %d j = %d\n", x,y,i,j );
      //printf( "%d > %d\n", matriz[x][y], D[i][j] );
      if ( matriz[x][y] > D[i][j] )
      {
        O[x][y] = 1;
      }
      else
      {
        O[x][y] = 0;
      }
    }
  }

  puts( "aplicou dither" );

  ImprimirMatriz( matriz, linhas, colunas );
  ImprimirMatriz( O, linhas, colunas );

  puts( "Imprimiu as matrizes" );

  SaveResult( argv[1] );

  puts( "Salvou o resultado" );

   // Libera o espaço em memória alocado pela matriz
  DestruirMatriz(matriz, linhas, colunas);
  DestruirMatriz(O, linhas, colunas);

  return 0;
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
    
    for (x = 0 ; x < linha ; x++) {
    
      printf("\(%d,%d\) = %d\n", x, y, M[x][y] );
    
    }
  }
}


int **DestruirMatriz(int **mat, int linhas, int colunas ) {
  int x;

  if ((linhas < 1) || (colunas < 1)) {

    printf("\nErro, parâmetro inválido (func: DestruirMatriz)\n");
    exit(1);
   
   }

  if (mat == NULL) return NULL;
  
  for (x = 0 ; x < linhas ; x++) free(mat[x]);
    free(mat);
   
   return NULL;   
}

int** CriarMatrizDither(int N)
{
 int** D  = NULL;
 const unsigned dim = 1 << N;

 D = CriarMatriz(dim, dim);
 if (D == NULL) {
  printf("\nErro ao criar matriz\n");
  return NULL;
}

//printf(" X=%u, Y=%u:\n", dim,dim);
for(unsigned y=0; y<dim; ++y)
{
  //printf("   ");
  for(unsigned x=0; x<dim; ++x)
  {
    unsigned v = 0, mask = N-1, xc = x ^ y, yc = y;

    for(unsigned bit=0; bit < 2*N; --mask)
    { 
      v |= ((yc >> mask)&1) << bit++;
      v |= ((xc >> mask)&1) << bit++;
    }
    //printf("%4d", v);
    D[y][x] = v;
    
  }
  //printf(" |");
  if(y == 0) printf(" 1/%u", dim * dim);
  //printf("\n");
}

return D;

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


int SaveResult( char* fileName )
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

int ReplaceValue( string line, char* szValue )
{
  string aux;
  aux = line.substr(0, (line.find( '(' ) + 1) );
  aux = aux + szValue;
  aux = aux + line.substr( line.find( ',', 5 ) );
  g_strBuffer = aux + "\n";

  return 0;
}