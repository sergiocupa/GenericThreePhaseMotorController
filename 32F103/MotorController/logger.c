#include "logger.h"
#include "usb_device.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "types.h"
//#include <stdarg.h>


#define USB_TX_BUFFER_SIZE 4096
#define BUFFER_SEND_SIZE   100

static int   MAX_BUFFER_OCCUPANCY = (int)((float)USB_TX_BUFFER_SIZE * 0.85f);
static int   buffer_position      = 0;
static byte  usb_tx_busy          = 0;
static int   send_length          = 0;

static char usb_tx_buffer[USB_TX_BUFFER_SIZE];
static byte usb_tx_temp[USB_TX_BUFFER_SIZE];


void logger_send(const byte* data, int length)
{
	// Se nao imprimir float, entao
	//    Project → Properties → C/C++ Build → Settings → MCU GCC Linker → Miscellaneous → -u _printf_float.
	//after_counter_leng = sprintf(buffer_send_format, "0:%lu|", cnt);

	//va_list ap;
	//va_start(ap, format);
	//int len = vsnprintf(buffer_send_format + after_counter_leng, BUFFER_SEND_SIZE, format, ap);
	//len += after_counter_leng;
	//va_end(ap);

	//buffer_send_format[len]   = '\n';
	//buffer_send_format[len+1] = 0;

	memcpy(usb_tx_buffer + buffer_position, data, length);
	buffer_position += length;

	if (buffer_position >= MAX_BUFFER_OCCUPANCY)// && !usb_tx_busy)
	{
		send_length = buffer_position;
		memcpy(usb_tx_temp, usb_tx_buffer, send_length);
		buffer_position = 0;
		usb_tx_busy = 1;
		usb_tx_temp[send_length] = 0;
		CDC_Transmit_FS(usb_tx_temp, send_length);
	}
}



void OnTransmissionCompleted(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
    usb_tx_busy = 0;
}



void USB_CDC_Init()
{
	MX_USB_DEVICE_Init(OnTransmissionCompleted);
	//MX_USB_DEVICE_Init();

    // ...
}


void MX_USB_PCD_Init(void)
{
	PCD_HandleTypeDef hpcd_USB_FS = {0};

    hpcd_USB_FS.Instance                     = USB;
    hpcd_USB_FS.Init.dev_endpoints           = 8;
    hpcd_USB_FS.Init.speed                   = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.low_power_enable        = DISABLE;
    hpcd_USB_FS.Init.lpm_enable              = DISABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;

    if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
    {
        Error_Handler();
    }
}


void logger_init(LoggerCommunicationMode mode)
{
	if(mode == LOGGER_COMM_USART)
	{
		// ...
	}
	else if(mode == LOGGER_COMM_USB)
	{
		MX_USB_PCD_Init();
		USB_CDC_Init(OnTransmissionCompleted);
	}
	else if(mode == LOGGER_COMM_ETH)
	{
		// ...
	}
	else
	{
		// other options...
	}
}
