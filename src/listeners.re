type eventListener =
  {
    .
    "key": string,
    [@bs.meth] "preventDefault": unit => unit
  } =>
  unit;

let forKey: (string, unit => unit) => eventListener =
  (str, cb, event) =>
    if (event##key == str) {
      event##preventDefault();
      cb();
    };

[@bs.scope ("window", "document")] [@bs.val]
external add : (string, eventListener) => unit = "addEventListener";

[@bs.scope ("window", "document")] [@bs.val]
external remove : (string, eventListener) => unit = "removeEventListener";
