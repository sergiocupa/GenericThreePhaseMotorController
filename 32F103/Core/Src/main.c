#include "main.h"
#include "logger.h"
#include "three_phase_drive_runner.h"
#include "three_phase_drive_preparer.h"


static ThreePhaseDriveData Instance = {0};


int main(void)
{

  three_phase_drive_prepare(&Instance);

  three_phase_drive_run(&Instance);

  //while (1)
 // {
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
	//HAL_Delay(50);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
    //HAL_Delay(10);



    //xx += 0.1f;
    //cnt++;

    //int leng = sprintf(buffer,"%d:%.3f|%d:%.3f|%d:%.3f", cnt, xx, cnt, xx, cnt, xx);


   // CDC_Transmit_FS(buffer, leng);

    //_logger_voltages(xx, xx, xx);




	//char msg[] = "Hello via USB!\r\n";
	//CDC_Transmit_FS((uint8_t*)msg, strlen(msg));


  //}
}
