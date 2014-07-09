"use strict";
var k8061 = require('./k8061.node');

function cmp(a, b){
  if (!(a instanceof Array && b instanceof Array)) return false;
  for (var i = 0, l = Math.max(8, a.length, b.length); i < l; i++) {
    if (a[i] != b[i]) return true;
  }
  return false;
}

function K8061(id, byAddress) {
  if (!(this instanceof K8061)) return new K8061(id, byAddress);
  var _k8061 = k8061.k8061(id, byAddress);
  var dOut, dOutOld, aOut, aOutOld, undef;
  this.close = function(){
    _k8061.close();
    _k8061 = null;
    return true;
  }
  Object.defineProperties(this, {
    "address": {
      get: function(){
        return _k8061.readAddress();
      },
      enumerable: true
    },
    "hasPower": {
      get: function(){
        return _k8061.readPower();
      },
      enumerable: true
    },
    "digitalIn": {
      get: function(){
        return _k8061.readDigitalIn();
      },
      enumerable: true
    },
    "digitalOut": {
      get: function(){
        if (!dOut) {
          dOut = _k8061.readDigitalOut();
          dOutOld = dOut.slice();
          process.nextTick(function(){
            if (cmp(dOut, dOutOld)) _k8061.writeDigitalOut(dOut);
            dOut = dOutOld = undef;
          });
        }
        return dOut;
      },
      set: function(val){
        dOut = dOutOld = undef;
        return _k8061.writeDigitalOut(val);
      },
      enumerable: true
    },
    "analogIn": {
      get: function(){
        return _k8061.readAnalogIn();
      },
      enumerable: true
    },
    "analogOut": {
      get: function(){
        if (!aOut) {
          aOut = _k8061.readAnalogOut();
          aOutOld = aOut.slice();
          process.nextTick(function(){
            if (cmp(aOut, aOutOld)) _k8061.writeAnalogOut(aOut);
            aOut = aOutOld = undef;
          });
        }
        return aOut;
      },
      set: function(val){
        aOut = aOutOld = undef;
        return _k8061.writeAnalogOut(val);
      },
      enumerable: true
    },
    "pwmOut": {
      get: function(){
        return _k8061.readPwmOut();
      },
      set: function(val){
        return _k8061.writePwmOut(val);
      },
      enumerable: true
    },
    "close": {
      configurable: false,
      enumerable: true,
      writable: false
    }
  });
}

Object.defineProperty(exports, "count", {
  get: function(){return k8061.count()},
  enumerable: true
});

exports.k8061 = K8061;