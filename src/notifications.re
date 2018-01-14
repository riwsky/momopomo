type permission = [ | `granted | `default | `denied];

[@bs.scope ("window", "Notification")] [@bs.val]
external requestPermission : (permission => unit) => unit = "";

[@bs.new] external create_notification : string => unit = "Notification";
