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
};

module DateTime = {
  type t;
  type u;
  [@bs.module "luxon"] external static : u = "DateTime";
  [@bs.send.pipe : t] external toString : string = "";
  [@bs.send] external local : u => t = "";
  let localNow = () : t => local(static);
  [@bs.send] external diff : (t, t) => Duration.t = "";
  [@bs.send] external minus : (t, Duration.t) => t = "";
};

let zz = DateTime.(local(static) |> toString);

let om = Duration.(toFormat(fromMillis(250000), "mm:ss"));