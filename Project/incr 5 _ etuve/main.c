#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"            
#include "timers.h"
#include <semphr.h>
#include <queue.h>
//#include <stdint.h>
//============================== variables ======================================
SemaphoreHandle_t SerialSemaphore;
QueueHandle_t mailbox;
TaskHandle_t ROUTIN;
TaskHandle_t lcd;
TaskHandle_t fin;
TaskHandle_t etv;

int t=10; //nombre des tissus
int temp = 0;

char msg[15];
void vApplicationIdleHook(void);
void RoutineDInterruption(void*);
void finTravail(void*);
void afficher(void*);
void etuve(void*);

//=========================== Routine D'Interruption ===========================
void RoutineDInterruption(void *pvParameters) {
 (void) pvParameters;
while(1){

printf("\ntask traitment de l'intruption \n");
  msg[0] = 'o';
  msg[1] = 'n';
  msg[2] = ' ';
  msg[3] = ' ';
  msg[4] = '\0';
  xQueueSendToBack( mailbox, &msg, ( TickType_t ) 10 );

xSemaphoreGiveFromISR(SerialSemaphore,NULL);

vTaskSuspend(ROUTIN);
 vTaskResume(fin);

}
}
//=============================== afficher ======================================
void afficher(void *pvParameters) {
 (void) pvParameters;
while(1){

xQueueReceive(mailbox, &msg,( TickType_t ) 10 );
printf("\ntask afficher ************* machine:   %s \n",msg);

}    
}


//=============================== Etuve ===============================

//Pour eliminer la paraffine

void etuve(void *pvParameters) {
  printf( "\nEliminer la paraffine \n\n");
   
 temp ++;
 if(temp = 60){ //la temperature doit etre 60°C
    printf("Task temperature = %d °C \n",temp);
    xQueueSendToBack( mailbox, &msg, ( TickType_t ) 10 );
    vTaskDelay(100);//les coupe de tissus sont mises dans le étuve pendant 1h 
   }
   
       printf("Task elimination de paraffine terminé\n");
       
   vTaskResume(fin);
  //vTaskResume(ROUTIN); 
}

//===============================finTravail===============================================
void finTravail(void *pvParameters) {
 (void) pvParameters;
  vTaskSuspend(fin);
 printf( "\ntask fin travail \n\n");

  msg[0] = 'o';
  msg[1] = 'f';
  msg[2] = 'f';
  msg[3] = ' ';
  msg[4] = '\0';
  
  xQueueSendToBack( mailbox, &msg, ( TickType_t ) 10 );

 

    vSemaphoreDelete(SerialSemaphore);
    vQueueDelete( mailbox);
    vTaskDelete(lcd);
    vTaskDelete(ROUTIN);
    
    vTaskEndScheduler();
}

//====================== =========== ==== main ====================================
int main ( void )
{

vSemaphoreCreateBinary( SerialSemaphore );
xSemaphoreTake( SerialSemaphore,( TickType_t ) 10);
mailbox= xQueueCreate( 1, sizeof( msg ) );

printf("cliquer sur entree pour lancer la simulation " );
getchar();

msg[0]='o';
msg[1]='f';
msg[2]='f';
msg[3] = ' ';
msg[4] = '\0';
xQueueSendToBack( mailbox,&msg,( TickType_t ) 10 );

xTaskCreate( afficher, "lcd", 128, NULL, 5, &lcd );
xTaskCreate( RoutineDInterruption, "gestion E/S", 128, NULL, 3, &ROUTIN );
xTaskCreate( finTravail, "suppression", 128, NULL, 4, &fin );
xTaskCreate( etuve, "etv", 128, NULL, 3, &etv );
vTaskStartScheduler();
    return 0;
}
//================================ end Main ============================

//================================== system =============================
void vAssertCalled( unsigned long ulLine, const char * const pcFileName )// gestion exception
{
     taskENTER_CRITICAL();
    {
        printf("[ end ] %s:%lu\n", pcFileName, ulLine);
        fflush(stdout);
    }
    taskEXIT_CRITICAL();
    exit(-1);
}

void vApplicationIdleHook(void){}

