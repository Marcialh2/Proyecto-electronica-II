// incluyendo libreria del 16x2 con i2c y Keypad
#include <LiquidCrystal_I2C.h> 
#include <Keypad.h>

//Variables Fisiológicas
long peso = 0; // long see refiere a los numreros enteros desde 32 bits 4 bytes "peso  = Peso
float IMC; // entero decimal que se declara como IMC
String Category;

//Memoria Estatura
#define max_distance 200
#define memory_size 5 
float memory[memory_size];   //{[],[],[],[],[]}
int mem_pos = 0;            //  0  1  2   3  4
float distancia_estimada; // se declara la distancia estimarda con floeat como entero decimal
int estatura; // declarando distancia 1 que seria la estatura de la persona

//Variables Ultrasonido
const int trigPin = 9; // declarando entrada trigger en el pin 9
const int echoPin = 10;// declarando entrada echo en el pin 10
long duration; // declarando variable numerica de numeros enteros 
int distance; // declarando distancia

//Variables KeyPad
char customKey; //Tecla KeyPad
const byte Linea = 4; //four rows
const byte Columna = 4; //four columns
char keyMap[Linea][Columna] = { // definiendo botones para el keypad
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[Linea] = {11, 2, 3, 4}; //Row pinouts of the keypad
byte colPins[Columna] = {5, 6, 7, 8}; //Column pinouts of the keypad

//Variables de Estado
int estado = 2; // declarar estado como entero

//Creación de Periféricos
Keypad customKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, Linea, Columna);  // se inicializa el teclado
LiquidCrystal_I2C lcd(0x27,20,4);

void setup() 
{

  lcd.init();                           // Se inicializa el LCD 16x2
  lcd.backlight();                      // se refiere al contraste del lcd    
  lcd.clear();                          // se refiere a la limpieza del lcd
  pinMode(trigPin, OUTPUT);             // el trigger pin se esta en modo de "salida"
  pinMode(echoPin, INPUT);              // el echo pin se declara como entrada
  Serial.begin(9600);
  }

void loop() 
{   
    if (estado == 2){       //se declara la condicion con if para el ingreso del peso

        lcd.setCursor(0,0); // se declara la fila y columna 0 para el impreso"
        lcd.print("Ingrese su peso");// se imprime "ingrese su peso"
        lcd.setCursor(0,1);
        lcd.print("Kg:");
        lcd.setCursor(7,1);
        lcd.print("Pulse A Next");

        

        delay(10); // tiempo de espera de 0.01 segundo
        customKey = customKeypad.getKey(); 
        switch(customKey)
        {
            case '0' ... '9': // Seleccionamos los valores del 0 al 9 de nuestro teclado.
            lcd.setCursor(3,1);
            peso = peso * 10 + (customKey - '0');
            lcd.print(peso);
            break;

            case 'A':
                //Cálculo estatura: 
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("calculando estatura");
                estado = 3; 

        }
    }

    else if (estado == 3){  //Calculo de la estatura


      //Ejecutar el trig
      // Escribe un pulso en el pin disparador HC-SR04
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      
      //Midiendo la demora del Eco. 
      duration = pulseIn(echoPin, HIGH);
      
      //Calculando la distancia a partir de la demora del ECO 
      distance= duration*0.034/2;
      estatura= 200 - distance ;

      //Guardamos el dato en memoria. //Esta estructura se llama "Buffer Circular"

      memory[mem_pos] = estatura; // se guarda la distancia en la memoria, en la posición mem_pos  
                                  // Como mem_pos = 0 {[estatura0],[],[],[],[]}
                                  // Como mem_pos = 1 {[estatura0],[estatura1],[],[],[]}
                                  //...
                                  // Como mem_pos = 4 {[estatura0],[estatura1],[estatura2],[estatura3],[estatura4]}
      
      mem_pos = mem_pos + 1;       // avanzar al siguiente paso de la mem
      
      if (mem_pos == memory_size)
      {
        mem_pos = 0;               //Esto permite que en el siguiente memory[mem_pos], ocurra: 
                                    // Como mem_pos = 0 {[estatura5],[estatura1],[estatura2],[estatura3],[estatura4]}
      }

      //Mostramos el último dato. 
      lcd.setCursor(0,0);
      lcd.print("HEIGHT: ");
      lcd.print(estatura);
      lcd.print("  cm  ");
    
      delay(500);


      //Si tenemos suficientes datos => Medición Lista
      if(mem_pos == (memory_size -1)){
        Serial.println("Medición Lista");
      }


      //Computo del Promedio de la Estatura
      customKey = customKeypad.getKey();

      if (customKey == 'B'){


        distancia_estimada = 0;
        for(int i = 0; i < memory_size; i++){

          distancia_estimada += memory[i]; //distancia_estimada = distancia_estimada + memory[i]
          // Serial.print(memory[i]); Serial.print("\t"); Serial.println(distancia_estimada); 
        }

        distancia_estimada = distancia_estimada / float(memory_size);
        // Serial.println(distancia_estimada);

        //Impresión de la Estatura Promedio
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Altura Medida: ");
        lcd.setCursor(0,1);
        lcd.print(distancia_estimada);
        lcd.print(" cm ");
        lcd.setCursor(0,2);
        lcd.print("Calculando IMC ...");
        lcd.setCursor(6,1);
        lcd.print("Pulse B");
        estado = 4;
      
      }


    }

    else if (estado == 4){  //Cálculo del IMC


      // Cómputo del IMC
      float altura = float(distancia_estimada)/100;
      IMC = float(peso) / (altura * altura); 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(IMC);
      lcd.print("Imc:%");
      lcd.setCursor(0,1);
      lcd.print("Pulse C");

      
      //Clasificación IMC
      if(IMC < 18.5){

          lcd.setCursor(0,1);
          Category = "bajo de peso";
          lcd.print("bajo de peso");
          }else if(IMC < 25){
          lcd.setCursor(0,1);
          Category = "Normal";
          lcd.print("Normal");
          }else if(IMC < 30){
          lcd.setCursor(0,1);
          lcd.print("Sobrepeso");
          Category = "Sobrepeso";
          }else{
          lcd.setCursor(0,1);
          lcd.print("Obesidad");
          Category = "Obesidad";
      }
      estado = 5; 
  } 
  else if(estado ==5){  //Reinicio y Comunicación 

    while(true){
      
      customKey = customKeypad.getKey();

      //Reinicio de Sistema
      if (customKey == '#'){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Reiniciando..");
        delay(2000);
        peso = 0;
        for(int i = 0; i < memory_size; i++){
          memory[i] = 0;
        }
        mem_pos = 0;
        break;

      }

      // Envío de Información

      else if(customKey == 'C'){

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enviando Info..");

        Serial.print("Peso: "); Serial.print(peso); Serial.println(" Kg ");
        Serial.print("Estatura: "); Serial.print(distancia_estimada); Serial.println(" cm ") ;
        Serial.print("IMC: "); Serial.print(IMC); Serial.println("%");
        Serial.print("Categoría IMC: "); Serial.println(Category);


        //Limpieza de la memoria y peso
        for(int i = 0; i < memory_size; i++){
          memory[i] = 0;
        }
        mem_pos = 0;

        peso = 0;

        delay(2000);
        
        break;

      }

      //Reinicio de Variables 
      estado = 2;
    }

  }
}
