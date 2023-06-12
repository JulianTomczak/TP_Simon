#include <Wire.h>

const byte MAX = 100;
const byte ESCLAVO = 1;

//Las variables que se modifican en interrupciones
//deben ser de tipo volatile
volatile boolean recibido = false;

byte secuencia[MAX];
int i=0;
volatile char acierto='f';

void setup()
{
  Wire.begin(ESCLAVO); //Inicio como esclavo
  DDRD=B00000000;
  DDRB=B111110;
  PORTB=B000000;
  //La función a ejecutar al recibir datos
  Wire.onReceive(atenderOrden);
  //La función a ejecutar al recibir solicitudes
  Wire.onRequest(atenderPeticion);
  
}

void loop()
{ 
  if (recibido==true){
    // Espera a que se reciba la secuencia del Arduino maestro
    reproducirSecuencia();
    jugar();
    recibido=false;
  }
}

//detecta los botones que pulsa el jugador
//guarda la secuencia, la compara con la del master 
//si es distinta acierto toma el valor f
void jugar(){
  char pulso='0';
  char secuenciaJugador[i];
  int j=0;
  while (j < i) {
    while ((digitalRead(7) && digitalRead(6) &&
             digitalRead(5) && digitalRead(4) &&
             digitalRead(2))) {
      // Espera a que se pulse un botón
    }
    if (!digitalRead(7)) {
      pulso = '1';
    }else if (!digitalRead(6)) {
      pulso = '2';
    }else if (!digitalRead(5)) {
      pulso = '3';
    }else if (!digitalRead(4)) {
      pulso = '4';
    }else if (!digitalRead(2)) {
      pulso = '5';
    }
    luzYSonido(pulso);
    delay(500);
    secuenciaJugador[j] = pulso;
    j++;
  }
  acierto='v';
  for(int j=0;j<i;j++){
    if(secuencia[j]!=secuenciaJugador[j]){
      acierto='f';
    }
  }
}

//envia el resultado de la comparacion de las secuencias
void atenderPeticion(){
  Wire.write(acierto);
  recibido=true;
}

//Copia la secuencia que llega del master
//Si llega t significa que el juego termino y borra la secuencia
void atenderOrden(int k){
  while (Wire.available() > 0)
  {
    secuencia[i] = Wire.read();
    i++;
  } 
  if(secuencia[i-1]=='t'){
    for(int j=0;j<i;j++){
      secuencia[j]=NULL;
    }
    i=0;
  }else{
    recibido=true;
  }
}

//reproduce la secuencia que llega del master
void reproducirSecuencia(){
  for(int j=0;j<i;j++){
    luzYSonido(secuencia[j]);
    delay(500);
  }  
}

//prende el led y genera la nota correspondiente
void luzYSonido(char led){
  int pin=pines(led);
  digitalWrite(pin,1);
  switch (led){
    case '1':tone(3,523.25,250);break;
    case '2':tone(3,587.33,250);break;
    case '3':tone(3,659.26,250);break;
    case '4':tone(3,698.46,250);break;
    case '5':tone(3,783.99,250);break;
    default:noTone(3);
  }
  delay(250);
  digitalWrite(pin,0);
}

//Da la direccion real del led
int pines(char led){
  int pin;
  switch(led){
    case '1':pin=8;break;
    case '2':pin=9;break;
    case '3':pin=10;break;
    case '4':pin=11;break;
    case '5':pin=12;break;
    default:pin=-1;
  }
  return pin;
}
    
  