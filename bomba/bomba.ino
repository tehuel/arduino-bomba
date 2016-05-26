#include <LiquidCrystal.h>
#include "TimerOne.h"
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int tiempoTranscurrido=0;

byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  
};

byte bomb[8] = {
  B00100,
  B00010,
  B00100,
  B01110,
  B11011,
  B10111,
  B11011,
  B01110
};

byte bomb2[8] = {
  B00010,
  B00100,
  B00100,
  B01110,
  B11011,
  B10111,
  B11011,
  B01110
};



void setup() {
  lcd.createChar(4,heart);
  lcd.createChar(1, bomb);
  lcd.createChar(2, bomb2);
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(A0, INPUT);
  randomSeed(leerSemillaAleatoria());
}


void loop() {

  iniciarPantallaBienvenida();

  //iniciarMenu();
 switch (cargarMenuPantalla()){
  case 0:
  iniciarJuego();
  break;
 }
  // Serial.println(numeroSecreto);
}

void iniciarPantallaBienvenida(){
   pantallaBienvenida();

  while (noApretoTecla()){
    actualizarBombaPantalla();
  }

  leerTeclaApretada();
}

void incrementar_tiempo(){
tiempoTranscurrido++;
lcd.setCursor(12,0);
lcd.leftToRight();
lcd.print(tiempoTranscurrido);

}

void iniciarJuego(){
 Timer1.initialize(1000000);         // initialize timer1, and set a 1/2 second period
 //Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
 Timer1.attachInterrupt(incrementar_tiempo);  // attaches callback() as a timer overflow interrup
  
  int numeroSecreto[4];
  generarNumeroSecreto(numeroSecreto);
  int vidasEnElJuego = 10;
  bool gano = false;
  while (vidasEnElJuego > 0 && not gano){
    Serial.print("CantidadDeVidas:");
    Serial.println(vidasEnElJuego);

    int codigoIngresado[4] ;
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("* * * * ");
    lcd.write(byte(4));
    lcd.print(":");
    lcd.print(vidasEnElJuego);
    leerCodigoIngresado(codigoIngresado);
    if (compararNumeros(numeroSecreto,codigoIngresado)){
      gano = true;
      } 
      else 
      {
      vidasEnElJuego--;
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("NPC: ");
      lcd.print(numeroEnPosicionCorrecta(numeroSecreto,codigoIngresado));
      lcd.print("NC: ");
      lcd.print(numerosCorrectos(numeroSecreto,codigoIngresado));
      }
  }
  Timer1.stop();
  if (gano){
  Serial.println("Gano");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("     BOMBA     ");
      lcd.setCursor(0,1);
      lcd.println("  DESACTIVADA  ");
      delay(5000);
      lcd.clear();
      lcd.print (tiempoTranscurrido);
      delay(5000);
  }
  else
  {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.println("     BOOM     ");
      delay(10000);
  }
}

// retorna TRUE cuando no hay ninguna tecla para leer en serial
bool noApretoTecla() {
  return (not Serial.available());
}


void pantallaBienvenida() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.write(byte(1));
  lcd.print(" BOMB ");
  lcd.write(byte(2));
  lcd.setCursor(2, 1);
  lcd.print("Press  Start");
}


void actualizarBombaPantalla() {
  lcd.setCursor(3, 0);
  lcd.write(byte(2));
  lcd.setCursor(10, 0);
  lcd.write(byte(1));
  delay(500);
  lcd.setCursor(3, 0);
  lcd.write(byte(1));
  lcd.setCursor(10, 0);
  lcd.write(byte(2));
  delay(500);
}


int leerTeclaApretada() {
  return (Serial.parseInt());
}


int leerSemillaAleatoria() {
  return (analogRead(A0));
}


// rellena el array con 4 numeros random, todos distintos
void generarNumeroSecreto(int numeroSecreto[4]){

  numeroSecreto[0]=random(0,9);

  for (int i = 1; i<4; i++) {

    int posible = random(0,9);

    while ( esIgualAAlguno(posible, numeroSecreto, i) ) {
      posible = random(0,9);
    }

    numeroSecreto[i]= posible;
  }

  Serial.print(numeroSecreto[0]);
  Serial.print(numeroSecreto[1]);
  Serial.print(numeroSecreto[2]);
  Serial.println(numeroSecreto[3]);
  //Serial.println("Numero Generado");

}

// comprueba que el numero actual sea distinto a los anteriores generados
bool esIgualAAlguno(int posible, int numeroSecreto[4], int i) {

  bool resultado = false;
  for (int x = 0; x < i; x++) {
    if (numeroSecreto[x] == posible) {
      resultado = true;
    }
  }
  return resultado;
}


int cargarMenuPantalla(){
 bool entro = false;
 int posicionCursor = 0;
 while (not entro){
    
    String textoMenu[3]={"Iniciar Juego","Puntaje","Acerca De..."};
    mostrarMenuEnPantalla(textoMenu,posicionCursor);
    while (noApretoTecla()){}
    switch (leerTeclaApretada())
    {
      case 2:
      posicionCursor--;
      //Serial.print("PosicionCursor=");
      //Serial.println(posicionCursor);
       mostrarMenuEnPantalla(textoMenu,posicionCursor);
      break;
      case 8:
  
      posicionCursor++;
      //Serial.print("PosicionCursor=");
      //Serial.println(posicionCursor);
  
      mostrarMenuEnPantalla(textoMenu,posicionCursor);
      break;
      case 5:
      entro= true;
      break;
    }
  }
  return posicionCursor;
}
void mostrarMenuEnPantalla(String* menu,int desde){

lcd.clear();
lcd.setCursor(0,0);
lcd.print(">");
lcd.print(menu[desde]);
lcd.setCursor(0,1);
lcd.print(" ");
lcd.print(menu[(desde+1)]);
}


// lee 4 veces el ingreso de datos, uno por cada digito
void leerCodigoIngresado(int codigoIngresado[4]){
  int teclasApretadas =0;
  while (teclasApretadas<4){

    while (noApretoTecla()){}
    codigoIngresado[teclasApretadas]= leerTeclaApretada();
    mostrarEnPantallaTeclaApretada(codigoIngresado[teclasApretadas],teclasApretadas);
    //Serial.print("numerosIngresado[");
    //Serial.print(teclasApretadas);
    //Serial.print("]=");
    //Serial.println(codigoIngresado[teclasApretadas]);
    teclasApretadas++;
  }
}

void mostrarEnPantallaTeclaApretada(int numero,int posicion){
  lcd.setCursor(posicion*2,0);
  lcd.print((String)numero);
}


// comprueba si el numero ingresado es el numero secreto
bool compararNumeros(int numeroSecreto[4],int numeroIngresado[4]){

  //Serial.print(numeroSecreto[0]);
  //Serial.print("=");
  //Serial.println(numeroIngresado[0]);

  return ( numeroSecreto[0] == numeroIngresado[0] 
    && numeroSecreto[1] == numeroIngresado[1] 
    && numeroSecreto[2] == numeroIngresado[2] 
    && numeroSecreto[3] == numeroIngresado[3] );
}


// comprueba cuantos numeros estan en su posicion correcta
int numeroEnPosicionCorrecta(int numeroSecreto[4],int numeroIngresado[4]){

  int cantidadDeNumeros=0;

  for (int x =0 ; x<4;x++){
    if (numeroSecreto[x]==numeroIngresado[x]){cantidadDeNumeros++;}
  }

  return (cantidadDeNumeros);
}


// comprueba cuantos de los numeros ingresados forman parte del numero secreto
int numerosCorrectos(int numeroSecreto[4],int numeroIngresado[4]){

  int cantidadDeNumeros=0;

  for (int x =0 ; x<4;x++){
    for (int y=0 ;y<4 ;y++){
      if (numeroIngresado[x]==numeroSecreto[y]){
        cantidadDeNumeros++;
      }
    }
  }
  return (cantidadDeNumeros);
}
