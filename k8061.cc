#include <stdio.h>
#include <stdlib.h>
#include <usb.h>
#include <string.h>
#include <v8.h>
#include <node.h>
#include "k8061.h"

using namespace v8;

Handle<Value> count(const Arguments& args) {
  HandleScope scope;

  struct usb_bus *bus; 
  struct usb_device *dev; 

  int sum = 0;
  usb_find_busses();
  usb_find_devices();
  for ( bus = usb_get_busses(); bus; bus = bus->next ) {
    for ( dev = bus->devices; dev; dev = dev->next ) {
      if ( ( dev->descriptor.idVendor  == k8061_idVendor  ) && 
           ( dev->descriptor.idProduct == k8061_idProduct ) ) {
        sum++;
      }
    }
  }
  return scope.Close(Integer::New(sum));
}

Handle<Value> resetAll(const Arguments& args) {
  HandleScope scope;

  struct usb_bus *bus; 
  struct usb_device *dev;
  usb_dev_handle *dh; 

  int sum = 0;
  usb_find_busses();
  usb_find_devices();
  for ( bus = usb_get_busses(); bus; bus = bus->next ) {
    for ( dev = bus->devices; dev; dev = dev->next ) {
      if ( ( dev->descriptor.idVendor  == k8061_idVendor  ) && 
           ( dev->descriptor.idProduct == k8061_idProduct ) ) {
        if ((dh = usb_open(dev))) {
          usb_reset(dh);
          usb_close(dh);
          sum++;
        }
      }
    }
  }
  return scope.Close(Integer::New(sum));
}

k8061::k8061() {

}

k8061::~k8061() {
  if (_k8061_Handle) usb_close(_k8061_Handle);
}

Persistent<Function> k8061::constructor;

void k8061::Init() {
  usb_init();
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("k8061"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readAddress", readAddress);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readPower", readPower);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readDigitalIn", readDigitalIn);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readDigitalOut", readDigitalOut);
  NODE_SET_PROTOTYPE_METHOD(tpl, "writeDigitalOut", writeDigitalOut);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readAnalogIn", readAnalogIn);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readAnalogOut", readAnalogOut);
  NODE_SET_PROTOTYPE_METHOD(tpl, "writeAnalogOut", writeAnalogOut);
  NODE_SET_PROTOTYPE_METHOD(tpl, "readPwmOut", readPwmOut);
  NODE_SET_PROTOTYPE_METHOD(tpl, "writePwmOut", writePwmOut);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", close);
  constructor = Persistent<Function>::New(tpl->GetFunction());
}

bool k8061::execIO(usb_dev_handle *dh, char *writeBuffer, int writeSize, char *readBuffer, int readSize){
  int n;
  n = usb_bulk_write(dh, k8061_epWrite, writeBuffer, writeSize, k8061_ioTimeOut);
  usleep(k8061_ioWait);
  if (n < 0) return false;
  n = usb_bulk_read(dh, k8061_epRead, readBuffer, readSize, k8061_ioTimeOut);
  usleep(k8061_ioWait);
  if (n < 0) return false;
  return true;
}

char k8061::getAddress(usb_dev_handle *dh) {
  char writeBuffer[1], readBuffer[2];
  writeBuffer[0] = k8061_cmdJumpers;
  if (!execIO(dh, writeBuffer, 1, readBuffer, 2)) return -1;
  return readBuffer[1];
}

char k8061::openDevice(struct usb_device *dev, usb_dev_handle *&dh) {
  if (!(dh = usb_open(dev))) return -1;
  if (usb_set_configuration(dh, k8061_iConfig) < 0) {
    usb_close(dh);
    return -2;
  }
  if (usb_claim_interface(dh, k8061_iClaim) < 0) {
    usb_close(dh);
    return -3;
  }
  return 0;
}

Handle<Value> k8061::readAddress(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char addr = getAddress(obj->_k8061_Handle);
  if (addr < 0) returnException("Cannot communicate with device");
  return scope.Close(Number::New(addr));
}

Handle<Value> k8061::readPower(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[1], readBuffer[2];
  writeBuffer[0] = k8061_cmdPowerStatus;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 1, readBuffer, 2)) returnException("Cannot communicate with device");
  return scope.Close(Number::New(readBuffer[1]));
}

Handle<Value> k8061::readDigitalIn(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[1], readBuffer[2];
  writeBuffer[0] = k8061_cmdReadDigitalByte;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 1, readBuffer, 2)) returnException("Cannot communicate with device");
  int i, mask;
  Local<Array> result = Array::New(8);
  for(i = 0, mask = 1; i < 8; i++, mask <<= 1) result->Set(i, Number::New((readBuffer[1] & mask) >> i));
  return scope.Close(result);
}

Handle<Value> k8061::readDigitalOut(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[1], readBuffer[2];
  writeBuffer[0] = k8061_cmdReadDigitalOut;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 1, readBuffer, 2)) returnException("Cannot communicate with device");
  int i, mask;
  Local<Array> result = Array::New(8);
  for(i = 0, mask = 1; i < 8; i++, mask <<= 1) result->Set(i, Number::New((readBuffer[1] & mask) >> i));
  return scope.Close(result);
}

Handle<Value> k8061::writeDigitalOut(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[2], readBuffer[3];
  unsigned char val = 0;
  writeBuffer[0] = k8061_cmdDigitalOut;
  if (args[0]->IsArray()){
    Local<Array> arr = Local<Array>::Cast(args[0]);
    char i, l = arr->Length();
    if (l > 8) l = 8;
    for (i = 0; i < l; i++)
      if (arr->Get(i)->Int32Value() != 0) val += 1 << i;
  } else {
    if (args[0]->Int32Value() != 0) val = 0xff;
  }
  writeBuffer[1] = val;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 2, readBuffer, 3)) returnException("Cannot communicate with device");
  return scope.Close(Boolean::New(true));
}

Handle<Value> k8061::readAnalogIn(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[1], readBuffer[17];
  writeBuffer[0] = k8061_cmdReadAllAnalog;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 1, readBuffer, 17)) returnException("Cannot communicate with device");
  int i;
  Local<Array> result = Array::New(8);
  for (i = 0; i < 8; i++) result->Set(i, Number::New((unsigned char)readBuffer[2 * i + 1] + 256 * (unsigned char)readBuffer[2 * i + 2]));
  return scope.Close(result);
}

Handle<Value> k8061::readAnalogOut(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[1], readBuffer[9];
  writeBuffer[0] = k8061_cmdReadAnalogOut;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 1, readBuffer, 9)) returnException("Cannot communicate with device");
  int i;
  Local<Array> result = Array::New(8);
  for (i = 0; i < 8; i++) result->Set(i, Number::New((unsigned char)readBuffer[i + 1]));
  return scope.Close(result);
}

Handle<Value> k8061::writeAnalogOut(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[9], readBuffer[2];
  writeBuffer[0] = k8061_cmdSetAllAnalog;
  char i, l;
  int v;
  if (args[0]->IsArray()){
    Local<Array> arr = Local<Array>::Cast(args[0]);
    l = arr->Length();
    for (i = 0; i < 8; i ++) {
      if (i < l) {
        v = arr->Get(i)->Int32Value();
        if (v < 0  ) v = 0;
        if (v > 255) v = 255;
      } else {
        v = 0;
      }
      writeBuffer[i + 1] = (unsigned char)v;
    }
  } else {
    v = args[0]->Int32Value();
    if (v < 0  ) v = 0;
    if (v > 255) v = 255;
    for (i = 0; i < 8; i ++) {
      writeBuffer[i + 1] = (unsigned char)v;
    }
  }
  if (!execIO(obj->_k8061_Handle, writeBuffer, 9, readBuffer, 2)) returnException("Cannot communicate with device");
  return scope.Close(Boolean::New(true));
}

Handle<Value> k8061::readPwmOut(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[1], readBuffer[3];
  writeBuffer[0] = k8061_cmdReadPWMOut;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 1, readBuffer, 3)) returnException("Cannot communicate with device");
  return scope.Close(Number::New((unsigned char)readBuffer[1] + 4 * (unsigned char)readBuffer[2]));
}

Handle<Value> k8061::writePwmOut(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  char writeBuffer[3], readBuffer[4];
  writeBuffer[0] = k8061_cmdOutputPWM;
  int val = args[0]->Int32Value();
  if (val < 0   ) val = 0;
  if (val > 1023) val = 1023;
  writeBuffer[1] = (unsigned char)(val & 0x03);
  writeBuffer[2] = (unsigned char)(val >> 2) & 0xFF;
  if (!execIO(obj->_k8061_Handle, writeBuffer, 3, readBuffer, 4)) returnException("Cannot communicate with device");
  return scope.Close(Boolean::New(true));
}

Handle<Value> k8061::close(const Arguments& args) {
  HandleScope scope;
  k8061* obj = ObjectWrap::Unwrap<k8061>(args.This());
  usb_close(obj->_k8061_Handle);
  obj->_k8061_Handle = NULL;
  return scope.Close(Boolean::New(true));
}

Handle<Value> k8061::New(const Arguments& args) {
  HandleScope scope;

  if (args.IsConstructCall()) {
    int id = args[0]->IsUndefined() ? 0 : args[0]->Int32Value();
    bool byAddress = args[1]->BooleanValue();

    if (id < 0) id = 0;

    struct usb_bus *bus; 
    struct usb_device *dev;
    usb_dev_handle *dh;
    bool found = false, initalized = false;
    int i = 0;
    usb_find_busses();
    usb_find_devices();

    for (bus = usb_get_busses(); bus; bus = bus->next) {
      for (dev = bus->devices; dev; dev = dev->next) {
        if ((dev->descriptor.idVendor  == k8061_idVendor) && 
             (dev->descriptor.idProduct == k8061_idProduct)) {
          if (byAddress) {
            i = openDevice(dev, dh);
            if (i < 0) continue;
            i = getAddress(dh);
            if (i == id) {
              found = true;
              initalized = true;
              break;
            }
            usb_close(dh);
          } else {
            if (i == id) {
              found = true;
              break;
            }
            i++;
          }
        }
      }
      if (found) break;
    }
    if (!found) {
      if (byAddress)
        returnException("Device is busy or not found");
      else
        returnException("Device not found");
    }

    if (!initalized) {
      i = openDevice(dev, dh);
      switch (i) {
        case -1:
          returnException("Cannot open device");
        case -2:
          returnException("Device is busy");
        case -3:
          returnException("Cannot claim interface");
      }
    }
    i = getAddress(dh);

    k8061* obj = new k8061();
    obj->_k8061_Handle = dh;
    
    obj->Wrap(args.This());
    return args.This();
  } else {
    Local<Value> argv[2] = {args[0], args[1]};
    return scope.Close(constructor->NewInstance(2, argv));
  }
}

Handle<Value> k8061::NewInstance(const Arguments& args) {
  HandleScope scope;
  Handle<Value> argv[2] = {args[0], args[1]};
  Local<Object> instance = constructor->NewInstance(2, argv);
  return scope.Close(instance);
}

Handle<Value> new_k8061(const Arguments& args) {
  HandleScope scope;
  return scope.Close(k8061::NewInstance(args));
}

void init (Handle<Object> target) {
  k8061::Init();
  NODE_SET_METHOD(target, "count",    count);
  NODE_SET_METHOD(target, "resetAll", resetAll);
  NODE_SET_METHOD(target, "k8061",    new_k8061);
}

NODE_MODULE(k8061, init)

