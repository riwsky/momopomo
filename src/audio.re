type t;

[@bs.new] external create : string => t = "Audio";

[@bs.send] external play : t => unit = "play";

let go: unit => unit = () => create("fire_pager.mp3") |> play;
