/////////////////////////////////////////////////////////////////////////////////////////
// Autor:       David Osollo
// Programa:    crypt_kicker
// Descripcion: C Implementacion para desencriptar mensajes
// Maestria:    MCC
// Materia:     Analisis y Diseño de Algoritomos
// C++ implementation Crypt Kicker
// compilation: g++ -Wall -g -w crypt_kicker.cpp -o crypt_kicker

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

#define CHUNK_SIZE           80000
#define SIZE_UTF8_AUX        3
#define PATRON_ARRAY_SIZE    200
#define CLAVE_ARRAY_SIZE     100
#define TEXT_DESENC          200
#define LINE_ZIZE            300
#define ONE_BYTES_CHAR       0x80
#define THREE_BYTES_CHAR     0xE2

char gAbcdario[]    = "abcdefghijklmnopqrstuvwxyzñüáéíóú";
char gClave[CLAVE_ARRAY_SIZE];
char gClaveDin[CLAVE_ARRAY_SIZE];
char gTextDesenc[]  = "el veloz murciélago hindú comía feliz cardillo y kiwi cuando la cigüeña tocaba el saxofón detrás del palenque de paja";
FILE *gpFileLog;


int  patronEspacios[PATRON_ARRAY_SIZE];
int  patronA[PATRON_ARRAY_SIZE];
int  patronE[PATRON_ARRAY_SIZE];
int  patronI[PATRON_ARRAY_SIZE];
int  patronO[PATRON_ARRAY_SIZE];
int  patronU[PATRON_ARRAY_SIZE];
int  iPosMatch=0;

bool desencriptar(char *sTextEnc, char *sTextConoEnc ,char *sTextDesen, char *sTextConoDesen, int iSize);
void regresaCharDesen(char *sCharEnc,char *sCharDesenc);
bool isExtendedChar(char c);
void regresaCharLogPos(int iLogPosTarget, const char *sText, char *sChar);
void buscarPatron(char *sBuscar, int arrPatron[]);
void initArrays();
bool buscarClave(char *sTextEnc);
int  verificarPatron(const char *sText, int patron[],char c_espacio);
bool buildKey(char *sTextConoEnc);
int buscarChar(const char *sText, char *sCharBusc);
void trim(char *string);
void procesarTexto(char *sText);

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: trim
/// Desc:    Eliminar los espacios de las orillas
/////////////////////////////////////////////////////////////////////////////////
void trim(char *string)
{
    
    char trimed_string[LINE_ZIZE+1];
    int j,i=0;
    
    while(i<strlen(string) && string[i]==' ')
    {
        i++;
    }
    
    j=strlen(string) -1;
    while(j>0 && string[j]==' ')
    {
        j--;
    }
    
    if(j>i)
    {
        strncpy(trimed_string,string+i,j-i+1);
        strcpy(string,trimed_string);
    }
    else if(i>j) strcpy(string,"");
    
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: initArrays
/// Desc:    Inicializar los arreglos de patron de letras
/////////////////////////////////////////////////////////////////////////////////
void initArrays()
{
    int i=0;
    for(i=0;i<PATRON_ARRAY_SIZE;i++)
    {
        patronEspacios[i]=-1;
        patronA[i]=-1;
        patronE[i]=-1;
        patronI[i]=-1;
        patronO[i]=-1;
        patronU[i]=-1;
    }
    
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: desencriptar
/// Desc:    Desencriptar el texto
/// Param:   sTextEnc           - Texto encriptado
///          sTextConoEnc       - Texto supuesto conocido encriptado
///          sTextDesen         - Texto desencriptado
///          sTextConoDesen     - Texto Conocido desencriptado
///          iSize              - Tamaño logico del texto conocido
/////////////////////////////////////////////////////////////////////////////////
bool desencriptar(char *sTextEnc, char *sTextConoEnc ,char *sTextDesen, char *sTextConoDesen, int iSize)
{
    int i=0;
    int iLogPos=0;
    char arr_utf8_aux[SIZE_UTF8_AUX];
    
    while(i<strlen(sTextEnc))
    {
        if(isExtendedChar(sTextEnc[i]))
        {
            regresaCharDesen(sTextEnc+i,arr_utf8_aux);
            i=i+2;
        }
        else
        {
            regresaCharDesen(sTextEnc+i,arr_utf8_aux);
            i++;
        }
        
        if((sTextEnc +i) > sTextConoEnc && iLogPos < iSize)
        {
            if(strcmp(arr_utf8_aux,"\n"))
            {
                strcat(sTextConoDesen,arr_utf8_aux);
            }
            else
            {
                strcat(sTextConoDesen," ");
            }
            iLogPos++;
            
        }
        else
        {
            strcat(sTextDesen,arr_utf8_aux);
        }
    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: regresaCharDesen
/// Desc:    Regresa un char desenciptado
/// Param:   sCharEnc           - Char encriptado
///          sCharDesenc        - Char desencriptado
/////////////////////////////////////////////////////////////////////////////////
void regresaCharDesen(char *sCharEnc,char *sCharDesenc)
{
    
    int i;
    int iLogicalPos=0;
    
    memset(sCharDesenc,'\0',sizeof(char)*SIZE_UTF8_AUX);
    
    if(*sCharEnc == ' ' || *sCharEnc == '\n')
    {
        strncpy(sCharDesenc,sCharEnc,1);
        return ;
    }
    
    for(i=0; i < strlen(gClave); i++)
    {
        if(isExtendedChar(gClave[i]))
        {
            if(*sCharEnc == gClave[i] && *(sCharEnc+1) == gClave[i+1])
            {
                strncpy(sCharDesenc,gClave+i,2);
                regresaCharLogPos(iLogicalPos, gAbcdario, sCharDesenc);
                return ;
            }
            i++;
        }
        else
        {
            if(*sCharEnc == gClave[i])
            {
                strncpy(sCharDesenc,gClave+i,1);
                regresaCharLogPos(iLogicalPos, gAbcdario, sCharDesenc);
                return ;
            }
        }
        iLogicalPos++;
    }
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: regresaCharLogPos
/// Desc:    Regresa la posicion un caracter en una posicion logica
/// Param:   iLogPosTarget      - Posicion del caracter a regresar
///          sText              - Texto a buscar el caracter
///          sChar              - Caracter encontado en la posicion
/////////////////////////////////////////////////////////////////////////////////
void regresaCharLogPos(int iLogPosTarget, const char *sText, char *sChar)
{
    int i=0;
    int iLogPos=0;
    
    memset(sChar,'\0',sizeof(char)*SIZE_UTF8_AUX);
    
    while(i < strlen(sText) && iLogPos!=iLogPosTarget)
    {
        iLogPos++;
        if(isExtendedChar(sText[i])) i=i+2;
        else i++;
    }
    
    if(isExtendedChar(sText[i]))
    {
        strncpy(sChar,sText+i,2);
    }
    else
    {
        strncpy(sChar,sText+i,1);
    }
    
}


/////////////////////////////////////////////////////////////////////////////////
/// Funcion: isExtendedChar
/// Desc:    Determina si es un caracter Extendido
/// Param:   c       - Caracter a evaluar
/////////////////////////////////////////////////////////////////////////////////
bool isExtendedChar(char c)
{
    
    if((unsigned int)c > ONE_BYTES_CHAR )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: buscarPatron
/// Desc:    Busca las posiciones de una letra de acuerdo al texto conocido
///          y los guarda en el arreglo
/// Param:   sBuscar       - Texto desencriptado
///          arrPatron     - Arreglo a guardar la posoicion de los caracter
///                          repetidos
/////////////////////////////////////////////////////////////////////////////////
void buscarPatron(char *sBuscar, int arrPatron[])
{
    int i;
    int iLogPos=0;
    int iMatch=0;
    
    for(i=0; i < strlen(gTextDesenc); i++)
    {
        if(isExtendedChar(gTextDesenc[i]))
        {
            
            if(isExtendedChar(sBuscar[0]))
            {
                if(*sBuscar == gTextDesenc[i] && *(sBuscar+1) == gTextDesenc[i+1])
                {
                    arrPatron[iMatch]=iLogPos;
                    iMatch++;
                }
            }
            i++;
        }
        else if(!isExtendedChar(gTextDesenc[i]) && !isExtendedChar(sBuscar[0]))
        {
            if(*sBuscar == gTextDesenc[i])
            {
                arrPatron[iMatch]=iLogPos;
                iMatch++;
            }
        }
        iLogPos++;
    }
    
    i=0;
    while(arrPatron[i] != -1)
    {
        i++;
    }
}


/////////////////////////////////////////////////////////////////////////////////
/// Funcion: buscarClave
/// Desc:    Busca la Clave y desencripta
///
/// Param:   sTextEnc      - Texto Encriptado
/////////////////////////////////////////////////////////////////////////////////
bool buscarClave(char *sTextEnc)
{
    int i=0;
    bool bMatch = false;
    char *sTextDesenc = (char *) malloc(sizeof(char) * CHUNK_SIZE);
    char *sTextConoDesen = (char *) malloc(sizeof(char) * CHUNK_SIZE);
    memset(sTextDesenc,'\0',sizeof(char) * CHUNK_SIZE);
    memset(sTextConoDesen,'\0',sizeof(char) * CHUNK_SIZE);
    
    for(i=0;i<strlen(sTextEnc);i++)
    {
        if(verificarPatron(sTextEnc+i,patronA,'a')==1)
        {
            if(verificarPatron(sTextEnc+i,patronE,'e')         == 1 &&
               verificarPatron(sTextEnc+i,patronI,'i')         == 1 &&
               verificarPatron(sTextEnc+i,patronO,'o')         == 1 &&
               verificarPatron(sTextEnc+i,patronU,'u')         == 1 &&
               verificarPatron(sTextEnc+i,patronEspacios,' ')  == 1)
            {
                if( buildKey(sTextEnc+i) )
                {
                    fprintf(gpFileLog,"\nPatron Concuerda\n");
                    desencriptar(sTextEnc,sTextEnc+i,sTextDesenc, sTextConoDesen,117);
                    fprintf(gpFileLog,"\nTexto Conocido \n[%s]\n",sTextConoDesen);
                    fprintf(gpFileLog,"\nTexto Desencriptado conocido \n[%s]\n",gTextDesenc);
                    if(!strcmp(sTextConoDesen,gTextDesenc))
                    {
                        bMatch = true;
                        break;
                    }
                    else fprintf(gpFileLog,"\nTexto conocido no Concuerda\n");
                }
            }
        }
    }

    if(bMatch == true)
    {
        printf("\nTexto Desencriptado:\n");
        printf("\n%s\n",sTextDesenc);
        fprintf(gpFileLog,"\nTexto Desencriptado:\n");
        fprintf(gpFileLog,"\n%s\n",sTextDesenc);
    }
    else
    {
        printf("\nNO SE ENCONTRO SOLUCIÓN  \n");
        fprintf(gpFileLog,"\nNO SE ENCONTRO SOLUCIÓN  \n");
    }
    
    free(sTextDesenc);
    free(sTextConoDesen);
    return true;
    
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: buildKey
/// Desc:    Obtiene la clave para desencriptar
///
/// Param:   sTextConoEnc   - Supuesto Texto conocido encriptado
/////////////////////////////////////////////////////////////////////////////////
bool buildKey(char *sTextConoEnc)
{
    
    int i,iLogPos;
    char arr_utf8_buscar[SIZE_UTF8_AUX];
    char arr_utf8_desenc[SIZE_UTF8_AUX];
    memset(gClave,'\0',sizeof(char) * CLAVE_ARRAY_SIZE);
    
    //Todas la letras del abcedario
    for(i=0;i<strlen(gAbcdario);i++)
    {
        
        memset(arr_utf8_buscar,'\0',sizeof(char) * SIZE_UTF8_AUX);
        if(isExtendedChar(gAbcdario[i]))
        {
            strncpy(arr_utf8_buscar,gAbcdario+i,2);
            iLogPos = buscarChar(gTextDesenc, arr_utf8_buscar);
            i++;
        }
        else
        {
            strncpy(arr_utf8_buscar,gAbcdario+i,1);
            iLogPos = buscarChar(gTextDesenc, arr_utf8_buscar);
            
        }
        
        
        regresaCharLogPos(iLogPos, sTextConoEnc, arr_utf8_desenc);
        
        //Invalido no se acepta sustitucion por el mismo caracter
        if(!strcmp(arr_utf8_buscar,arr_utf8_desenc)) return false;
        
        strcat(gClave,arr_utf8_desenc);
        
        
    }// End for
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: buscarChar
/// Desc:    Busca un caracter en un texto y regresa la posicion logica
///
/// Param:   sText     - Texto donde buscar el caracter
///          sCharBusc - Caracter a buscar
/// Return:            - Regresa la posicion logica
/////////////////////////////////////////////////////////////////////////////////
int buscarChar(const char *sText, char *sCharBusc)
{
    int i=0;
    int iLogPos=0;
    
    while(i < strlen(sText))
    {
        if(isExtendedChar(sText[i]))
        {
            if(sText[i]==sCharBusc[0] && sText[i+1]==sCharBusc[1])
            {
                return iLogPos;
            }
            i++;
        }
        else
        {
            if(sText[i]==sCharBusc[0])
            {
                return iLogPos;
            }
        }
        iLogPos++;
        i++;
    }
    
    return -1;
    
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: verificarPatron
/// Desc:    Verificar que la la letra o espacio concuerde
///
/// Param:   sText     - Texto donde buscar el patroin
///          spatron   - Arreglo con posiciones de caracter
///          c_espacio - Indica si es un espacio
/// Return:            - Regresa si se cumple con el patron
/////////////////////////////////////////////////////////////////////////////////
int verificarPatron(const char *sText, int patron[],char c_espacio)
{
    
    int i=0;
    int j=0;
    int iMatch = 0;
    bool bCharBuscado = true;
    char arr_utf8_aux[SIZE_UTF8_AUX];
    char arr_utf8_buscar[SIZE_UTF8_AUX];
    
    while(i<strlen(sText))
    {
        
        memset(arr_utf8_buscar,'\0',sizeof(char)*SIZE_UTF8_AUX);
        if(isExtendedChar(sText[i]))
        {
            strncpy(arr_utf8_buscar,sText+i,2);
        }
        else
        {
            strncpy(arr_utf8_buscar,sText+i,1);
        }
        
        if(strlen(arr_utf8_buscar)>1)
        {
            if(sText[i]==arr_utf8_buscar[0] && sText[i+1]== arr_utf8_buscar[1])
                bCharBuscado = true;
            else
                bCharBuscado = false;
        }
        else
        {
            if(sText[i]==arr_utf8_buscar[0])
                bCharBuscado = true;
            else
                bCharBuscado = false;
        }
        
        iMatch = 0;
        
        
        if(bCharBuscado == true || (c_espacio==' ' && sText[i]== '\n'))
        {
            j=0;
            while(patron[j] != -1)
            {
                
                regresaCharLogPos(patron[j], sText, arr_utf8_aux);
                
                if(c_espacio==' ' && arr_utf8_aux[0]!=' ' && arr_utf8_aux[0]!='\n')
                {
                    iMatch = - 1;
                    break;
                }
                else if(c_espacio != ' ' && strcmp(arr_utf8_buscar,arr_utf8_aux))
                {
                    iMatch = -1;
                    break;
                }
                
                j++;
            }
            
            if(iMatch!=-1) return 1;
        }
        
        if(strlen(arr_utf8_buscar)>1) i++;
        i++;
    }
    
    return iMatch;
    
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: procesarTexto
/// Desc:    Buca la llave y desencripta
///
/// Param:   sText     - Texto a desencriptar
///
/////////////////////////////////////////////////////////////////////////////////
void procesarTexto(char *sText)
{
    
    
    char strPatVoc[3];
    if(!sText)
    {
        fprintf(stderr, "Initial allocation failed.\n");
        return;
    }
    
    initArrays();
    
    strcpy(strPatVoc," ");
    buscarPatron(strPatVoc, patronEspacios);
    
    strcpy(strPatVoc,"a");
    buscarPatron(strPatVoc, patronA);
    
    strcpy(strPatVoc,"e");
    buscarPatron(strPatVoc, patronE);
    
    strcpy(strPatVoc,"i");
    buscarPatron(strPatVoc, patronI);
    
    strcpy(strPatVoc,"o");
    buscarPatron(strPatVoc, patronO);
    
    strcpy(strPatVoc,"u");
    buscarPatron(strPatVoc, patronU);
    
    //Obtener llave
    buscarClave(sText);
    
}

/////////////////////////////////////////////////////////////////////////////////
/// Funcion: main
/// Desc:    Lee Archivo de entrada y procesa el numero de casos
///
/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    
    system("@cls||clear");
    char sLineToProcess[LINE_ZIZE];
    
    char *sFileText = (char *) malloc(sizeof(char) * CHUNK_SIZE);
    
    memset(sFileText,'\0',sizeof(char) * CHUNK_SIZE);
    memset(sLineToProcess,'\0',sizeof(char) * LINE_ZIZE);
    
    
    int iNumCases=0;
    int iCase=0;
    bool bNewCase=false;
    
    
    gpFileLog = fopen("crypt_kicker.log","w+");
    
    printf("\nArchivo de entrada: %s \n",argv[1]);
    fprintf(gpFileLog,"\nArchivo de entrada: %s \n",argv[1]);
    
    std::ifstream infile(argv[1]);
    std::string line;
    
    while (std::getline(infile, line))
    {
        
        if(!iNumCases)
        {
            strcpy(sLineToProcess, line.c_str());
            trim(sLineToProcess);
            iNumCases=atoi(sLineToProcess);
            printf("\nNumero de casos a Processar= %d ",iNumCases);
            fprintf(gpFileLog,"\nNumero de casos a Processar= %d ",iNumCases);
        }
        else
        {
            
            strcpy(sLineToProcess, line.c_str());
            strcat(sFileText,sLineToProcess);
            strcat(sFileText,"\n");
            
            trim(sLineToProcess);
            if(!strcmp(sLineToProcess,""))
            {
                if(bNewCase)
                {
                    iCase++;
                    printf("\n\n===================================================================== ");
                    printf("\nCaso Numero %d : \n\n", iCase);
                    printf("\nTexto Encriptado: \n\n%s \n\n", sFileText);
                    
                    fprintf(gpFileLog,"\n\n===================================================================== ");
                    fprintf(gpFileLog,"\nCaso Numero %d : \n\n", iCase);
                    fprintf(gpFileLog,"\nTexto Encriptado: \n\n%s \n\n", sFileText);
                    procesarTexto(sFileText);
                    memset(sFileText,'\0',sizeof(char) * CHUNK_SIZE);
                    bNewCase=false;
                }
            }
            else bNewCase=true;
        }
    }
    
    if(bNewCase==true && iNumCases > 0)
    {
        iCase++;
        printf("\n\n===================================================================== ");
        printf("\nCaso Numero %d : \n\n", iCase);
        printf("\nTexto Encriptado: \n\n%s \n\n", sFileText);
        
        fprintf(gpFileLog,"\n\n===================================================================== ");
        fprintf(gpFileLog,"\nCaso Numero %d : \n\n", iCase);
        fprintf(gpFileLog,"\nTexto Encriptado: \n\n%s \n\n", sFileText);
        procesarTexto(sFileText);
        
    }
    
    if(iNumCases == 0) printf("\nNO HAY CASOS PARA PROCESAR VERIFIQUE EL ARCHIVO DE ENTRADA: \n");
    
    printf("\n\n");
    
    fclose (gpFileLog);
    free(sFileText);
    
    return 0;
} 
