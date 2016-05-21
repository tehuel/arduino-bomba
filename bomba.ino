#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
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

byte estadoMenu=0;

void setup() {
  lcd.createChar(10, bomb);
  lcd.createChar(11, bomb2);
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(A0, INPUT);
  randomSeed(leerSemillaAleatoria());
  
}

void loop() {
  int numeroSecreto[4];
  generarNumeroSecreto(numeroSecreto);
  pantallaBienvenida();
 
  while (noApretoTecla())
  {
    actualizarBombaPantalla();
  }
  
  leerTeclaApretada();

  int vidasEnElJuego = 10;
  bool gano = false;
 // Serial.println(numeroSecreto);
  while (vidasEnElJuego > 0 && not gano)
   {
     Serial.print("CantidadDeVidas:");
     Serial.println(vidasEnElJuego);
     int codigoIngresado[4] ;
     leerCodigoIngresado(codigoIngresado);
     if (compararNumeros(numeroSecreto,codigoIngresado)){
      gano = true;
      Serial.println("Gano");
     }
     else
     {
      vidasEnElJuego--;
      
     }
   }
  
}

bool noApretoTecla() {
  return (not Serial.available());
}

void pantallaBienvenida() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.write(10);
  lcd.print(" BOMB ");
  lcd.write(11);
  lcd.setCursor(2, 1);
  lcd.print("Press  Start");
}

void actualizarBombaPantalla() {
  lcd.setCursor(3, 0);
  lcd.write(11);
  lcd.setCursor(10, 0);
  lcd.write(10);
  delay(500);
  lcd.setCursor(3, 0);
  lcd.write(10);
  lcd.setCursor(10, 0);
  lcd.write(11);
  delay(500);
}

int leerTeclaApretada() {
  return (Serial.parseInt());
}


int leerSemillaAleatoria() {
  return (analogRead(A0));
}


void generarNumeroSecreto(int numeroSecreto[4]){
  numeroSecreto[0]=random(0,9);
  
  for (int i = 1; i<3; i++) {
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
  Serial.println("Numero Generado");
}

bool esIgualAAlguno(int posible, int numeroSecreto[4], int i) {

  bool resultado = false;
  for (int x = 0; x < i; x++) {
    if (numeroSecreto[x] == posible) {
        resultado = true;
    }
  }
  return resultado;
}


void cargarMenuPantalla(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("> Iniciar Juego");
  estadoMenu=1;
  lcd.setCursor(0,1);
  lcd.print(" Acerca De Bomb");
}
void leerCodigoIngresado(int codigoIngresado[4]){
  int teclasApretadas =0;
  while (teclasApretadas<4){
    while (noApretoTecla()){}
    codigoIngresado[teclasApretadas]= leerTeclaApretada();
    Serial.print("numerosIngresado[");
    Serial.print(teclasApretadas);
    Serial.print("]=");
    Serial.println(codigoIngresado[teclasApretadas]);
    teclasApretadas++;
    }
 
}
bool compararNumeros(int numeroSecreto[4],int numeroIngresado[4]){
Serial.print(numeroSecreto[0]);
Serial.print("=");
Serial.println(numeroIngresado[0]);
  
return ( numeroSecreto[0]== numeroIngresado[0] 
      && numeroSecreto[1]== numeroIngresado[1] 
      && numeroSecreto[2]== numeroIngresado[2] 
      && numeroSecreto[3]== numeroIngresado[3] );
}
int numeroEnPosicionCorrecta(int numeroSecreto[4],int numeroIngresado[4]){
int cantidadDeNumeros=0;
for (int x =0 ; x<4;x++){
  if (numeroSecreto[x]==numeroIngresado[x]){cantidadDeNumeros++;}
}
return (cantidadDeNumeros);
}

int numeroCorrectos(int numeroSecreto[4],int numeroIngresado[4]){
int cantidadDeNumeros=0;
for (int x =0 ; x<4;x++){
    for (int y=0 ;y<4 ;y++)
    {
      if (numeroIngresado[x]==numeroSecreto[y]){cantidadDeNumeros++;}
    }
  }
return (cantidadDeNumeros);
}
