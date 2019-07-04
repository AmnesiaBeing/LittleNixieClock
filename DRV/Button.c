#include "BSP/BSP.h"
#include "DRV/Button.h"
#include "cmsis_os.h"

osSemaphoreId ButtonSemHandle;

void Button_Init(void)
{
    osSemaphoreDef(ButtonSemHandle);
    ButtonSemHandle = osSemaphoreCreate(osSemaphore(ButtonSemHandle), 1);
}

static void (*Button_Callback)(void);

void Button_Int_Callback(void)
{
    if (Button_Callback)
        Button_Callback();
}

void Button_Register_Callback(void (*callback)(void))
{
    osSemaphoreWait(ButtonSemHandle, osWaitForever);
    do
    {
        Button_Callback = callback;
        HAL_NVIC_EnableIRQ(Button_IRQ);
    } while (0);
    osSemaphoreRelease(ButtonSemHandle);
}

void Button_Unregister_Callback(void)
{
    osSemaphoreWait(ButtonSemHandle, osWaitForever);
    do
    {
        Button_Callback = NULL;
        HAL_NVIC_DisableIRQ(Button_IRQ);
    } while (0);
    osSemaphoreRelease(ButtonSemHandle);
}