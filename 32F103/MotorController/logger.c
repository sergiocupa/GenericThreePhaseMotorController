#include "logger.h"
#include "../32F103UsbDevice/usb_device_32f103.h"
#include "../32F103UsbDevice/usbd_cdc.h"

#include "types.h"


#define USB_TX_BUFFER_SIZE 4096
#define BUFFER_SEND_SIZE   100

static   int   MAX_BUFFER_OCCUPANCY = (int)((float)USB_TX_BUFFER_SIZE * 0.85f);
static   int   buffer_position      = 0;
volatile byte  usb_tx_busy          = 0;
static   int   send_length          = 0;

static char usb_tx_buffer[USB_TX_BUFFER_SIZE];
static byte usb_tx_temp[USB_TX_BUFFER_SIZE];


void logger_transmit_complete()
{
	usb_tx_busy = 0;
}


void logger_send(const byte* data, int length)
{
	if((buffer_position + length) < USB_TX_BUFFER_SIZE)
	{
		memcpy(usb_tx_buffer + buffer_position, data, length);
		buffer_position += length;
	}

	if ((buffer_position >= MAX_BUFFER_OCCUPANCY) && !usb_tx_busy)
	{
		send_length = buffer_position;

		memcpy(usb_tx_temp, usb_tx_buffer, send_length);

		if(CDC_Transmit_FS(usb_tx_temp, send_length) == USBD_OK)
		{
			usb_tx_busy     = 1;
			buffer_position = 0;
		}
	}
}


void USB_CDC_Init()
{

	MX_USB_DEVICE_Init();
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
		USB_CDC_Init();

		UsbTransmitComplete = logger_transmit_complete;
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
