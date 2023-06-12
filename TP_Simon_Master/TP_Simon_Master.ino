#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

const short MAX = 100; //Máximo de bytes a recibir
const short ESCLAVO = 1; //Dirección del esclavo
char secuenciaActual[MAX];
char resultado; //guarda el resultado de la comparacion 'v' o 'f'

char tecla;
int jugadores=0;
int jugadorActual=1;
int colores=0;
int reintentos=0;
int secuencia=0;
int tiempo=3000;

//Columnas y filas
const byte FILAS=4;
const byte COLS=4;
const byte MAX_CHARS=16;
const byte MAX_ROWS=2;

//Mapa de teclas
char teclas[FILAS][COLS]{
  '1','2','3','A',
  '4','5','6','B',
  '7','8','9','C',
  '*','0','#','D'
};

//array de filas y columnas
byte pinesFilas[FILAS]={12,11,10,9};
byte pinesCols[COLS]={8,7,6,5};

//mapeo de teclas
Keypad kp= Keypad(makeKeymap(teclas),pinesFilas,pinesCols,FILAS,COLS);

//pines
const byte RS=A1;
const byte EN=A0;
const byte BD4=A2;
const byte BD5=A3;
const byte BD6=2;
const byte BD7=3;

//Creacion de objeto LCD
LiquidCrystal lcd(RS,EN,BD4,BD5,BD6,BD7);

void setup()
{
  Wire.begin(); //Inicia como maestro
  //inicializa el lcd
  lcd.begin(MAX_CHARS, MAX_ROWS);
  lcd.setCursor(0,0);
}

void loop()
{
  cantidadDeJugadores();
  nivel();
  juego();
}

void cantidadDeJugadores(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Nro Jugadores");
  lcd.setCursor(0,1);
  lcd.print("1.Uno - 2.Dos");
  tecla=kp.getKey();
  while(tecla==NO_KEY){
    tecla=kp.getKey();
    switch(tecla){
      case '1':jugadores=1;break;
      case '2':jugadores=2;break;
      default:;
    }
  }
}

void cambiarJugador(){
  if(jugadorActual==1){
    jugadorActual=2;
  }
  else{
    jugadorActual=1;
  }
}

void nivel(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Nivel: A.Bajo");
  lcd.setCursor(0,1);
  lcd.print("B.Medio C.Alto");
  tecla=kp.getKey();
  while(tecla==NO_KEY){
    tecla=kp.getKey();
    switch(tecla){
      case 'A':{
        colores=3;
        reintentos=3;
      };break;
      case 'B':{
        colores=4;
        reintentos=2;
      };break;
      case 'C':{
        colores=5;
        reintentos=1;
      };break;
      default:;
    }
  }
}

void juego(){
  boolean jugar=true;
  while(reintentos>0){
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("J:");
     lcd.print(jugadorActual);
     lcd.print(" NroEs:");
     lcd.print(secuencia+1);
     lcd.setCursor(0,1);
     lcd.print("Reintentos: ");
     lcd.print(reintentos);
     if(jugar==true){
       //genera un numero aleatorio
       int num=random(1,colores);
       secuenciaActual[secuencia]+=num+'0';
       Wire.beginTransmission(ESCLAVO);
       Wire.write(secuenciaActual[secuencia]); //Envío el aleatorio    
       Wire.endTransmission();
     }  
     if(jugadores==2){
      cambiarJugador();
     }
     secuencia++;
     //le da un tiempo al jugador para que pueda jugar
     delay(tiempo);
     tiempo=tiempo+1000;
     //pide el resultado al slave
     Wire.requestFrom(ESCLAVO, MAX);     
     if(Wire.available()>0 ){
        resultado = (char)Wire.read(); 
      }
     if(resultado=='f'){
        reintentos--;
        jugar=false;
     }
    if(resultado=='v'){
      jugar=true;
    }
  }
  
  if(reintentos==0){
    if(jugadores==1){
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("FIN DEL JUEGO!");
       lcd.setCursor(0,1);
       lcd.print("Puntaje: ");
       lcd.print(secuencia-1);
    }
    if(jugadores==2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("GANADOR J");
      lcd.print(jugadorActual);
      lcd.print("!");
      lcd.setCursor(0,1);
      lcd.print("Puntaje: ");
      lcd.print(secuencia-1);
    }
  }
 //avisa al slave que el juego termino
 Wire.beginTransmission(ESCLAVO);
 Wire.write('t');   
 Wire.endTransmission();
 for(int j=0;j<secuencia;j++){
     secuenciaActual[j]=NULL;
  } 
 secuencia=0;
 tiempo=3000;
 while(tecla!='#'){
    tecla=kp.getKey();
 } 
 
}