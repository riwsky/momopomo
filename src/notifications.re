type permission = [ | `granted | `default | `denied];

[@bs.scope ("window", "Notification")] [@bs.val]
external requestPermission : (permission => unit) => unit = "";

[@bs.new] external create_notification : string => unit = "Notification";

[@bs.val] external document : {. [@bs.set] "title": string} = "";

let set_title : string => unit = str => document##title #= str;