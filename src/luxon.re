module Duration = {
  type t;
  type u;
  [@bs.module "luxon"] external static : u = "Duration";
  [@bs.send] external fromObject : (u, {. "millis": int}) => t = "fromObject";
  let fromMillis: int => t = ms => fromObject(static, {"millis": ms});
};

module DateTime = {
  type t;
  type u;
  [@bs.module "luxon"] external static : u = "DateTime";
  [@bs.send.pipe : t] external toString : string = "";
  [@bs.send] external local : u => t = "";
  let localNow = () : t => local(static);
  [@bs.send] external diff : (t, t) => Duration.t = "";
};

let zz = DateTime.local(DateTime.static) |> DateTime.toString;