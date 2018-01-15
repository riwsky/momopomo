type dom;

[@bs.scope "document"] [@bs.val] [@bs.return nullable]
external querySelector : string => option(dom) = "";

[@bs.send] external play : dom => unit = "";

let tag: ReasonReact.reactElement =
  <audio src="../fire_pager.mp3" _type="audio/mpeg" id="audioTag" />;

let go: unit => unit =
  () =>
    switch (querySelector("#audioTag")) {
    | Some(d) => play(d)
    | _ => ()
    };
