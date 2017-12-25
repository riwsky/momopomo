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

type status = Ready;

let component = ReasonReact.reducerComponent("Execute");

let make = (_children) => {
    ...component,
    initialState: () => Ready,
    render: (_self) => 
    <div>
        <p> (ReasonReact.stringToElement("Omg hello world amirite")) </p>
        <PlayButton/>
    </div>
};