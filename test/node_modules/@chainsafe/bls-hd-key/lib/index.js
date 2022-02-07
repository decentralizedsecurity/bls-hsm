"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _keyDerivation = require("./key-derivation");

Object.keys(_keyDerivation).forEach(function (key) {
  if (key === "default" || key === "__esModule") return;
  Object.defineProperty(exports, key, {
    enumerable: true,
    get: function () {
      return _keyDerivation[key];
    }
  });
});

var _path = require("./path");

Object.keys(_path).forEach(function (key) {
  if (key === "default" || key === "__esModule") return;
  Object.defineProperty(exports, key, {
    enumerable: true,
    get: function () {
      return _path[key];
    }
  });
});
//# sourceMappingURL=index.js.map