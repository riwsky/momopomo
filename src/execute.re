[%bs.raw {|require('./semantic/semantic.css')|}];

let s2e = ReasonReact.stringToElement;

type action =
  | Play
  | Pause
  | Stop
  | Tick(Luxon.DateTime.t);

module type ButtonInfo = {
  let getAction: action;
  let getColor: string;
  let getIconName: string;
  let getName: string;
};

module Button = (Info: ButtonInfo) => {
  let component = ReasonReact.statelessComponent(Info.getName ++ "Button");
  let make = (~reducer, _children) => {
    ...component,
    render: _self =>
      <button
        className=("ui labeled icon button " ++ Info.getColor)
        onClick=(reducer(_event => Info.getAction))>
        <i className=(Info.getIconName ++ " icon") />
        (s2e(Info.getName))
      </button>
  };
};

module PlayButton =
  Button(
    {
      let getAction = Play;
      let getColor = "blue";
      let getIconName = "play";
      let getName = "Play";
    }
  );

module StopButton =
  Button(
    {
      let getAction = Stop;
      let getColor = "red";
      let getIconName = "stop";
      let getName = "Stop";
    }
  );

module PauseButton =
  Button(
    {
      let getAction = Pause;
      let getColor = "gray";
      let getIconName = "pause";
      let getName = "Pause";
    }
  );

let pomoDuration: Luxon.Duration.t = Luxon.Duration.fromMillis(1000 * 60 * 25);

type status =
  | Ready
  | PlayingSince(Luxon.DateTime.t)
  | PausedAfter(Luxon.Duration.t);

type state = {
  now: Luxon.DateTime.t,
  status,
  intervalId: option(Js.Global.intervalId)
};

let component = ReasonReact.reducerComponent("Execute");

let timeLeft: state => Luxon.Duration.t = ({now, status}) =>
    switch status {
        | Ready => pomoDuration
        | PlayingSince(someTime) => Luxon.Duration.minus(pomoDuration, Luxon.DateTime.diff(now, someTime))
        | PausedAfter(someDur) => Luxon.Duration.minus(pomoDuration, someDur)
    };

let mmSs: Luxon.Duration.t => string =
  dur => Luxon.Duration.toFormat(dur, "mm:ss");

let make = _children => {
  ...component,
  initialState: () => {now: Luxon.DateTime.localNow(), status: Ready, intervalId: None},
  didMount: self => ReasonReact.Update({...self.state,
    intervalId: Some(Js.Global.setInterval(self.reduce((_) => Tick(Luxon.DateTime.localNow())), 1000))}),
  reducer: (action, state) =>
    switch action {
    | Tick(newNow) =>
      let newStatus =
        switch state.status {
        | PlayingSince(someTime) when Luxon.DateTime.(asEpochMillis(someTime) <= asEpochMillis(minus(newNow, pomoDuration))) =>
              Ready;
        | PlayingSince(_)
        | Ready
        | PausedAfter(_) => state.status
        };
      ReasonReact.Update({...state, now: newNow, status: newStatus});
    | Play =>
      switch state.status {
      | Ready =>
        ReasonReact.Update({...state, status: PlayingSince(state.now)})
      | PlayingSince(_) => ReasonReact.NoUpdate
      | PausedAfter(dur) =>
        ReasonReact.Update({
          ...state,
          status: PlayingSince(Luxon.DateTime.minus(state.now, dur))
        })
      }
    | Pause =>
      switch state.status {
      | Ready
      | PausedAfter(_) => ReasonReact.NoUpdate
      | PlayingSince(someTime) =>
        ReasonReact.Update({
          ...state,
          status: PausedAfter(Luxon.DateTime.diff(state.now, someTime))
        })
      }
    | Stop => ReasonReact.Update({...state, status: Ready})
    },
  render: self =>
    <div>
      <p> (s2e("Omg hello world amirite Time Left: " ++ mmSs(timeLeft(self.state)))) </p>
      (
        switch self.state.status {
        | Ready => <PlayButton reducer=self.reduce />
        | PlayingSince(_) =>
          <div>
            <PauseButton reducer=self.reduce />
            <StopButton reducer=self.reduce />
          </div>
        | PausedAfter(_) =>
          <div>
            <PlayButton reducer=self.reduce />
            <StopButton reducer=self.reduce />
          </div>
        }
      )
    </div>
};