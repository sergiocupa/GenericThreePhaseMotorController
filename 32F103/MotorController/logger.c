#include "logger.h"
#include "usb_device.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "types.h"
#include <stdarg.h>


#define USB_TX_BUFFER_SIZE 1024
#define BUFFER_SEND_SIZE   100

static int   MAX_BUFFER_OCCUPANCY = (int)((float)USB_TX_BUFFER_SIZE * 0.85f);
static int   buffer_position      = 0;
static int   PARAM_HEADER_SIZE    = (sizeof(uint) * 3) + sizeof(ulong);
static byte  usb_tx_busy          = 0;
static ulong counter              = 0;
static int   send_length          = 0;
static int   after_counter_leng   = 0;

static char usb_tx_buffer[USB_TX_BUFFER_SIZE];
static byte usb_tx_temp[USB_TX_BUFFER_SIZE];
static char buffer_send_format[BUFFER_SEND_SIZE];


void logger_send(const ulong cnt, const char* format, ...)
{
	after_counter_leng = sprintf(buffer_send_format, "0:%lu|", cnt);

	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(buffer_send_format + after_counter_leng, BUFFER_SEND_SIZE, format, ap);
	len += after_counter_leng;
	va_end(ap);

	buffer_send_format[len]   = '\n';
	buffer_send_format[len+1] = 0;

	memcpy(usb_tx_buffer + buffer_position, buffer_send_format, len+1);
	buffer_position += len+1;

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



void _logger_send(LoggerParam* params, uint param_count)
{
	uint before = buffer_position;
	uint pos = buffer_position + PARAM_HEADER_SIZE;
	int ix = 0;
	while (ix < param_count)//                                                                Protocol: header complement, parameter sizes
	{
		memcpy(usb_tx_buffer + pos, &params[ix].Size, sizeof(uint));
		pos += 4;
		ix++;
	}

	usb_tx_buffer[buffer_position]     = 255;//                                               Protocol: initial header
	usb_tx_buffer[buffer_position + 1] = 254;//                                               Protocol: initial header
	usb_tx_buffer[buffer_position + 2] = 253;//                                               Protocol: initial header
	usb_tx_buffer[buffer_position + 3] = PLATFORM_SIZE;//                                     Protocol: platform size, exemple: 8, 16, 32, 64 bits

	if (counter == UINT64_MAX) counter = 0;
	counter++;

	memcpy(usb_tx_buffer + buffer_position + 4, &pos, sizeof(uint));//                        Protocol: Header size
	memcpy(usb_tx_buffer + buffer_position + 8, &param_count, sizeof(uint));//                Protocol: Param count
	memcpy(usb_tx_buffer + buffer_position + 12, &counter, sizeof(ulong));//                  Protocol: Package counter

	buffer_position += pos+1;

	ix = 0;
	while (ix < param_count)
	{
		LoggerParam param = params[ix];
		memcpy(usb_tx_buffer + buffer_position, &param.DataType, sizeof(byte));
		buffer_position++;

		memcpy(usb_tx_buffer + buffer_position, &param.Address, sizeof(uint));
		buffer_position += sizeof(uint);

		memcpy(usb_tx_buffer + buffer_position, &param.Size, sizeof(uint));
		buffer_position += sizeof(uint);

		memcpy(usb_tx_buffer + buffer_position, param.Data, param.Size);
		buffer_position += param.Size;

		ix++;
	}

	uint psize = buffer_position - before;

	if ((buffer_position + psize) >= MAX_BUFFER_OCCUPANCY && !usb_tx_busy)
	{
		send_length = buffer_position;
		memcpy(usb_tx_temp, usb_tx_buffer, send_length);
		buffer_position = 0;
		usb_tx_busy = 1;
		CDC_Transmit_FS(usb_tx_temp,send_length);
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
