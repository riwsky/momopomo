[%bs.raw {|require('./semantic/semantic.css')|}];

module DateTime = {
    type t;
    [@bs.send] external local: t => t = "";
    [@bs.send.pipe: t] external toString: string = "";
};

[@bs.module "luxon"] external dateTime : DateTime.t = "DateTime";

let zz = DateTime.local(dateTime) |> DateTime.toString;

type action = Play | Pause | Stop;

module PlayButton = {
    let component = ReasonReact.statelessComponent("PlayButton");
    let make = (_children) => {
        ...component,
        render: (_self) =>
        <button className="ui labeled icon blue button">
        <i className="play icon"></i>
        (ReasonReact.stringToElement("Play"))
        </button>
    };
};

type status = Ready | Playing | Paused;

let component = ReasonReact.reducerComponent("Execute");

let make = (_children) => {
    ...component,
    initialState: () => Ready,
    reducer: (action, state) => {
        switch action {
        | Play => ReasonReact.Update(Playing)
        | Pause => ReasonReact.Update(Paused)
        | Stop => ReasonReact.Update(Ready)
        }
    },
    render: (_self) => 
    <div>
        <p> (ReasonReact.stringToElement("Omg hello world amirite" ++ zz)) </p>
        <PlayButton/>
    </div>
};