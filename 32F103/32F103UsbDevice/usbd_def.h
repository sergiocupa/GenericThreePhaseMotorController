#ifndef __USBD_DEF_H
#define __USBD_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_conf.h"


#ifndef NULL
#define NULL                                            0U
#endif /* NULL */

#ifndef USBD_MAX_NUM_INTERFACES
#define USBD_MAX_NUM_INTERFACES                         1U
#endif

#ifndef USBD_MAX_NUM_CONFIGURATION
#define USBD_MAX_NUM_CONFIGURATION                      1U
#endif

#ifndef USBD_LPM_ENABLED
#define USBD_LPM_ENABLED                                0U
#endif

#ifndef USBD_SELF_POWERED
#define USBD_SELF_POWERED                               1U
#endif

#ifndef USBD_SUPPORT_USER_STRING_DESC
#define USBD_SUPPORT_USER_STRING_DESC                   0U
#endif

#define  USB_LEN_DEV_QUALIFIER_DESC                     0x0AU
#define  USB_LEN_DEV_DESC                               0x12U
#define  USB_LEN_CFG_DESC                               0x09U
#define  USB_LEN_IF_DESC                                0x09U
#define  USB_LEN_EP_DESC                                0x07U
#define  USB_LEN_OTG_DESC                               0x03U
#define  USB_LEN_LANGID_STR_DESC                        0x04U
#define  USB_LEN_OTHER_SPEED_DESC_SIZ                   0x09U

#define  USBD_IDX_LANGID_STR                            0x00U
#define  USBD_IDX_MFC_STR                               0x01U
#define  USBD_IDX_PRODUCT_STR                           0x02U
#define  USBD_IDX_SERIAL_STR                            0x03U
#define  USBD_IDX_CONFIG_STR                            0x04U
#define  USBD_IDX_INTERFACE_STR                         0x05U

#define  USB_REQ_TYPE_STANDARD                          0x00U
#define  USB_REQ_TYPE_CLASS                             0x20U
#define  USB_REQ_TYPE_VENDOR                            0x40U
#define  USB_REQ_TYPE_MASK                              0x60U

#define  USB_REQ_RECIPIENT_DEVICE                       0x00U
#define  USB_REQ_RECIPIENT_INTERFACE                    0x01U
#define  USB_REQ_RECIPIENT_ENDPOINT                     0x02U
#define  USB_REQ_RECIPIENT_MASK                         0x03U

#define  USB_REQ_GET_STATUS                             0x00U
#define  USB_REQ_CLEAR_FEATURE                          0x01U
#define  USB_REQ_SET_FEATURE                            0x03U
#define  USB_REQ_SET_ADDRESS                            0x05U
#define  USB_REQ_GET_DESCRIPTOR                         0x06U
#define  USB_REQ_SET_DESCRIPTOR                         0x07U
#define  USB_REQ_GET_CONFIGURATION                      0x08U
#define  USB_REQ_SET_CONFIGURATION                      0x09U
#define  USB_REQ_GET_INTERFACE                          0x0AU
#define  USB_REQ_SET_INTERFACE                          0x0BU
#define  USB_REQ_SYNCH_FRAME                            0x0CU

#define  USB_DESC_TYPE_DEVICE                           0x01U
#define  USB_DESC_TYPE_CONFIGURATION                    0x02U
#define  USB_DESC_TYPE_STRING                           0x03U
#define  USB_DESC_TYPE_INTERFACE                        0x04U
#define  USB_DESC_TYPE_ENDPOINT                         0x05U
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                 0x06U
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        0x07U
#define  USB_DESC_TYPE_BOS                              0x0FU

#define USB_CONFIG_REMOTE_WAKEUP                        0x02U
#define USB_CONFIG_SELF_POWERED                         0x01U

#define USB_FEATURE_EP_HALT                             0x00U
#define USB_FEATURE_REMOTE_WAKEUP                       0x01U
#define USB_FEATURE_TEST_MODE                           0x02U

#define USB_DEVICE_CAPABITY_TYPE                        0x10U

#define USB_HS_MAX_PACKET_SIZE                          512U
#define USB_FS_MAX_PACKET_SIZE                          64U
#define USB_MAX_EP0_SIZE                                64U


#define USBD_STATE_DEFAULT                              0x01U
#define USBD_STATE_ADDRESSED                            0x02U
#define USBD_STATE_CONFIGURED                           0x03U
#define USBD_STATE_SUSPENDED                            0x04U


#define USBD_EP0_IDLE                                   0x00U
#define USBD_EP0_SETUP                                  0x01U
#define USBD_EP0_DATA_IN                                0x02U
#define USBD_EP0_DATA_OUT                               0x03U
#define USBD_EP0_STATUS_IN                              0x04U
#define USBD_EP0_STATUS_OUT                             0x05U
#define USBD_EP0_STALL                                  0x06U

#define USBD_EP_TYPE_CTRL                               0x00U
#define USBD_EP_TYPE_ISOC                               0x01U
#define USBD_EP_TYPE_BULK                               0x02U
#define USBD_EP_TYPE_INTR                               0x03U


typedef  struct  usb_setup_req
{
  uint8_t   bmRequest;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
} USBD_SetupReqTypedef;

struct _USBD_HandleTypeDef;

typedef struct _Device_cb
{
  uint8_t (*Init)(struct _USBD_HandleTypeDef *pdev, uint8_t cfgidx);
  uint8_t (*DeInit)(struct _USBD_HandleTypeDef *pdev, uint8_t cfgidx);
  /* Control Endpoints*/
  uint8_t (*Setup)(struct _USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req);
  uint8_t (*EP0_TxSent)(struct _USBD_HandleTypeDef *pdev);
  uint8_t (*EP0_RxReady)(struct _USBD_HandleTypeDef *pdev);
  /* Class Specific Endpoints*/
  uint8_t (*DataIn)(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);
  uint8_t (*DataOut)(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);
  uint8_t (*SOF)(struct _USBD_HandleTypeDef *pdev);
  uint8_t (*IsoINIncomplete)(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);
  uint8_t (*IsoOUTIncomplete)(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);

  uint8_t  *(*GetHSConfigDescriptor)(uint16_t *length);
  uint8_t  *(*GetFSConfigDescriptor)(uint16_t *length);
  uint8_t  *(*GetOtherSpeedConfigDescriptor)(uint16_t *length);
  uint8_t  *(*GetDeviceQualifierDescriptor)(uint16_t *length);
#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
  uint8_t  *(*GetUsrStrDescriptor)(struct _USBD_HandleTypeDef *pdev, uint8_t index,  uint16_t *length);
#endif

} USBD_ClassTypeDef;


typedef enum
{
  USBD_SPEED_HIGH  = 0U,
  USBD_SPEED_FULL  = 1U,
  USBD_SPEED_LOW   = 2U,
} USBD_SpeedTypeDef;


typedef enum
{
  USBD_OK   = 0U,
  USBD_BUSY,
  USBD_FAIL,
} USBD_StatusTypeDef;


typedef struct
{
  uint8_t  *(*GetDeviceDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
  uint8_t  *(*GetLangIDStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
  uint8_t  *(*GetManufacturerStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
  uint8_t  *(*GetProductStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
  uint8_t  *(*GetSerialStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
  uint8_t  *(*GetConfigurationStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
  uint8_t  *(*GetInterfaceStrDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
#if (USBD_LPM_ENABLED == 1U)
  uint8_t  *(*GetBOSDescriptor)(USBD_SpeedTypeDef speed, uint16_t *length);
#endif
} USBD_DescriptorsTypeDef;


typedef struct
{
  uint32_t                status;
  uint32_t                is_used;
  uint32_t                total_length;
  uint32_t                rem_length;
  uint32_t                maxpacket;
} USBD_EndpointTypeDef;


typedef struct _USBD_HandleTypeDef
{
  uint8_t                 id;
  uint32_t                dev_config;
  uint32_t                dev_default_config;
  uint32_t                dev_config_status;
  USBD_SpeedTypeDef       dev_speed;
  USBD_EndpointTypeDef    ep_in[16];
  USBD_EndpointTypeDef    ep_out[16];
  uint32_t                ep0_state;
  uint32_t                ep0_data_len;
  uint8_t                 dev_state;
  uint8_t                 dev_old_state;
  uint8_t                 dev_address;
  uint8_t                 dev_connection_status;
  uint8_t                 dev_test_mode;
  uint32_t                dev_remote_wakeup;

  USBD_SetupReqTypedef    request;
  USBD_DescriptorsTypeDef *pDesc;
  USBD_ClassTypeDef       *pClass;
  void                    *pClassData;
  void                    *pUserData;
  void                    *pData;
} USBD_HandleTypeDef;


#define  SWAPBYTE(addr)        (((uint16_t)(*((uint8_t *)(addr)))) + \
                               (((uint16_t)(*(((uint8_t *)(addr)) + 1U))) << 8U))

#define LOBYTE(x)  ((uint8_t)((x) & 0x00FFU))
#define HIBYTE(x)  ((uint8_t)(((x) & 0xFF00U) >> 8U))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))


#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif /* __packed */
#endif /* __GNUC__ */


#if defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
#ifndef __ALIGN_END
#define __ALIGN_END    __attribute__ ((aligned (4U)))
#endif /* __ALIGN_END */
#ifndef __ALIGN_BEGIN
#define __ALIGN_BEGIN
#endif /* __ALIGN_BEGIN */
#else
#ifndef __ALIGN_END
#define __ALIGN_END
#endif /* __ALIGN_END */
#ifndef __ALIGN_BEGIN
#if defined   (__CC_ARM)      /* ARM Compiler */
#define __ALIGN_BEGIN    __align(4U)
#elif defined (__ICCARM__)    /* IAR Compiler */
#define __ALIGN_BEGIN
#endif /* __CC_ARM */
#endif /* __ALIGN_BEGIN */
#endif /* __GNUC__ */


#ifdef __cplusplus
}
#endif

#endif /* __USBD_DEF_H */

