#ifndef K8061_H
#define K8061_H

#include <node.h>
#include <usb.h>

#define k8061_idVendor                 0x10cf /**< k8061 Vendor ID                             */
#define k8061_idProduct                0x8061 /**< k8061 Product ID                            */
#define k8061_epRead                   0x81   /**< Value of k8061 read end-point               */
#define k8061_epWrite                  0x01   /**< Value of k8061 write end-point              */
#define k8061_ioTimeOut                50     /**< timeout for usb_bulk_read/write calls       */
#define k8061_ioWait                   800    /**< wait after usb_bulk_read/write calls        */
#define k8061_iConfig                  1      /**< usb config to use on card                   */
#define k8061_iClaim                   0      /**< interface to claim                          */

#define k8061_cmdReadAnalogChannel     0x00   /**< k8061 Command: Read analog channel          */
#define k8061_cmdReadAllAnalog         0x01   /**< k8061 Command: Read all analog channels     */
#define k8061_cmdSetAnalogChannel      0x02   /**< k8061 Command: Set analog channel           */
#define k8061_cmdSetAllAnalog          0x03   /**< k8061 Command: Set all analog channels      */
#define k8061_cmdOutputPWM             0x04   /**< k8061 Command: Output PWM level             */
#define k8061_cmdReadDigitalByte       0x05   /**< k8061 Command: Read the digital byte        */
#define k8061_cmdDigitalOut            0x06   /**< k8061 Command: Set Digital output           */
#define k8061_cmdClearDigitalChannel   0x07   /**< k8061 Command: Set a signal digital channel */
#define k8061_cmdSetDigitalChannel     0x08   /**< k8061 Command: Set a signal digital channel */
#define k8061_cmdReadCounters          0x09   /**< k8061 Command: Read the number of errors    */
#define k8061_cmdResetCounters         0x0a   /**< k8061 Command: Reset the number of errors   */
#define k8061_cmdReadVersion           0x0b   /**< k8061 Command: Read version                 */
#define k8061_cmdJumpers               0x0c   /**< k8061 Command: Get Jumper status            */
#define k8061_cmdPowerStatus           0x0d   /**< k8061 Command: Power status                 */
#define k8061_cmdReadDigitalOut        0x0e   /**< k8061 Command: Read back digital out        */
#define k8061_cmdReadAnalogOut         0x0f   /**< k8061 Command: Read back analog out         */
#define k8061_cmdReadPWMOut            0x10   /**< k8061 Command: Read back PWM out            */

#define returnException(err) return scope.Close(ThrowException(Exception::Error(String::New(err))))

class k8061 : public node::ObjectWrap {
 public:
  static void Init();
  static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);

 private:
  k8061();
  ~k8061();
  static v8::Persistent<v8::Function> constructor;
  static v8::Handle<v8::Value> New             (const v8::Arguments& args);
  static bool                  execIO          (usb_dev_handle *dh, char *writeBuffer, int writeSize, char *readBuffer, int readSize);
  static char                  getAddress      (usb_dev_handle *dh);
  static char                  openDevice      (struct usb_device *dev, usb_dev_handle *&dh);
  static v8::Handle<v8::Value> readAddress     (const v8::Arguments& args);
  static v8::Handle<v8::Value> readPower       (const v8::Arguments& args);
  static v8::Handle<v8::Value> readDigitalIn   (const v8::Arguments& args);
  static v8::Handle<v8::Value> readDigitalOut  (const v8::Arguments& args);
  static v8::Handle<v8::Value> writeDigitalOut (const v8::Arguments& args);
  static v8::Handle<v8::Value> readAnalogIn    (const v8::Arguments& args);
  static v8::Handle<v8::Value> readAnalogOut   (const v8::Arguments& args);
  static v8::Handle<v8::Value> writeAnalogOut  (const v8::Arguments& args);
  static v8::Handle<v8::Value> readPwmOut      (const v8::Arguments& args);
  static v8::Handle<v8::Value> writePwmOut     (const v8::Arguments& args);
  static v8::Handle<v8::Value> close           (const v8::Arguments& args);

  usb_dev_handle* _k8061_Handle; /**< Array with filehandles for the k8061 devices */
};

#endif