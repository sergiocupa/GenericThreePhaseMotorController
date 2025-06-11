/*
 * usb_device_32f103.h
 *
 *  Created on: Jun 10, 2025
 *      Author: sergio
 */

#ifndef USB_DEVICE_32F103_H_
#define USB_DEVICE_32F103_H_


#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_cdc.h"
#include "usbd_def.h"


#define APP_RX_DATA_SIZE  1024
#define APP_TX_DATA_SIZE  1024


extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;


void MX_USB_DEVICE_Init();
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);



#endif /* USB_DEVICE_32F103_H_ */
