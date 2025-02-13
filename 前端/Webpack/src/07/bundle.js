(() => {
  var __webpack_modules__ = {
    "./src/test.js": (
      __unused_webpack_module,
      __webpack_exports__,
      __webpack_require__
    ) => {
      "use strict";
      __webpack_require__.r(__webpack_exports__);
      __webpack_require__.d(__webpack_exports__, {
        foo: () => /* binding */ foo,
      });
      console.log(`hello test.js`);

      function foo() {
        console.log(`test.js foo`);
      }
    },

    "./src/util/add.js": (
      __unused_webpack_module,
      __webpack_exports__,
      __webpack_require__
    ) => {
      "use strict";
      __webpack_require__.r(__webpack_exports__);
      __webpack_require__.d(__webpack_exports__, {
        add: () => /* binding */ add,
      });
      function add(a, b) {
        return a + b;
      }
    },

    "./src/util/data.js": (module) => {
      function today() {
        return "2024.24.24";
      }

      module.exports = {
        today,
      };
    },
  };
  var __webpack_module_cache__ = {};

  function __webpack_require__(moduleId) {
    var cachedModule = __webpack_module_cache__[moduleId];
    if (cachedModule !== undefined) {
      return cachedModule.exports;
    }
    var module = (__webpack_module_cache__[moduleId] = {
      exports: {},
    });

    __webpack_modules__[moduleId](module, module.exports, __webpack_require__);

    return module.exports;
  }

  (() => {
    __webpack_require__.n = (module) => {
      var getter =
        module && module.__esModule ? () => module["default"] : () => module;
      __webpack_require__.d(getter, { a: getter });
      return getter;
    };
  })();

  (() => {
    __webpack_require__.d = (exports, definition) => {
      for (var key in definition) {
        if (
          __webpack_require__.o(definition, key) &&
          !__webpack_require__.o(exports, key)
        ) {
          Object.defineProperty(exports, key, {
            enumerable: true,
            get: definition[key],
          });
        }
      }
    };
  })();

  (() => {
    __webpack_require__.o = (obj, prop) =>
      Object.prototype.hasOwnProperty.call(obj, prop);
  })();

  (() => {
    __webpack_require__.r = (exports) => {
      if (typeof Symbol !== "undefined" && Symbol.toStringTag) {
        Object.defineProperty(exports, Symbol.toStringTag, { value: "Module" });
      }
      Object.defineProperty(exports, "__esModule", { value: true });
    };
  })();

  var __webpack_exports__ = {};
  (() => {
    "use strict";
    __webpack_require__.r(__webpack_exports__);
      var _util_add__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ./util/add */ "./src/util/add.js");
      var _util_data__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./util/data */ "./src/util/data.js");
      var _util_data__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_util_data__WEBPACK_IMPORTED_MODULE_1__);
      var _test__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./test */ "./src/test.js");

      const add = __webpack_require__(/*! ./util/add */ "./src/util/add.js");

      const data1 = __webpack_require__(/*! ./util/data */ "./src/util/data.js");

      console.log(add.add(1, 2));
      console.log(_util_add__WEBPACK_IMPORTED_MODULE_0__.add(1, 2));
      console.log(_util_data__WEBPACK_IMPORTED_MODULE_1__.today())
      console.log(data1.today())
  })();
})();
