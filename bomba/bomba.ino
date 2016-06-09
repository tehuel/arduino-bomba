#include <LiquidCrystal.h> // Libreria Para LCD 16x2
#include <SoftwareSerial.h> // Libreria Para Wifi ESP8826
#include "TimerOne.h" // Libreria Para Manejar Interrupciones
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Configuracion LCD
SoftwareSerial wifi(8, 9); // Configuracion Wifi RX | TX

int tiempoTranscurrido=0; //Contador de Tiempo De La Partida, Se Utiliza para poder calcular el puntaje del juego

byte heart[8] = {B00000,B01010,B11111,B11111,B11111,B01110,B00100,B00000}; //Array para dibujar Corazon

byte bomb[8] =  {B00100,B00010,B00100,B01110,B11011,B10111,B11011,B01110}; //Array para dibujar Bomba1

byte bomb2[8] = {B00010,B00100,B00100,B01110,B11011,B10111,B11011,B01110}; //Array para dibujar Bomba2
String redesSSID[20]="";

void setup() {
  wifi.begin(1200); // Inicializo el modulo Wifi

  // Cargo los graficos en el lcd
  lcd.createChar(4,heart);
  lcd.createChar(1, bomb);
  lcd.createChar(2, bomb2);

  Serial.begin(9600); //Inicializo Comunicacion Serial (0,1) RX | TX
  
  lcd.begin(16, 2);   // Configuro el LCD en dos Lineas Con 16 Caracteres
  
  pinMode(A0, INPUT); //Configuro la entrada Analogica A0 , esta se utiliza Para Leer un valor aleatorio de la antena y posteriormente generar numero aleatorio
  randomSeed(leerSemillaAleatoria()); // Configuro Semilla Aleatoria
}


void loop() {

  iniciarPantallaBienvenida(); // Pantalla De Bienvenida Con el Dibujo de las Bombas

  switch (cargarMenuPantallaInicial()){
    case 0:
      iniciarJuego();
    break;
    case 1:
      menuConfiguracion();
    break;
 }
}

int cargarMenuPantallaInicial(){
  String textoMenu[4]={"Iniciar Juego","Configuracion","Puntaje","Acerca De..."};
  return (cargarMenuPantalla(textoMenu));
}

void menuConfiguracion(){
  String textoMenu[2]={"Buscar Redes","IP"};
  switch (cargarMenuPantalla(textoMenu)){
     case 0:
      buscarRedesWifi();
    break;
    case 1:
      mostrarMiIP();
    break;   
  }
}
void listarRedesWifi()
{
  for (int s=0;s<20;s++){
  redesSSID[s]="";
  }
  String command="AT+CWLAP\r\n";
  const int timeout=10000;
  boolean debug=false;
    wifi.print(command); // send the read character to the esp8266
    long int time = millis();
    int x=0;
    bool esDato=false;
    int esPrimero=0;
    while( (time+timeout) > millis())
    {
      while(wifi.available())
      {
        char c = wifi.read(); 
        Serial.print(c);
        if (c=='"'){esDato=!esDato;esPrimero++;if (esPrimero==2){redesSSID[x].replace("\"","");x++;/*response+="\"\r\n";*/}}
        if(esPrimero==4){esPrimero=0;}
        if (esDato && esPrimero==1 && x<20)
        {redesSSID[x]+=c;}
        
      }  
    }
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    wifi.print(command); // send the read character to the esp8266
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(wifi.available())
      {
        char c = wifi.read(); 
        
        response+=c;
        
      }  
    }
    
    return response;
}

void mostrarMiIP(){}
void buscarRedesWifi(){
  String lectura =sendData("AT\r\n",1000,false);
  if (lectura.indexOf("OK")>0){
    lectura="";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Buscando Redes");
    lcd.setCursor(0,1);
    listarRedesWifi();
  int opcion= cargarMenuPantalla(redesSSID);
  String lectura =sendData("AT+CWJAP=\""+redesSSID[opcion]+"\",""\r\n",1000,false);
  Serial.print(lectura);
   lectura =sendData("AT+CWJAP=\""+redesSSID[opcion]+"\",""\r\n",1000,false);
  Serial.print(lectura);
  
   lectura =sendData("AT+CIFSR\r\n",1000,false);
  Serial.print(lectura);
  
  delay(6000);
  }
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
}

void iniciarJuego(){
 Timer1.initialize(1000000);         // initialize timer1, and set a 1/2 second period
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


int cargarMenuPantalla(String* textoMenu){
 bool entro = false;
 int posicionCursor = 0;
 while (not entro){
    mostrarMenuEnPantalla(textoMenu,posicionCursor);
    while (noApretoTecla()){}
    switch (leerTeclaApretada())
    {
      case 2:
      posicionCursor--;
       mostrarMenuEnPantalla(textoMenu,posicionCursor);
      break;
      case 8:
      posicionCursor++;
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

