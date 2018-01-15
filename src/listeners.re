type eventListener =
  {
    .
    "key": string,
    [@bs.meth] "preventDefault": unit => unit
  } =>
  unit;

[@bs.scope ("window", "document")] [@bs.val]
external add : (string, eventListener) => unit = "addEventListener";

[@bs.scope ("window", "document")] [@bs.val]
external remove : (string, eventListener) => unit = "removeEventListener";
