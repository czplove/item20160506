//

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "app/framework/plugin/reporting/reporting.h"
#include "app/framework/plugin/ezmode-commissioning/ez-mode.h"
#include "app/framework/plugin-soc/connection-manager/connection-manager.h"
#include "hal/micro/led-blink.h"

#define BUTTON_DEBOUNCE_TIME_MS     50
#define MAX_TIME_BETWEEN_PRESSES_MS 600
static uint8_t consecutiveButtonPressCount = 0;
// Custom event stubs. Custom events will be run along with all other events in
// the application framework. They should be managed using the Ember Event API
// documented in stack/include/events.h

// Event control struct declaration
EmberEventControl emberAfButton0PressCountEventControl;

// Event function forward declaration
void emberAfButton0PressCountEventHandler(void);

// Event function stub
void emberAfButton0PressCountEventHandler(void) {


    // If on a network:
    // 2 presses activates identify
    // 3 presses blinks network status
    // 4 presses initiates a proactive rejoin
    if(consecutiveButtonPressCount == 1){
//      emberAfPluginConnectionManagerLedNetworkFoundBlink();
    }else if (consecutiveButtonPressCount == 2) {
      emberAfEzmodeServerCommission(emberAfPrimaryEndpoint()); /* ezmode server */
    } else if (consecutiveButtonPressCount == 3) {
      halLedBlinkBlink(1,0);
      emberAfBroadcastPermitJoin(0);
    } else if (consecutiveButtonPressCount == 4) {
      halLedBlinkBlink(240,500);
      emberAfBroadcastPermitJoin(240);
    } else if (consecutiveButtonPressCount == 6) {
      emberAfFindUnusedPanIdAndFormCallback();
    }

  consecutiveButtonPressCount = 0;

  emberEventControlSetInactive(emberAfButton0PressCountEventControl);

}

/** @brief Button0 Pressing
 *
 * This function is periodically called when button 0 is being pressed.
 *
 */
void emberAfPluginButtonInterfaceButton0PressingCallback(void)
{
  emberLeaveNetwork();
  halLedBlinkBlink(1,0);
}


/** @brief Button0 Pressed Short
 *
 * This function returns the number of times a button was short pressed.
 *
 * @param timePressedMs Time (in ms) button 0 was pressed  Ver.: always
 */
void emberAfPluginButtonInterfaceButton0PressedShortCallback(uint16_t timePressedMs)
{
    if(timePressedMs > BUTTON_DEBOUNCE_TIME_MS *20 ){
    consecutiveButtonPressCount = 0;
    return;
  }
  consecutiveButtonPressCount++;

  emberEventControlSetDelayMS(emberAfButton0PressCountEventControl,
                              MAX_TIME_BETWEEN_PRESSES_MS);

}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
extern bool HeartbeatEnable;
boolean emberAfStackStatusCallback(EmberStatus status)
{
  if(status == EMBER_NETWORK_UP)
  {
    HeartbeatEnable = true;
  }
  else
  {
    HeartbeatEnable = false;
  }
  return false;
}

EmberAfOtaStorageStatus emberAfOtaStorageDeleteImageCallback(const EmberAfOtaImageId* id)
{

  return EMBER_AF_OTA_STORAGE_SUCCESS;
}

void emAfOtaStorageInfoPrint(void)
{

}

/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{
	uint32_t lastMs,nowMs,i;
  uint8_t port;
  uint8_t pin;

	nowMs = halCommonGetInt32uMillisecondTick();
	lastMs = nowMs;

	//-if (state == NETWORK_FIND_NONE) {//-?¨¤¦Ì¨¨?¦Ì?¡Â??¨®D?¡§¨ª?¨¦?¦Ì?D¨¨¨°a¨¦¨¢¦Ì?
		//-halLedBlinkBlink(2,500);
		for(i = 0;i < 4;i++)
		{
			while(elapsedTimeInt32u(lastMs, nowMs) <= 500)
			{
				nowMs = halCommonGetInt32uMillisecondTick();
        halResetWatchdog();
			}
			lastMs = halCommonGetInt32uMillisecondTick();
			if((i == 0) || (i == 2))
			{
        port = (BOARDLED0) >> 3;
        pin = (BOARDLED0) & 0x07;
				halLedBlinkSleepyClearGpio( port, pin );
				port = (BOARDLED1) >> 3;
        pin = (BOARDLED1) & 0x07;
				halLedBlinkSleepyClearGpio( port, pin );
			}
			else
			{
				port = (BOARDLED0) >> 3;
        pin = (BOARDLED0) & 0x07;
				halLedBlinkSleepySetGpio( port, pin );
				port = (BOARDLED1) >> 3;
        pin = (BOARDLED1) & 0x07;
				halLedBlinkSleepySetGpio( port, pin );
			}
		}
	//-}
}