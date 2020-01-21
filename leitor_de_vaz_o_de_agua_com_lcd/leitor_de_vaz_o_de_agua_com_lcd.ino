 /*********************************
 ***Autor: Vinicius Marra Santos***
 ***********12/01/2020*************
 *************Atenção**************
 ***O Codigo esta livre para uso,**
 *faça bom proveito e compartilhe!*
 *melhorias são sempre bem vindas**
 **********************************
 *O SISTEMA FUNCIONA COM***********
 *UM SENSOR DE VAZAO DE AGUA*******
 *MODELO YF-F201 1/2 POL***********
 *UM RELE QUE CONTROLA*************
 *A VALVULA SOLENOIDE**************
 *ABRINDO E FECHANDO O FLUXO*******
 *DE AGUA CONFORME OS LITROS*******
 *CONFUGIRADO PELO OPERADOR********
 *PINAGEM ANALOGICA:***************
 *A4 E A5 PARA DISPLAY I2C*********
 *PINAGEM DIGITAL:*****************
 *2 = SENSOR DE VAZAO DE AGUA******
 *3 = RELE (INICIA EM HIGH)********
 *7 = BOTÃO MAIS AGUA**************
 *8 = BOTÃO MENOS AGUA*************
 *9 = BOTÃO ZERAR VALORES**********
 *10 = BOTÃO MAIS 5 LITROS DE AGUA*
 *********************************/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//MELHORIAS FUTURAS: TEMPO DE RESPOSTA DO BOTÃO 

//LCD CONFIG, CONFIGURAÇÃO DO LCD PARA MODULO DE I2C E LCD 16*2
//dispositivo real
LiquidCrystal_I2C lcd(0x27,16,2);
// para simulador no protheus
//LiquidCrystal_I2C lcd(0x20,16,2);

//CARACTERES A E F ESTILIZADOS
byte FechadoB[] = {
  B11111,
  B10001,
  B10111,
  B10011,
  B10111,
  B10111,
  B11111,
  B11111
};
byte AbertoB[] = {
  B11111,
  B10001,
  B10101,
  B10001,
  B10101,
  B10101,
  B11111,
  B11111
};


// VARIAVEIS DO SENSOR DE FLUXO
float vazao;                    //Variável para armazenar o valor em L/min
float media = 0;                //Variável para fazer a média
int contaPulso;                 //Variável para a quantidade de pulsos
int Min = 0;                    //Variável para minutos
float Litros = 0;               //Variável para Quantidade de agua
float MiliLitros = 0;           //Variavel para Conversão
float LitosDesejados = 0;       //Variavel de total de litros desejados


// CONFIGURAÇÕES DOS BOTÕES
int MenosAguaButtun = 7;          // Atribuição do pino do botao menos agua 
int MaisAguaButtun = 8;           // Atribuição do pino do botao mais agua
int ZeraAguaButtun = 9;           // Atribuição do pino do botao zera valores
int MaisAguaCincoButtun = 10;     // Atribuição do pino do botao mais 5 litros

//CONFIGURAÇÃO DA PINAGEM DO RELE
int rele = 3;

void setup() {
  lcd.init();                   //Init do LCD
  lcd.backlight();              //Ativa backlight   
  lcd.createChar(0, FechadoB);  //Seta o Char F
  lcd.createChar(1, AbertoB);   //Seta o Char A
  //lcd.home();

  //CONFIGURAÇÕES DO SENSOR DE AGUA
  pinMode(2, INPUT);
  attachInterrupt(0, incpulso, RISING); //Configura o pino 2(Interrupção 0) interrupção

  //CONFUGURAÇÕES DOS BOTÕES
  pinMode(MenosAguaButtun, INPUT_PULLUP);  
  pinMode(MaisAguaButtun, INPUT_PULLUP);
  pinMode(ZeraAguaButtun, INPUT_PULLUP);
  pinMode(MaisAguaCincoButtun, INPUT_PULLUP);
  
  //CONFIGURAÇÃOES DO RELE PARA VALVULA SOLENOIDE
  pinMode(rele, OUTPUT);
  digitalWrite(rele,LOW);

  //CREDITOS DO AUTOR
  lcd.setCursor(0,0);
  lcd.print("PRJ LEITOR DE");
  lcd.setCursor(0,1);
  lcd.print("VAZAO D AGUA 1.0");
  delay (2000); 
  lcd.clear();   
}

void loop() {

  contaPulso = 0;//Zera a variável
  sei(); //Habilita interrupção
  
  // ******* VALIDAR DE TEM QUE SER 1 SEGUNDO PARA A LEITURA CORRETA DA VAZÃO DE AGUA
  delay (1000); //Aguarda 1 segundo

  // CALCULOS DO SENSOR, VALORES PADRÃO PARA O SENSOR UTILIZADO
  vazao = contaPulso / 5.5; //Converte para L/min
  MiliLitros = vazao / 60;     // Calculo de mililitros
  Litros = Litros + MiliLitros;   // Calculo de quantos litros passou pelo sensor

  // ESCRITA DO DISPLAY OS VALOES SETADOS E OS VALORES DO SENSOR
  lcd.setCursor(0,0);
  lcd.print(vazao);
  lcd.print(" L/min"); 
  if(vazao < 10 ){
    lcd.setCursor(10,0); 
    lcd.print("   "); 
  }
    
  lcd.setCursor(0,1);
  lcd.print(Litros); 
  lcd.print(" L ");
  
  if(LitosDesejados <= Litros){
    lcd.setCursor(15,0);
    lcd.write(0);
    digitalWrite(rele,HIGH); 
  } else {
    lcd.setCursor(15,0);
    lcd.write(1); 
    digitalWrite(rele,LOW);
  }

  lcd.setCursor(15,1);
  lcd.print("L");  
  checkIfSelectButtonIsPressed();                 // Confere por Pulling o estado do botão
}

//METODO PARA INCREMENTAR A VARIAVEL DE PULSO
void incpulso ()  {
  contaPulso++; //Incrementa a variável de pulsos
}

//CHECA O ESTADO DOS BOTÕES E APLICA A REGRA PARA CADA UM
void checkIfSelectButtonIsPressed()
{
   lcd.setCursor(7,1);
   lcd.print(" / "); 
   lcd.print(LitosDesejados); 
   if (digitalRead(MenosAguaButtun) == LOW){
      if(LitosDesejados > 0){
        LitosDesejados--;
        if(LitosDesejados < 10){
           lcd.setCursor(7,1);
           lcd.print(" / ");  
           lcd.print(LitosDesejados); 
           lcd.setCursor(14,1);
           lcd.print(" "); 
        }
      }
    } else if (digitalRead(MaisAguaButtun) == LOW){
      LitosDesejados++; 
    } else if (digitalRead(MaisAguaCincoButtun) == LOW){
      LitosDesejados = LitosDesejados + 5; 
    } else if(digitalRead(ZeraAguaButtun) == LOW){
      LitosDesejados = 0;
      vazao = 0; //Variável para armazenar o valor em L/min
      media = 0; //Variável para fazer a média
      Min = 0; //Variável para minutos
      Litros = 0; //Variável para Quantidade de agua
      MiliLitros = 0; //Variavel para Conversão
      LitosDesejados = 0; // variavel de total de litros desejados
      lcd.clear(); 
     
  }
}
