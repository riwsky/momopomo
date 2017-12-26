module Duration = {
  type t;
  type u;
  [@bs.module "luxon"] external static : u = "Duration";
  [@bs.send]
  external fromObject : (u, {. "milliseconds": int}) => t = "fromObject";
  [@bs.send] external toString : t => string = "";
  [@bs.send] external toISO : t => string = "";
  let fromMillis: int => t = ms => fromObject(static, {"milliseconds": ms});
  [@bs.send] external toFormat : (t, string) => string = "";
  [@bs.send] external as_ : (t, string) => int = "as";
  let toMillis: t => int = dur => as_(dur, "milliseconds");
  let minus: (t, t) => t = (a, b) => fromMillis(toMillis(a) - toMillis(b))
};

module type DateTimeI = {
  type t;
  let localNow: unit => t;
  let toString: t => string;
  let diff : (t, t) => Duration.t;
  let minus: (t, Duration.t) => t;
  let asEpochMillis : t => int;
};

module DateTime : DateTimeI = {
  type t;
  type u;
  [@bs.module "luxon"] external static : u = "DateTime";
  [@bs.send.pipe : t] external toString : string = "";
  [@bs.send] external local : u => t = "";
  let localNow = () : t => local(static);
  [@bs.send] external diff : (t, t) => Duration.t = "";
  [@bs.send] external minusBorked : (t, int) => t = "minus";
  let minus = (dt, dur) => minusBorked(dt, Duration.toMillis(dur));
  [@bs.send] external asEpochMillis : t => int = "valueOf";
};