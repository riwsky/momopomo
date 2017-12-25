[%bs.raw {|require('./semantic/semantic.css')|}];

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

type status = Ready | PlayingSince(Luxon.DateTime.t) | PausedAfter(Luxon.Duration.t);

let component = ReasonReact.reducerComponent("Execute");

let make = (_children) => {
    ...component,
    initialState: () => Ready,
    reducer: (action, state) => {
        switch action {
        | Play => ReasonReact.Update(PlayingSince(Luxon.DateTime.localNow()))
        | Pause => switch state {
        | Ready => ReasonReact.NoUpdate
        | PlayingSince(someTime) => ReasonReact.Update(
                    PausedAfter(Luxon.DateTime.diff(Luxon.DateTime.localNow(), someTime)))
        | PausedAfter(_) => ReasonReact.NoUpdate 
        }
        | Stop => ReasonReact.Update(Ready)
        }
    },
    render: (_self) => 
    <div>
        <p> (ReasonReact.stringToElement("Omg hello world amirite" ++ Luxon.zz)) </p>
        <PlayButton/>
    </div>
};