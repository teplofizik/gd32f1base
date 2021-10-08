/*!
    \file    hid_keyboard_itf.c
    \brief   standard HID keyboard interface driver

    \version 2021-04-30, V3.0.0, demo for GD32F10x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "standard_hid_core.h"
#include "drv_usb_hw.h"

#define USBD_HID_INTERFACE                  0U
#define HID_IN_EP                           EP1_IN  /* EP1 for data IN */
#define HID_IN_PACKET                       8U

typedef enum
{
    CHAR_A = 1,
    CHAR_B,
    CHAR_C
} key_char;

static void key_config(void);
static uint8_t key_state(void);
static void hid_key_data_send(usb_dev *udev);

hid_fop_handler fop_handler = {
    .hid_itf_config = key_config,
    .hid_itf_data_process = hid_key_data_send
};

/*!
    \brief      configure the keys
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void key_config (void)
{
    /* configure the wakeup key in EXTI mode to remote wakeup */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);

    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);

    exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
}

/*!
    \brief      to get usb keyboard state
    \param[in]  none
    \param[out] none
    \retval     the char
*/
static uint8_t key_state (void)
{
    /* have pressed tamper key */
    if (!gd_eval_key_state_get(KEY_TAMPER)) {
        usb_mdelay(50U);

        if (!gd_eval_key_state_get(KEY_TAMPER)) {
            return CHAR_A;
        }
    }

    /* have pressed wakeup key */
    if (!gd_eval_key_state_get(KEY_WAKEUP)) {
        usb_mdelay(50U);
        
        if (!gd_eval_key_state_get(KEY_WAKEUP)) {
            return CHAR_B;
        }
    }

    /* have pressed user key */
    if (!gd_eval_key_state_get(KEY_USER)) {
        usb_mdelay(50U);

        if (!gd_eval_key_state_get(KEY_USER)) {
            return CHAR_C;
        }
    }

    /* no pressed any key */
    return 0U;
}

static void hid_key_data_send(usb_dev *udev)
{
    standard_hid_handler *hid = (standard_hid_handler *)udev->dev.class_data[USBD_HID_INTERFACE];

    if (hid->prev_transfer_complete) {
        switch (key_state()) {
        case CHAR_A:
            hid->data[2] = 0x04;
            break;
        case CHAR_B:
            hid->data[2] = 0x05;
            break;
        case CHAR_C:
            hid->data[2] = 0x06;
            break;
        default:
            break;
        }

        if (0 != hid->data[2]) {
            hid_report_send(udev, hid->data, HID_IN_PACKET);
        }
    }
}
